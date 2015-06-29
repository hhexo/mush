 
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

#ifndef CORE_GAME_CHARACTER_H
#define CORE_GAME_CHARACTER_H

#include "core/game/Move.h"
#include <string>
#include <vector>
#include <mutex>
#include <memory>
#include <ostream>

namespace core {

namespace ctrl {
    class AttackControl;
    class DefendControl;
}

namespace game {

/// \brief An enumeration identifying the possible modes for gaining SP.
enum SPMode : int {
    SP_COMBO,
    SP_DAMAGE,
    SP_AP,
    SP_WOUND,
    SP_DEFENCE,
    SP_END__,
    SP_BEGIN__ = SP_COMBO
};
/// \brief Needed to make the SPMode enum iterable.
SPMode operator++(SPMode&);
/// \brief Needed to make the SPMode enum iterable.
SPMode operator*(SPMode);
/// \brief Needed to make the SPMode enum iterable.
SPMode begin(SPMode);
/// \brief Needed to make the SPMode enum iterable.
SPMode end(SPMode);
/// \brief Returns a textual name for a SPMode enum item.
std::string const & toString(SPMode);

/**
 * \brief This class represents a Musha Shugyo character, with all their
 *        attributes and special moves; it also contains public data that is
 *        accessed and used during a fight.
 */
class Character {
public:
    /// \brief The main ctor takes name, RA, AT, DF, SP, the attack control
    ///        system, and the defend control system.
    ///        The Character takes shared ownership of 'att' and 'def'.
    Character(std::string const &n, int r, int a, int d, SPMode s, 
              std::shared_ptr<ctrl::AttackControl> att, 
              std::shared_ptr<ctrl::DefendControl> def);

    /// A Character cannot be copied or moved, but it can be cloned (see clone).
    Character(Character const &other) = delete;

    /// A Character cannot be copied or moved, but it can be cloned (see clone).
    Character(Character &&other) = delete;

    /// A Character cannot be copied or moved, but it can be cloned (see clone).
    Character& operator=(Character const &other) = delete;

    /// A Character cannot be copied or moved, but it can be cloned (see clone).
    Character& operator=(Character &&other) = delete;

    /// \brief This function is used to create clones to use for combat, while
    ///        the original Character instance is used as a model and to store
    ///        points. This allows for easy multi-threading.
    Character * clone() const;

    /// \brief Adds a move and take ownership; used for special and super moves.
    Character & addMove(Move const &move);

    /// \brief Debug dump.
    void dump(std::ostream &stream);

    /// \brief Brief dump.
    void brief(std::ostream &stream);

    /// \brief Character name. Public for ease of use.
    std::string const name;
    /// \brief Character RA. Public for ease of use.
    int const ra;
    /// \brief Character AT. Public for ease of use.
    int const at;
    /// \brief Character DF. Public for ease of use.
    int const df;
    /// \brief Character SP gain mode. Public for ease of use.
    SPMode const sp;
    /// \brief Character attack control. Public for ease of use.
    std::shared_ptr<ctrl::AttackControl> actrl;
    /// \brief Character defence control. Public for ease of use.
    std::shared_ptr<ctrl::DefendControl> dctrl;
    /// \brief Character moves. Public for ease of use.
    std::vector<Move> moves;

    /// \brief Current AP during a fight.
    int cur_ap;
    /// \brief Current SP during a fight.
    int cur_sp;
    /// \brief Current life points during a fight.
    int cur_life;
    /// \brief Current combo points during a fight.
    int cur_combo;
    /// \brief Is the character currently airborne?
    bool air;
    /// \brief Is the character currently prone?
    bool down;

    /// \brief Convenience counter for how many moves the character has
    ///        performed in the round.
    size_t nth_move_of_the_round;

    /// \brief Convenience storage for the last moves performed by this
    ///        Character; this is often inspected by the AI.
    std::vector<size_t> moves_performed;

    /// \brief Current score.
    int total_points;

    /// \brief Unique identifier for the Character; this is used by AIs in
    ///        multi-threaded mode to maintain contexts.
    size_t uid;

    /// \brief Mutex to protect the content.
    std::mutex content_mutex;

};

/// \brief Generates a character with random values, moves and AI.
std::shared_ptr<Character> generateRandomCharacter();

}
}

#endif
