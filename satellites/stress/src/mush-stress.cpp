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


/// \file mush-stress.cpp
/// \brief Main file for the MUSH Stress Test.

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

namespace {

using namespace core::game;
using namespace core::chars;

// --- MAIN STUFF
bool progress = false;
bool verbose = false;
bool threaded = false;
int extra_chars = 0;

bool parseCommandLine(int argc, char* argv[]) {
    for(int i = 1; i < argc; ++i) {
        std::string arg = argv[i];
        if(arg == std::string("-v")) {
            verbose = true;
        } else if(arg == std::string("-p")) {
            progress = true;
        } else if(arg == std::string("-t")) {
            threaded = true;
        } else if(arg[0] == '-' && arg[1] == 'c') {
            if(arg.length() > 2) {
                extra_chars = std::stoi(arg.substr(2, arg.npos).c_str());
            }
            else if(i + 1 < argc) {
                ++i;
                extra_chars = std::stoi(argv[i]);
            }
        } else if(arg[0] == '-' && arg[1] == 's') {
            if(arg.length() > 2) {
                getRandomGenerator().seed(std::stoul(arg.substr(2, arg.npos).c_str()));
            }
            else if(i + 1 < argc) {
                ++i;
                getRandomGenerator().seed(std::stoul(argv[i]));
            }
        } else if(arg == std::string("-h")
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
    std::cout << "mush-stress - Musha Shugyo stress test program" << std::endl
              << std::endl
              << "Usage:" << std::endl
              << "    mush -h" << std::endl
              << "or" << std::endl
              << "    mush [-c <number>] [-p] [-v] [-t] [-s <number>]" << std::endl
              << std::endl
              << "Options:" << std::endl
              << "    -h     : Display this help and exit." << std::endl
              << "    -c <n> : Number of extra random characters." << std::endl
              << "    -p     : Print progress on stderr." << std::endl
              << "    -v     : Verbose (chronicle for debugging)." << std::endl
              << "    -t     : Multi-threaded mode. CPU intensive." << std::endl
              << "    -s <n> : Random number generator seed." << std::endl
              << std::endl;
    return 1;
}

void singleFight(std::shared_ptr<Character> c1, std::shared_ptr<Character> c2) {
    Duel d(c1, c2, verbose ? &std::cout : nullptr);
    d.fight();
    if(progress) {
        std::cerr << c1->name << " vs " << c2->name << " just ended."
                  << std::endl;
    }
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
            if(progress) {
                std::cerr << f.first->name << " vs " << f.second->name
                          << " is starting." << std::endl;
            }
        }
        Duel d(f.first, f.second, verbose ? &std::cout : nullptr);
        d.fight();
    }
}

int run() {
    std::vector<std::shared_ptr<Character>> characters;
    std::vector<std::shared_ptr<Character>> original_characters = 
        core::chars::getOriginalCharacters();
    characters.insert(characters.end(),
                      original_characters.cbegin(), 
                      original_characters.cend());
    std::vector<std::shared_ptr<Character>> siege_of_syde_characters =
        core::chars::getSiegeOfSydeCharacters();;
    characters.insert(characters.end(),
                      siege_of_syde_characters.cbegin(), 
                      siege_of_syde_characters.cend());
    for(int i = 0; i < extra_chars; ++i) {
        auto c = generateRandomCharacter();
        characters.push_back(c);
    }

    // Handle automated batch of fights first...
    std::vector<std::pair<std::shared_ptr<Character>, std::shared_ptr<Character>>> fights;
    for(int i = 0; i < (extra_chars == 0 ? 500 : 5); ++i) {
        for(auto const &c1 : characters) {
            for(auto const &c2 : characters) {
                if(c1 != c2) {
                    fights.push_back(std::make_pair(c1, c2));
                }
            }
        }
    }
    if(threaded) {
        std::vector<std::thread> threads;
        std::mutex fights_mutex;
        for(int i = 0; i < 4; ++i) {
            threads.push_back(
                std::thread(keepPickingFights, &fights_mutex, &fights));
        }
        for (auto& th : threads) th.join();
    } else {
        for(auto &f: fights) {
            singleFight(f.first, f.second);
        }
    }

    std::sort(characters.begin(), characters.end(), 
              [](std::shared_ptr<Character> c1, std::shared_ptr<Character>  c2){
                return c1->total_points < c2->total_points; 
              });
    for(auto const &c : characters) {
        c->dump(std::cout);
    }

    return 0;
}

} // close anonymous namespace

/// \brief The main function for the stress test program.
int main(int argc, char* argv[]) {
    if(!parseCommandLine(argc, argv)) {
        return usage();
    }
    return run();
}
