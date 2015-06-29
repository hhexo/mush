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

#ifndef CORE_CTRL_EVOLVE_AI_CTRL_H
#define CORE_CTRL_EVOLVE_AI_CTRL_H

#include "core/ctrl/CtrlInterfaces.h"

#include <vector>
#include <unordered_map>
#include <stdint.h>
#include <mutex>

namespace core {
namespace ctrl {

/**
 * \brief This class represents a decision matrix for the evolving AI. It is of
 *        the order of hundreds of kilobytes, so quite manageable.
 */
class DecisionMatrix {
public:
    /// \brief Maximum number of "down" cases (true or false).
    static constexpr size_t const MAX_DOWN_CASES = 2;
    /// \brief Maximum number of "air" cases (true or false).
    static constexpr size_t const MAX_AIR_CASES = 2;
    /// \brief Maximum number of opponent's at/df cases (0 to 5).
    static constexpr size_t const MAX_AT_DF_CASES = 6;
    /// \brief Maximum number of our AP; must saturate to 9.
    static constexpr size_t const MAX_OWN_AP_CASES = 10;
    /// \brief Maximum number of our SP; must saturate to 6.
    static constexpr size_t const MAX_OWN_SP_CASES = 7;
    /// \brief Maximum number of parameter cases; just enough for the moves.
    static constexpr size_t const MAX_PARAM_CASES = 13;

    /// \brief Absolute size of the decision matrix.
    static constexpr size_t const DECISION_MATRIX_SIZE = MAX_DOWN_CASES *
        MAX_AIR_CASES * MAX_AT_DF_CASES * MAX_OWN_AP_CASES * MAX_OWN_SP_CASES *
        MAX_PARAM_CASES;

    /// \brief Main constructor initializes the matrix.
    DecisionMatrix();
    /// \brief Gets the currently stored "goodness" of a parametric choice.
    double getGoodness(bool down, bool in_air, int at_or_df, int own_ap, int own_sp, int parameter);
    /// \brief Updates the currently stored "goodness" of a parametric choice.
    void updateGoodness(bool down, bool in_air, int at_or_df, int own_ap, int own_sp, int parameter, double good);
    /// \brief Dumps the state of the matrix for debugging.
    void dump(std::string const & with_name, 
              std::vector<game::Move> const &with_moves) const;
private:
    /// \brief The matrix of goodness probabilities of each parametric choice.
    double m_goodness[DECISION_MATRIX_SIZE];
    /// \brief A learning factor that increases with time, representing the fact
    ///        that initial choices count less than later choices.
    double m_learning_factor;
};

/// \brief A decision that has been made.
struct DecisionRecord {
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

/// \brief A list of decisions that have been made.
typedef std::vector<DecisionRecord> DecisionVector;

/// \brief A context-based data structure for storing DecisionVectors.
typedef std::unordered_map<size_t, DecisionVector> DecisionVectorMap;

/**
 * \brief This class implements a positive-reinforcement MonteCarlo AI that
 *        plays the game in attack.
 */
class EvolveAIAttack : public AttackControl {
public:
    EvolveAIAttack();
    virtual ~EvolveAIAttack();

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
                         game::Move const &move, 
                         bool successful) override;

    void updateAfterMatch(game::Character const &me,
                          game::Character const &opponent,
                          bool has_won) override;

protected:
    /// Decision matrix for investing APs into SPs, if necessary.
    DecisionMatrix m_ap_to_sp_matrix;

    /// Decision matrix for spending APs to fall without damage.
    DecisionMatrix m_fall_matrix;

    /// Decision matrix for choosing the next move.
    DecisionMatrix m_move_matrix;

    /// Decision matrix for lowering cost.
    DecisionMatrix m_lower_matrix;

    /// Decision matrix for concatenating.
    DecisionMatrix m_concat_matrix;

    /// Decision matrix for agility bonus, if necessary.
    DecisionMatrix m_agility_matrix;

    /// Decision matrix for attack boost.
    DecisionMatrix m_at_boost_matrix;

    /// Decision matrix for damage boost.
    DecisionMatrix m_dmg_boost_matrix;

    /// Last decisions, AP to SP
    DecisionVectorMap m_last_ap_to_sp_decisions;

    /// Last decisions, AP to fall
    DecisionVectorMap m_last_fall_decisions;

    /// Last decisions, move
    DecisionVectorMap m_last_move_decisions;

    /// Last decisions, lowering cost
    DecisionVectorMap m_last_lower_decisions;

    /// Last decisions, concatenating
    DecisionVectorMap m_last_concat_decisions;

    /// Last decisions, agility bonus
    DecisionVectorMap m_last_agility_decisions;

    /// Last decisions, attack boost
    DecisionVectorMap m_last_at_boost_decisions;

    /// Last decisions, damage boost
    DecisionVectorMap m_last_dmg_boost_decisions;

