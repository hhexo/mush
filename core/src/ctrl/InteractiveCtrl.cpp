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

#include "core/ctrl/InteractiveCtrl.h"
#include "core/game/Move.h"
#include "core/game/Character.h"

#include <iostream>

namespace {

bool askTrueFalse(std::string name, std::string message) {
    std::string reply;
    std::cout << name << " - " << message << std::endl;
    while(true) {
        std::cout << "(Y/N) : " << std::flush;
        std::getline(std::cin, reply);
        if(reply == "Y" || reply == "y") {
            return true;
        }
        else if(reply == "N" || reply == "n") {
            return false;
        }
    }
    return false; // Just to satisfy compiler.
}

int askNumber(std::string name, std::string message) {
    std::string reply;
    std::cout << name << " - " << message << std::endl;
    while(true) {
        std::cout << "(number) : " << std::flush;
        std::getline(std::cin, reply);
        if(reply.empty()
           || reply.find_first_not_of("0123456789") != reply.npos) {
            continue;
        }
        return std::stoi(reply);
    }
    return 0; // Just to satisfy compiler.
}

}



namespace core {
namespace ctrl {

InteractiveAttackControl::~InteractiveAttackControl() {}

int InteractiveAttackControl::shouldSpendAPToGainSP(game::Character const &me,
                          game::Character const &opponent,
                          bool far) {
    return askNumber(me.name, "How many AP do you want to spend to gain SP?");
}

bool InteractiveAttackControl::shouldSpendAPToFallStanding(game::Character const &me,
                                 game::Character const &opponent,
                                 bool far) {
    return askTrueFalse(me.name, 
                        "Do you want to spend 1AP to avoid falling damage?");
}

game::Move const & InteractiveAttackControl::getNextMove(game::Character const &me,
                               game::Character const &opponent,
                               bool far) {
    std::cout << me.name << " - You have " << me.cur_ap << "AP and " 
              << me.cur_sp << "SP." << std::endl;
    std::cout << "You are " << (far ? "FAR FROM" : "CLOSE TO") 
              << " the opponent." << std::endl;
    std::cout << "Your available moves are:" << std::endl;
    for(size_t i = 0; i < me.moves.size(); ++i) {
        std::cout << i << ") " << me.moves[i].getName();
        if(!me.moves[i].isWait()) {
            std::cout << " [ ";
            for(auto const &s: me.moves[i].getSymbols()) {
                std::cout << toString(s) << " ";
            }
            std::cout << "]";
            if(me.moves[i].isSuper()) {
                std::cout << " - SUPER";
            }
        }
        std::cout << std::endl;
    }
    int res = askNumber(me.name, "Which move do you want to execute?");
    if(res >= 0 && res < me.moves.size()) {
        return me.moves[res];
    }
    std::cout << "What? That number is not in the list of moves! " << std::endl;
    std::cout << "You will be punished by skipping the turn! :-P" << std::endl;
    return game::getWaitMove();
}

bool InteractiveAttackControl::shouldSpendSPToLowerAPCost(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move,
                                int current_ap_cost) {
    std::cout << "The current AP cost of the move is " << current_ap_cost
              << "." << std::endl;
    return askTrueFalse(me.name, "Do you want to spend 1SP to reduce it by 1?");
}

bool InteractiveAttackControl::shouldSpendSPToConcatenate(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return askTrueFalse(me.name,
                        "Do you want to spend 1SP to concatenate the move?");
}

bool InteractiveAttackControl::shouldSpendSPForUltraAgility(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &my_move) {
    return askTrueFalse(me.name,
                        "Do you want to spend 1SP to power Ultra Agility?");
}

bool InteractiveAttackControl::shouldSpendSPToBoostAttack(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return askTrueFalse(me.name,
        "Do you want to spend 1SP to have a generic attack and damage boost?");
}

bool InteractiveAttackControl::shouldSpendSPToBoostDamage(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move) {
    return askTrueFalse(me.name,
                        "Do you want to spend 1SP to power Ultra Hardness?");
}

void InteractiveAttackControl::updateAfterMove(game::Character const &me,
                                     game::Character const &opponent,
                                     game::Move const &move, bool successful) {
    // Do nothing.
}

void InteractiveAttackControl::updateAfterMatch(game::Character const &me,
                                     game::Character const &opponent,
                                     bool has_won) {
    // Do nothing.
}


InteractiveDefenceControl::~InteractiveDefenceControl() {}

game::Move const & InteractiveDefenceControl::getCounterMove(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &opponent_move) {
    std::cout << me.name << " - The opponent is attacking with " 
              << opponent_move.getName() << "." << std::endl;
    std::cout << me.name << " - You have " << me.cur_ap << "AP and " 
              << me.cur_sp << "SP." << std::endl;
    std::cout << "You are " << (far ? "FAR FROM" : "CLOSE TO") 
              << " the opponent." << std::endl;
    if(!askTrueFalse(me.name, "Do you want to counter the move?")) {
        return game::getWaitMove();
    }
    std::cout << "Your available moves are:" << std::endl;
    for(size_t i = 0; i < me.moves.size(); ++i) {
        std::cout << i << ") " << me.moves[i].getName();
        if(!me.moves[i].isWait()) {
            std::cout << " [ ";
            for(auto const &s: me.moves[i].getSymbols()) {
                std::cout << toString(s) << " ";
            }
            std::cout << "]";
            if(me.moves[i].isSuper()) {
                std::cout << " - SUPER";
            }
        }
        std::cout << std::endl;
    }
    int res = askNumber(me.name, "Which move do you want to execute?");
    if(res >= 0 && res < me.moves.size()) {
        return me.moves[res];
    }
    std::cout << "What? That number is not in the list of moves! " << std::endl;
    std::cout << "You will be punished by not countering! :-P" << std::endl;
    return game::getWaitMove();
}

bool InteractiveDefenceControl::shouldSpendSPToComboBreak(game::Character const &me,
                               game::Character const &opponent,
                               bool far,
                               game::Move const &opponent_move) {
    return askTrueFalse(me.name,
                        "Do you want to spend 2SP to Combo Break?");
}

bool InteractiveDefenceControl::shouldSpendSPToLowerAPCost(game::Character const &me,
                                game::Character const &opponent,
                                bool far,
                                game::Move const &my_move,
                                int current_ap_cost) {
    std::cout << "The current AP cost of the counter is " << current_ap_cost
              << "." << std::endl;
    return askTrueFalse(me.name,
                        "Do you want to spend 1SP to reduce it by 1?");
}

bool InteractiveDefenceControl::shouldSpendSPForUltraAgility(game::Character const &me,
                                  game::Character const &opponent,
                                  bool far,
                                  game::Move const &my_move) {
    return askTrueFalse(me.name,
                        "Do you want to spend 1SP to power Ultra Agility?");
}

bool InteractiveDefenceControl::shouldSpendSPToBoostDefence(game::Character const &me,
                                 game::Character const &opponent,
                                 bool far,
                                 game::Move const &my_move) {
    return askTrueFalse(me.name,
        "Do you want to spend 1SP to have a generic defence boost?");
}

bool InteractiveDefenceControl::shouldSpendSPToBoostCounterDamage(game::Character const &me,
                                       game::Character const &opponent,
                                       bool far,
                                       game::Move const &my_move) {
    return askTrueFalse(me.name,
                        "Do you want to spend 1SP to power Ultra Hardness?");
}

void InteractiveDefenceControl::updateAfterMove(game::Character const &me,
                                     game::Character const &opponent,
                                     game::Move const &move, bool successful) {
    // Do nothing.
}

void InteractiveDefenceControl::updateAfterMatch(game::Character const &me,
                                     game::Character const &opponent,
                                     bool has_won) {
    // Do nothing.
}


}
}
