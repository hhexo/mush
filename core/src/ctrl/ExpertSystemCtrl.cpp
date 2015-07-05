// Copyright 2015 Dario Domizioli
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//     http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#include "core/ctrl/ExpertSystemCtrl.h"
#include "core/game/Move.h"
#include "core/game/Character.h"

#include <cassert>

namespace core {
namespace ctrl {



// ========================================================================
//    ATTACK EXPERT SYSTEMS
// ========================================================================



GruntAtk::~GruntAtk() {}

char const * const GruntAtk::getName() const { return "Grunt"; }

int GruntAtk::shouldSpendAPToGainSP(game::Character const &me,
                          game::Character const &opponent,
                          bool far) {
    if(me.cur_ap > 4) {
        return me.cur_ap - 4;
    }
    return 0;
}

game::Move const & GruntAtk::getNextMove(game::Character const &me,
                               game::Character const &opponent,
                               bool far) {
    size_t which_move = 0;
    size_t this_move = 0;
    size_t last_move = me.moves_performed.empty() 
                       ? 0 
                       : me.moves_performed.back();
    int max_predicted_dmg = 0;
    for( ; which_move < me.moves.size(); ++which_move) {
        game::Move const &m = me.moves[which_move];
        if(m.isWait()) {
            continue;
        }
        if((!me.moves[last_move].isWait()) && which_move == last_move) {
            continue;
        }
        if(m.apCost(far, false) > me.cur_ap) {
            continue;
        }
        if(m.isSuper() && me.cur_sp < 4) {
            continue;
        }
        int dmg = m.damage(1, me.at, me.df, 0);
        if(dmg > max_predicted_dmg) {
            this_move = which_move;
            max_predicted_dmg = dmg;
        }
    }
    return me.moves[this_move];
}

bool GruntAtk::shouldSpendSPToLowerAPCost(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move,
                                int current_ap_cost) {
    return (my_move.apCost(far, false) == (me.cur_ap + 1)) && me.cur_sp > 1;
}

bool GruntAtk::shouldSpendSPToConcatenate(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return me.cur_sp > 1;
}

bool GruntAtk::shouldSpendSPForUltraAgility(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &my_move) {
    return true;
}

bool GruntAtk::shouldSpendSPToBoostAttack(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return true;
}

bool GruntAtk::shouldSpendSPToBoostDamage(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return true;
}


static constexpr int const BRAWLER_AP_TO_ATTACK = 6;

BrawlerAtk::~BrawlerAtk() {}

char const * const BrawlerAtk::getName() const { return "Brawler"; }

int BrawlerAtk::shouldSpendAPToGainSP(game::Character const &me,
                          game::Character const &opponent,
                          bool far) {
    // Gain one SP only, and only if we are not going to do any move this turn.
    if(me.cur_ap < BRAWLER_AP_TO_ATTACK) {
        return 1;
    }
    return 0;
}

game::Move const & BrawlerAtk::getNextMove(game::Character const &me,
                               game::Character const &opponent,
                               bool far) {
    // Only attack when we had a large number of AP to use.
    int ap_discriminator = me.cur_ap + 1;
    for(auto const &mv: me.moves_performed) {
        ap_discriminator += me.moves[mv].apCost(far, false);
    }
    if(ap_discriminator < BRAWLER_AP_TO_ATTACK) {
        return game::getWaitMove();
    }
    size_t which_move = 0;
    size_t this_move = 0;
    size_t last_move = me.moves_performed.empty() 
                       ? 0 
                       : me.moves_performed.back();
    float goodness = 0.0;
    for( ; which_move < me.moves.size(); ++which_move) {
        game::Move const &m = me.moves[which_move];
        if(m.isWait()) {
            continue;
        }
        if((!me.moves[last_move].isWait()) && which_move == last_move) {
            continue;
        }
        if(m.apCost(far, false) > me.cur_ap) {
            continue;
        }
        if(m.isSuper() && me.cur_sp < 4) {
            continue;
        }
        int dmg = m.damage(1, me.at, me.df, 0);
        int combo = m.comboPoints();
        float cur_goodness = dmg / (float)(m.apCost(far, false)) + (float)combo;
        if(m.hasThrow() && me.moves_performed.empty()) {
            // Skew towards throw on the first move
            cur_goodness += 5.0f;
        }
        if(goodness < cur_goodness) {
            this_move = which_move;
            goodness = cur_goodness;
        }
    }
    return me.moves[this_move];
}

bool BrawlerAtk::shouldSpendSPToLowerAPCost(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move,
                                int current_ap_cost) {
    return (my_move.apCost(far, false) == (me.cur_ap + 1)) && me.cur_sp > 1;
}

bool BrawlerAtk::shouldSpendSPToConcatenate(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return true;
}

bool BrawlerAtk::shouldSpendSPForUltraAgility(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &my_move) {
    return me.cur_sp > 1;
}

bool BrawlerAtk::shouldSpendSPToBoostAttack(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return me.cur_sp > 1;
}

bool BrawlerAtk::shouldSpendSPToBoostDamage(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return me.cur_sp > 1;
}

BufferAtk::~BufferAtk() {}

char const * const BufferAtk::getName() const { return "Buffer"; }

int BufferAtk::shouldSpendAPToGainSP(game::Character const &me,
                          game::Character const &opponent,
                          bool far) {
    // Gain SPs but save enough APs for a special move.
    if(me.cur_ap >= 4) {
        return std::min(me.cur_ap - 3, 6 - me.cur_sp);
    }
    return 0;
}

game::Move const & BufferAtk::getNextMove(game::Character const &me,
                               game::Character const &opponent,
                               bool far) {
    // Only attack with one move per turn, but give it all.
    if(!me.moves_performed.empty()) {
        return game::getWaitMove();
    }
    // If we gain SP by Defence or Wounds, attack only when we have more than 4
    // AP. This is so that we alternate attack rounds and defence rounds.
    if(me.sp == game::SP_DEFENCE || me.sp == game::SP_WOUND) {
        if(me.cur_ap <= 4) {
            return game::getWaitMove();
        }
    }
    size_t which_move = 0;
    size_t this_move = 0;
    size_t last_move = me.moves_performed.empty() 
                       ? 0 
                       : me.moves_performed.back();
    float goodness = 0.0;
    for( ; which_move < me.moves.size(); ++which_move) {
        game::Move const &m = me.moves[which_move];
        if(m.isWait()) {
            continue;
        }
        if((!me.moves[last_move].isWait()) && which_move == last_move) {
            continue;
        }
        if(m.apCost(far, false) > me.cur_ap) {
            continue;
        }
        if(m.isSuper() && me.cur_sp < 4) {
            continue;
        }
        int dmg = m.damage(1, me.at, me.df, 0);
        int combo = m.comboPoints();
        float potential_sp = 0.0f;
        if(me.sp == game::SP_DAMAGE) {
            potential_sp = (float)dmg / 3.0f;
        }
        else if(me.sp == game::SP_COMBO) {
            potential_sp = (float)combo / 2.0;
        }
        float cur_goodness = dmg / (float)(m.apCost(far, false)) + potential_sp;
        if(goodness < cur_goodness) {
            this_move = which_move;
            goodness = cur_goodness;
        }
    }
    return me.moves[this_move];
}

bool BufferAtk::shouldSpendSPToLowerAPCost(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move,
                                int current_ap_cost) {
    return (my_move.apCost(far, false) == (me.cur_ap + 1)) && me.cur_sp > 1;
}

bool BufferAtk::shouldSpendSPToConcatenate(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return false;
}

bool BufferAtk::shouldSpendSPForUltraAgility(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &my_move) {
    return me.cur_sp > 1;
}

bool BufferAtk::shouldSpendSPToBoostAttack(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return true;
}

bool BufferAtk::shouldSpendSPToBoostDamage(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return me.cur_sp > 1;
}

CounterAttackerAtk::~CounterAttackerAtk() {}

char const * const CounterAttackerAtk::getName() const { return "Counter"; }

int CounterAttackerAtk::shouldSpendAPToGainSP(game::Character const &me,
                          game::Character const &opponent,
                          bool far) {
    if(me.cur_ap > 4) {
        return std::min(me.cur_ap - 4, 6 - me.cur_sp);
    }
    return 0;
}

game::Move const & CounterAttackerAtk::getNextMove(game::Character const &me,
                               game::Character const &opponent,
                               bool far) {
    // ALWAYS keep 4 AP to counter-attack.
    if(me.cur_ap <= 4) {
        return game::getWaitMove();
    }
    // Otherwise just try to do some damage
    size_t which_move = 0;
    size_t this_move = 0;
    size_t last_move = me.moves_performed.empty() 
                       ? 0 
                       : me.moves_performed.back();
    int max_predicted_dmg = 0;
    for( ; which_move < me.moves.size(); ++which_move) {
        game::Move const &m = me.moves[which_move];
        if(m.isWait()) {
            continue;
        }
        if((!me.moves[last_move].isWait()) && which_move == last_move) {
            continue;
        }
        if(m.apCost(far, false) > me.cur_ap) {
            continue;
        }
        if(m.isSuper() && me.cur_sp < 4) {
            continue;
        }
        int dmg = m.damage(1, me.at, me.df, 0);
        if(dmg > max_predicted_dmg) {
            this_move = which_move;
            max_predicted_dmg = dmg;
        }
    }
    return me.moves[this_move];
}

KillerChainAtk::~KillerChainAtk() {}

char const * const KillerChainAtk::getName() const { return "KillerChain"; }

int KillerChainAtk::shouldSpendAPToGainSP(game::Character const &me,
                          game::Character const &opponent,
                          bool far) {
    // Power up while waiting.
    if(me.cur_ap < 7) {
        return std::min(std::max(1, me.ra - 2), 6 - me.cur_sp);
    }
    return 0;
}

game::Move const & KillerChainAtk::getNextMove(game::Character const &me,
                               game::Character const &opponent,
                               bool far) {
    // Find the Super.
    size_t super_move_idx = 0;
    for(; super_move_idx < me.moves.size(); ++super_move_idx) {
        if(me.moves[super_move_idx].isSuper()) {
            break;
        }
    }
    assert(super_move_idx < me.moves.size());
    // If we've already done the super, just bail out.
    if(!me.moves_performed.empty() 
       && me.moves_performed.back() == super_move_idx) {
        return game::getWaitMove();
    }
    // Otherwise if we can do the super, let's do it.
    if(me.cur_ap >= me.moves[super_move_idx].apCost(far, false)) {
        if(me.cur_sp >= 4) {
            return me.moves[super_move_idx];
        }
    }
    // Otherwise, we must be able to do a chain of three moves.
    // This means we had a lot of AP. This logic is similar to the Brawler but
    // our limit is 7 because it's two three-AP moves plus a basic attack.
    int ap_discriminator = me.cur_ap + 1;
    for(auto const &mv: me.moves_performed) {
        ap_discriminator += me.moves[mv].apCost(far, false);
    }
    if(ap_discriminator < 7) {
        return game::getWaitMove();
    }
    // At this point just get the strongest move.
    size_t which_move = 0;
    size_t this_move = 0;
    size_t last_move = me.moves_performed.empty() 
                       ? 0 
                       : me.moves_performed.back();
    int max_predicted_dmg = 0;
    for( ; which_move < me.moves.size(); ++which_move) {
        game::Move const &m = me.moves[which_move];
        if(m.isWait()) {
            continue;
        }
        if((!me.moves[last_move].isWait()) && which_move == last_move) {
            continue;
        }
        if(m.apCost(far, false) > me.cur_ap) {
            continue;
        }
        if(m.isSuper() && me.cur_sp < 4) {
            continue;
        }
        // Assume we're spending one SP to power it.
        int dmg = m.damage(1, me.at, me.df, 1);
        if(dmg > max_predicted_dmg) {
            this_move = which_move;
            max_predicted_dmg = dmg;
        }
    }
    return me.moves[this_move];
}

bool KillerChainAtk::shouldSpendSPToLowerAPCost(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move,
                                int current_ap_cost) {
    // Only spend one.
    return current_ap_cost >= my_move.apCost(far, false);
}

bool KillerChainAtk::shouldSpendSPToConcatenate(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return true;
}

bool KillerChainAtk::shouldSpendSPForUltraAgility(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &my_move) {
    return true;
}

bool KillerChainAtk::shouldSpendSPToBoostAttack(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return true;
}

bool KillerChainAtk::shouldSpendSPToBoostDamage(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return true;
}

KiteSniperAtk::~KiteSniperAtk() {}

char const * const KiteSniperAtk::getName() const { return "Kite/Sniper"; }

int KiteSniperAtk::shouldSpendAPToGainSP(game::Character const &me,
                          game::Character const &opponent,
                          bool far) {
    if(me.cur_ap > 4) {
        return me.cur_ap - 4;
    }
    return 0;
}

game::Move const & KiteSniperAtk::getNextMove(game::Character const &me,
                               game::Character const &opponent,
                               bool far) {
    // Verify that we have moves with PUSH or DISTANCE
    bool has_push = false;
    bool has_distance = false;
    for(auto const &m : me.moves) {
        if(m.hasSymbol(game::MS_PUSH)) {
            has_push = true;
        } else if(m.hasSymbol(game::MS_DISTANCE)) {
            has_distance = true;
        }
    }
    // Just get the strongest move, but prefer the ones with MS_PUSH.
    size_t which_move = 0;
    size_t this_move = 0;
    size_t last_move = me.moves_performed.empty() 
                       ? 0 
                       : me.moves_performed.back();
    float max_predicted_dmg = 0.0f;
    for( ; which_move < me.moves.size(); ++which_move) {
        game::Move const &m = me.moves[which_move];
        if(m.isWait()) {
            continue;
        }
        if((!me.moves[last_move].isWait()) && which_move == last_move) {
            continue;
        }
        if(m.apCost(far, false) > me.cur_ap) {
            continue;
        }
        if(m.isSuper() && me.cur_sp < 4) {
            continue;
        }
        // If we have both PUSH and DISTANCE moves, then stay afar and don't do
        // any other move.
        if(has_push && has_distance) {
            if(!m.hasSymbol(game::MS_PUSH) || !m.hasSymbol(game::MS_DISTANCE)) {
                continue;
            }
        }
        float dmg = (float)m.damage(1, me.at, me.df, 0);
        // Skew towards PUSH if we are close.
        if(!far && m.hasSymbol(game::MS_PUSH)) {
            dmg *= 2.0f;
        }
        // Maximize damage per AP.
        dmg /= (float)m.apCost(far, false);
        if(dmg > max_predicted_dmg) {
            this_move = which_move;
            max_predicted_dmg = dmg;
        }
    }
    return me.moves[this_move];
}

bool KiteSniperAtk::shouldSpendSPToLowerAPCost(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move,
                                int current_ap_cost) {
    // Only spend one.
    return current_ap_cost >= my_move.apCost(far, false);
}

bool KiteSniperAtk::shouldSpendSPToConcatenate(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return true;
}

bool KiteSniperAtk::shouldSpendSPForUltraAgility(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &my_move) {
    return me.cur_sp > 1;
}

bool KiteSniperAtk::shouldSpendSPToBoostAttack(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return me.cur_sp > 1;
}

bool KiteSniperAtk::shouldSpendSPToBoostDamage(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return me.cur_sp > 1;
}

BalancedAtk::~BalancedAtk() {}

char const * const BalancedAtk::getName() const { return "Balanced"; }

int BalancedAtk::shouldSpendAPToGainSP(game::Character const &me,
                          game::Character const &opponent,
                          bool far) {
    // Gain SPs but save enough APs for a special move.
    if(me.cur_ap >= 4) {
        return std::min(me.cur_ap - 3, 6 - me.cur_sp);
    }
    return 0;
}

game::Move const & BalancedAtk::getNextMove(game::Character const &me,
                               game::Character const &opponent,
                               bool far) {
    // Only attack with one move per turn.
    if(!me.moves_performed.empty()) {
        return game::getWaitMove();
    }
    size_t which_move = 0;
    size_t this_move = 0;
    size_t last_move = me.moves_performed.empty() 
                       ? 0 
                       : me.moves_performed.back();
    float goodness = 0.0;
    for( ; which_move < me.moves.size(); ++which_move) {
        game::Move const &m = me.moves[which_move];
        if(m.isWait()) {
            continue;
        }
        if((!me.moves[last_move].isWait()) && which_move == last_move) {
            continue;
        }
        // Keep 2 APs for the counterattack.
        if(m.apCost(far, false) > me.cur_ap + 2) {
            continue;
        }
        if(m.isSuper() && me.cur_sp < 4) {
            continue;
        }
        int dmg = m.damage(1, me.at, me.df, 0);
        int combo = m.comboPoints();
        float potential_sp = 0.0f;
        if(me.sp == game::SP_DAMAGE) {
            potential_sp = (float)dmg / 3.0f;
        }
        else if(me.sp == game::SP_COMBO) {
            potential_sp = (float)combo / 2.0f;
        }
        float cur_goodness = dmg + potential_sp * 3.0f;
        if(goodness < cur_goodness) {
            this_move = which_move;
            goodness = cur_goodness;
        }
    }
    return me.moves[this_move];
}

bool BalancedAtk::shouldSpendSPToLowerAPCost(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move,
                                int current_ap_cost) {
    return (my_move.apCost(far, false) == (me.cur_ap + 1)) && me.cur_sp > 1;
}

bool BalancedAtk::shouldSpendSPToConcatenate(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return false;
}

bool BalancedAtk::shouldSpendSPForUltraAgility(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &my_move) {
    return me.cur_sp > 1;
}

bool BalancedAtk::shouldSpendSPToBoostAttack(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return me.cur_sp > 1;
}

bool BalancedAtk::shouldSpendSPToBoostDamage(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return me.cur_sp > 1;
}

TacticalAtk::~TacticalAtk() {}

char const * const TacticalAtk::getName() const { return "Tactical"; }

int TacticalAtk::shouldSpendAPToGainSP(game::Character const &me,
                          game::Character const &opponent,
                          bool far) {
    // Gain SPs but save enough APs for a special move.
    if(me.cur_ap > 4) {
        return std::min(me.cur_ap - 4, 6 - me.cur_sp);
    }
    return 0;
}

game::Move const & TacticalAtk::getNextMove(game::Character const &me,
                               game::Character const &opponent,
                               bool far) {
    // If our defence is better than the opponent's attack, then prefer counter
    // attacking (i.e. save 4 AP), otherwise proceed.
    if(me.df > opponent.at) {
        if(me.cur_ap <= 4) {
            return game::getWaitMove();
        }
    }
    size_t which_move = 0;
    size_t this_move = 0;
    size_t last_move = me.moves_performed.empty() 
                       ? 0 
                       : me.moves_performed.back();
    float goodness = 0.0;
    for( ; which_move < me.moves.size(); ++which_move) {
        game::Move const &m = me.moves[which_move];
        if(m.isWait()) {
            continue;
        }
        if((!me.moves[last_move].isWait()) && which_move == last_move) {
            continue;
        }
        if(m.apCost(far, false) > me.cur_ap) {
            continue;
        }
        if(m.isSuper() && me.cur_sp < 4) {
            continue;
        }
        int dmg = m.damage(1, me.at, me.df, 0);
        int combo = m.comboPoints();
        float potential_sp = 0.0f;
        if(me.sp == game::SP_DAMAGE) {
            potential_sp = (float)dmg / 3.0f;
        }
        else if(me.sp == game::SP_COMBO) {
            potential_sp = (float)combo / 2.0f;
        }
        float cur_goodness = dmg + potential_sp * 3.0f;
        if(goodness < cur_goodness) {
            this_move = which_move;
            goodness = cur_goodness;
        }
    }
    return me.moves[this_move];
}

bool TacticalAtk::shouldSpendSPToLowerAPCost(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move,
                                int current_ap_cost) {
    return (my_move.apCost(far, false) == (me.cur_ap + 1)) && me.cur_sp > 1;
}

bool TacticalAtk::shouldSpendSPToConcatenate(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return false;
}

bool TacticalAtk::shouldSpendSPForUltraAgility(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &my_move) {
    return false;
}

bool TacticalAtk::shouldSpendSPToBoostAttack(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return false;
}

bool TacticalAtk::shouldSpendSPToBoostDamage(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return true;
}



// ========================================================================
//    DEFENCE EXPERT SYSTEMS
// ========================================================================



GruntDef::~GruntDef() {}

char const * const GruntDef::getName() const { return "Grunt"; }

game::Move const & GruntDef::getCounterMove(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &opponent_move) {
    size_t which_move = 0;
    size_t this_move = 0;
    int max_predicted_dmg = 0;
    for( ; which_move < me.moves.size(); ++which_move) {
        game::Move const &m = me.moves[which_move];
        if(m.isWait()) {
            continue;
        }
        if(m.apCost(far, true) > me.cur_ap) {
            continue;
        }
        if(m.isSuper() && me.cur_sp < 4) {
            continue;
        }
        int dmg = m.counterDamage(1, me.at, me.df, 0, 
            opponent_move.damage(1, opponent.at, opponent.df, 0));
        if(dmg > max_predicted_dmg) {
            this_move = which_move;
            max_predicted_dmg = dmg;
        }
    }
    return me.moves[this_move];
}

BrawlerDef::~BrawlerDef() {}

char const * const BrawlerDef::getName() const { return "Brawler"; }

game::Move const & BrawlerDef::getCounterMove(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &opponent_move) {
    // Only counter when we have a large number of AP to use.
    if(me.cur_ap < BRAWLER_AP_TO_ATTACK) {
        return game::getWaitMove();
    }
    size_t which_move = 0;
    size_t this_move = 0;
    float goodness = 0.0f;
    for( ; which_move < me.moves.size(); ++which_move) {
        game::Move const &m = me.moves[which_move];
        if(m.isWait()) {
            continue;
        }
        if(m.apCost(far, true) > me.cur_ap) {
            continue;
        }
        if(m.isSuper() && me.cur_sp < 4) {
            continue;
        }
        int dmg = m.counterDamage(1, me.at, me.df, 0, 
            opponent_move.damage(1, opponent.at, opponent.df, 0));
        int combo = m.comboPoints();
        float cur_goodness = dmg / (float)(m.apCost(far, true)) + (float)combo;
        if(m.hasThrow() && me.moves_performed.empty()) {
            // Skew towards throw on the counter move
            cur_goodness += 5.0f;
        }
        if(goodness < cur_goodness) {
            this_move = which_move;
            goodness = cur_goodness;
        }
    }
    return me.moves[this_move];
}

BufferDef::~BufferDef() {}

char const * const BufferDef::getName() const { return "Buffer"; }

game::Move const & BufferDef::getCounterMove(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &opponent_move) {
    // Only counter when we have enough AP to use and we can spend SP.
    if(me.cur_ap <= 4 || me.cur_sp < 2) {
        return game::getWaitMove();
    }
    size_t which_move = 0;
    size_t this_move = 0;
    float goodness = 0.0f;
    for( ; which_move < me.moves.size(); ++which_move) {
        game::Move const &m = me.moves[which_move];
        if(m.isWait()) {
            continue;
        }
        if(m.apCost(far, true) > me.cur_ap) {
            continue;
        }
        if(m.isSuper() && me.cur_sp < 4) {
            continue;
        }
        int dmg = m.counterDamage(1, me.at, me.df, 0, 
            opponent_move.damage(1, opponent.at, opponent.df, 0));
        int combo = m.comboPoints();
        float potential_sp = 0.0f;
        if(me.sp == game::SP_DAMAGE) {
            potential_sp = (float)dmg / 3.0f;
        }
        else if(me.sp == game::SP_COMBO) {
            potential_sp = (float)combo / 2.0;
        }
        float cur_goodness = dmg / (float)(m.apCost(far, false)) + potential_sp;
        if(goodness < cur_goodness) {
            this_move = which_move;
            goodness = cur_goodness;
        }
    }
    return me.moves[this_move];
}

bool BufferDef::shouldSpendSPToLowerAPCost(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move,
                                int current_ap_cost) {
    return me.cur_sp > 1 && current_ap_cost >= 3;
}

bool BufferDef::shouldSpendSPForUltraAgility(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &my_move) {
    return true;
}

bool BufferDef::shouldSpendSPToBoostDefence(game::Character const &me,
                                 game::Character const &opponent,
                                 bool far,
                                 game::Move const &my_move) {
    return true;
}

bool BufferDef::shouldSpendSPToBoostCounterDamage(game::Character const &me,
                                       game::Character const &opponent,
                                       bool far,
                                       game::Move const &my_move) {
    return me.cur_sp > 1;
}

CounterAttackerDef::~CounterAttackerDef() {}

char const * const CounterAttackerDef::getName() const { return "Counter"; }

game::Move const & CounterAttackerDef::getCounterMove(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &opponent_move) {
    size_t which_move = 0;
    size_t this_move = 0;
    int max_predicted_dmg = 0;
    for( ; which_move < me.moves.size(); ++which_move) {
        game::Move const &m = me.moves[which_move];
        if(m.isWait()) {
            continue;
        }
        if(m.apCost(far, true) > me.cur_ap) {
            continue;
        }
        if(m.isSuper() && me.cur_sp < 4) {
            continue;
        }
        int dmg = m.counterDamage(1, me.at, me.df, 0, 
            opponent_move.damage(1, opponent.at, opponent.df, 0));
        if(dmg > max_predicted_dmg) {
            this_move = which_move;
            max_predicted_dmg = dmg;
        }
    }
    return me.moves[this_move];
}

bool CounterAttackerDef::shouldSpendSPToComboBreak(game::Character const &me,
                               game::Character const &opponent,
                               bool far,
                               game::Move const &opponent_move) {
    // ALWAYS break.
    return true;
}

bool CounterAttackerDef::shouldSpendSPToLowerAPCost(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move,
                                int current_ap_cost) {
    // Only spend one.
    return current_ap_cost >= my_move.apCost(far, true);
}

bool CounterAttackerDef::shouldSpendSPForUltraAgility(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &my_move) {
    return true;
}

bool CounterAttackerDef::shouldSpendSPToBoostDefence(game::Character const &me,
                                 game::Character const &opponent,
                                 bool far,
                                 game::Move const &my_move) {
    return true;
}

bool CounterAttackerDef::shouldSpendSPToBoostCounterDamage(game::Character const &me,
                                       game::Character const &opponent,
                                       bool far,
                                       game::Move const &my_move) {
    return true;
}


KillerChainDef::~KillerChainDef() {}

char const * const KillerChainDef::getName() const { return "KillerChain"; }

game::Move const & KillerChainDef::getCounterMove(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &opponent_move) {
    // Never counterattack, unless we have a lot of APs and we can start the
    // chain.
    if(me.cur_ap >= 7) {
        size_t which_move = 0;
        size_t this_move = 0;
        int max_predicted_dmg = 0;
        for( ; which_move < me.moves.size(); ++which_move) {
            game::Move const &m = me.moves[which_move];
            if(m.isWait()) {
                continue;
            }
            if(m.apCost(far, true) > me.cur_ap) {
                continue;
            }
            if(m.isSuper() && me.cur_sp < 4) {
                continue;
            }
            int dmg = m.counterDamage(1, me.at, me.df, 0, 
                opponent_move.damage(1, opponent.at, opponent.df, 0));
            if(dmg > max_predicted_dmg) {
                this_move = which_move;
                max_predicted_dmg = dmg;
            }
        }
        return me.moves[this_move];
    }
    return game::getWaitMove();
}

KiteDef::~KiteDef() {}

char const * const KiteDef::getName() const { return "Kite"; }

game::Move const & KiteDef::getCounterMove(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &opponent_move) {
    size_t which_move = 0;
    size_t this_move = 0;
    int max_predicted_dmg = 0;
    for( ; which_move < me.moves.size(); ++which_move) {
        game::Move const &m = me.moves[which_move];
        if(m.isWait()) {
            continue;
        }
        // ONLY counter with a PUSH move.
        if(!m.hasSymbol(game::MS_PUSH)) {
            continue;
        }
        if(m.apCost(far, true) > me.cur_ap) {
            continue;
        }
        if(m.isSuper() && me.cur_sp < 4) {
            continue;
        }
        int dmg = m.damage(1, me.at, me.df, 0);
        if(dmg > max_predicted_dmg) {
            this_move = which_move;
            max_predicted_dmg = dmg;
        }
    }
    return me.moves[this_move];
}

bool KiteDef::shouldSpendSPToLowerAPCost(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move,
                                int current_ap_cost) {
    // Only do this for PUSH moves, and only spend one.
    return my_move.hasSymbol(game::MS_PUSH) 
           && current_ap_cost >= my_move.apCost(far, true);
}

bool KiteDef::shouldSpendSPForUltraAgility(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &my_move) {
    return my_move.hasSymbol(game::MS_PUSH);
}

bool KiteDef::shouldSpendSPToBoostDefence(game::Character const &me,
                                 game::Character const &opponent,
                                 bool far,
                                 game::Move const &my_move) {
    return my_move.hasSymbol(game::MS_PUSH);
}

bool KiteDef::shouldSpendSPToBoostCounterDamage(game::Character const &me,
                                       game::Character const &opponent,
                                       bool far,
                                       game::Move const &my_move) {
    return my_move.hasSymbol(game::MS_PUSH);
}

SniperDef::~SniperDef() {}

char const * const SniperDef::getName() const { return "Sniper"; }

game::Move const & SniperDef::getCounterMove(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &opponent_move) {
    size_t which_move = 0;
    size_t this_move = 0;
    int max_predicted_dmg = 0;
    for( ; which_move < me.moves.size(); ++which_move) {
        game::Move const &m = me.moves[which_move];
        if(m.isWait()) {
            continue;
        }
        // ONLY counter from afar and with a DISTANCE move.
        if(!far || !m.hasSymbol(game::MS_DISTANCE)) {
            continue;
        }
        if(m.apCost(far, true) > me.cur_ap) {
            continue;
        }
        if(m.isSuper() && me.cur_sp < 4) {
            continue;
        }
        int dmg = m.damage(1, me.at, me.df, 0);
        if(dmg > max_predicted_dmg) {
            this_move = which_move;
            max_predicted_dmg = dmg;
        }
    }
    return me.moves[this_move];
}

bool SniperDef::shouldSpendSPToLowerAPCost(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move,
                                int current_ap_cost) {
    // Only do this for DISTANCE moves, and only spend one.
    return my_move.hasSymbol(game::MS_DISTANCE) 
           && current_ap_cost >= my_move.apCost(far, true);
}

bool SniperDef::shouldSpendSPForUltraAgility(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &my_move) {
    return my_move.hasSymbol(game::MS_DISTANCE);
}

bool SniperDef::shouldSpendSPToBoostDefence(game::Character const &me,
                                 game::Character const &opponent,
                                 bool far,
                                 game::Move const &my_move) {
    return my_move.hasSymbol(game::MS_DISTANCE);
}

bool SniperDef::shouldSpendSPToBoostCounterDamage(game::Character const &me,
                                       game::Character const &opponent,
                                       bool far,
                                       game::Move const &my_move) {
    return my_move.hasSymbol(game::MS_DISTANCE);
}

BalancedDef::~BalancedDef() {}

char const * const BalancedDef::getName() const { return "Balanced"; }

game::Move const & BalancedDef::getCounterMove(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &opponent_move) {
    size_t which_move = 0;
    size_t this_move = 0;
    float goodness = 0.0f;
    for( ; which_move < me.moves.size(); ++which_move) {
        game::Move const &m = me.moves[which_move];
        if(m.isWait()) {
            continue;
        }
        if(m.apCost(far, true) > me.cur_ap) {
            continue;
        }
        if(m.isSuper() && me.cur_sp < 4) {
            continue;
        }
        int dmg = m.counterDamage(1, me.at, me.df, 0, 
            opponent_move.damage(1, opponent.at, opponent.df, 0));
        int combo = m.comboPoints();
        float potential_sp = 0.0f;
        if(me.sp == game::SP_DAMAGE) {
            potential_sp = (float)dmg / 3.0f;
        }
        else if(me.sp == game::SP_COMBO) {
            potential_sp = (float)combo / 2.0;
        }
        float cur_goodness = dmg + potential_sp * 3.0f;
        if(goodness < cur_goodness) {
            this_move = which_move;
            goodness = cur_goodness;
        }
    }
    return me.moves[this_move];
}

bool BalancedDef::shouldSpendSPToLowerAPCost(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move,
                                int current_ap_cost) {
    return true;
}

bool BalancedDef::shouldSpendSPForUltraAgility(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &my_move) {
    return me.cur_sp > 1;
}

bool BalancedDef::shouldSpendSPToBoostDefence(game::Character const &me,
                                 game::Character const &opponent,
                                 bool far,
                                 game::Move const &my_move) {
    return true;
}

bool BalancedDef::shouldSpendSPToBoostCounterDamage(game::Character const &me,
                                       game::Character const &opponent,
                                       bool far,
                                       game::Move const &my_move) {
    return me.cur_sp > 1;
}

TacticalDef::~TacticalDef() {}

char const * const TacticalDef::getName() const { return "Tactical"; }

game::Move const & TacticalDef::getCounterMove(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &opponent_move) {
    size_t which_move = 0;
    size_t this_move = 0;
    float goodness = 0.0f;
    for( ; which_move < me.moves.size(); ++which_move) {
        game::Move const &m = me.moves[which_move];
        if(m.isWait()) {
            continue;
        }
        if(m.apCost(far, true) > me.cur_ap) {
            continue;
        }
        if(m.isSuper() && me.cur_sp < 4) {
            continue;
        }
        int dmg = m.counterDamage(1, me.at, me.df, 0, 
            opponent_move.damage(1, opponent.at, opponent.df, 0));
        int combo = m.comboPoints();
        float potential_sp = 0.0f;
        if(me.sp == game::SP_DAMAGE) {
            potential_sp = (float)dmg / 3.0f;
        }
        else if(me.sp == game::SP_COMBO) {
            potential_sp = (float)combo / 2.0;
        }
        float cur_goodness = dmg + potential_sp * 3.0f;
        if(goodness < cur_goodness) {
            this_move = which_move;
            goodness = cur_goodness;
        }
    }
    return me.moves[this_move];
}

bool TacticalDef::shouldSpendSPToLowerAPCost(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move,
                                int current_ap_cost) {
    return true;
}

bool TacticalDef::shouldSpendSPToComboBreak(game::Character const &me,
                               game::Character const &opponent,
                               bool far,
                               game::Move const &opponent_move) {
    // ALWAYS break.
    return true;
}

bool TacticalDef::shouldSpendSPForUltraAgility(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &my_move) {
    return false;
}

bool TacticalDef::shouldSpendSPToBoostDefence(game::Character const &me,
                                 game::Character const &opponent,
                                 bool far,
                                 game::Move const &my_move) {
    return false;
}

bool TacticalDef::shouldSpendSPToBoostCounterDamage(game::Character const &me,
                                       game::Character const &opponent,
                                       bool far,
                                       game::Move const &my_move) {
    return true;
}



// ========================================================================
//    UTILITY FUNCTIONS
// ========================================================================



int getNumExpertSystemsCombinations() {
    return 9;
}

std::shared_ptr<AttackControl> getAttackExpertSystem(int combination) {
    switch(combination) {
        case 0: return std::make_shared<GruntAtk>();
        case 1: return std::make_shared<BrawlerAtk>();
        case 2: return std::make_shared<BufferAtk>();
        case 3: return std::make_shared<CounterAttackerAtk>();
        case 4: return std::make_shared<KillerChainAtk>();
        case 5: // Fall-through
        case 6: return std::make_shared<KiteSniperAtk>();
        case 7: return std::make_shared<BalancedAtk>();
        case 8: return std::make_shared<TacticalAtk>();
        default: break;
    }
    return std::make_shared<DumbAttackControl>();
}

std::shared_ptr<DefendControl> getDefenceExpertSystem(int combination) {
    switch(combination) {
        case 0: return std::make_shared<GruntDef>();
        case 1: return std::make_shared<BrawlerDef>();
        case 2: return std::make_shared<BufferDef>();
        case 3: return std::make_shared<CounterAttackerDef>();
        case 4: return std::make_shared<KillerChainDef>();
        case 5: return std::make_shared<KiteDef>();
        case 6: return std::make_shared<SniperDef>();
        case 7: return std::make_shared<BalancedDef>();
        case 8: return std::make_shared<TacticalDef>();
        default: break;
    }
    return std::make_shared<DumbDefendControl>();
}



}
}
