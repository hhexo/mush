 
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

#include "core/game/Dice.h"

namespace {
std::default_random_engine random_generator;
std::mutex rgen_mutex;
std::uniform_int_distribution<int> d6(1,6);
std::uniform_int_distribution<int> d20(1,20);
}

namespace core {
namespace game {

int d6WithCrit() {
    std::lock_guard<std::mutex> protect(rgen_mutex);
    int x = d6(random_generator);
    if(x == 6) {
        x = 8;
    }
    return x;
}

bool fuzz() {
    std::lock_guard<std::mutex> protect(rgen_mutex);
    int x = d20(random_generator);
    return x < 2;
}

std::mutex &getRandomGenMutex() { return rgen_mutex; }

std::default_random_engine &getRandomGenerator() { return random_generator; }

} 
}
