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

/// \file __namespaces_doc.h
/// \brief Contains the Doxygen documentation entries for the core namespaces.

/// \brief The top level namespace for the MUSH project.
namespace core {

/// \brief The namespace containing all the functionality implementing the game
///        system.
namespace game {}

/// \brief The namespace containing all the functionality implementing the
///        decision systems: AIs, Expert Systems, interactive systems...
namespace ctrl {}

/// \brief The namespace containing functionality to create specific sets of
///        characters like the ones from the original game or the Siege of Syde
///        expansion.
namespace chars {}

} // namespace core
