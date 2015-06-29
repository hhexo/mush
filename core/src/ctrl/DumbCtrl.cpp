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

#include "core/ctrl/DumbCtrl.h"
#include "core/game/Move.h"
#include "core/game/Character.h"

namespace core {
namespace ctrl {

DumbAttackControl::~DumbAttackControl() {}

int DumbAttackControl::shouldSpendAPToGainSP(game::Character const &me,
                          game::Character const &opponent,
                          bool far) {
    return 0;
}

bool DumbAttackControl::shouldSpendAPToFallStanding(game::Character const &me,
                                 game::Character const &opponent,
                                 bool far) {
    return false;
}

game::Move const & DumbAttackControl::getNextMove(game::Character const &me,
                               game::Character const &opponent,
                               bool far) {
    for(game::Move const &m : me.moves) {
        if(!m.isWait()) {
            return m;
        }
    }
    return game::getWaitMove();
}

bool DumbAttackControl::shouldSpendSPToLowerAPCost(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move,
                                int current_ap_cost) {
    return false;
}

bool DumbAttackControl::shouldSpendSPToConcatenate(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return false;
}

bool DumbAttackControl::shouldSpendSPForUltraAgility(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &my_move) {
    return false;
}

bool DumbAttackControl::shouldSpendSPToBoostAttack(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return false;
}

bool DumbAttackControl::shouldSpendSPToBoostDamage(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return false;
}

void DumbAttackControl::updateAfterMove(game::Character const &me,
                                        game::Character const &opponent,
                                        game::Move const &move,
                                        bool successful) {
    // Do nothing.
}

void DumbAttackControl::updateAfterMatch(game::Character const &me,
                                         game::Character const &opponent,
                                         bool has_won) {
    // Do nothing.
}


DumbDefendControl::~DumbDefendControl() {}

game::Move const & DumbDefendControl::getCounterMove(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &opponent_move) {
    return game::getWaitMove();
}

bool DumbDefendControl::shouldSpendSPToComboBreak(game::Character const &me,
                               game::Character const &opponent,
                               bool far,
                               game::Move const &opponent_move) {
    return false;
}

bool DumbDefendControl::shouldSpendSPToLowerAPCost(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move,
                                int current_ap_cost) {
    return false;
}

bool DumbDefendControl::shouldSpendSPForUltraAgility(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &my_move) {
    return false;
}

bool DumbDefendControl::shouldSpendSPToBoostDefence(game::Character const &me,
                                 game::Character const &opponent,
                                 bool far,
                                 game::Move const &my_move) {
    return false;
}

bool DumbDefendControl::shouldSpendSPToBoostCounterDamage(game::Character const &me,
                                       game::Character const &opponent,
                                       bool far,
                                       game::Move const &my_move) {
    return false;
}

void DumbDefendControl::updateAfterMove(game::Character const &me,
                                        game::Character const &opponent,
                                        game::Move const &move,
                                        bool successful) {
    // Do nothing.
}

void DumbDefendControl::updateAfterMatch(game::Character const &me,
                                         game::Character const &opponent,
                                         bool has_won) {
    // Do nothing.
}


}
}
