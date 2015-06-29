 
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

#ifndef CORE_GAME_DICE_H
#define CORE_GAME_DICE_H


#include <random>
#include <mutex>
#include <thread>

namespace core {
namespace game {

/// \brief Returns the roll of a d6, with the 6 changed into an 8.
int d6WithCrit();

/// \brief Returns true with a very small probability.
bool fuzz();

/// \brief Gets the random generator mutex (for locking when using the random
///        generator).
std::mutex &getRandomGenMutex();

/// \brief Gets the random generator; be sure to lock the mutex first!
std::default_random_engine &getRandomGenerator();

} 
}

#endif
