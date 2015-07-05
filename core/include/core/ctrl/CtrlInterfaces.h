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

#ifndef CORE_CTRL_INTERFACES_H
#define CORE_CTRL_INTERFACES_H

// Just to have size_t.
#include <string>

namespace core {

namespace game {
class Move;
class Character;
}

namespace ctrl {

/**
 * \brief This class represents the interface of a control system for the attack
 *        decisions; it can be an AI or even a user interface.
 *
 * Control systems can maintain 'instances'; the same control system can be used
 * concurrently in different matches.
 */
class AttackControl {
public:
    /// \brief Virtual destructor.
    virtual ~AttackControl();

    /// \brief Provides the name of the control system.
    virtual char const * const getName() const = 0;

    /// \brief Decides how many AP to spend to gain SP (in SP_AP mode).
    virtual int shouldSpendAPToGainSP(game::Character const &me,
                                      game::Character const &opponent,
                                      bool far) = 0;

    /// \brief Decides whether to spend an AP to avoid falling damage.
    virtual bool shouldSpendAPToFallStanding(game::Character const &me,
                                             game::Character const &opponent,
                                             bool far) = 0;

    /// \brief Decides the next move based on the parameters; if the next move
    ///        is a MT_WAIT move, it means that we pass.
    virtual game::Move const & getNextMove(game::Character const &me,
                                           game::Character const &opponent,
                                           bool far) = 0;

    /// \brief Decides whether to spend an SP to lower the AP cost of the move.
    virtual bool shouldSpendSPToLowerAPCost(game::Character const &me,
                                            game::Character const &opponent,
                                            bool far,
                                            game::Move const &my_move,
                                            int current_ap_cost) = 0;

    /// \brief Decides whether to spend an SP to concatenate the move.
    virtual bool shouldSpendSPToConcatenate(game::Character const &me,
                                            game::Character const &opponent,
                                            bool far,
                                            game::Move const &my_move) = 0;

    /// \brief Decides whether to spend an SP for Ultra Agility.
    virtual bool shouldSpendSPForUltraAgility(game::Character const &me,
                                              game::Character const &opponent,
                                              bool far,
                                              game::Move const &my_move) = 0;

    /// \brief Decides whether to spend an SP to boost the attack in general.
    virtual bool shouldSpendSPToBoostAttack(game::Character const &me,
                                            game::Character const &opponent,
                                            bool far,
                                            game::Move const &my_move) = 0;

    /// \brief Decides whether to spend an SP to boost damage with Hardness.
    virtual bool shouldSpendSPToBoostDamage(game::Character const &me,
                                            game::Character const &opponent,
                                            bool far,
                                            game::Move const &my_move) = 0;

    /// \brief Updates the state of the control system after a move, depending
    ///        on whether it was successful or not. Useful for AIs.
    virtual void updateAfterMove(game::Character const &me, 
                                 game::Character const &opponent,
                                 game::Move const &move,
                                 bool successful) = 0;

    /// \brief Updates the state of the control system after a match, depending
    ///        on whether it was won or lost. Useful for AIs.
    virtual void updateAfterMatch(game::Character const &me,
                                  game::Character const &opponent,
                                  bool has_won) = 0;
};

/**
 * \brief This class represents the interface of a control system for the defend
 *        decisions; it can be an AI or even a user interface.
 */
class DefendControl {
public:
    /// \brief Virtual destructor.
    virtual ~DefendControl();

    /// \brief Provides the name of the control system.
    virtual char const * const getName() const = 0;

    /// \brief Decides the counter move based on the parameters; if the move
    ///        is a MT_WAIT move, it means that we do not wish to counter.
    virtual game::Move const & getCounterMove(game::Character const &me,
                                              game::Character const &opponent,
                                              bool far,
                                              game::Move const &opponent_move) = 0;

    /// \brief Decides whether to spend an SP to break out a concatenation.
    virtual bool shouldSpendSPToComboBreak(game::Character const &me,
                                           game::Character const &opponent,
                                           bool far,
                                           game::Move const &opponent_move) = 0;

    /// \brief Decides whether to spend SP to lower the AP cost of the counter.
    virtual bool shouldSpendSPToLowerAPCost(game::Character const &me,
                                            game::Character const &opponent,
                                            bool far,
                                            game::Move const &my_move,
                                            int current_ap_cost) = 0;

    /// \brief Decides whether to spend an SP for Ultra Agility.
    virtual bool shouldSpendSPForUltraAgility(game::Character const &me,
                                              game::Character const &opponent,
                                              bool far,
                                              game::Move const &my_move) = 0;

    /// \brief Decides whether to spend an SP to boost the defence in general.
    virtual bool shouldSpendSPToBoostDefence(game::Character const &me,
                                             game::Character const &opponent,
                                             bool far,
                                             game::Move const &my_move) = 0;

    /// \brief Decides whether to spend an SP to boost the defence in general.
    virtual bool shouldSpendSPToBoostCounterDamage(game::Character const &me,
                                                   game::Character const &opponent,
                                                   bool far,
                                                   game::Move const &my_move) = 0;

    /// \brief Updates the state of the control system after a move, depending
    ///        on whether it was successful or not. Useful for AIs.
    virtual void updateAfterMove(game::Character const &me,
                                 game::Character const &opponent,
                                 game::Move const &move,
                                 bool successful) = 0;

    /// \brief Updates the state of the control system after a match, depending
    ///        on whether it was won or lost. Useful for AIs.
    virtual void updateAfterMatch(game::Character const &me,
                                  game::Character const &opponent,
                                  bool has_won) = 0;
};

}
}

#endif
