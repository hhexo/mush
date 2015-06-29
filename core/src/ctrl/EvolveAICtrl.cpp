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

#include "core/ctrl/EvolveAICtrl.h"
#include "core/game/Move.h"
#include "core/game/Character.h"
#include "core/game/Dice.h"

#include <array>
#include <cassert>
#include <iostream>

namespace core {
namespace ctrl {

using namespace core::game;

DecisionMatrix::DecisionMatrix() {
    for(auto &x : m_goodness) {
        x = 0.5;
    }
    m_learning_factor = 0.5;
}

double DecisionMatrix::getGoodness(bool down, bool in_air, int at_or_df,
                                   int own_ap, int own_sp, 
                                   int parameter) {
    // Saturate ap/sp
    if(own_ap >= MAX_OWN_AP_CASES) {
        own_ap = MAX_OWN_AP_CASES - 1;
    }
    if(own_sp >= MAX_OWN_SP_CASES) {
        own_sp = MAX_OWN_SP_CASES - 1;
    }
    // Assert on other saturation
    assert(at_or_df >= 0);
    assert(own_ap >= 0);
    assert(own_sp >= 0);
    assert(parameter >= 0);
    assert(at_or_df < MAX_AT_DF_CASES);
    assert(own_sp < MAX_OWN_SP_CASES);
    assert(parameter < MAX_PARAM_CASES);
    size_t idx = 0;
    if(down) {
        idx += MAX_AIR_CASES * MAX_AT_DF_CASES * MAX_OWN_AP_CASES * MAX_OWN_SP_CASES * MAX_PARAM_CASES;
    }
    if(in_air) {
        idx += MAX_AT_DF_CASES * MAX_OWN_AP_CASES * MAX_OWN_SP_CASES * MAX_PARAM_CASES;
    }
    idx += at_or_df * MAX_OWN_AP_CASES * MAX_OWN_SP_CASES * MAX_PARAM_CASES;
    idx += own_ap * MAX_OWN_SP_CASES * MAX_PARAM_CASES;
    idx += own_sp * MAX_PARAM_CASES;
    idx += parameter;
    assert(idx < DECISION_MATRIX_SIZE);
    return m_goodness[idx];
}

void DecisionMatrix::updateGoodness(bool down, bool in_air, int at_or_df,
                                    int own_ap, int own_sp, 
                                    int parameter, double good) {
    // Saturate ap/sp
    if(own_ap >= MAX_OWN_AP_CASES) {
        own_ap = MAX_OWN_AP_CASES - 1;
    }
    if(own_sp >= MAX_OWN_SP_CASES) {
        own_sp = MAX_OWN_SP_CASES - 1;
    }
    // Assert on other saturation
    assert(at_or_df >= 0);
    assert(own_ap >= 0);
    assert(own_sp >= 0);
    assert(parameter >= 0);
    assert(at_or_df < MAX_AT_DF_CASES);
    assert(own_sp < MAX_OWN_SP_CASES);
    assert(parameter < MAX_PARAM_CASES);
    size_t idx = 0;
    if(down) {
        idx += MAX_AIR_CASES * MAX_AT_DF_CASES * MAX_OWN_AP_CASES * MAX_OWN_SP_CASES * MAX_PARAM_CASES;
    }
    if(in_air) {
        idx += MAX_AT_DF_CASES * MAX_OWN_AP_CASES * MAX_OWN_SP_CASES * MAX_PARAM_CASES;
    }
    idx += at_or_df * MAX_OWN_AP_CASES * MAX_OWN_SP_CASES * MAX_PARAM_CASES;
    idx += own_ap * MAX_OWN_SP_CASES * MAX_PARAM_CASES;
    idx += own_sp * MAX_PARAM_CASES;
    idx += parameter;
    assert(idx < DECISION_MATRIX_SIZE);
    m_goodness[idx] = (m_goodness[idx] + good * (1.0 - m_learning_factor)) * 0.5;
    m_learning_factor = m_learning_factor * 0.9999;
}

void DecisionMatrix::dump(std::string const & with_name, 
                          std::vector<Move> const &with_moves) const {
    std::cout << "MATRIX '" << with_name << "', learning factor " 
              << m_learning_factor << ", interesting edge results:" 
              << std::endl;
    for(size_t down = 0; down < DecisionMatrix::MAX_DOWN_CASES; ++down) {
    for(size_t air = 0; air < DecisionMatrix::MAX_AIR_CASES; ++air) {
    for(size_t df = 0; df < DecisionMatrix::MAX_AT_DF_CASES; ++df) {
    for(size_t ap = 0; ap < DecisionMatrix::MAX_OWN_AP_CASES; ++ap) {
    for(size_t sp = 0; sp < DecisionMatrix::MAX_OWN_SP_CASES; ++sp) {
    for(size_t mv = 0; mv < with_moves.size(); ++mv) {
        size_t idx = 0;
        idx += down * MAX_DOWN_CASES * MAX_OWN_AP_CASES * MAX_OWN_SP_CASES * MAX_PARAM_CASES;
        idx += air * MAX_AT_DF_CASES * MAX_OWN_AP_CASES * MAX_OWN_SP_CASES * MAX_PARAM_CASES;
        idx += df * MAX_OWN_AP_CASES * MAX_OWN_SP_CASES * MAX_PARAM_CASES;
        idx += ap * MAX_OWN_SP_CASES * MAX_PARAM_CASES;
        idx += sp * MAX_PARAM_CASES;
        idx += mv;
        double good = m_goodness[idx];
        if(good > 0.99 || good < 0.01) {
            std::cout << "[ ";
            std::cout << ((down == 0) ? "nodown " : "  down ");
            std::cout << ((air == 0) ? "noair " : "  air ");
            std::cout << df;
            std::cout << "at/df ";
            std::cout << ap;
            std::cout << "ap ";
            std::cout << sp;
            std::cout << "sp '";
            std::cout << with_moves[mv].getName();
            std::cout << "' ] : ";
            std::cout << good;
            std::cout << std::endl;
        }
    }}}}}}
    std::cout << std::endl;
}

static bool shouldDoX(Character const &me, Character const &opponent, 
                      int parameter, 
                      DecisionMatrix &matrix, 
                      DecisionVector &to_update, 
                      bool check_sp = false,
                      bool attacking = true) {
    // Early exit.
    if(check_sp && me.cur_sp <= 0) {
        return false;
    }
    size_t move_idx = parameter;
    assert(move_idx < DecisionMatrix::MAX_PARAM_CASES);
    // Query matrix.
    double prob = matrix.getGoodness(opponent.down, opponent.air, 
                                     attacking ? opponent.df : opponent.at, 
                                     me.cur_ap, 
                                     me.cur_sp, 
                                     move_idx);
    // Roll probability.
    bool yes = false;
    {
        std::lock_guard<std::mutex> protect(getRandomGenMutex());
        std::uniform_real_distribution<double> zero_to_one(0.0, 1.0);
        yes = zero_to_one(getRandomGenerator()) < prob;
        if(yes) {
            DecisionRecord cur_decision{opponent.down, opponent.air, 
                                        attacking ? opponent.df : opponent.at, 
                                        me.cur_ap, 
                                        me.cur_sp, 
                                        move_idx, 
                                        1.0};
            to_update.push_back(cur_decision);
            return true;
        }
        DecisionRecord cur_decision{opponent.down, opponent.air, 
                                    attacking ? opponent.df : opponent.at, 
                                    me.cur_ap, 
                                    me.cur_sp, 
                                    move_idx, 
                                    0.0};
        to_update.push_back(cur_decision);
    }
    return false;
}

static void updateX(bool has_won, DecisionMatrix &matrix, 
                    DecisionVector &last_decisions) {
    while(!last_decisions.empty()) {
        auto &dr = last_decisions.back();
        if(has_won) {
            dr.goodness = (dr.goodness > 0.0) ? 1.0 : 0.0;
        } else {
            dr.goodness = (dr.goodness > 0.0) ? 0.0 : 1.0;
        }
        matrix.updateGoodness(dr.down, dr.air, dr.enemy_at_or_df, dr.own_ap,
                              dr.own_sp, dr.parameter, dr.goodness);
        last_decisions.pop_back();
    }
}

EvolveAIAttack::EvolveAIAttack() {}

EvolveAIAttack::~EvolveAIAttack() {}

int EvolveAIAttack::shouldSpendAPToGainSP(game::Character const &me,
                          game::Character const &opponent,
                          bool far) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    // Early exit if we already have max SP
    if(me.cur_sp >= 6) {
        return 0;
    }

