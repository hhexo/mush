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

#ifndef CORE_CTRL_NNCTRL_H
#define CORE_CTRL_NNCTRL_H

#include "core/ctrl/CtrlInterfaces.h"

#include <vector>
#include <unordered_map>
#include <stdint.h>
#include <mutex>
#include <math.h>

namespace core {
namespace ctrl {

/// A simple neural network implementation with one hidden layer.
class NN {
public:
    NN(size_t inputs, size_t hidden, size_t outputs);

    std::vector<double> feed(std::vector<double> const& inputs);

    void backpropagate(std::vector<double> const& inputs, 
                       std::vector<double> const& outputs);

private:
    size_t m_inputs;
    size_t m_hidden;
    size_t m_outputs;
    std::vector<double> m_hidden_layer;
    std::vector<double> m_output_layer;

    double sigmoid(double x) { return 1.0 / (1.0 + exp(-x)); }
    double derivative(double x) { return x * (1.0 - x); }

    std::pair<std::vector<double>, std::vector<double>> 
    feedInternal(std::vector<double> const& inputs);
};

/// \brief A decision that has been made by the Neural Network.
struct NNDecisionRecord {
    static constexpr size_t const MAX_PARAM_CASES = 13;

    /// \brief Whether the opponent was down.
    bool down;
    /// \brief Whether the opponent was in air.
    bool air;
    /// \brief The opponent's AT or DF.
    int enemy_at_or_df;
    /// \brief How many AP we had.
    int own_ap;
    /// \brief How many SP we had.
    int own_sp;
    /// \brief Which parameter (usually a move) has been chosen.
    size_t parameter;
    /// \brief Used to record probabilities.
    double goodness;
};

/// \brief A list of decisions that have been made by the Neural Network.
typedef std::vector<NNDecisionRecord> NNDecisionVector;

/// \brief A context-based data structure for storing NNDecisionVectors.
typedef std::unordered_map<size_t, NNDecisionVector> NNDecisionVectorMap;


/**
 * \brief This class is a Neural Network based control system for attack.
 */
class NNAttackControl : public AttackControl {
public:
    NNAttackControl();
    virtual ~NNAttackControl();

    char const * const getName() const override;

    int shouldSpendAPToGainSP(game::Character const &me,
                              game::Character const &opponent,
                              bool far) override;

    bool shouldSpendAPToFallStanding(game::Character const &me,
                                     game::Character const &opponent,
                                     bool far) override;

    game::Move const & getNextMove(game::Character const &me,
                                   game::Character const &opponent,
                                   bool far) override;

    bool shouldSpendSPToLowerAPCost(game::Character const &me,
                                    game::Character const &opponent,
                                    bool far,
                                    game::Move const &my_move,
                                    int current_ap_cost) override;

    bool shouldSpendSPToConcatenate(game::Character const &me,
                                    game::Character const &opponent,
                                    bool far,
                                    game::Move const &my_move) override;

    bool shouldSpendSPForUltraAgility(game::Character const &me,
                                      game::Character const &opponent,
                                      bool far,
                                      game::Move const &my_move) override;

    bool shouldSpendSPToBoostAttack(game::Character const &me,
                                    game::Character const &opponent,
                                    bool far,
                                    game::Move const &my_move) override;

    bool shouldSpendSPToBoostDamage(game::Character const &me,
                                    game::Character const &opponent,
                                    bool far,
                                    game::Move const &my_move) override;

    void updateAfterMove(game::Character const &me,
                         game::Character const &opponent,
                         game::Move const &move, bool successful) override;

    void updateAfterMatch(game::Character const &me,
                          game::Character const &opponent,
                          bool has_won) override;

protected:
    /// Neural Network for investing APs into SPs, if necessary.
    NN m_ap_to_sp_network;

    /// Neural Network for spending APs to fall without damage.
    NN m_fall_network;

    /// Neural Network for choosing the next move.
    NN m_move_network;

