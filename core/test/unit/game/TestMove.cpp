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

#include "catch/catch.hpp"

#include "core/game/Move.h"

using namespace core;
using namespace core::game;

TEST_CASE( "Move", "[game]" ) {

    SECTION("GenericTest") {

        std::vector<Move> moves = getStandardMoves();

        REQUIRE(getWaitMove() == moves[0]);
        REQUIRE(&getWaitMove() == &getStandardMoves()[0]);

        REQUIRE(moves[1].getName() == "Punch");
        REQUIRE(moves[1].getType() == MT_NORMAL);
        REQUIRE(moves[1].getSymbols().size() == 0);

        REQUIRE(moves[2].getName() == "Kick");
        REQUIRE(moves[2].getType() == MT_NORMAL);
        REQUIRE(moves[2].getSymbols().size() == 1);
        REQUIRE(moves[2].getSymbols()[0] == MS_POWERFUL);

        REQUIRE(moves[2].apCost(false, false) == 3);
        REQUIRE(moves[2].apCost(true, false) == 4);

        // AirKick
        REQUIRE(moves[5].apCost(false, false) == 4);
        REQUIRE(moves[5].apCost(true, false) == 4);

        // Dash
        REQUIRE(moves[6].apCost(false, false) == 3);
        REQUIRE(moves[6].apCost(true, false) == 3);
    }

}
