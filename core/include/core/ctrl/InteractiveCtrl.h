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

#ifndef CORE_CTRL_INTERACTIVECTRL_H
#define CORE_CTRL_INTERACTIVECTRL_H

#include "core/ctrl/CtrlInterfaces.h"

namespace core {
namespace ctrl {

/**
 * \brief This class is a control system for attack which asks the user what to
 *        do for each choice it is going to make.
 */
class InteractiveAttackControl : public AttackControl {
public:
    virtual ~InteractiveAttackControl();

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
};

/**
 * \brief This class is a control system for defence which asks the user what to
 *        do for each choice it is going to make.
 */
class InteractiveDefenceControl : public DefendControl {
public:
    ~InteractiveDefenceControl();

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
};

}
}

#endif