    /// Neural Network for lowering cost.
    NN m_lower_network;

    /// Neural Network for concatenating.
    NN m_concat_network;

    /// Neural Network for agility bonus, if necessary.
    NN m_agility_network;

    /// Neural Network for attack boost.
    NN m_at_boost_network;

    /// Neural Network for damage boost.
    NN m_dmg_boost_network;

    /// Last decisions, AP to SP
    NNDecisionVectorMap m_last_ap_to_sp_decisions;

    /// Last decisions, fall
    NNDecisionVectorMap m_last_fall_decisions;

    /// Last decisions, move
    NNDecisionVectorMap m_last_move_decisions;

    /// Last decisions, lowering cost
    NNDecisionVectorMap m_last_lower_decisions;

    /// Last decisions, concatenating
    NNDecisionVectorMap m_last_concat_decisions;

    /// Last decisions, agility bonus
    NNDecisionVectorMap m_last_agility_decisions;

    /// Last decisions, attack boost
    NNDecisionVectorMap m_last_at_boost_decisions;

    /// Last decisions, damage boost
    NNDecisionVectorMap m_last_dmg_boost_decisions;

    /// Content mutex to protect against multi-threading.
    std::mutex m_content_mutex;

};

/**
 * \brief This class is a Neural Network based control system for defence.
 */
class NNDefendControl : public DefendControl {
public:
    NNDefendControl();
    ~NNDefendControl();

    char const * const getName() const override;

    game::Move const & getCounterMove(game::Character const &me,
                                      game::Character const &opponent,
                                      bool far,
                                      game::Move const &opponent_move) override;

    bool shouldSpendSPToComboBreak(game::Character const &me,
                                   game::Character const &opponent,
                                   bool far,
                                   game::Move const &opponent_move) override;

    bool shouldSpendSPToLowerAPCost(game::Character const &me,
                                    game::Character const &opponent,
                                    bool far,
                                    game::Move const &my_move,
                                    int current_ap_cost) override;

    bool shouldSpendSPForUltraAgility(game::Character const &me,
                                      game::Character const &opponent,
                                      bool far,
                                      game::Move const &my_move) override;

    bool shouldSpendSPToBoostDefence(game::Character const &me,
                                     game::Character const &opponent,
                                     bool far,
                                     game::Move const &my_move) override;

    bool shouldSpendSPToBoostCounterDamage(game::Character const &me,
                                           game::Character const &opponent,
                                           bool far,
                                           game::Move const &my_move) override;

    void updateAfterMove(game::Character const &me,
                         game::Character const &opponent,
                         game::Move const &move, bool successful) override;

    void updateAfterMatch(game::Character const &me,
                          game::Character const &opponent,
                          bool has_won) override;

protected:
    /// Neural Network for choosing the counter move.
    NN m_move_network;

    /// Neural Network for breaking concatenation.
    NN m_break_network;

    /// Neural Network for lowering cost.
    NN m_lower_network;

    /// Neural Network for agility bonus, if necessary.
    NN m_agility_network;

    /// Neural Network for defence boost.
    NN m_df_boost_network;

    /// Neural Network for damage boost.
    NN m_dmg_boost_network;

    /// Last decisions, move
    NNDecisionVectorMap m_last_move_decisions;

    /// Last decisions, breaking concatenation
    NNDecisionVectorMap m_last_break_decisions;

    /// Last decisions, lowering cost
    NNDecisionVectorMap m_last_lower_decisions;

    /// Last decisions, agility bonus
    NNDecisionVectorMap m_last_agility_decisions;

    /// Last decisions, defence boost
    NNDecisionVectorMap m_last_df_boost_decisions;

    /// Last decisions, damage boost
    NNDecisionVectorMap m_last_dmg_boost_decisions;

    /// Content mutex to protect against multi-threading.
    std::mutex m_content_mutex;
};

}
}

#endif