    int i = 0;
    for(; i < DecisionMatrix::MAX_OWN_SP_CASES; ++i) {
        if(!shouldDoX(me, opponent, i, 
                      m_ap_to_sp_matrix, m_last_ap_to_sp_decisions[me.uid])) {
            break;
        }
    }
    return i;
}

bool EvolveAIAttack::shouldSpendAPToFallStanding(game::Character const &me,
                                 game::Character const &opponent,
                                 bool far) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    return shouldDoX(me, opponent, 0, 
                     m_fall_matrix, m_last_fall_decisions[me.uid]);
}

game::Move const & EvolveAIAttack::getNextMove(game::Character const &me,
                               game::Character const &opponent,
                               bool far) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    size_t which_move = 0;
    size_t last_move_index = me.moves_performed.empty() 
                             ? 0 
                             : me.moves_performed.back();
    bool at_least_one_move = false;
    assert(me.moves.size() < DecisionMatrix::MAX_PARAM_CASES);
    std::array<double, DecisionMatrix::MAX_PARAM_CASES> moves_goodness;
    for(auto &d : moves_goodness) {
        d = 0.000001;
    }

    for( ; which_move < me.moves.size(); ++which_move) {
        Move const &m = me.moves[which_move];
        // Don't consider last move (unless it was a wait).
        if(!m.isWait() && last_move_index == which_move) {
            continue;
        }
        // Don't consider a MS_REFLECT move in attack.
        if(m.hasReflect()) {
            continue;
        }
        // Compute AP cost.
        int ap_cost = m.apCost(far, false);
        // If it costs too much, don't consider it.
        if(ap_cost >= me.cur_ap) {
            continue;
        }
        // If it's a super, don't consider it if we don't have the SP.
        if(m.isSuper() && me.cur_sp < 4) {
            continue;
        }
        // If it's a distance move, don't consider it if we're close up...
        // ... unless it's a super, in which case yeah, why not.
        if(!far && !m.isSuper() && m.hasSymbol(MS_DISTANCE)) {
            continue;
        }
        at_least_one_move = true;
        double cur_goodness = m_move_matrix.getGoodness(
            opponent.down, opponent.air,
            opponent.df, me.cur_ap, me.cur_sp, 
            which_move);
        assert(which_move < me.moves.size());
        moves_goodness[which_move] = cur_goodness;
    }

    // Now choose the move based on probabilities
    if(!at_least_one_move) {
        // Just wait
        which_move = 0; // Assumes me.moves[0] is the wait move!
    }
    else {
        std::lock_guard<std::mutex> protect_random(getRandomGenMutex());
        std::discrete_distribution<int> weighted(moves_goodness.cbegin(), 
                                                 moves_goodness.cend());
        which_move = weighted(getRandomGenerator());
        if(which_move >= me.moves.size()) {
            which_move = 0;
        }
    }

    // Now commit to the move and store it in the current match list.
    DecisionRecord cur_decision{opponent.down, opponent.air, opponent.df, 
                                me.cur_ap, me.cur_sp, which_move, 1.0};
    m_last_move_decisions[me.uid].push_back(cur_decision);

    assert(which_move < me.moves.size());
    return me.moves[which_move];
}

