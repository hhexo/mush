 
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

#ifndef CORE_GAME_MOVE_H
#define CORE_GAME_MOVE_H

#include <string>
#include <map>
#include <vector>

namespace core {
namespace game {

/**
 * \brief An enumeration representing the possible Move Symbols in Musha Shugyo.
 */
enum MoveSymbol : int {
    MS_2XCOMBO,
    MS_BONUS_AIR,
    MS_BONUS_DOWN,
    MS_BONUS_FAR,
    MS_BONUS_NEAR,
    MS_BONUS_COUNTER,
    MS_FALL,
    MS_DISTANCE,
    MS_POWERFUL,
    MS_JUMPOK,
    MS_THROW,
    MS_SMASH,
    MS_PUSH,
    MS_DASH,
    MS_REFLECT,
    MS_ULTRA_AGILITY,
    MS_ULTRA_HARDNESS,
    MS_END__,
    MS_BEGIN__ = MS_2XCOMBO
};
/// \brief Needed to make the MoveSymbol enum iterable.
MoveSymbol operator++(MoveSymbol&);
/// \brief Needed to make the MoveSymbol enum iterable.
MoveSymbol operator*(MoveSymbol);
/// \brief Needed to make the MoveSymbol enum iterable.
MoveSymbol begin(MoveSymbol);
/// \brief Needed to make the MoveSymbol enum iterable.
MoveSymbol end(MoveSymbol);
/// \brief Returns a textual name for a MoveSymbol enum item.
std::string const & toString(MoveSymbol);

/**
 * \brief An enumeration representing the move types: wait (do nothing), normal, 
 *        special or super.
 */
enum MoveType: int {
    MT_WAIT,
    MT_NORMAL,
    MT_SPECIAL,
    MT_SUPER,
    MT_END__,
    MT_BEGIN__ = MT_WAIT
};
/// \brief Needed to make the MoveType enum iterable.
MoveType operator++(MoveType&);
/// \brief Needed to make the MoveType enum iterable.
MoveType operator*(MoveType);
/// \brief Needed to make the MoveType enum iterable.
MoveType begin(MoveType);
/// \brief Needed to make the MoveType enum iterable.
MoveType end(MoveType);
/// \brief Returns a textual name for a MoveType enum item.
std::string const & toString(MoveType);

/**
 * \brief Represents a Move in Musha Shugyo. A Move is a collection of Symbols
 *        and can either be a normal attack, a special move, or a super move.
 *        Once created, a Move is immutable.
 */
class Move {
public:
    /// \brief Constructor takes the name and type of the move and its symbols.
    Move(std::string name, MoveType type, 
         std::vector<MoveSymbol> const &symbols);

    /// \brief A Move can be copied.
    Move(Move const &other) = default;

    /// \brief A Move can be moved.
    Move(Move &&other) = default;

    /// \brief A Move can be copy-assigned.
    Move& operator=(Move const &other) = default;

    /// \brief A Move can be move-assigned.
    Move& operator=(Move &&other) = default;

    /// \brief Equality operator.
    bool operator==(Move const &other) const;

    /// \brief Strict ordering operator.
    bool operator<(Move const &other) const;

    /// \brief Returns the name of the move.
    std::string const & getName() const { return m_name; }

    /// \brief Returns the type of the move.
    MoveType getType() const { return m_type; }

    /// \brief Returns the move symbols.
    std::vector<MoveSymbol> const & getSymbols() const { return m_symbols; }

    /// \brief Returns true if the move has the specified symbol.
    bool hasSymbol(MoveSymbol sym) const;

    /// \brief Calculates the AP cost of the move; parameters specify whether
    ///        the target is distant and whether to consider a counter cost.
    int apCost(bool is_far, bool is_countering) const;

    /// \brief Calculates the move damage based on the provided test result,
    ///        character's values, and SP spent to increase damage.
    int damage(int test_result, int character_at, int character_df, 
               int sp_spent) const;

    /// \brief Calculates the move damage for a counter; this could be based on
    //         the provided test result, character's values, and SP spent to 
    ///        increase damage, OR it could be based on the reflection of the
    ///        opponent's attack, plus any bonus derived from the move itself.
    int counterDamage(int test_result, int character_at, int character_df,
                      int sp_spent, int reflected) const;

    /// \brief Modifies the character's and opponent's states based on the move,
    ///        before resolution.
    void modifyStatePreResolution(bool &far, 
                                  bool &own_air, bool &own_down, 
                                  bool &other_air, bool &other_down) const;

    /// \brief Modifies the character's and opponent's states based on the move,
    ///        after resolution.
    void modifyStatePostResolution(bool &far, 
                                   bool &own_air, bool &own_down, 
                                   bool &other_air, bool &other_down) const;

    /// \brief Returns the number of combo points gained from the move.
    int comboPoints() const;

    /// \brief Returns true if the move is MT_WAIT.
    bool isWait() const { return m_type == MT_WAIT; }

    /// \brief Returns true if the move is MT_NORMAL.
    bool isNormal() const { return m_type == MT_NORMAL; }

    /// \brief Returns true if the move is MT_SPECIAL.
    bool isSpecial() const { return m_type == MT_SPECIAL; }

    /// \brief Returns true if the move is MT_SUPER.
    bool isSuper() const { return m_type == MT_SUPER; }

    /// \brief Returns true if the move has the MS_REFLECT symbol.
    bool hasReflect() const { return hasSymbol(MS_REFLECT); }

    /// \brief Returns true if the move has the MS_JUMPOK symbol.
    bool hasJumpOk() const { return hasSymbol(MS_JUMPOK); }

    /// \brief Returns true if the move has the MS_THROW symbol.
    bool hasThrow() const { return hasSymbol(MS_THROW); }

    /// \brief Returns true if the move has the MS_FALL symbol.
    bool hasFall() const { return hasSymbol(MS_FALL); }

    /// \brief Returns true if the move has the MS_SMASH symbol.
    bool hasSmash() const { return hasSymbol(MS_SMASH); }

    /// \brief Returns true if the move has the MS_DISTANCE or MS_DASH symbol.
    bool canHitDistance() const {
        return hasSymbol(MS_DISTANCE) || hasSymbol(MS_DASH);
    }

    /// \brief Returns true if the move has the MS_ULTRA_HARDNESS symbol.
    bool hasUltraAgility() const { return hasSymbol(MS_ULTRA_AGILITY); }

    /// \brief Returns true if the move has the MS_ULTRA_HARDNESS symbol.
    bool hasUltraHardness() const { return hasSymbol(MS_ULTRA_HARDNESS); }

private:
    std::string m_name;
    MoveType m_type;
    std::vector<MoveSymbol> m_symbols;
};

/// \brief Gets the single standard "Wait" move.
Move const & getWaitMove();

/**
 * \brief This function returns all the standard moves considered in this
 *        implementation; these include the standard "Wait" move and all the
 *        normal attacks.
 */
std::vector<Move> const & getStandardMoves();

}
}

#endif
