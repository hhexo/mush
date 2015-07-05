//  Copyright 2015 Dario Domizioli
//
//  Licensed under the Apache License, Version 2.0 (the "License");
//  you may not use this file except in compliance with the License.
//  You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
//  Unless required by applicable law or agreed to in writing, software
//  distributed under the License is distributed on an "AS IS" BASIS,
//  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
//  See the License for the specific language governing permissions and
//  limitations under the License.

#include <iostream>
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <sstream>
#include <algorithm>

#include "core/game/Dice.h"
#include "core/game/Duel.h"
#include "core/chars/NamedCharacters.h"
#include "core/ctrl/InteractiveCtrl.h"
#include "core/ctrl/EvolveAICtrl.h"
#include "core/ctrl/ExpertSystemCtrl.h"

namespace {

using namespace core::game;
using namespace core::chars;
using namespace core::ctrl;

// --- MAIN STUFF

bool askTrueFalse(std::string message) {
    std::string reply;
    std::cout << message << std::endl;
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

int askNumber(std::string message) {
    std::string reply;
    std::cout << message << std::endl;
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

std::shared_ptr<Character> interactiveCreateCharacter() {
    // Name
    std::string name;
    std::cout << "Please input the character's name: ";
    std::getline(std::cin, name);
    // Stats
    int ra = 0; int at = 0; int df = 0;
    while(ra + at + df != 7) {
        std::cout << "Please input the character's stat values "
                  << "(that must add up to 7)." << std::endl;
        ra = askNumber("Choose character's RA.");
        at = askNumber("Choose character's AT.");
        df = askNumber("Choose character's DF.");
    }
    // SP Mode - trick with the iterable enum here. :-)
    int sp = -1;
    while(sp < 0 || sp >= (int)SP_END__) {
        std::cout << "Available SP gain modes:" << std::endl;
        for(auto sp : SPMode{}) {
            std::cout << (int)sp << ") " << toString(sp) << std::endl;
        }
        sp = askNumber("Choose the character's SP mode.");
    }
    // AI
    std::vector<std::pair<std::shared_ptr<AttackControl>, std::shared_ptr<DefendControl>>> ai_modes;
    for(int i = 0; i < getNumExpertSystemsCombinations(); ++i) {
        ai_modes.push_back(std::make_pair(getAttackExpertSystem(i), 
                                          getDefenceExpertSystem(i)));
    }
    ai_modes.push_back(std::make_pair(std::make_shared<EvolveAIAttack>(), 
                                      std::make_shared<EvolveAIDefence>()));
    ai_modes.push_back(std::make_pair(std::make_shared<MarkovAIAttack>(), 
                                      std::make_shared<EvolveAIDefence>()));
    std::cout << "Available AI combinations:" << std::endl;
    for(size_t i = 0; i < ai_modes.size(); ++i) {
        std::cout << i << ") " << ai_modes[i].first->getName() << "," 
                  << ai_modes[i].second->getName() << std::endl;
    }
    int ai = askNumber("Choose the character's AI combination.");
    // Create character.
    auto c = std::make_shared<Character>(name, ra, at, df, (SPMode)sp, 
                                         ai_modes[ai].first, 
                                         ai_modes[ai].second);
    // Now populate moves.
    std::cout << "We will now create the SPECIAL moves." << std::endl;
    for(int i = 0; i < 3; ++i) {
        std::string move_name;
        std::cout << "Please input the name of the move number " << i << ": ";
        std::getline(std::cin, move_name);
        std::vector<MoveSymbol> move_symbols;
        std::cout << "Available move symbols:" << std::endl;
        for(auto ms : MoveSymbol{}) {
            std::cout << (int)ms << ") " << toString(ms) << std::endl;
        }
        for(int j = 0; j < 4; ++j) {
            int reply = askNumber("Please choose a move symbol, or type a number higher than the maximum to finish.");
            if(reply >= (int)MS_END__) {
                break;
            }
            move_symbols.push_back((MoveSymbol)reply);
        }
        c->addMove(Move{move_name, MT_SPECIAL, move_symbols});
        std::cout << "Added move." << std::endl;
    }
    std::cout << "We will now create the SUPER move." << std::endl;
    for(int i = 0; i < 1; ++i) {
        std::string move_name;
        std::cout << "Please input the name of the move number " << i << ": ";
        std::getline(std::cin, move_name);
        std::vector<MoveSymbol> move_symbols;
        std::cout << "Available move symbols:" << std::endl;
        for(auto ms : MoveSymbol{}) {
            std::cout << (int)ms << ") " << toString(ms) << std::endl;
        }
        for(int j = 0; j < 4; ++j) {
            int reply = askNumber("Please choose a move symbol, or type a number higher than the maximum to finish.");
            if(reply >= (int)MS_END__) {
                break;
            }
            move_symbols.push_back((MoveSymbol)reply);
        }
        c->addMove(Move{move_name, MT_SUPER, move_symbols});
        std::cout << "Added move." << std::endl;
    }
    std::cout << "The resulting character is:" << std::endl;
    c->dump(std::cout);
    std::cout << std::endl;
    return c;
}


bool parseCommandLine(int argc, char* argv[]) {
    for(int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if(arg == std::string("-h")
                  || arg == std::string("--help")) {
            return false; // Just display help
        } else {
            std::cerr << "Invalid argument " << arg << std::endl;
            return false;
        }
    }
    return true;
}

int usage() {
    std::cout << "mush-versus - Musha Shugyo single match program" << std::endl
              << std::endl
              << "Usage:" << std::endl
              << "    mush-versus -h/--help" << std::endl
              << "or" << std::endl
              << "    mush-versus" << std::endl
              << std::endl
              << "This is an interactive program and will ask questions " << std::endl
              << "through the command line interface." << std::endl
              << std::endl;
    return 1;
}

void singleFight(std::shared_ptr<Character> c1, std::shared_ptr<Character> c2) {
    Duel d(c1, c2, &std::cout);
    d.fight();
}

void keepPickingFights(std::mutex *fights_mutex, 
                       std::vector<std::pair<std::shared_ptr<Character>, std::shared_ptr<Character>>> *fights) {
    while(true) {
        std::pair<std::shared_ptr<Character>, std::shared_ptr<Character>> f;
        if(true) { // Just to have a scope
            std::lock_guard<std::mutex> protect(*fights_mutex);
            if(fights->size() == 0) {
                break;
            }
            f = fights->back();
            fights->pop_back();
        }
        Duel d(f.first, f.second, nullptr);
        d.fight();
    }
}

int run() {
    bool p1_human = true;
    bool p2_human = false;
    int  p1_char = 0;
    int  p2_char = 0;
    bool original = false;
    bool siege_of_syde = false;
    bool created_character = false;
    std::vector<std::shared_ptr<Character>> characters;

    original = askTrueFalse("Do you want the original characters?");
    siege_of_syde = askTrueFalse("Do you want the Siege of Syde characters?");
    while(askTrueFalse("Do you want to create another character?")) {
        created_character = true;
        characters.push_back(interactiveCreateCharacter());
    }
    if(!original && !siege_of_syde && !created_character) {
        std::cout << "You have selected no characters! How can we have a match?"
                  << std::endl << "Sorry, exiting." << std::endl;
        return 1;
    }

    if(original) {
        std::vector<std::shared_ptr<Character>> original_characters = 
            core::chars::getOriginalCharacters();
        characters.insert(characters.end(),
                          original_characters.cbegin(), 
                          original_characters.cend());
    }
    if(siege_of_syde) {
        std::vector<std::shared_ptr<Character>> siege_of_syde_characters =
            core::chars::getSiegeOfSydeCharacters();;
        characters.insert(characters.end(),
                          siege_of_syde_characters.cbegin(), 
                          siege_of_syde_characters.cend());
    }

    p1_human = askTrueFalse("Is Player 1 human?");
    p2_human = askTrueFalse("Is Player 2 human?");

    // Handle AI training first, if needed.
    std::vector<std::pair<std::shared_ptr<Character>, std::shared_ptr<Character>>> fights;
    if(!p1_human || !p2_human) {
        int level = -1;
        while (level < 0 || level > 3) {
            std::cout << "Levels of AI proficiency:" << std::endl;
            std::cout << "0) A bit dumb" << std::endl;
            std::cout << "1) Confused" << std::endl;
            std::cout << "2) Focused" << std::endl;
            std::cout << "3) Highly trained" << std::endl;
            level = askNumber(
                "Which AI do you want? (high levels take longer to train)");
        }
        std::cout << std::endl 
                  << "Please wait while the AIs are being trained with these characters..."
                  << std::endl << std::endl;
        for(int i = 0; i < (level+2) * 100; ++i) {
            for(auto const &c1 : characters) {
                for(auto const &c2 : characters) {
                    if(c1 != c2) {
                        fights.push_back(std::make_pair(c1, c2));
                    }
                }
            }
        }
        std::vector<std::thread> threads;
        std::mutex fights_mutex;
        for(int i = 0; i < 4; ++i) {
            threads.push_back(
                std::thread(keepPickingFights, &fights_mutex, &fights));
        }
        for (auto& th : threads) th.join();
    }

    std::cout << "These are the available characters:" << std::endl;
    for(size_t i = 0; i < characters.size(); ++i) {
        std::cout << i << ") " << characters[i]->name << std::endl;
    }
    while(p1_char == p2_char) {
        std::cout << "Please choose two different characters for the match."
                  << std::endl;
        p1_char = askNumber("Which character do you want for Player 1?");
        p2_char = askNumber("Which character do you want for Player 2?");
    }

    if(p1_char >= characters.size()) {
        std::cout << "Invalid character for Player 1!!!" << std::endl;
        return 1;
    }
    if(p2_char >= characters.size()) {
        std::cout << "Invalid character for Player 2!!!" << std::endl;
        return 1;
    }

    std::cout << std::endl << "PLAYER 1'S CHARACTER:" << std::endl;
    characters[p1_char]->dump(std::cout);
    if(p1_human) {
        characters[p1_char]->actrl = std::make_shared<InteractiveAttackControl>();
        characters[p1_char]->dctrl = std::make_shared<InteractiveDefenceControl>();
    }

    std::cout << "PLAYER 2'S CHARACTER:" << std::endl;
    characters[p2_char]->dump(std::cout);
    if(p2_human) {
        characters[p2_char]->actrl = std::make_shared<InteractiveAttackControl>();
        characters[p2_char]->dctrl = std::make_shared<InteractiveDefenceControl>();
    }

    std::cout << std::endl << "--- LET THE FIGHT BEGIN!!! ---" << std::endl;

    singleFight(characters[p1_char], characters[p2_char]);

    return 0;
}

} // close anonymous namespace





/// \brief The main function for the single match program.
int main(int argc, char* argv[]) {
    if(!parseCommandLine(argc, argv)) {
        return usage();
    }
    return run();
}