bool EvolveAIAttack::shouldSpendSPToLowerAPCost(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move,
                                int current_ap_cost) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    for(int i = 0; i < me.moves.size(); ++i) {
        if(me.moves[i] == my_move) {
            return shouldDoX(me, opponent, i, 
                             m_lower_matrix, m_last_lower_decisions[me.uid], true);
        }
    }
    return false;
}

bool EvolveAIAttack::shouldSpendSPToConcatenate(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    for(int i = 0; i < me.moves.size(); ++i) {
        if(me.moves[i] == my_move) {
            return shouldDoX(me, opponent, i, 
                             m_concat_matrix, m_last_concat_decisions[me.uid], true);
        }
    }
    return false;
}

bool EvolveAIAttack::shouldSpendSPForUltraAgility(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &my_move) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    for(int i = 0; i < me.moves.size(); ++i) {
        if(me.moves[i] == my_move) {
            return shouldDoX(me, opponent, i, 
                             m_agility_matrix, m_last_agility_decisions[me.uid], true);
        }
    }
    return false;
}

bool EvolveAIAttack::shouldSpendSPToBoostAttack(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    for(int i = 0; i < me.moves.size(); ++i) {
        if(me.moves[i] == my_move) {
            return shouldDoX(me, opponent, i, 
                             m_at_boost_matrix, m_last_at_boost_decisions[me.uid],
                             true);
        }
    }
    return false;
}

