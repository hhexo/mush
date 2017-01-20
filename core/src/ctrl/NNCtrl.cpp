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

#include "core/ctrl/NNCtrl.h"
#include "core/game/Move.h"
#include "core/game/Character.h"
#include "core/game/Dice.h"

#include <array>
#include <cassert>


namespace {

double const ALPHA = 0.3;
  
}


namespace core {
namespace ctrl {

using namespace core::game;

NN::NN(size_t inputs, size_t hidden, size_t outputs):
    m_inputs(inputs), m_hidden(hidden), m_outputs(outputs),
    m_hidden_layer(std::vector<double>((inputs+1)*hidden, 0.1)),
    m_output_layer(std::vector<double>((hidden+1)*outputs, -0.1)) {}

std::pair<std::vector<double>, std::vector<double>> 
NN::feedInternal(std::vector<double> const& inputs) {
    std::vector<double> hres;
    std::vector<double> ores;
    for(size_t h = 0; h < m_hidden; ++h) {
        double hx = 0.0;
        for(size_t i = 0; i < m_inputs; ++i) {
            hx += m_hidden_layer[h*(m_inputs+1) + i] * inputs[i];
        }
        hx += m_hidden_layer[h*(m_inputs+1) + m_inputs] * 1.0; // bias
        hres.push_back(sigmoid(hx));
    }
    for(size_t o = 0; o < m_outputs; ++o) {
        double ox = 0.0;
        for(size_t h = 0; h < m_hidden; ++h) {
            ox += m_output_layer[o*(m_hidden+1) + h] * hres[h];
        }
        ox += m_hidden_layer[o*(m_hidden+1) + m_hidden] * 1.0; // bias
        ores.push_back(sigmoid(ox));
    }
    return std::make_pair(hres, ores);
}

std::vector<double> NN::feed(std::vector<double> const& inputs) {
    return feedInternal(inputs).second;
}

void NN::backpropagate(std::vector<double> const& inputs, 
                       std::vector<double> const& outputs) {
    auto prediction = feedInternal(inputs);
    auto hres = prediction.first;
    auto ores = prediction.second;

    // Calculate output layer errors and gradients
    std::vector<double> ol_gradients;
    for(size_t o = 0; o < m_outputs; ++o) {
        ol_gradients.push_back( derivative(ores[o]) * (outputs[o] - ores[o]) );
    }

    // Calculate hidden layer errors and gradients
    std::vector<double> hl_gradients;
    for(size_t h = 0; h < m_hidden+1; ++h) {
        double back_gradient = 0.0;
        for(size_t o = 0; o < m_outputs; ++o) {
            back_gradient += m_output_layer[o*(m_hidden+1) + h] * ol_gradients[o];
        }
        hl_gradients.push_back(derivative(hres[h]) * back_gradient);
    }

    // Update weights in layers
    for(size_t o = 0; o < m_outputs; ++o) {
        for(size_t h = 0; h < m_hidden; ++h) {
            m_output_layer[o*(m_hidden+1) + h] += ALPHA * hres[h] * ol_gradients[o];
        }
        m_output_layer[o*(m_hidden+1) + m_hidden] += ALPHA * 1.0 * ol_gradients[o]; // bias
    }
    for(size_t h = 0; h < m_hidden; ++h) {
        for(size_t i = 0; i < m_inputs; ++i) {
            m_hidden_layer[h*(m_inputs+1) + i] += ALPHA * inputs[i] * hl_gradients[h];
        }
        m_hidden_layer[h*(m_inputs+1) + m_inputs] += ALPHA * 1.0 * hl_gradients[h]; // bias
    }
}


static bool shouldDoX(Character const &me, Character const &opponent, 
                      int parameter, 
                      NN &network, 
                      NNDecisionVector &to_update, 
                      bool check_sp = false,
                      bool attacking = true) {
    // Early exit.
    if(check_sp && me.cur_sp <= 0) {
        return false;
    }
    assert(parameter < NNDecisionRecord::MAX_PARAM_CASES);
    // Query network.
    std::vector<double> nn_inputs;
    nn_inputs.push_back(opponent.down ? 1.0 : 0.0);
    nn_inputs.push_back(opponent.air ? 1.0 : 0.0);
    nn_inputs.push_back(static_cast<double>(attacking ? opponent.df : opponent.at));
    nn_inputs.push_back(static_cast<double>(me.cur_ap));
    nn_inputs.push_back(static_cast<double>(me.cur_sp));
    nn_inputs.push_back(static_cast<double>(parameter));
    auto vprob = network.feed(nn_inputs);
    assert(vprob.size() == 1);
    double prob = vprob[0];

    // Roll probability.
    bool yes = false;
    {
        std::lock_guard<std::mutex> protect(getRandomGenMutex());
        std::uniform_real_distribution<double> zero_to_one(0.0, 1.0);
        yes = zero_to_one(getRandomGenerator()) < prob;
        if(yes) {
            NNDecisionRecord cur_decision{opponent.down, opponent.air, 
                                          attacking ? opponent.df : opponent.at, 
                                          me.cur_ap, 
                                          me.cur_sp, 
                                          static_cast<size_t>(parameter), 
                                          1.0};
            to_update.push_back(cur_decision);
            return true;
        }
        NNDecisionRecord cur_decision{opponent.down, opponent.air, 
                                      attacking ? opponent.df : opponent.at, 
                                      me.cur_ap, 
                                      me.cur_sp, 
                                      static_cast<size_t>(parameter), 
                                      0.0};
        to_update.push_back(cur_decision);
    }
    return false;
}

static void updateX(bool has_won, NN &network, 
                    NNDecisionVector &last_decisions) {
    while(!last_decisions.empty()) {
        auto &dr = last_decisions.back();
        if(has_won) {
            dr.goodness = (dr.goodness > 0.0) ? 1.0 : 0.0;
        } else {
            dr.goodness = (dr.goodness > 0.0) ? 0.0 : 1.0;
        }
        std::vector<double> nn_inputs;
        nn_inputs.push_back(dr.down ? 1.0 : 0.0);
        nn_inputs.push_back(dr.air ? 1.0 : 0.0);
        nn_inputs.push_back(static_cast<double>(dr.enemy_at_or_df));
        nn_inputs.push_back(static_cast<double>(dr.own_ap));
        nn_inputs.push_back(static_cast<double>(dr.own_sp));
        nn_inputs.push_back(static_cast<double>(dr.parameter));
        std::vector<double> nn_outputs = std::vector<double>(1, dr.goodness);

        network.backpropagate(nn_inputs, nn_outputs);

        last_decisions.pop_back();
    }
}


NNAttackControl::NNAttackControl():
    m_ap_to_sp_network(6, 32, 1),
    m_fall_network(6, 32, 1),
    m_move_network(5, 32, NNDecisionRecord::MAX_PARAM_CASES),
    m_lower_network(6, 32, 1),
    m_concat_network(6, 32, 1),
    m_agility_network(6, 32, 1),
    m_at_boost_network(6, 32, 1),
    m_dmg_boost_network(6, 32, 1) {}

NNAttackControl::~NNAttackControl() {}

char const * const NNAttackControl::getName() const { return "Neural"; }

int NNAttackControl::shouldSpendAPToGainSP(game::Character const &me,
                          game::Character const &opponent,
                          bool far) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    // Early exit if we already have max SP
    if(me.cur_sp >= 6) {
        return 0;
    }

