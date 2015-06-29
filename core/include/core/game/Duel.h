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

#ifndef CORE_GAME_DUEL_H
#define CORE_GAME_DUEL_H

#include "core/game/Move.h"
#include "core/game/Character.h"

#include <memory>

namespace core {
namespace game {


/**
 * \brief This class is used to manage a fight between two characters. It
 *        embodies the combat rules of the Musha Shugyo game and runs a single
 *        fight until one of the contenders wins or the fight results in a draw.
 *        The class uses "clones" of the original Character instances, so that
 *        individual fights can be run in parallel in a multi-threaded
 *        environment. Decisions during the fight are delegated to the control
 *        systems of the Characters so that this class only cares about the game
 *        rules.
 */
class Duel {
public:
    /// \brief Main ctor. Takes two characters and creates clones for combat,
    ///        but keeps pointers to the originals to update results; also it
    ///        is possible to pass a stream to report progress to, if the stream
    ///        is null no progress is reported.
    Duel(std::shared_ptr<Character> c1, std::shared_ptr<Character> c2,
         std::ostream *report_stream);

    /// \brief Performs the fight.
    void fight();

    /// \brief Returns the flag storing whether the contenders are currently at
    ///        a distance.
    bool & far() { return m_far; }

private:
    /// \brief Runs a single turn with attacker and defender.
    void turn(Character &attacker, Character &defender);

    /// \brief Performs an attack (with a possible counter). Returns true if the 
    ///        attack was successful, false if unsuccessful; "interrupted" is
    ///        also set if the attack was interrupted/countered.
    bool performAttack(Character &attacker, Character &defender,
                       Move const &attack_move,
                       int modified_ap_cost, 
                       bool &interrupted);

    /// The first fighter, original.
    std::shared_ptr<Character> m_c1;
    /// The second fighter, original.
    std::shared_ptr<Character> m_c2;
    /// The report stream.
    std::ostream *m_report_stream;
    /// Clone of the first character; we own this.
    std::unique_ptr<Character> m_c1_clone;
    /// Clone of the second character; we own this.
    std::unique_ptr<Character> m_c2_clone;
    /// Are the contenders at a distance?
    bool m_far;
    /// Turn counter
    int m_turn_counter;
};

}
}

#endif 