bool EvolveAIAttack::shouldSpendSPToBoostDamage(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    for(int i = 0; i < me.moves.size(); ++i) {
        if(me.moves[i] == my_move) {
            return shouldDoX(me, opponent, i, 
                             m_dmg_boost_matrix, m_last_dmg_boost_decisions[me.uid],
                             true);
        }
    }
    return false;
}

void EvolveAIAttack::updateAfterMove(game::Character const &me,
                                     game::Character const &opponent,
                                     game::Move const &move, bool successful) {
    // Do nothing.
}

void EvolveAIAttack::updateAfterMatch(game::Character const &me,
                                      game::Character const &opponent,
                                      bool has_won) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    updateX(has_won, m_ap_to_sp_matrix, m_last_ap_to_sp_decisions[me.uid]);
    updateX(has_won, m_fall_matrix, m_last_fall_decisions[me.uid]);
    updateX(has_won, m_lower_matrix, m_last_lower_decisions[me.uid]);
    updateX(has_won, m_concat_matrix, m_last_concat_decisions[me.uid]);
    updateX(has_won, m_agility_matrix, m_last_agility_decisions[me.uid]);
    updateX(has_won, m_at_boost_matrix, m_last_at_boost_decisions[me.uid]);
    updateX(has_won, m_dmg_boost_matrix, m_last_dmg_boost_decisions[me.uid]);

    // Moves are a special case.
    while(!m_last_move_decisions[me.uid].empty()) {
        auto &dr = m_last_move_decisions[me.uid].back();
        dr.goodness = has_won ? 1.0 : 0.0;
        m_move_matrix.updateGoodness(dr.down, dr.air, dr.enemy_at_or_df, 
                                     dr.own_ap, dr.own_sp, dr.parameter,
                                     dr.goodness);
        // If we have won, for all OTHER moves, reduce probability!
        if(has_won) {
            for(size_t i = 0; i < DecisionMatrix::MAX_PARAM_CASES; ++i) {
                if(i == dr.parameter) {
                    continue;
                }
                m_move_matrix.updateGoodness(dr.down, dr.air, dr.enemy_at_or_df,
                                             dr.own_ap, dr.own_sp, i, 0.5);
            }
        }
        m_last_move_decisions[me.uid].pop_back();
    }
}


EvolveAIDefence::~EvolveAIDefence() {}

