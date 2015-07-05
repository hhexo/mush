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

namespace core {
namespace ctrl {


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


static constexpr int const BRAWLER_AP_TO_ATTACK = 5;

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
        float cur_goodness = ((float)dmg / 3.0f + (float)combo) / (float)(m.apCost(far, false));
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
        int dmg = m.damage(1, me.at, me.df, 0);
        int combo = m.comboPoints();
        float cur_goodness = ((float)dmg / 3.0f + (float)combo) / (float)(m.apCost(far, true));
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



int getNumExpertSystemsCombinations() {
    return 2;
}

std::shared_ptr<AttackControl> getAttackExpertSystem(int combination) {
    switch(combination) {
        case 0: return std::make_shared<GruntAtk>();
        case 1: return std::make_shared<BrawlerAtk>();
        default: break;
    }
    return std::make_shared<DumbAttackControl>();
}

std::shared_ptr<DefendControl> getDefenceExpertSystem(int combination) {
    switch(combination) {
        case 0: return std::make_shared<GruntDef>();
        case 1: return std::make_shared<BrawlerDef>();
        default: break;
    }
    return std::make_shared<DumbDefendControl>();
}



}
}
