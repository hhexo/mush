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

#ifndef CORE_CTRL_EXPERTSYSTEMCTRL_H
#define CORE_CTRL_EXPERTSYSTEMCTRL_H

#include "core/ctrl/CtrlInterfaces.h"
#include "core/ctrl/DumbCtrl.h"

#include <memory>

namespace core {
namespace ctrl {



// ========================================================================
//    ATTACK EXPERT SYSTEMS
// ========================================================================



/**
 * \brief This class implements the Grunt expert system in attack.
 */
class GruntAtk : public DumbAttackControl {
public:
    virtual ~GruntAtk();

    char const * const getName() const override;

    int shouldSpendAPToGainSP(game::Character const &me,
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
};

/**
 * \brief This class implements the Brawler expert system in attack.
 */
class BrawlerAtk : public DumbAttackControl {
public:
    virtual ~BrawlerAtk();

    char const * const getName() const override;

    int shouldSpendAPToGainSP(game::Character const &me,
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
};


/**
 * \brief This class implements the Buffer expert system in attack.
 */
class BufferAtk : public DumbAttackControl {
public:
    virtual ~BufferAtk();

    char const * const getName() const override;

    int shouldSpendAPToGainSP(game::Character const &me,
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
};

/**
 * \brief This class implements the CounterAttacker expert system in attack.
 */
class CounterAttackerAtk : public DumbAttackControl {
public:
    virtual ~CounterAttackerAtk();

    char const * const getName() const override;

    int shouldSpendAPToGainSP(game::Character const &me,
                              game::Character const &opponent,
                              bool far) override;

    game::Move const & getNextMove(game::Character const &me,
                                   game::Character const &opponent,
                                   bool far) override;
};

/**
 * \brief This class implements the KillerChain expert system in attack.
 */
class KillerChainAtk : public DumbAttackControl {
public:
    virtual ~KillerChainAtk();

    char const * const getName() const override;

    int shouldSpendAPToGainSP(game::Character const &me,
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
};

/**
 * \brief This class implements the Kite or Sniper expert system in attack.
 */
class KiteSniperAtk : public DumbAttackControl {
public:
    virtual ~KiteSniperAtk();

    char const * const getName() const override;

    int shouldSpendAPToGainSP(game::Character const &me,
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
};

/**
 * \brief This class implements the Sniper expert system in attack.
 */
class SniperAtk : public DumbAttackControl {
public:
    virtual ~SniperAtk();

    char const * const getName() const override;

    int shouldSpendAPToGainSP(game::Character const &me,
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
};

/**
 * \brief This class implements the Balanced expert system in attack.
 */
class BalancedAtk : public DumbAttackControl {
public:
    virtual ~BalancedAtk();

    char const * const getName() const override;

    int shouldSpendAPToGainSP(game::Character const &me,
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
};

/**
 * \brief This class implements the Tactical expert system in attack.
 */
class TacticalAtk : public DumbAttackControl {
public:
    virtual ~TacticalAtk();

    char const * const getName() const override;

    int shouldSpendAPToGainSP(game::Character const &me,
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
};



// ========================================================================
//    DEFENCE EXPERT SYSTEMS
// ========================================================================



/**
 * \brief This class implements the Grunt expert system in defence.
 */
class GruntDef : public DumbDefendControl {
public:
    virtual ~GruntDef();

    char const * const getName() const override;

    game::Move const & getCounterMove(game::Character const &me,
                                      game::Character const &opponent,
                                      bool far,
                                      game::Move const &opponent_move) override;
};

/**
 * \brief This class implements the Brawler expert system in defence.
 */
class BrawlerDef : public DumbDefendControl {
public:
    virtual ~BrawlerDef();

    char const * const getName() const override;

    game::Move const & getCounterMove(game::Character const &me,
                                      game::Character const &opponent,
                                      bool far,
                                      game::Move const &opponent_move) override;
};

/**
 * \brief This class implements the Buffer expert system in defence.
 */
class BufferDef : public DumbDefendControl {
public:
    virtual ~BufferDef();

    char const * const getName() const override;

    game::Move const & getCounterMove(game::Character const &me,
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
};

/**
 * \brief This class implements the CounterAttacker expert system in defence.
 */
class CounterAttackerDef : public DumbDefendControl {
public:
    virtual ~CounterAttackerDef();

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
};

/**
 * \brief This class implements the Killer Chain expert system in defence.
 */
class KillerChainDef : public DumbDefendControl {
public:
    virtual ~KillerChainDef();

    char const * const getName() const override;

    game::Move const & getCounterMove(game::Character const &me,
                                      game::Character const &opponent,
                                      bool far,
                                      game::Move const &opponent_move) override;
};

/**
 * \brief This class implements the Kite expert system in defence.
 */
class KiteDef : public DumbDefendControl {
public:
    virtual ~KiteDef();

    char const * const getName() const override;

    game::Move const & getCounterMove(game::Character const &me,
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
};

/**
 * \brief This class implements the Sniper expert system in defence.
 */
class SniperDef : public DumbDefendControl {
public:
    virtual ~SniperDef();

    char const * const getName() const override;

    game::Move const & getCounterMove(game::Character const &me,
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
};

/**
 * \brief This class implements the Balanced expert system in defence.
 */
class BalancedDef : public DumbDefendControl {
public:
    virtual ~BalancedDef();

    char const * const getName() const override;

    game::Move const & getCounterMove(game::Character const &me,
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
};

/**
 * \brief This class implements the Tactical expert system in defence.
 */
class TacticalDef : public DumbDefendControl {
public:
    ~TacticalDef();

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
};



// ========================================================================
//    UTILITY FUNCTIONS
// ========================================================================



/**
 * \brief For character generation; this function returns how many combinations
 *        of expert systems (attack and defence) are allowed.
 */
int getNumExpertSystemsCombinations();

/**
 * \brief For character generation; this function returns the attack control
 *        system for the specified combination.
 */
std::shared_ptr<AttackControl> getAttackExpertSystem(int combination);

/**
 * \brief For character generation; this function returns the defence control
 *        system for the specified combination.
 */
std::shared_ptr<DefendControl> getDefenceExpertSystem(int combination);

}
}

#endif
