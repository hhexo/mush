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

/// \file mush-dumb-test.cpp
/// \brief Main file for a program that just runs a fight between two random
///        dumb characters.

#include "core/game/Character.h"
#include "core/game/Duel.h"

#include <iostream>

/// \brief Main function for a program that runs a fight between two random
///        dumb characters.
int main() {
    std::ostream *report_stream = &std::cout;

    auto c1 = core::game::generateRandomCharacter();
    auto c2 = core::game::generateRandomCharacter();

    core::game::Duel d{c1, c2, report_stream};

    d.fight();

    c1->dump(std::cout);
    c2->dump(std::cout);

    return 0;
}
