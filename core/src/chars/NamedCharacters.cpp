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

#include "core/chars/NamedCharacters.h"

#include "core/ctrl/DumbCtrl.h"
#include "core/ctrl/EvolveAICtrl.h"
//#include "AI.h"
//#include "EvolveAI.h"

namespace core {
namespace chars {

using namespace core::game;
using namespace core::ctrl;
/*
    c = std::make_shared<Character>(
        "", 0, 0, 0, SP_COMBO, 
        std::make_shared<EvolveAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"", MT_SPECIAL, 
                    {}});
    c->addMove(Move{"", MT_SPECIAL, 
                    {}});
    c->addMove(Move{"", MT_SPECIAL, 
                    {}});
    c->addMove(Move{"", MT_SUPER, 
                    {}});
    characters.push_back(c);
*/

std::vector<std::shared_ptr<game::Character>> getOriginalCharacters() {
    std::vector<std::shared_ptr<game::Character>> characters{};
    std::shared_ptr<Character> c;

    // Stella
    c = std::make_shared<Character>(
        "Stella D'Argento", 4, 3, 0, SP_COMBO, 
        std::make_shared<MarkovAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"Colpo Affilato", MT_SPECIAL, 
                    {MS_SMASH}});
    c->addMove(Move{"Proiezione Tagliente", MT_SPECIAL, 
                    {MS_FALL, MS_2XCOMBO}});
    c->addMove(Move{"Lama Sagittale", MT_SPECIAL,
                    {MS_BONUS_AIR, MS_BONUS_DOWN}});
    c->addMove(Move{"Ruota della Spada", MT_SUPER, 
                    {MS_ULTRA_AGILITY, MS_2XCOMBO, MS_2XCOMBO, MS_2XCOMBO}});
    characters.push_back(c);

    // Phanumas
    c = std::make_shared<Character>(
        "Phanumas", 3, 3, 1, SP_WOUND, 
        std::make_shared<EvolveAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"Dragon Knee", MT_SPECIAL, 
                    {MS_DASH, MS_THROW}});
    c->addMove(Move{"Dragon Fang", MT_SPECIAL, 
                    {MS_ULTRA_HARDNESS, MS_ULTRA_AGILITY}});
    c->addMove(Move{"Dragon's Breath", MT_SPECIAL, 
                    {MS_DISTANCE}});
    c->addMove(Move{"Flying Dragon", MT_SUPER, 
                    {MS_DASH, MS_SMASH}});
    characters.push_back(c);

