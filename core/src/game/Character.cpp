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

#include "core/game/Character.h"
#include "core/game/Dice.h"
#include "core/ctrl/CtrlInterfaces.h"
#include "core/ctrl/DumbCtrl.h"
#include "core/ctrl/EvolveAICtrl.h"

#include <iostream>
#include <sstream>

namespace {

size_t nextUID() {
    static size_t uid = 0;
    return uid++;
}

}


namespace core {
namespace game {

static std::string const s_sp_mode_names[] = {
    "Combo", "Damage", "AP", "Wounds", "Defence"
};
SPMode operator++(SPMode& sp) {
    return static_cast<SPMode>(static_cast<int>(sp) + 1);
}
SPMode operator*(SPMode sp) { return sp; }
SPMode begin(SPMode) { return SP_BEGIN__; }
SPMode end(SPMode) { return SP_END__; }
std::string const & toString(SPMode sp) {
    return s_sp_mode_names[static_cast<int>(sp)];
}

// --- CHARACTER IMPLEMENTATION

Character::Character(std::string const &n, int r, int a, int d, SPMode s, 
                     std::shared_ptr<ctrl::AttackControl> att, 
                     std::shared_ptr<ctrl::DefendControl> def)
:   name(n), ra(r), at(a), df(d), sp(s), actrl(att), dctrl(def), 
    moves(getStandardMoves()), 
    cur_ap(r), cur_sp(0), cur_life(30), cur_combo(0), air(false), down(false),
    nth_move_of_the_round(0), total_points(0), uid(nextUID()),
    content_mutex() {}

Character * Character::clone() const {
    Character *new_char = new Character(name, ra, at, df, sp, actrl, dctrl);
    new_char->moves = moves;
    return new_char;
}

Character & Character::addMove(Move const &move) {
    std::lock_guard<std::mutex> lock(content_mutex);
    moves.push_back(move);
    return *this;
}

void Character::dump(std::ostream &stream) {
    stream << "========================================" << std::endl;
    stream << name << "    : " << total_points << " points" << std::endl
           << "--------------------" << std::endl
           << "RA: " << ra << std::endl
           << "AT: " << at << std::endl
           << "DF: " << df << std::endl
           << "SP: " << toString(sp) << std::endl;
           //<< "Attack AI: " << aai->getName() << std::endl
           //<< "Defend AI: " << dai->getName() << std::endl;
    for(auto const& m : moves) {
        if(!m.isSpecial() && !m.isSuper()) {
            continue;
        }
        stream << m.getName() << " [";
        for(auto const& s : m.getSymbols()) {
            stream << " " << toString(s);
        }
        stream << " ]";
        if(m.isSuper()) {
            stream << " - SUPER";
        }
        stream << std::endl;
    }
    stream << std::endl;
    //aai->dumpState(*this);
    //dai->dumpState(*this);
}

void Character::brief(std::ostream &stream) {
    stream << " --- " << name << ": " << cur_life << "life, "
           << cur_ap << "ap, " << cur_sp << "sp";
    if(air) {
        stream << ", air";
    }
    if(down) {
        stream << ", down";
    }
}

std::shared_ptr<Character> generateRandomCharacter() {
    static size_t counter = 0ULL;
    std::lock_guard<std::mutex> lock(getRandomGenMutex());
    std::stringstream ss;
    ss << "Char" << counter++;

    std::uniform_int_distribution<int> rra(1, 5);
    int ra = rra(getRandomGenerator());
    std::uniform_int_distribution<int> rat(1, 6 - ra);
    int at = rat(getRandomGenerator());
    int df = 7 - ra - at;
    
    std::uniform_int_distribution<int> rsp(SP_BEGIN__, SP_END__ - 1);
    SPMode sp = static_cast<SPMode>(rsp(getRandomGenerator()));
/*
    std::uniform_int_distribution<int> ai_dist(1, 16);
    int aai = ai_dist(getRandomGenerator());
    AttackAI *attack_ai = nullptr;
    if(aai == 1) {
        attack_ai = new Grunt();
    } else if(aai == 2) {
        attack_ai = new Brawler();
    } else if(aai == 3) {
        attack_ai = new SPUser();
    } else if(aai == 4) {
        attack_ai = new CounterAttacker();
    } else if(aai == 5) {
        attack_ai = new TacticalAttacker();
    } else if(aai == 6) {
        attack_ai = new KillingChain();
        sp = SP_DAMAGE;
        if(at < df) {
            std::swap(at, df);
        }
    } else if(aai > 11) {
        attack_ai = new EvolveAttackAI();
    } else if(aai > 6) {
        attack_ai = new MarkovAttackAI();
    } else {
        attack_ai = new DumbAttacker();
    }

    std::uniform_int_distribution<int> aid_dist(0, 10);
    int dai = aid_dist(getRandomGenerator());
    DefendAI *defend_ai = nullptr;
    if(aai == 4) {
        defend_ai = new CounterAttackDefender();
    }
    else if(aai == 5) {
        defend_ai = new TacticalDefender();
    }
    else if(dai == 1) {
        defend_ai = new CautiousDefender();
    } else if(dai == 2 && aai != 6) {
        defend_ai = new CounterAttackDefender();
    } else if(dai == 3) {
        defend_ai = new TacticalDefender();
    } else if(dai > 4) {
        defend_ai = new EvolveDefendAI();
    } else {
        defend_ai = new DumbDefender();
    }
    AttackAI *attack_ai = (sp == SP_COMBO) ? new MarkovAttackAI() : new EvolveAttackAI();
    DefendAI *defend_ai = new EvolveDefendAI();
    */

    auto actrl = (sp == SP_COMBO) ? std::make_shared<ctrl::MarkovAIAttack>()
                                  : std::make_shared<ctrl::EvolveAIAttack>();
    std::shared_ptr<Character> c = std::make_shared<Character>(
        ss.str(), ra, at, df, sp, actrl,
        std::make_shared<ctrl::EvolveAIDefence>());

    std::vector<MoveSymbol> first_move, second_move, third_move, super_move;
    std::uniform_int_distribution<int> rms(MS_BEGIN__, MS_END__ - 1);
    first_move.push_back( (MoveSymbol) rms(getRandomGenerator()) );
    second_move.push_back( (MoveSymbol) rms(getRandomGenerator()) );
    second_move.push_back( (MoveSymbol) rms(getRandomGenerator()) );
    third_move.push_back( (MoveSymbol) rms(getRandomGenerator()) );
    third_move.push_back( (MoveSymbol) rms(getRandomGenerator()) );
    super_move.push_back( (MoveSymbol) rms(getRandomGenerator()) );
    super_move.push_back( (MoveSymbol) rms(getRandomGenerator()) );
    super_move.push_back( (MoveSymbol) rms(getRandomGenerator()) );
    c->addMove(Move{"First", MT_SPECIAL, first_move});
    c->addMove(Move{"Second", MT_SPECIAL, second_move});
    c->addMove(Move{"Third", MT_SPECIAL, third_move});
    c->addMove(Move{"Super", MT_SUPER, super_move});

    return c;
}

}
}