game::Move const & EvolveAIDefence::getCounterMove(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &opponent_move) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    size_t which_move = 0;
    bool at_least_one_move = false;
    assert(me.moves.size() < DecisionMatrix::MAX_PARAM_CASES);
    std::array<double, DecisionMatrix::MAX_PARAM_CASES> moves_goodness;
    for(auto &d : moves_goodness) {
        d = 0.000001;
    }

    for( ; which_move < me.moves.size(); ++which_move) {
        Move const &m = me.moves[which_move];
        // If we're distant we can only counter with a MS_DISTANCE or MS_DASH.
        if(far && !m.canHitDistance()) {
            continue;
        }
        // Compute AP cost.
        int ap_cost = m.apCost(far, true);
        // If it costs too much, don't consider it.
        if(ap_cost >= me.cur_ap) {
            continue;
        }
        // If it's a super, don't consider it if we don't have the SP.
        if(m.isSuper() && me.cur_sp < 4) {
            continue;
        }
        at_least_one_move = true;
        double cur_goodness = m_move_matrix.getGoodness(
            opponent.down, opponent.air,
            opponent.at, me.cur_ap, me.cur_sp, 
            which_move);
        assert(which_move < me.moves.size());
        moves_goodness[which_move] = cur_goodness;
    }

    // Now choose the move based on probabilities
    if(!at_least_one_move) {
        // Just wait
        which_move = 0; // Assumes me.moves[0] is the wait move!
    }
    else {
        std::lock_guard<std::mutex> protect_random(getRandomGenMutex());
        std::discrete_distribution<int> weighted(moves_goodness.cbegin(), 
                                                 moves_goodness.cend());
        which_move = weighted(getRandomGenerator());
        if(which_move >= me.moves.size()) {
            which_move = 0;
        }
    }

    // Now commit to the move and store it in the current match list.
    DecisionRecord cur_decision{opponent.down, opponent.air, opponent.df, 
                                me.cur_ap, me.cur_sp, which_move, 1.0};
    m_last_move_decisions[me.uid].push_back(cur_decision);

    assert(which_move < me.moves.size());
    return me.moves[which_move];
}

bool EvolveAIDefence::shouldSpendSPToComboBreak(game::Character const &me,
                               game::Character const &opponent,
                               bool far,
                               game::Move const &opponent_move) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    for(int i = 0; i < opponent.moves.size(); ++i) {
        if(opponent.moves[i] == opponent_move) {
            return shouldDoX(me, opponent, i, 
                             m_break_matrix, m_last_break_decisions[me.uid],
                             true, false);
        }
    }
    return false;
}

bool EvolveAIDefence::shouldSpendSPToLowerAPCost(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move,
                                int current_ap_cost) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    for(int i = 0; i < me.moves.size(); ++i) {
        if(me.moves[i] == my_move) {
            return shouldDoX(me, opponent, i, 
                             m_lower_matrix, m_last_lower_decisions[me.uid], true);
        }
    }
    return false;
}

bool EvolveAIDefence::shouldSpendSPForUltraAgility(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &my_move) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    for(int i = 0; i < me.moves.size(); ++i) {
        if(me.moves[i] == my_move) {
            return shouldDoX(me, opponent, i, 
                             m_agility_matrix, m_last_agility_decisions[me.uid],
                             true, false);
        }
    }
    return false;
}

bool EvolveAIDefence::shouldSpendSPToBoostDefence(game::Character const &me,
                                 game::Character const &opponent,
                                 bool far,
                                 game::Move const &my_move) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    for(int i = 0; i < me.moves.size(); ++i) {
        if(me.moves[i] == my_move) {
            return shouldDoX(me, opponent, i, 
                             m_df_boost_matrix, m_last_df_boost_decisions[me.uid],
                             true, false);
        }
    }
    return false;
}

bool EvolveAIDefence::shouldSpendSPToBoostCounterDamage(game::Character const &me,
                                       game::Character const &opponent,
                                       bool far,
                                       game::Move const &my_move) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    for(int i = 0; i < me.moves.size(); ++i) {
        if(me.moves[i] == my_move) {
            return shouldDoX(me, opponent, i, 
                             m_dmg_boost_matrix, m_last_dmg_boost_decisions[me.uid],
                             true, false);
        }
    }
    return false;
}

