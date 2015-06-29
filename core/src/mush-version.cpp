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

/// \file mush-version.cpp
/// \brief Main file for a program that prints out the project version.

#include <stdio.h>
#include "core/Version.h"

/// \brief Main function for a program that just prints out the proejct
///        version.
int main() {
    printf("%s\n", core::getCoreVersionString().c_str());
    return 0;
}