    // Jeanne Roux
    c = std::make_shared<Character>(
        "Jeanne Roux", 2, 4, 1, SP_DAMAGE, 
        std::make_shared<EvolveAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"Descending Rotation", MT_SPECIAL, 
                    {MS_FALL, MS_BONUS_AIR}});
    c->addMove(Move{"Ascending Rotation", MT_SPECIAL, 
                    {MS_THROW, MS_BONUS_DOWN}});
    c->addMove(Move{"Lightning Throw", MT_SPECIAL, 
                    {MS_FALL}});
    c->addMove(Move{"Tornado Throw", MT_SUPER, 
                    {MS_THROW, MS_SMASH}});
    characters.push_back(c);

    // Romeo Rabemanjara
    c = std::make_shared<Character>(
        "Romeo Rabemanjara", 3, 4, 0, SP_COMBO, 
        std::make_shared<MarkovAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"Gatling Attack", MT_SPECIAL, 
                    {MS_2XCOMBO, MS_2XCOMBO, MS_2XCOMBO}});
    c->addMove(Move{"Rampaging Kicks", MT_SPECIAL, 
                    {MS_2XCOMBO}});
    c->addMove(Move{"Pressing Punch", MT_SPECIAL, 
                    {MS_DASH}});
    c->addMove(Move{"Overwhelming Combo", MT_SUPER, 
                    {MS_ULTRA_AGILITY, MS_THROW, MS_2XCOMBO, MS_2XCOMBO}});
    characters.push_back(c);

    // Luke Blades
    c = std::make_shared<Character>(
        "Luke Blades", 3, 3, 1, SP_DAMAGE, 
        std::make_shared<EvolveAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"Ascending Flash", MT_SPECIAL, 
                    {MS_THROW}});
    c->addMove(Move{"Falling Flash", MT_SPECIAL, 
                    {MS_ULTRA_AGILITY, MS_JUMPOK}});
    c->addMove(Move{"Lightning Kick", MT_SPECIAL, 
                    {MS_ULTRA_HARDNESS}});
    c->addMove(Move{"Tornado Flash", MT_SUPER, 
                    {MS_POWERFUL, MS_ULTRA_AGILITY}});
    characters.push_back(c);

    // Ryouko Iga
    c = std::make_shared<Character>(
        "Ryouko Iga", 5, 0, 2, SP_AP, 
        std::make_shared<EvolveAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"Cobra no Kyushu", MT_SPECIAL, 
                    {MS_THROW, MS_DISTANCE}});
    c->addMove(Move{"Nishikimebi no Toguro", MT_SPECIAL, 
                    {MS_FALL, MS_DISTANCE}});
    c->addMove(Move{"Mamushi no Kiba", MT_SPECIAL, 
                    {MS_ULTRA_AGILITY, MS_PUSH}});
    c->addMove(Move{"Yamata no Orochi no Kamikizu", MT_SUPER, 
                    {MS_ULTRA_AGILITY, MS_BONUS_AIR}});
    characters.push_back(c);

    // Johnny Yamashita
    c = std::make_shared<Character>(
        "Johnny Yamashita", 3, 1, 3, SP_WOUND, 
        std::make_shared<EvolveAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"Cross Counter", MT_SPECIAL, 
                    {MS_BONUS_COUNTER, MS_BONUS_COUNTER}});
    c->addMove(Move{"Hurricane Punch", MT_SPECIAL, 
                    {MS_POWERFUL, MS_ULTRA_HARDNESS}});
    c->addMove(Move{"Tornado Uppercut", MT_SPECIAL, 
                    {MS_THROW}});
    c->addMove(Move{"XXX Counter", MT_SUPER, 
                    {MS_ULTRA_AGILITY, MS_BONUS_COUNTER, MS_BONUS_COUNTER}});
    characters.push_back(c);

    // Namgung Hyon-Su
    c = std::make_shared<Character>(
        "Namgung Hyon-Su", 3, 2, 2, SP_COMBO, 
        std::make_shared<MarkovAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"Rising Darkness", MT_SPECIAL, 
                    {MS_THROW, MS_ULTRA_AGILITY}});
    c->addMove(Move{"Dark Arrows", MT_SPECIAL, 
                    {MS_DISTANCE, MS_2XCOMBO}});
    c->addMove(Move{"Dark Moon", MT_SPECIAL, 
                    {MS_THROW, MS_PUSH, MS_2XCOMBO}});
    c->addMove(Move{"Hell's Flames", MT_SUPER, 
                    {MS_THROW, MS_DISTANCE, MS_ULTRA_AGILITY}});
    characters.push_back(c);

    // Zhang Shi
    c = std::make_shared<Character>(
        "Zhang Shi", 2, 3, 2, SP_DAMAGE, 
        std::make_shared<EvolveAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"Gang Zhao", MT_SPECIAL, 
                    {MS_ULTRA_HARDNESS}});
    c->addMove(Move{"Feng Zhao", MT_SPECIAL, 
                    {MS_DISTANCE}});
    c->addMove(Move{"Xin Yue Ti", MT_SPECIAL, 
                    {MS_PUSH, MS_BONUS_AIR}});
    c->addMove(Move{"Fengbao Zhao", MT_SUPER, 
                    {MS_ULTRA_AGILITY, MS_DISTANCE, MS_BONUS_FAR}});
    characters.push_back(c);

    // Rock Stark
    c = std::make_shared<Character>(
        "Rock Stark", 2, 2, 3, SP_DEFENCE, 
        std::make_shared<EvolveAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"Radiant Reflection", MT_SPECIAL, 
                    {MS_REFLECT, MS_BONUS_COUNTER}});
    c->addMove(Move{"Radiant Kick", MT_SPECIAL, 
                    {MS_THROW}});
    c->addMove(Move{"Flash Uppercut", MT_SPECIAL, 
                    {MS_ULTRA_AGILITY, MS_BONUS_AIR}});
    c->addMove(Move{"Atomic Punch", MT_SUPER, 
                    {MS_POWERFUL, MS_ULTRA_AGILITY, MS_BONUS_COUNTER}});
    characters.push_back(c);

    return characters;
}