void EvolveAIDefence::updateAfterMove(game::Character const &me,
                                      game::Character const &opponent,
                                      game::Move const &move, bool successful) {
    // Do nothing.
}

void EvolveAIDefence::updateAfterMatch(game::Character const &me,
                                       game::Character const &opponent,
                                       bool has_won) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    updateX(has_won, m_break_matrix, m_last_break_decisions[me.uid]);
    updateX(has_won, m_lower_matrix, m_last_lower_decisions[me.uid]);
    updateX(has_won, m_agility_matrix, m_last_agility_decisions[me.uid]);
    updateX(has_won, m_df_boost_matrix, m_last_df_boost_decisions[me.uid]);
    updateX(has_won, m_dmg_boost_matrix, m_last_dmg_boost_decisions[me.uid]);

    // Moves are a special case.
    while(!m_last_move_decisions[me.uid].empty()) {
        auto &dr = m_last_move_decisions[me.uid].back();
        dr.goodness = has_won ? 1.0 : 0.0;
        m_move_matrix.updateGoodness(dr.down, dr.air, dr.enemy_at_or_df, 
                                     dr.own_ap, dr.own_sp, dr.parameter,
                                     dr.goodness);
        // If we have won, for all OTHER moves, reduce probability!
        if(has_won) {
            for(size_t i = 0; i < DecisionMatrix::MAX_PARAM_CASES; ++i) {
                if(i == dr.parameter) {
                    continue;
                }
                m_move_matrix.updateGoodness(dr.down, dr.air, dr.enemy_at_or_df,
                                             dr.own_ap, dr.own_sp, i, 0.5);
            }
        }
        m_last_move_decisions[me.uid].pop_back();
    }
}


MarkovDecisionMatrix::MarkovDecisionMatrix() {
    for(auto &x : m_goodness) {
        x = 0.5;
    }
    m_learning_factor = 0.5;
}

double MarkovDecisionMatrix::getGoodness(int at_or_df,
                                         int own_ap, int own_sp, 
                                         int parameter[3]) {
    // Saturate ap/sp
    if(own_ap >= MAX_OWN_AP_CASES) {
        own_ap = MAX_OWN_AP_CASES - 1;
    }
    if(own_sp >= MAX_OWN_SP_CASES) {
        own_sp = MAX_OWN_SP_CASES - 1;
    }
    // Assert on other saturation
    assert(at_or_df >= 0);
    assert(own_ap >= 0);
    assert(own_sp >= 0);
    assert(parameter[0] >= 0);
    assert(parameter[1] >= 0);
    assert(parameter[2] >= 0);
    assert(at_or_df < MAX_AT_DF_CASES);
    assert(own_sp < MAX_OWN_SP_CASES);
    assert(parameter[0] < MAX_PARAM_CASES);
    assert(parameter[1] < MAX_PARAM_CASES);
    assert(parameter[2] < MAX_PARAM_CASES);
    size_t idx = 0;
    idx += at_or_df * MAX_OWN_AP_CASES * MAX_OWN_SP_CASES * MAX_PARAM_CASES * MAX_PARAM_CASES * MAX_PARAM_CASES;
    idx += own_ap * MAX_OWN_SP_CASES * MAX_PARAM_CASES * MAX_PARAM_CASES * MAX_PARAM_CASES;
    idx += own_sp * MAX_PARAM_CASES * MAX_PARAM_CASES * MAX_PARAM_CASES;
    idx += parameter[0] * MAX_PARAM_CASES * MAX_PARAM_CASES;
    idx += parameter[1] * MAX_PARAM_CASES;
    idx += parameter[2];
    assert(idx < DECISION_MATRIX_SIZE);
    return m_goodness[idx];
}