    int i = 0;
    for(; i < 6; ++i) {
        if(!shouldDoX(me, opponent, i, 
                      m_ap_to_sp_network, m_last_ap_to_sp_decisions[me.uid])) {
            break;
        }
    }
    return i;
}

bool NNAttackControl::shouldSpendAPToFallStanding(game::Character const &me,
                                 game::Character const &opponent,
                                 bool far) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    return shouldDoX(me, opponent, 0, 
                     m_fall_network, m_last_fall_decisions[me.uid]);
}

game::Move const & NNAttackControl::getNextMove(game::Character const &me,
                               game::Character const &opponent,
                               bool far) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    size_t which_move = 0;
    size_t last_move_index = me.moves_performed.empty() 
                             ? 0 
                             : me.moves_performed.back();
    bool at_least_one_move = false;
    assert(me.moves.size() < NNDecisionRecord::MAX_PARAM_CASES);
    // We can compute these in advance, and then only update the ones we care
    // about. This way we compute them only once.
    std::vector<double> nn_inputs;
    nn_inputs.push_back(opponent.down ? 1.0 : 0.0);
    nn_inputs.push_back(opponent.air ? 1.0 : 0.0);
    nn_inputs.push_back(static_cast<double>(opponent.df));
    nn_inputs.push_back(static_cast<double>(me.cur_ap));
    nn_inputs.push_back(static_cast<double>(me.cur_sp));
    auto tentative_moves_goodness = m_move_network.feed(nn_inputs);

    std::array<double, NNDecisionRecord::MAX_PARAM_CASES> moves_goodness;
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
        moves_goodness[which_move] = tentative_moves_goodness[which_move];
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
    assert(which_move < me.moves.size());
    NNDecisionRecord cur_decision{opponent.down, opponent.air, opponent.df, 
                                  me.cur_ap, me.cur_sp, which_move, 1.0};
    m_last_move_decisions[me.uid].push_back(cur_decision);

    return me.moves[which_move];
}

