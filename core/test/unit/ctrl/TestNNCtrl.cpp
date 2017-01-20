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

#include "core/ctrl/NNCtrl.h"

#include <iostream>

using namespace core;
using namespace core::ctrl;

TEST_CASE( "NeuralNetwork", "[ctrl]" ) {

    SECTION("ORTest") {

        NN nn{2, 7, 1};

        auto inputs00 = std::vector<double>(2, 0.0);
        auto inputs01 = std::vector<double>(2, 0.0);
        inputs01[1] = 1.0;
        auto inputs10 = std::vector<double>(2, 0.0);
        inputs10[0] = 1.0;
        auto inputs11 = std::vector<double>(2, 1.0);
        auto outputs0 = std::vector<double>(1, 0.0);
        auto outputs1 = std::vector<double>(1, 1.0);
        for(size_t i = 0; i < 100000; ++i) {
            nn.backpropagate(inputs00, outputs0);
            nn.backpropagate(inputs01, outputs1);
            nn.backpropagate(inputs10, outputs1);
            nn.backpropagate(inputs11, outputs1);
        }

        std::cout << nn.feed(inputs00)[0] << std::endl;
        std::cout << nn.feed(inputs01)[0] << std::endl;
        std::cout << nn.feed(inputs10)[0] << std::endl;
        std::cout << nn.feed(inputs11)[0] << std::endl;

        REQUIRE(nn.feed(inputs00)[0] < 0.1);
        REQUIRE(nn.feed(inputs01)[0] > 0.9);
        REQUIRE(nn.feed(inputs10)[0] > 0.9);
        REQUIRE(nn.feed(inputs11)[0] > 0.9);
    }

    SECTION("ANDTest") {

        NN nn{2, 7, 1};

        auto inputs00 = std::vector<double>(2, 0.0);
        auto inputs01 = std::vector<double>(2, 0.0);
        inputs01[1] = 1.0;
        auto inputs10 = std::vector<double>(2, 0.0);
        inputs10[0] = 1.0;
        auto inputs11 = std::vector<double>(2, 1.0);
        auto outputs0 = std::vector<double>(1, 0.0);
        auto outputs1 = std::vector<double>(1, 1.0);
        for(size_t i = 0; i < 100000; ++i) {
            nn.backpropagate(inputs00, outputs0);
            nn.backpropagate(inputs01, outputs0);
            nn.backpropagate(inputs10, outputs0);
            nn.backpropagate(inputs11, outputs1);
        }

        std::cout << nn.feed(inputs00)[0] << std::endl;
        std::cout << nn.feed(inputs01)[0] << std::endl;
        std::cout << nn.feed(inputs10)[0] << std::endl;
        std::cout << nn.feed(inputs11)[0] << std::endl;

        REQUIRE(nn.feed(inputs00)[0] < 0.1);
        REQUIRE(nn.feed(inputs01)[0] < 0.1);
        REQUIRE(nn.feed(inputs10)[0] < 0.1);
        REQUIRE(nn.feed(inputs11)[0] > 0.9);
    }

    SECTION("XORTest") {

        NN nn{2, 7, 1};

        auto inputs00 = std::vector<double>(2, 0.0);
        auto inputs01 = std::vector<double>(2, 0.0);
        inputs01[1] = 1.0;
        auto inputs10 = std::vector<double>(2, 0.0);
        inputs10[0] = 1.0;
        auto inputs11 = std::vector<double>(2, 1.0);
        auto outputs0 = std::vector<double>(1, 0.0);
        auto outputs1 = std::vector<double>(1, 1.0);
        for(size_t i = 0; i < 100000; ++i) {
            nn.backpropagate(inputs00, outputs0);
            nn.backpropagate(inputs01, outputs1);
            nn.backpropagate(inputs10, outputs1);
            nn.backpropagate(inputs11, outputs0);
        }

        std::cout << nn.feed(inputs00)[0] << std::endl;
        std::cout << nn.feed(inputs01)[0] << std::endl;
        std::cout << nn.feed(inputs10)[0] << std::endl;
        std::cout << nn.feed(inputs11)[0] << std::endl;

        REQUIRE(nn.feed(inputs00)[0] < 0.1);
        REQUIRE(nn.feed(inputs01)[0] > 0.9);
        REQUIRE(nn.feed(inputs10)[0] > 0.9);
        REQUIRE(nn.feed(inputs11)[0] < 0.1);
    }

}