void MarkovDecisionMatrix::updateGoodness(int at_or_df,
                                          int own_ap, int own_sp, 
                                          int parameter[3], double good) {
    // Saturate ap/sp
    if(own_ap >= MAX_OWN_AP_CASES) {
        own_ap = MAX_OWN_AP_CASES - 1;
    }
    if(own_sp >= MAX_OWN_SP_CASES) {
        own_sp = MAX_OWN_SP_CASES - 1;
    }
    // Assert on other saturation
    assert(at_or_df >= 0);
    assert(own_ap >= 0);
    assert(own_sp >= 0);
    assert(parameter[0] >= 0);
    assert(parameter[1] >= 0);
    assert(parameter[2] >= 0);
    assert(at_or_df < MAX_AT_DF_CASES);
    assert(own_sp < MAX_OWN_SP_CASES);
    assert(parameter[0] < MAX_PARAM_CASES);
    assert(parameter[1] < MAX_PARAM_CASES);
    assert(parameter[2] < MAX_PARAM_CASES);
    size_t idx = 0;
    idx += at_or_df * MAX_OWN_AP_CASES * MAX_OWN_SP_CASES * MAX_PARAM_CASES * MAX_PARAM_CASES * MAX_PARAM_CASES;
    idx += own_ap * MAX_OWN_SP_CASES * MAX_PARAM_CASES * MAX_PARAM_CASES * MAX_PARAM_CASES;
    idx += own_sp * MAX_PARAM_CASES * MAX_PARAM_CASES * MAX_PARAM_CASES;
    idx += parameter[0] * MAX_PARAM_CASES * MAX_PARAM_CASES;
    idx += parameter[1] * MAX_PARAM_CASES;
    idx += parameter[2];
    assert(idx < DECISION_MATRIX_SIZE);
    m_goodness[idx] = (m_goodness[idx] + good * (1.0 - m_learning_factor)) * 0.5;
    m_learning_factor = m_learning_factor * 0.9999;
}

void MarkovDecisionMatrix::dump(std::string const & with_name, 
                          std::vector<Move> const &with_moves) const {
    // TODO
/*
    std::cout << "MATRIX '" << with_name << "', learning factor " 
              << m_learning_factor << ", interesting edge results:" 
              << std::endl;
    for(size_t down = 0; down < DecisionMatrix::MAX_DOWN_CASES; ++down) {
    for(size_t air = 0; air < DecisionMatrix::MAX_AIR_CASES; ++air) {
    for(size_t df = 0; df < DecisionMatrix::MAX_AT_DF_CASES; ++df) {
    for(size_t ap = 0; ap < DecisionMatrix::MAX_OWN_AP_CASES; ++ap) {
    for(size_t sp = 0; sp < DecisionMatrix::MAX_OWN_SP_CASES; ++sp) {
    for(size_t mv = 0; mv < with_moves.size(); ++mv) {
        size_t idx = 0;
        idx += down * MAX_DOWN_CASES * MAX_OWN_AP_CASES * MAX_OWN_SP_CASES * MAX_PARAM_CASES;
        idx += air * MAX_AT_DF_CASES * MAX_OWN_AP_CASES * MAX_OWN_SP_CASES * MAX_PARAM_CASES;
        idx += df * MAX_OWN_AP_CASES * MAX_OWN_SP_CASES * MAX_PARAM_CASES;
        idx += ap * MAX_OWN_SP_CASES * MAX_PARAM_CASES;
        idx += sp * MAX_PARAM_CASES;
        idx += mv;
        double good = m_goodness[idx];
        if(good > 0.99 || good < 0.01) {
            std::cout << "[ ";
            std::cout << ((down == 0) ? "nodown " : "  down ");
            std::cout << ((air == 0) ? "noair " : "  air ");
            std::cout << df;
            std::cout << "at/df ";
            std::cout << ap;
            std::cout << "ap ";
            std::cout << sp;
            std::cout << "sp '";
            std::cout << with_moves[mv].getName();
            std::cout << "' ] : ";
            std::cout << good;
            std::cout << std::endl;
        }
    }}}}}}
    std::cout << std::endl;
*/
}