bool NNAttackControl::shouldSpendSPToLowerAPCost(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move,
                                int current_ap_cost) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    for(int i = 0; i < me.moves.size(); ++i) {
        if(me.moves[i] == my_move) {
            return shouldDoX(me, opponent, i, 
                             m_lower_network, m_last_lower_decisions[me.uid], true);
        }
    }
    return false;
}

bool NNAttackControl::shouldSpendSPToConcatenate(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    for(int i = 0; i < me.moves.size(); ++i) {
        if(me.moves[i] == my_move) {
            return shouldDoX(me, opponent, i, 
                             m_concat_network, m_last_concat_decisions[me.uid], true);
        }
    }
    return false;
}

bool NNAttackControl::shouldSpendSPForUltraAgility(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &my_move) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    for(int i = 0; i < me.moves.size(); ++i) {
        if(me.moves[i] == my_move) {
            return shouldDoX(me, opponent, i, 
                             m_agility_network, m_last_agility_decisions[me.uid], true);
        }
    }
    return false;
}

bool NNAttackControl::shouldSpendSPToBoostAttack(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    for(int i = 0; i < me.moves.size(); ++i) {
        if(me.moves[i] == my_move) {
            return shouldDoX(me, opponent, i, 
                             m_at_boost_network, m_last_at_boost_decisions[me.uid],
                             true);
        }
    }
    return false;
}

bool NNAttackControl::shouldSpendSPToBoostDamage(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    for(int i = 0; i < me.moves.size(); ++i) {
        if(me.moves[i] == my_move) {
            return shouldDoX(me, opponent, i, 
                             m_dmg_boost_network, m_last_dmg_boost_decisions[me.uid],
                             true);
        }
    }
    return false;
}

void NNAttackControl::updateAfterMove(game::Character const &me,
                                        game::Character const &opponent,
                                        game::Move const &move,
                                        bool successful) {
    // Do nothing.
}