    /// Content mutex to protect against multi-threading.
    std::mutex m_content_mutex;

};

/**
 * \brief This class implements a positive-reinforcement MonteCarlo AI that
 *        plays the game in defence.
 */
class EvolveAIDefence : public DefendControl {
public:
    ~EvolveAIDefence();

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
                         game::Move const &move,
                         bool successful) override;

    void updateAfterMatch(game::Character const &me,
                          game::Character const &opponent,
                          bool has_won) override;

protected:
    /// Decision matrix for choosing the counter move.
    DecisionMatrix m_move_matrix;

    /// Decision matrix for breaking concatenation.
    DecisionMatrix m_break_matrix;

    /// Decision matrix for lowering cost.
    DecisionMatrix m_lower_matrix;

    /// Decision matrix for agility bonus, if necessary.
    DecisionMatrix m_agility_matrix;

    /// Decision matrix for defence boost.
    DecisionMatrix m_df_boost_matrix;

    /// Decision matrix for damage boost.
    DecisionMatrix m_dmg_boost_matrix;

    /// Last decisions, move
    DecisionVectorMap m_last_move_decisions;

    /// Last decisions, breaking concatenation
    DecisionVectorMap m_last_break_decisions;

    /// Last decisions, lowering cost
    DecisionVectorMap m_last_lower_decisions;

    /// Last decisions, agility bonus
    DecisionVectorMap m_last_agility_decisions;

    /// Last decisions, defence boost
    DecisionVectorMap m_last_df_boost_decisions;

    /// Last decisions, damage boost
    DecisionVectorMap m_last_dmg_boost_decisions;

    /// Content mutex to protect against multi-threading.
    std::mutex m_content_mutex;

};


/**
 * \brief This class represents a decision matrix for the Markov Attack AI. It
 *        is of the order of hundreds of kilobytes, so quite manageable.
 */
class MarkovDecisionMatrix {
public:
    /// \brief Maximum number of opponent's at/df cases (0 to 5).
    static constexpr size_t const MAX_AT_DF_CASES = 6;
    /// \brief Maximum number of our AP; must saturate to 9.
    static constexpr size_t const MAX_OWN_AP_CASES = 10;
    /// \brief Maximum number of our SP; must saturate to 6.
    static constexpr size_t const MAX_OWN_SP_CASES = 7;
    /// \brief Maximum number of parameter cases; just enough for the moves.
    static constexpr size_t const MAX_PARAM_CASES = 13;

    /// \brief Absolute size of the decision matrix.
    static constexpr size_t const DECISION_MATRIX_SIZE =
        MAX_AT_DF_CASES * MAX_OWN_AP_CASES * MAX_OWN_SP_CASES *
        MAX_PARAM_CASES * MAX_PARAM_CASES * MAX_PARAM_CASES;

    /// \brief Main constructor initializes the matrix.
    MarkovDecisionMatrix();
    /// \brief Gets the currently stored "goodness" of a parametric choice.
    double getGoodness(int at_or_df, int own_ap, int own_sp, int parameter[3]);
    /// \brief Updates the currently stored "goodness" of a parametric choice.
    void updateGoodness(int at_or_df, int own_ap, int own_sp, int parameter[3], double good);
    /// \brief Dumps the state of the matrix for debugging.
    void dump(std::string const & with_name, 
              std::vector<game::Move> const &with_moves) const;
private:
    /// \brief The matrix of goodness probabilities of each parametric choice.
    double m_goodness[DECISION_MATRIX_SIZE];
    /// \brief A learning factor that increases with time, representing the fact
    ///        that initial choices count less than later choices.
    double m_learning_factor;
};

/// \brief A decision that has been made.
struct MarkovDecisionRecord {
    /// \brief The opponent's AT or DF.
    int enemy_at_or_df;
    /// \brief How many AP we had.
    int own_ap;
    /// \brief How many SP we had.
    int own_sp;
    /// \brief Which parameters (usually moves) have been chosen.
    int parameter[3];
    /// \brief Used to record probabilities.
    double goodness;
};

/// \brief A list of decisions that have been made.
typedef std::vector<MarkovDecisionRecord> MarkovDecisionVector;

/// \brief A context-based data structure for storing MarkovDecisionVectors.
typedef std::unordered_map<size_t, MarkovDecisionVector> MarkovDecisionVectorMap;

/**
 * \brief This class implements a positive-reinforcement Markovian AI that
 *        plays the game in attack.
 */
class MarkovAIAttack : public EvolveAIAttack {
public:
    MarkovAIAttack();
    virtual ~MarkovAIAttack();

    game::Move const & getNextMove(game::Character const &me,
                                   game::Character const &opponent,
                                   bool far) override;

    void updateAfterMatch(game::Character const &me,
                          game::Character const &opponent,
                          bool has_won) override;

private:
    /// Markovian decision matrix for choosing the next move.
    MarkovDecisionMatrix m_markov_matrix;

    /// Last decisions, move, based on Markovian logic.
    MarkovDecisionVectorMap m_last_markov_decisions;

};

}
}

#endif