std::vector<std::shared_ptr<game::Character>> getSiegeOfSydeCharacters() {
    std::vector<std::shared_ptr<game::Character>> characters{};
    std::shared_ptr<Character> c;

    // ========================================================================
    // The Army of the Principality
    // ========================================================================

    // Alexis
    c = std::make_shared<Character>(
        "Duke Alexis the Sun Spear", 2, 1, 4, SP_DEFENCE, 
        std::make_shared<EvolveAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"Steel Spear", MT_SPECIAL, 
                    {MS_SMASH}});
    c->addMove(Move{"Resounding Parry", MT_SPECIAL, 
                    {MS_REFLECT, MS_PUSH}});
    c->addMove(Move{"Rolling Wave of Earth", MT_SPECIAL, 
                    {MS_BONUS_NEAR, MS_THROW}});
    c->addMove(Move{"Invincible Kata of the Spear", MT_SUPER, 
                    {MS_SMASH, MS_POWERFUL, MS_BONUS_NEAR}});
    characters.push_back(c);

    // Gunther
    c = std::make_shared<Character>(
        "Gunther the Thundering Gryphon", 3, 3, 1, SP_WOUND, 
        std::make_shared<EvolveAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"Crackling Mace", MT_SPECIAL, 
                    {MS_PUSH}});
    c->addMove(Move{"Thunder Clap", MT_SPECIAL, 
                    {MS_BONUS_NEAR, MS_ULTRA_HARDNESS}});
    c->addMove(Move{"Titan's Maul", MT_SPECIAL, 
                    {MS_PUSH, MS_SMASH}});
    c->addMove(Move{"Call Lightning", MT_SUPER, 
                    {MS_SMASH, MS_POWERFUL, MS_DISTANCE}});
    characters.push_back(c);

    // Yavanna
    c = std::make_shared<Character>(
        "Yavanna the Rampant Bear", 2, 4, 1, SP_DAMAGE, 
        std::make_shared<EvolveAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"Bear Claw", MT_SPECIAL, 
                    {MS_POWERFUL}});
    c->addMove(Move{"Bear Hug", MT_SPECIAL, 
                    {MS_SMASH, MS_BONUS_NEAR}});
    c->addMove(Move{"Caber Throw", MT_SPECIAL, 
                    {MS_SMASH, MS_THROW}});
    c->addMove(Move{"Fury of the Bear", MT_SUPER, 
                    {MS_PUSH, MS_POWERFUL, MS_SMASH}});
    characters.push_back(c);

    // Julius
    c = std::make_shared<Character>(
        "Julius the Sleeping Dragon", 2, 3, 2, SP_WOUND, 
        std::make_shared<EvolveAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"Waking the Dragon", MT_SPECIAL, 
                    {MS_THROW}});
    c->addMove(Move{"Tidal Wave", MT_SPECIAL, 
                    {MS_DISTANCE, MS_BONUS_FAR}});
    c->addMove(Move{"Tears of the Dragon", MT_SPECIAL, 
                    {MS_POWERFUL, MS_FALL}});
    c->addMove(Move{"The Storm Where The Sky Meets The Sea", MT_SUPER, 
                    {MS_DISTANCE, MS_POWERFUL, MS_ULTRA_HARDNESS}});
    characters.push_back(c);

    // Hartwin
    c = std::make_shared<Character>(
        "Hartwin the White Raven", 3, 3, 1, SP_DAMAGE, 
        std::make_shared<EvolveAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"Magic Staff", MT_SPECIAL, 
                    {MS_SMASH}});
    c->addMove(Move{"Blades of Light", MT_SPECIAL, 
                    {MS_ULTRA_AGILITY, MS_POWERFUL}});
    c->addMove(Move{"Pressure Wave", MT_SPECIAL, 
                    {MS_ULTRA_AGILITY, MS_BONUS_NEAR}});
    c->addMove(Move{"Summoning the Raven of Light", MT_SUPER, 
                    {MS_DISTANCE, MS_POWERFUL, MS_POWERFUL}});
    characters.push_back(c);

    // Isolde
    c = std::make_shared<Character>(
        "Isolde the Chrysalis", 1, 1, 5, SP_DEFENCE, 
        std::make_shared<EvolveAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"Mantis Pounce", MT_SPECIAL, 
                    {MS_JUMPOK}});
    c->addMove(Move{"Falling Tree", MT_SPECIAL, 
                    {MS_POWERFUL, MS_FALL}});
    c->addMove(Move{"Horrid Swarm", MT_SPECIAL, 
                    {MS_SMASH, MS_ULTRA_AGILITY}});
    c->addMove(Move{"First Flight of the Chrysalis", MT_SUPER, 
                    {MS_JUMPOK, MS_DASH, MS_POWERFUL}});
    characters.push_back(c);

    // Ross
    c = std::make_shared<Character>(
        "Ross Garnet the Master of Axes", 4, 2, 1, SP_COMBO, 
        std::make_shared<MarkovAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"Rising Quarter", MT_SPECIAL, 
                    {MS_THROW}});
    c->addMove(Move{"Fleeting Crescent", MT_SPECIAL, 
                    {MS_BONUS_NEAR, MS_2XCOMBO}});
    c->addMove(Move{"Rolling Moon", MT_SPECIAL, 
                    {MS_2XCOMBO, MS_2XCOMBO}});
    c->addMove(Move{"Feral Fury", MT_SUPER, 
                    {MS_DASH, MS_SMASH, MS_FALL}});
    characters.push_back(c);

    // ========================================================================
    // The Defenders of Syde
    // ========================================================================

    // Thorsten
    c = std::make_shared<Character>(
        "Duke Thorsten the Dragonslayer", 2, 4, 1, SP_DAMAGE, 
        std::make_shared<EvolveAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"Rising Strike", MT_SPECIAL, 
                    {MS_BONUS_NEAR}});
    c->addMove(Move{"Away from me!", MT_SPECIAL, 
                    {MS_THROW, MS_PUSH}});
    c->addMove(Move{"Bone Crushing Slash", MT_SPECIAL, 
                    {MS_ULTRA_HARDNESS, MS_FALL}});
    c->addMove(Move{"Dragon Decapitation", MT_SUPER, 
                    {MS_PUSH, MS_SMASH, MS_POWERFUL}});
    characters.push_back(c);

    // Bertram
    c = std::make_shared<Character>(
        "Bertram the Flying Fish", 2, 0, 5, SP_DEFENCE, 
        std::make_shared<EvolveAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"Ice Mirror", MT_SPECIAL, 
                    {MS_REFLECT}});
    c->addMove(Move{"Salmon Leap", MT_SPECIAL, 
                    {MS_JUMPOK, MS_ULTRA_AGILITY}});
    c->addMove(Move{"Insidious Ice", MT_SPECIAL, 
                    {MS_BONUS_NEAR, MS_FALL}});
    c->addMove(Move{"Snow Blizzard", MT_SUPER, 
                    {MS_DISTANCE, MS_BONUS_FAR, MS_POWERFUL}});
    characters.push_back(c);

    // Eleanor
    c = std::make_shared<Character>(
        "Eleanor the Hummingsword", 3, 3, 1, SP_DAMAGE, 
        std::make_shared<EvolveAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"Hummingbird Kiss", MT_SPECIAL, 
                    {MS_SMASH}});
    c->addMove(Move{"Parting the Silk", MT_SPECIAL, 
                    {MS_POWERFUL, MS_BONUS_NEAR}});
    c->addMove(Move{"Flight of the Hummingbird", MT_SPECIAL, 
                    {MS_THROW, MS_ULTRA_AGILITY}});
    c->addMove(Move{"Virtuoso Sword Dance", MT_SUPER, 
                    {MS_BONUS_NEAR, MS_FALL, MS_ULTRA_HARDNESS}});
    characters.push_back(c);

    // Lothar
    c = std::make_shared<Character>(
        "Lothar the Riding Avalanche", 3, 3, 1, SP_WOUND, 
        std::make_shared<EvolveAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"Ride-by Attack", MT_SPECIAL, 
                    {MS_PUSH}});
    c->addMove(Move{"Impetuous Charge", MT_SPECIAL, 
                    {MS_DASH, MS_BONUS_NEAR}});
    c->addMove(Move{"Trampling Assault", MT_SPECIAL, 
                    {MS_ULTRA_HARDNESS, MS_POWERFUL}});
    c->addMove(Move{"Riding the Avalanche", MT_SUPER, 
                    {MS_PUSH, MS_POWERFUL, MS_BONUS_NEAR}});
    characters.push_back(c);

    // Kestin
    c = std::make_shared<Character>(
        "Kestin the Cloud of Arrows", 1, 1, 5, SP_DEFENCE, 
        std::make_shared<EvolveAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"Hunter Trap", MT_SPECIAL, 
                    {MS_FALL}});
    c->addMove(Move{"Sure Strike", MT_SPECIAL, 
                    {MS_DISTANCE, MS_BONUS_FAR}});
    c->addMove(Move{"Arrow Nova", MT_SPECIAL, 
                    {MS_PUSH, MS_THROW}});
    c->addMove(Move{"Energy Arrow", MT_SUPER, 
                    {MS_DISTANCE, MS_POWERFUL, MS_ULTRA_HARDNESS}});
    characters.push_back(c);

    // Scarlet
    c = std::make_shared<Character>(
        "Scarlet the Ardent Flame", 3, 2, 2, SP_COMBO, 
        std::make_shared<MarkovAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"Flame Pillar", MT_SPECIAL, 
                    {MS_THROW}});
    c->addMove(Move{"Spinning Ring of Fire", MT_SPECIAL, 
                    {MS_2XCOMBO, MS_POWERFUL}});
    c->addMove(Move{"The Good Old Fireball", MT_SPECIAL, 
                    {MS_DISTANCE, MS_BONUS_FAR}});
    c->addMove(Move{"Burning Wind of Noon", MT_SUPER, 
                    {MS_DASH, MS_SMASH, MS_2XCOMBO}});
    characters.push_back(c);

    // Vune
    c = std::make_shared<Character>(
        "Vune the Livid Necromancer", 2, 2, 3, SP_DEFENCE, 
        std::make_shared<EvolveAIAttack>(),
        std::make_shared<EvolveAIDefence>());
    c->addMove(Move{"Horror Vacui", MT_SPECIAL, 
                    {MS_FALL}});
    c->addMove(Move{"Gust of Lost Souls", MT_SPECIAL, 
                    {MS_ULTRA_AGILITY, MS_BONUS_NEAR}});
    c->addMove(Move{"Reaping Darkness", MT_SPECIAL, 
                    {MS_POWERFUL, MS_POWERFUL}});
    c->addMove(Move{"Infernal Comet", MT_SUPER, 
                    {MS_DASH, MS_SMASH, MS_ULTRA_HARDNESS}});
    characters.push_back(c);

    return characters;
}
 
}
}