void NNAttackControl::updateAfterMatch(game::Character const &me,
                                         game::Character const &opponent,
                                         bool has_won) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    updateX(has_won, m_ap_to_sp_network, m_last_ap_to_sp_decisions[me.uid]);
    m_last_ap_to_sp_decisions.erase(me.uid);
    updateX(has_won, m_fall_network, m_last_fall_decisions[me.uid]);
    m_last_fall_decisions.erase(me.uid);
    updateX(has_won, m_lower_network, m_last_lower_decisions[me.uid]);
    m_last_lower_decisions.erase(me.uid);
    updateX(has_won, m_concat_network, m_last_concat_decisions[me.uid]);
    m_last_concat_decisions.erase(me.uid);
    updateX(has_won, m_agility_network, m_last_agility_decisions[me.uid]);
    m_last_agility_decisions.erase(me.uid);
    updateX(has_won, m_at_boost_network, m_last_at_boost_decisions[me.uid]);
    m_last_at_boost_decisions.erase(me.uid);
    updateX(has_won, m_dmg_boost_network, m_last_dmg_boost_decisions[me.uid]);
    m_last_dmg_boost_decisions.erase(me.uid);

    // Moves are a special case.
    while(!m_last_move_decisions[me.uid].empty()) {
        auto &dr = m_last_move_decisions[me.uid].back();
        dr.goodness = has_won ? 1.0 : 0.0;

        std::vector<double> nn_inputs;
        nn_inputs.push_back(dr.down ? 1.0 : 0.0);
        nn_inputs.push_back(dr.air ? 1.0 : 0.0);
        nn_inputs.push_back(static_cast<double>(dr.enemy_at_or_df));
        nn_inputs.push_back(static_cast<double>(dr.own_ap));
        nn_inputs.push_back(static_cast<double>(dr.own_sp));

        std::vector<double> nn_outputs = std::vector<double>(
            NNDecisionRecord::MAX_PARAM_CASES, has_won ? 0.0 : 0.5);
        nn_outputs[dr.parameter] = dr.goodness;

        m_move_network.backpropagate(nn_inputs, nn_outputs);

        m_last_move_decisions[me.uid].pop_back();
    }
    m_last_move_decisions.erase(me.uid);
}


NNDefendControl::NNDefendControl():
    m_move_network(5, 32, NNDecisionRecord::MAX_PARAM_CASES),
    m_break_network(6, 32, 1),
    m_lower_network(6, 32, 1),
    m_agility_network(6, 32, 1),
    m_df_boost_network(6, 32, 1),
    m_dmg_boost_network(6, 32, 1) {}

NNDefendControl::~NNDefendControl() {}

char const * const NNDefendControl::getName() const { return "Neural"; }

game::Move const & NNDefendControl::getCounterMove(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &opponent_move) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    size_t which_move = 0;
    bool at_least_one_move = false;
    assert(me.moves.size() < NNDecisionRecord::MAX_PARAM_CASES);
    // We can compute these in advance, and then only update the ones we care
    // about. This way we compute them only once.
    std::vector<double> nn_inputs;
    nn_inputs.push_back(opponent.down ? 1.0 : 0.0);
    nn_inputs.push_back(opponent.air ? 1.0 : 0.0);
    nn_inputs.push_back(static_cast<double>(opponent.at));
    nn_inputs.push_back(static_cast<double>(me.cur_ap));
    nn_inputs.push_back(static_cast<double>(me.cur_sp));
    auto tentative_moves_goodness = m_move_network.feed(nn_inputs);

    std::array<double, NNDecisionRecord::MAX_PARAM_CASES> moves_goodness;
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
        assert(which_move < me.moves.size());
        moves_goodness[which_move] = tentative_moves_goodness[which_move];
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
    NNDecisionRecord cur_decision{opponent.down, opponent.air, opponent.at, 
                                  me.cur_ap, me.cur_sp, which_move, 1.0};
    m_last_move_decisions[me.uid].push_back(cur_decision);

    assert(which_move < me.moves.size());
    return me.moves[which_move];
}

bool NNDefendControl::shouldSpendSPToComboBreak(game::Character const &me,
                               game::Character const &opponent,
                               bool far,
                               game::Move const &opponent_move) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    for(int i = 0; i < opponent.moves.size(); ++i) {
        if(opponent.moves[i] == opponent_move) {
            return shouldDoX(me, opponent, i, 
                             m_break_network, m_last_break_decisions[me.uid],
                             true, false);
        }
    }
    return false;
}

