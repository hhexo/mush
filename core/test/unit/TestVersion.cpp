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

#include "core/Version.h"

using namespace core;

TEST_CASE( "VersionNumber", "[version]" ) {

    SECTION("MajorMinorPatch") {
        REQUIRE(getCoreMajorVersion() == 1);
        REQUIRE(getCoreMinorVersion() == 0);
        REQUIRE(getCorePatchVersion() == 0);
    }

}