MarkovAIAttack::MarkovAIAttack() : EvolveAIAttack() {}

MarkovAIAttack::~MarkovAIAttack() {}

game::Move const & MarkovAIAttack::getNextMove(game::Character const &me,
                               game::Character const &opponent,
                               bool far) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    size_t which_move = 0;
    size_t num_already_performed = me.moves_performed.size();
    size_t last_move_index = (num_already_performed <= 0)
                             ? 0 
                             : me.moves_performed.back();
    size_t second_last_move_index = (num_already_performed <= 1) 
                                    ? 0 
                                    : me.moves_performed[num_already_performed - 2];
    bool at_least_one_move = false;
    assert(me.moves.size() < DecisionMatrix::MAX_PARAM_CASES);
    std::array<double, DecisionMatrix::MAX_PARAM_CASES> moves_goodness;
    for(auto &d : moves_goodness) {
        d = 0.000001;
    }

    for( ; which_move < me.moves.size(); ++which_move) {
        Move const &m = me.moves[which_move];
        // Don't consider last move (unless it was a wait).
        if(!m.isWait() && last_move_index == which_move) {
            continue;
        }
        // Don't consider a MS_REFLECT move in attack.
        if(m.hasReflect()) {
            continue;
        }
        // Compute AP cost.
        int ap_cost = m.apCost(far, false);
        // If it costs too much, don't consider it.
        if(ap_cost >= me.cur_ap) {
            continue;
        }
        // If it's a super, don't consider it if we don't have the SP.
        if(m.isSuper() && me.cur_sp < 4) {
            continue;
        }
        // If it's a distance move, don't consider it if we're close up...
        // ... unless it's a super, in which case yeah, why not.
        if(!far && !m.isSuper() && m.hasSymbol(MS_DISTANCE)) {
            continue;
        }
        at_least_one_move = true;
        int parameter[3] = { 
            (int)second_last_move_index, 
            (int)last_move_index, 
            (int)which_move
        };
        double cur_goodness = m_markov_matrix.getGoodness(
            opponent.df, me.cur_ap, me.cur_sp, 
            parameter);
        assert(which_move < me.moves.size());
        moves_goodness[which_move] = cur_goodness;
    }

    // Now choose the move based on probabilities
    if(!at_least_one_move) {
        // Just wait
        which_move = 0; // Assumes me.moves[0] is the wait move!
    }
    else {
        std::lock_guard<std::mutex> protect_random(getRandomGenMutex());
        std::discrete_distribution<int> weighted(moves_goodness.cbegin(), 
                                                 moves_goodness.cend());
        which_move = weighted(getRandomGenerator());
        if(which_move >= me.moves.size()) {
            which_move = 0;
        }
    }

    // Now commit to the move and store it in the current match list.
    MarkovDecisionRecord cur_decision{opponent.df, 
                                      me.cur_ap, me.cur_sp, 
                                      { (int)second_last_move_index, (int)last_move_index, (int)which_move },
                                      1.0};
    m_last_markov_decisions[me.uid].push_back(cur_decision);

    assert(which_move < me.moves.size());
    return me.moves[which_move];
}

void MarkovAIAttack::updateAfterMatch(game::Character const &me,
                                      game::Character const &opponent,
                                      bool has_won) {
    EvolveAIAttack::updateAfterMatch(me, opponent, has_won);

    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    // Moves are a special case.
    while(!m_last_markov_decisions[me.uid].empty()) {
        auto &dr = m_last_markov_decisions[me.uid].back();
        dr.goodness = has_won ? 1.0 : 0.0;
        m_markov_matrix.updateGoodness(dr.enemy_at_or_df, 
                                       dr.own_ap, dr.own_sp, dr.parameter,
                                       dr.goodness);
        m_last_markov_decisions[me.uid].pop_back();
    }
}

}
}