bool NNDefendControl::shouldSpendSPToLowerAPCost(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move,
                                int current_ap_cost) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    for(int i = 0; i < me.moves.size(); ++i) {
        if(me.moves[i] == my_move) {
            return shouldDoX(me, opponent, i, 
                             m_lower_network, m_last_lower_decisions[me.uid], true);
        }
    }
    return false;
}

bool NNDefendControl::shouldSpendSPForUltraAgility(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &my_move) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    for(int i = 0; i < me.moves.size(); ++i) {
        if(me.moves[i] == my_move) {
            return shouldDoX(me, opponent, i, 
                             m_agility_network, m_last_agility_decisions[me.uid],
                             true, false);
        }
    }
    return false;
}

bool NNDefendControl::shouldSpendSPToBoostDefence(game::Character const &me,
                                 game::Character const &opponent,
                                 bool far,
                                 game::Move const &my_move) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    for(int i = 0; i < me.moves.size(); ++i) {
        if(me.moves[i] == my_move) {
            return shouldDoX(me, opponent, i, 
                             m_df_boost_network, m_last_df_boost_decisions[me.uid],
                             true, false);
        }
    }
    return false;
}

bool NNDefendControl::shouldSpendSPToBoostCounterDamage(game::Character const &me,
                                       game::Character const &opponent,
                                       bool far,
                                       game::Move const &my_move) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    for(int i = 0; i < me.moves.size(); ++i) {
        if(me.moves[i] == my_move) {
            return shouldDoX(me, opponent, i, 
                             m_dmg_boost_network, m_last_dmg_boost_decisions[me.uid],
                             true, false);
        }
    }
    return false;
}

void NNDefendControl::updateAfterMove(game::Character const &me,
                                        game::Character const &opponent,
                                        game::Move const &move,
                                        bool successful) {
    // Do nothing.
}

void NNDefendControl::updateAfterMatch(game::Character const &me,
                                         game::Character const &opponent,
                                         bool has_won) {
    // Protect ourselves against multi-threading
    std::lock_guard<std::mutex> protect(m_content_mutex);

    updateX(has_won, m_break_network, m_last_break_decisions[me.uid]);
    m_last_break_decisions.erase(me.uid);
    updateX(has_won, m_lower_network, m_last_lower_decisions[me.uid]);
    m_last_lower_decisions.erase(me.uid);
    updateX(has_won, m_agility_network, m_last_agility_decisions[me.uid]);
    m_last_agility_decisions.erase(me.uid);
    updateX(has_won, m_df_boost_network, m_last_df_boost_decisions[me.uid]);
    m_last_df_boost_decisions.erase(me.uid);
    updateX(has_won, m_dmg_boost_network, m_last_dmg_boost_decisions[me.uid]);
    m_last_dmg_boost_decisions.erase(me.uid);

    // Moves are a special case.
    while(!m_last_move_decisions[me.uid].empty()) {
        auto &dr = m_last_move_decisions[me.uid].back();
        dr.goodness = has_won ? 1.0 : 0.0;

        std::vector<double> nn_inputs;
        nn_inputs.push_back(dr.down ? 1.0 : 0.0);
        nn_inputs.push_back(dr.air ? 1.0 : 0.0);
        nn_inputs.push_back(static_cast<double>(dr.enemy_at_or_df));
        nn_inputs.push_back(static_cast<double>(dr.own_ap));
        nn_inputs.push_back(static_cast<double>(dr.own_sp));

        std::vector<double> nn_outputs = std::vector<double>(
            NNDecisionRecord::MAX_PARAM_CASES, has_won ? 0.0 : 0.5);
        nn_outputs[dr.parameter] = dr.goodness;

        m_move_network.backpropagate(nn_inputs, nn_outputs);

        m_last_move_decisions[me.uid].pop_back();
    }
    m_last_move_decisions.erase(me.uid);
}


}
}
