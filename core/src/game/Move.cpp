 
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

#include "core/game/Move.h"

namespace core {
namespace game {

// Move Symbol

static std::string const s_move_symbol_names[] = {
    "2xCombo", "Bonus(air)", "Bonus(down)", "Bonus(far)", "Bonus(near)", 
    "Bonus(counter)", "Fall",
    "Distance", "Powerful", "JumpOK", "Throw", "Smash", "Push", "Dash",
    "Reflect", "UltraAgility", "UltraHardness"
};
MoveSymbol operator++(MoveSymbol &m) { 
    auto i = std::underlying_type<MoveSymbol>::type(m) + 1;
    return m = (MoveSymbol)(i);
}
MoveSymbol operator*(MoveSymbol m) { return m; }
MoveSymbol begin(MoveSymbol) { return MS_BEGIN__; }
MoveSymbol end(MoveSymbol) { return MS_END__; }
std::string const & toString(MoveSymbol m) {
    return s_move_symbol_names[static_cast<int>(m)];
}

// Move Type

static std::string const s_move_type_names[] = {
    "Wait", "Normal", "Special", "Super"
};
MoveType operator++(MoveType& m) {
    auto i = std::underlying_type<MoveType>::type(m) + 1;
    return m = (MoveType)(i);
}
MoveType operator*(MoveType m) { return m; }
MoveType begin(MoveType) { return MT_BEGIN__; }
MoveType end(MoveType) { return MT_END__; }
std::string const & toString(MoveType m)  {
    return s_move_symbol_names[static_cast<int>(m)];
}

// Move class

Move::Move(std::string name, MoveType type, 
           std::vector<MoveSymbol> const &symbols)
: m_name(name), m_type(type), m_symbols(symbols) {}

bool Move::operator==(Move const &other) const {
    return m_type == other.m_type
           && m_name == other.m_name 
           && m_symbols == other.m_symbols;
}

bool Move::operator<(Move const &other) const {
    if(m_type < other.m_type) {
        return true;
    } else if(other.m_type < m_type) {
        return false;
    }
    if(m_name < other.m_name) {
        return true;
    } else if(other.m_name < m_name) {
        return false;
    }
    return m_symbols < other.m_symbols;
}

bool Move::hasSymbol(MoveSymbol sym) const {
    for(auto const &ms : m_symbols) {
        if(ms == sym) {
            return true;
        }
    }
    return false;
}

int Move::apCost(bool is_far, bool is_countering) const {
    int cost = 0;
    if(m_type == MT_WAIT) {
        return cost;
    } else if(m_type == MT_NORMAL) {
        cost = 1 + 2 * m_symbols.size();
    } else if(m_type == MT_SPECIAL) {
        cost = 1 + m_symbols.size();
    } else if(m_type == MT_SUPER) {
        cost = m_symbols.size();
    }
    // Consider movement.
    if(is_far) {
        if(!canHitDistance()) {
            return cost + 1; // Regardless of jumping (jump is included).
        }
    } 
    // It could still cost 1 more if we want to jump.
    if(hasJumpOk()) {
        return cost + 1;
    }
    // It could cost more if it's a counter, unless it has MS_REFLECT.
    if(is_countering && !hasReflect()) {
        cost += 1;
    }
    return cost;
}

int Move::damage(int test_result, int character_at, int character_df, 
                 int sp_spent) const {
    if(m_type == MT_WAIT) {
        return 0;
    }
    // Check for damage/at modifiers.
    if(hasJumpOk()) {
        character_at = character_df;
    }
    for(auto const &ms : m_symbols) {
        if(ms == MS_POWERFUL) {
            character_at += 1;
        }
    }
    // Check for move type
    int damage = 0;
    if(m_type == MT_NORMAL) {
        damage = test_result + character_at;
    } else if(m_type == MT_SPECIAL) {
        damage = test_result + 2 * character_at;
    } else if(m_type == MT_SUPER) {
        damage = test_result + 4 * character_at;
    }
    // Process Ultra Hardness after multiplication.
    for(auto const &ms : m_symbols) {
        if(ms == MS_ULTRA_HARDNESS) {
            damage += 2 * sp_spent;
        }
    }
    return damage;
}

int Move::counterDamage(int test_result, int character_at, int character_df, 
                        int sp_spent, int reflected) const {
    if(m_type == MT_WAIT) {
        return 0;
    }
    if(!hasReflect()) {
        return damage(test_result, character_at, character_df, sp_spent);
    }
    // Check for damage/at modifiers.
    int multiplier = 1;
    if(m_type == MT_SPECIAL) {
        multiplier = 2;
    } else if(m_type == MT_SUPER) {
        multiplier = 4;
    }
    for(auto const &ms : m_symbols) {
        if(ms == MS_POWERFUL) {
            reflected += multiplier;
        } else if(ms == MS_ULTRA_HARDNESS) {
            reflected += 2 * sp_spent;
        }
    }
    return reflected;
}

void Move::modifyStatePreResolution(bool &far, 
                                    bool &own_air, bool &own_down, 
                                    bool &other_air, bool &other_down) const {
    // Distance state
    if(far) {
        // We must have moved to hit, unless we have the distance symbol.
        if(!hasSymbol(MS_DISTANCE)) {
            far = false;
        }
    }
    // Own state: if we're jumping, we're in air.
    if(hasJumpOk()) {
        own_air = true;
        own_down = false;
    } else {
        // Otherwise there's no reason to be in any state.
        own_air = false;
        own_down = false;
    }
}

void Move::modifyStatePostResolution(bool &far, 
                                     bool &own_air, bool &own_down, 
                                     bool &other_air, bool &other_down) const {
    // Distance state
    if(hasSymbol(MS_PUSH)) {
        // We push to be far anyway.
        far = true;
    }
    // Own state: if we were jumping, we go back to the ground.
    own_air = false;
    own_down = false;
    // Other state depends on throw and fall.
    if(hasThrow()) {
        other_air = true;
        other_down = false;
    } else if(hasFall()) {
        other_air = false;
        other_down = true;
    } else {
        other_air = false;
        other_down = false;
    }
}

int Move::comboPoints() const {
    int cp = 0;
    for(auto const &ms : m_symbols) {
        if(ms == MS_2XCOMBO) {
            cp += 2;
        }
    }
    return std::max(1, cp);
}

static std::vector<Move> const s_standard_moves = {
    Move{"Wait", MT_WAIT, {}},
    Move{"Punch", MT_NORMAL, {}},
    Move{"Kick", MT_NORMAL, {MS_POWERFUL}},
    Move{"Uppercut", MT_NORMAL, {MS_THROW}},
    Move{"Sweep", MT_NORMAL, {MS_FALL}},
    Move{"AirKick", MT_NORMAL, {MS_JUMPOK}},
    Move{"Dash", MT_NORMAL, {MS_DASH}},
    Move{"Charge", MT_NORMAL, {MS_PUSH}}
};

Move const & getWaitMove() {
    return s_standard_moves[0];
}

std::vector<Move> const & getStandardMoves() {
    return s_standard_moves;
}

}
}
