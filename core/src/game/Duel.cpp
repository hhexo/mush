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

#include "core/game/Duel.h"
#include "core/game/Dice.h"
#include "core/ctrl/CtrlInterfaces.h"

#include <cassert>

namespace core {
namespace game {

Duel::Duel(std::shared_ptr<Character> c1, std::shared_ptr<Character> c2, 
           std::ostream *report_stream)
:   m_c1(c1), m_c2(c2), m_report_stream(report_stream), 
    m_c1_clone(c1->clone()), m_c2_clone(c2->clone()), 
    m_far(true), m_turn_counter(0) {}

void Duel::fight() {
    // Initiative
    auto init1 = d6WithCrit() + m_c1_clone->ra;
    auto init2 = d6WithCrit() + m_c2_clone->ra;
    bool t = init1 > init2;

    // Main loop
    while(m_c1_clone->cur_life > 0 && m_c2_clone->cur_life > 0) {
        if(t) {
            turn(*m_c1_clone, *m_c2_clone);
        }
        else {
            turn(*m_c2_clone, *m_c1_clone);
        }
        t = !t;
        // Catch the case where the AIs are stuck. If both arrived to 20AP then
        // nobody is doing anything.
        if(m_c1_clone->cur_ap >= 20 && m_c2_clone->cur_ap >= 20) {
            // Force a draw.
            m_c1_clone->cur_life = 0;
            m_c2_clone->cur_life = 0;
        }
    }

    if(m_c1_clone->cur_life <= 0 && m_c2_clone->cur_life <= 0) {
        if(m_report_stream) {
            *m_report_stream << m_c1_clone->name << " and " << m_c2_clone->name 
                             << " draw!" << std::endl << std::endl;
        }
        std::lock(m_c1->content_mutex, m_c2->content_mutex);
        m_c1->total_points += 1;
        m_c2->total_points += 1;
        // A draw is considered a win by both for AI purposes.
        m_c1->actrl->updateAfterMatch(*m_c1_clone, *m_c2_clone, true);
        m_c1->dctrl->updateAfterMatch(*m_c1_clone, *m_c2_clone, true);
        m_c2->actrl->updateAfterMatch(*m_c2_clone, *m_c1_clone, true);
        m_c2->dctrl->updateAfterMatch(*m_c2_clone, *m_c1_clone, true);
        m_c1->content_mutex.unlock();
        m_c2->content_mutex.unlock();
    }
    else if(m_c1_clone->cur_life <= 0) {
        if(m_report_stream) {
            *m_report_stream << m_c2_clone->name << " wins!" << std::endl 
                             << std::endl;
        }
        std::lock(m_c1->content_mutex, m_c2->content_mutex);
        m_c2->total_points += 3;
        m_c1->actrl->updateAfterMatch(*m_c1_clone, *m_c2_clone, false);
        m_c1->dctrl->updateAfterMatch(*m_c1_clone, *m_c2_clone, false);
        m_c2->actrl->updateAfterMatch(*m_c2_clone, *m_c1_clone, true);
        m_c2->dctrl->updateAfterMatch(*m_c2_clone, *m_c1_clone, true);
        m_c1->content_mutex.unlock();
        m_c2->content_mutex.unlock();
    }
    else if(m_c2_clone->cur_life <= 0) {
        if(m_report_stream) {
            *m_report_stream << m_c1_clone->name << " wins!" << std::endl 
                             << std::endl;
        }
        std::lock(m_c1->content_mutex, m_c2->content_mutex);
        m_c1->total_points += 3;
        m_c1->actrl->updateAfterMatch(*m_c1_clone, *m_c2_clone, true);
        m_c1->dctrl->updateAfterMatch(*m_c1_clone, *m_c2_clone, true);
        m_c2->actrl->updateAfterMatch(*m_c2_clone, *m_c1_clone, false);
        m_c2->dctrl->updateAfterMatch(*m_c2_clone, *m_c1_clone, false);
        m_c1->content_mutex.unlock();
        m_c2->content_mutex.unlock();
    }
}

void Duel::turn(Character &attacker, Character &defender) {
    // Restore AP
    if(m_turn_counter >= 2) {
        attacker.cur_ap += attacker.ra;
        if(attacker.cur_ap > 20) {
            attacker.cur_ap = 20;
        }
    }

    // Initialize and report new turn.
    if(m_report_stream) {
        *m_report_stream << std::endl << "Turn " << m_turn_counter;
        attacker.brief(*m_report_stream);
        defender.brief(*m_report_stream);
        *m_report_stream << std::endl;
    }
    ++m_turn_counter;
    attacker.nth_move_of_the_round = 0;

    // Fall down
    if(attacker.air) {
        attacker.air = false;

        // DECISION POINT.
        if(attacker.cur_ap > 0
           && attacker.actrl->shouldSpendAPToFallStanding(
                attacker, defender, m_far)) {
            if(m_report_stream) {
                *m_report_stream << attacker.name 
                                 << " decides to spend 1AP to cushion the fall."
                                 << std::endl;
            }
            attacker.cur_ap -= 1;
        }
        else {
            if(m_report_stream) {
                *m_report_stream << attacker.name 
                                 << " falls badly and loses 1 further life."
                                 << std::endl;
            }
            attacker.cur_life -= 1;
        }
    }
    // Get up
    else if(attacker.down) {
        if(m_report_stream) {
            *m_report_stream << attacker.name 
                             << " falls badly, loses 1 further life, "
                             << "and has to spend 1AP to get up."
                             << std::endl;
        }
        attacker.down = false;
        if(attacker.cur_ap > 0) {
            attacker.cur_ap -= 1;
        }
        attacker.cur_life -= 1;
    }

    // We might have died now! In which case we complete the turn.
    if(attacker.cur_life <= 0) {
        if(m_report_stream) {
            *m_report_stream << attacker.name 
                             << " has succumbed due to the damage!"
                             << std::endl;
        }
        return;
    }

    // Reset combo accumulated during defence (???).
    //attacker.cur_combo = 0;

    // Gain SP in SP_AP mode.
    if(attacker.sp == SP_AP && attacker.cur_sp < 6) {
        // DECISION POINT.
        int how_many = attacker.actrl->shouldSpendAPToGainSP(
            attacker, defender, m_far);
        if(how_many > 0) {
            if(m_report_stream) {
                *m_report_stream << attacker.name 
                                 << " has decided to spend " << how_many
                                 << "AP and gain " << how_many << "SP."
                                 << std::endl;
            }
            attacker.cur_ap -= how_many;
            attacker.cur_sp += how_many;
        }
    }

    bool pass = false;
    while(!pass && defender.cur_life > 0 && attacker.cur_life > 0) {
        // DECISION POINT.
        Move const &attack_move = attacker.actrl->getNextMove(attacker, 
                                                              defender, 
                                                              m_far);

        // If we decided to pass, then pass.
        if(attack_move.isWait()) {
            if(m_report_stream) {
                *m_report_stream << attacker.name 
                                 << " has decided to pass."
                                 << std::endl;
            }
            // It was a "successful wait". :-)
            attacker.actrl->updateAfterMove(attacker, defender, attack_move, true);
            pass = true;
            continue;
        }
        if(m_report_stream) {
            *m_report_stream << attacker.name 
                             << " has decided to attack with '" 
                             << attack_move.getName() << "' [ ";
            for(auto const &s : attack_move.getSymbols()) {
                *m_report_stream << toString(s) << " ";
            }
            *m_report_stream << "]..." << std::endl;
        }

        // Calculate AP cost.
        int original_ap_cost = attack_move.apCost(m_far, false);
        int ap_cost = original_ap_cost;

        // Lower the AP cost.
        while(attacker.cur_sp > (attack_move.isSuper() ? 4 : 0)
              && ap_cost > 1) {
            // DECISION POINT.
            bool lower = attacker.actrl->shouldSpendSPToLowerAPCost(
                attacker, defender, m_far, attack_move, ap_cost);
            if(lower) {
                ap_cost -= 1;
                attacker.cur_sp -= 1;
            } else {
                break;
            }
        }
        if(m_report_stream && original_ap_cost > ap_cost) {
            *m_report_stream << "  (" << attacker.name 
                             << " has decided to spend "
                             << (original_ap_cost - ap_cost) << "SP to lower "
                             << "the AP cost of the move by "
                             << (original_ap_cost - ap_cost) << ")" 
                             << std::endl;
        }

        // Sanity check.
        if(ap_cost > attacker.cur_ap) {
            if(m_report_stream) {
                *m_report_stream << "... but has not enough AP and must pass."
                                 << std::endl;
            }
            // It was a "successful wait". :-)
            attacker.actrl->updateAfterMove(attacker, defender, attack_move, true);
            pass = true;
            continue;
        }
        assert(ap_cost <= attacker.cur_ap);
        if(attack_move.isSuper()) {
            if(attacker.cur_sp < 4) {
                if(m_report_stream) {
                    *m_report_stream << "... but has not enough SP and must "
                                     << "pass." << std::endl;
                }
                // It was a "successful wait". :-)
                attacker.actrl->updateAfterMove(attacker, defender, attack_move, true);
                pass = true;
                continue;
            }
            assert(attacker.cur_sp >= 4);
        }

        // Now do it.
        bool interrupted = false;
        performAttack(attacker, defender, attack_move, ap_cost, interrupted);

        // Have we been interrupted?
        if(interrupted) {
            if(m_report_stream) {
                *m_report_stream << attacker.name 
                                 << " has been interrupted and must pass."
                                 << std::endl;
            }
            pass = true;
            continue;
        }

        // Update move counter.
        attacker.nth_move_of_the_round += 1;
        for(size_t i = 0; i < attacker.moves.size(); ++i) {
            if(attacker.moves[i] == attack_move) {
                attacker.moves_performed.push_back(i);
                break;
            }
        }
    }

    // At the end of turn, handle combo points.
    int sp_from_combo = attacker.cur_combo / 6;
    if(attacker.sp == SP_COMBO) {
        sp_from_combo += attacker.cur_combo / 3;
    }
    if(m_report_stream) {
        *m_report_stream << attacker.name 
                         << " has totalled " << attacker.cur_combo 
                         << " combo points and gains " << sp_from_combo << "SP."
                         << std::endl;
    }
    attacker.cur_sp += sp_from_combo;
    attacker.cur_combo = 0;

    // Reset moves
    attacker.nth_move_of_the_round = 0;
    attacker.moves_performed.clear();
}

bool Duel::performAttack(Character &attacker, Character &defender, 
                         Move const &attack_move, int modified_ap_cost,
                         bool &interrupted) {
    // Init some variables.
    int at_test_bonus = 0;
    int at_agility_bonus = 0;
    int df_test_bonus = 0;
    int df_agility_bonus = 0;
    int at_bonus_for_damage = 0;

    // Consume APs and SPs
    if(m_report_stream) {
        *m_report_stream << "  (" << attacker.name << " spends "
                         << modified_ap_cost << "AP and "
                         << (attack_move.isSuper() ? 4 : 0) 
                         << "SP for the move)" << std::endl;
    }
    assert(attacker.cur_ap >= modified_ap_cost);
    attacker.cur_ap -= modified_ap_cost;
    if(attack_move.isSuper()) {
        assert(attacker.cur_sp >= 4);
        attacker.cur_sp -= 4;
    }

    // See if the attacker wants to prevent the defender from defending.
    bool can_defend = !defender.air;
    if(can_defend 
       && (attacker.cur_combo > 0) // Must not be the first move.
       && attacker.cur_sp > (attack_move.isSuper() ? 4 : 0)) {
        // DECISION POINT.
        if(attacker.actrl->shouldSpendSPToConcatenate(
                attacker, defender, m_far, attack_move)) {
            if(m_report_stream) {
                *m_report_stream << "  (" << attacker.name 
                                 << " spends 1SP to concatenate)" << std::endl;
            }
            attacker.cur_sp -= 1;
            can_defend = false;
        }
    }

    // Then see if the defender wants to spend SP to defend even if they can't.
    if(!can_defend) {
        // DECISION POINT.
        if(defender.cur_sp > 1 && defender.dctrl->shouldSpendSPToComboBreak(
                attacker, defender, m_far, attack_move)) {
            if(m_report_stream) {
                *m_report_stream << "  (" << defender.name 
                                 << " spends 2SP to combo break!)" << std::endl;
            }
            defender.cur_sp -= 2;
            can_defend = true;
        }
    }

    // Now let the attacker choose if they want to spend SP to boost tests...
    if(attacker.cur_sp > (attack_move.isSuper() ? 4 : 0) 
       && attack_move.hasUltraAgility()) {
        // DECISION POINT.
        if(attacker.actrl->shouldSpendSPForUltraAgility(
                attacker, defender, m_far, attack_move)) {
            if(m_report_stream) {
                *m_report_stream << "  (" << attacker.name
                                 << " spends 1SP to power Ultra Agility)"
                                 << std::endl;
            }
            attacker.cur_sp -= 1;
            at_agility_bonus += 2;
        }
    }
    if(attacker.cur_sp > (attack_move.isSuper() ? 4 : 0)) {
        // DECISION POINT.
        if(attacker.actrl->shouldSpendSPToBoostAttack(
                attacker, defender, m_far, attack_move)) {
            if(m_report_stream) {
                *m_report_stream << "  (" << attacker.name
                                 << " spends 1SP to boost attack and damage)"
                                 << std::endl;
            }
            attacker.cur_sp -= 1;
            at_test_bonus += 1;
            at_bonus_for_damage += 1;
        }
    }

    // We are ready to resolve!
    // Modify state pre resolution.
    attack_move.modifyStatePreResolution(m_far, attacker.air, attacker.down,
                                         defender.air, defender.down);

    // At this point we can roll the attack test.
    int at_test = d6WithCrit();
    if(m_report_stream) {
        *m_report_stream << "  (" << attacker.name << " rolls " 
                         << at_test << ")" << std::endl;
    }
    // Gain SP
    if(at_test == 8 && attacker.cur_sp < 6) {
        if(m_report_stream) {
            *m_report_stream << "  (" << attacker.name 
                             << " gains 1SP because of the dice roll)"
                             << std::endl;
        }
        attacker.cur_sp += 1;
    }
    // Check if we benefit from bonuses.
    for(auto const &s : attack_move.getSymbols()) {
        if(   (m_far && s == MS_BONUS_FAR)
           || (!m_far && s == MS_BONUS_NEAR)
           || (defender.air && s == MS_BONUS_AIR)
           || (defender.down && s == MS_BONUS_DOWN)) {
            at_test_bonus += 1;
        }
    }
    if(m_report_stream) {
        *m_report_stream << "  (" << attacker.name 
                         << " benefits from an attack test bonus of +"
                         << at_test_bonus + attacker.at 
                         << " and an agility bonus of +"
                         << at_agility_bonus << ")" << std::endl;
    }

    // Calculate preliminary damage (for SP_DEFENCE).
    int preliminary_damage = attack_move.damage(at_test + at_test_bonus,
                                                attacker.at, attacker.df, 0);

    // Process defence if possible
    if(can_defend) {
        // DECISION POINT.
        Move const &tentative_counter_move = defender.dctrl->getCounterMove(
            defender, attacker, m_far, attack_move);
        if(m_report_stream && !tentative_counter_move.isWait()) {
            *m_report_stream << "  (" << defender.name
                             << " plans to counter with '" 
                             << tentative_counter_move.getName() << "' [ ";
            for(auto const &s : tentative_counter_move.getSymbols()) {
                *m_report_stream << toString(s) << " ";
            }
            *m_report_stream << "])" << std::endl;
        }
        int counter_ap_cost = tentative_counter_move.apCost(m_far, true);
        int counter_sp_cost = tentative_counter_move.isSuper() ? 4 : 0;

        // Lower the AP cost.
        while(defender.cur_sp > counter_sp_cost && counter_ap_cost > 1) {
            // DECISION POINT.
            bool lower = defender.dctrl->shouldSpendSPToLowerAPCost(
                defender, attacker, m_far, tentative_counter_move, counter_ap_cost);
            if(lower) {
                counter_ap_cost -= 1;
                defender.cur_sp -= 1;
            } else {
                break;
            }
        }
        if(m_report_stream && counter_ap_cost < tentative_counter_move.apCost(m_far, true)) {
            *m_report_stream << "  (" << defender.name 
                             << " has decided to spend "
                             << (tentative_counter_move.apCost(m_far, true) - counter_ap_cost)
                             << "SP to lower the AP cost of the counter by "
                             << (tentative_counter_move.apCost(m_far, true) - counter_ap_cost) 
                             << ")" << std::endl;
        }

        // Now spend AP and SP if we were countering.
        bool forfeit_counter = false;
        if(!tentative_counter_move.isWait()) {
            if(defender.cur_ap < counter_ap_cost) {
                if(m_report_stream) {
                    *m_report_stream << "  (" << defender.name 
                                     << " has not enough AP and must forfeit "
                                     << "the counter)" << std::endl;
                }
                forfeit_counter = true;
            }
            if(defender.cur_sp < counter_sp_cost) {
                if(m_report_stream) {
                    *m_report_stream << "  (" << defender.name 
                                     << " has not enough SP and must forfeit "
                                     << "the counter)" << std::endl;
                }
                forfeit_counter = true;
            }
            if(!forfeit_counter) {
                if(m_report_stream) {
                    *m_report_stream << "  (" << defender.name 
                                     << " spends " << counter_ap_cost 
                                     << "AP and " << counter_sp_cost 
                                     << "SP for the counter move)" << std::endl;
                }
                defender.cur_ap -= counter_ap_cost;
                defender.cur_sp -= counter_sp_cost;
            }
        }
        Move const &counter_move = forfeit_counter ? getWaitMove() 
                                                   : tentative_counter_move;

        // Choose whether to spend SP to boost defence tests.
        if(!counter_move.isWait()) {
            // Check counter bonuses.
            for(auto const &s: counter_move.getSymbols()) {
                if(s == MS_BONUS_COUNTER) {
                    df_test_bonus += 1;
                }
            }
            // Check Ultra Agility.
            if(defender.cur_sp > (counter_move.isSuper() ? 4 : 0) 
               && counter_move.hasUltraAgility()) {
                // DECISION POINT.
                if(defender.dctrl->shouldSpendSPForUltraAgility(
                        defender, attacker, m_far, counter_move)) {
                    if(m_report_stream) {
                        *m_report_stream << "  (" << defender.name
                                         << " spends 1SP to power Ultra Agility)"
                                         << std::endl;
                    }
                    defender.cur_sp -= 1;
                    df_agility_bonus += 2;
                }
            }
        }
        // Check boost. We can do this even without a counter.
        if(defender.cur_sp > (counter_move.isSuper() ? 4 : 0)) {
            // DECISION POINT.
            if(defender.dctrl->shouldSpendSPToBoostDefence(
                    defender, attacker, m_far, counter_move)) {
                if(m_report_stream) {
                    *m_report_stream << "  (" << defender.name
                                     << " spends 1SP to boost defence)"
                                     << std::endl;
                }
                defender.cur_sp -= 1;
                df_test_bonus += 1;
            }
        }

        // Now roll defence.
        int df_test = d6WithCrit();
        if(m_report_stream) {
            *m_report_stream << "  (" << defender.name << " rolls " 
                             << df_test << ")" << std::endl;
        }
        // Gain SP
        if(df_test == 8 && defender.cur_sp < 6) {
            if(m_report_stream) {
                *m_report_stream << "  (" << defender.name 
                                 << " gains 1SP because of the dice roll)"
                                 << std::endl;
            }
            defender.cur_sp += 1;
        }
        // Check if we benefit from bonuses.
        for(auto const &s : counter_move.getSymbols()) {
            if(   (m_far && s == MS_BONUS_FAR)
               || (!m_far && s == MS_BONUS_NEAR)
               || (attacker.air && s == MS_BONUS_AIR)
               || (attacker.down && s == MS_BONUS_DOWN)) {
                df_test_bonus += 1;
            }
        }
        if(m_report_stream) {
            *m_report_stream << "  (" << defender.name 
                             << " benefits from a defence test bonus of +"
                             << df_test_bonus + defender.df 
                             << " and an agility bonus of +"
                             << df_agility_bonus << ")" << std::endl;
        }

        // FINALLY perform the test!
        int final_test_result = at_test + at_test_bonus + at_agility_bonus 
                              + attacker.at - df_test - df_test_bonus 
                              - df_agility_bonus - defender.df;
        if(final_test_result <= 0) {
            int counter_damage = 0;
            if(counter_move.isWait()) {
                if(m_report_stream) {
                    *m_report_stream << "... and misses." << std::endl;
                }

                // Defender gains SP.
                if(defender.sp == SP_DEFENCE) {
                    defender.cur_sp += preliminary_damage / 3;
                    if(m_report_stream) {
                        *m_report_stream << "  (" << defender.name
                                         << " has avoided " << preliminary_damage
                                         << " potential damage and gains "
                                         << (preliminary_damage / 3)
                                         << "SP due to defence)" << std::endl;
                    }
                }
                // Enforce maximum SP.
                if(defender.cur_sp > 6) {
                    defender.cur_sp = 6;
                }
                
                // Update control system.
                attacker.actrl->updateAfterMove(attacker, defender, attack_move, false);
                // Get out, attack failed.
                return false;
            } else {
                // Ooh, we have a counter!
                interrupted = true;
                
                // Apply state modifiers, pre resolution.
                counter_move.modifyStatePreResolution(m_far, defender.air, 
                                                      defender.down,
                                                      attacker.air, 
                                                      attacker.down);
                
                // Let's check for defender's Ultra Hardness.
                int sp_for_uh = 0;
                if(defender.cur_sp > 0 && counter_move.hasUltraHardness()) {
                    // DECISION POINT.
                    if(defender.dctrl->shouldSpendSPToBoostCounterDamage(
                            defender, attacker, m_far, counter_move)){
                        if(m_report_stream) {
                            *m_report_stream << "  (" << defender.name
                                             << " spends 1SP to boost counter "
                                             << "damage with Ultra Hardness)"
                                             << std::endl;
                        }
                        defender.cur_sp -= 1;
                        sp_for_uh = 1;
                    }
                }

                // Compute damage.
                counter_damage = counter_move.counterDamage(
                    df_test + df_test_bonus, defender.at, defender.df,
                    sp_for_uh, preliminary_damage);

                // Soak
                if(!counter_move.hasSmash()) {
                    counter_damage -= attacker.df;
                }
                if(counter_damage < 0) {
                    counter_damage = 0;
                }

                // Report resolution
                if(m_report_stream) {
                    *m_report_stream << "... and was countered for " 
                                     << counter_damage << " damage!"
                                     << std::endl;
                }

                // Apply damage.
                attacker.cur_life -= counter_damage;
                
                // Apply state modifiers, post resolution.
                counter_move.modifyStatePostResolution(m_far, defender.air, 
                                                       defender.down,
                                                       attacker.air, 
                                                       attacker.down);
                // Apply combo points.
                defender.cur_combo += counter_move.comboPoints();

                // Update control system.
                defender.dctrl->updateAfterMove(defender, attacker, counter_move, true);
                // Counter was successful, record move in defender.
                for(size_t i = 0; i < defender.moves.size(); ++i) {
                    if(defender.moves[i] == counter_move) {
                        defender.moves_performed.push_back(i);
                        break;
                    }
                }
            }

            // Apply SP gain due to damage, both for attacker and defender
            if(attacker.sp == SP_WOUND && counter_damage > 0) {
                attacker.cur_sp += counter_damage / 3;
                if(m_report_stream) {
                    *m_report_stream << "  (" << attacker.name
                                     << " gains " << (counter_damage / 3)
                                     << "SP due to wounds)" << std::endl;
                }
            }
            if(defender.sp == SP_DAMAGE && counter_damage > 0) {
                defender.cur_sp += counter_damage / 3;
                if(m_report_stream) {
                    *m_report_stream << "  (" << defender.name
                                     << " gains " << (counter_damage / 3)
                                     << "SP due to damage)" << std::endl;
                }
            }
            if(defender.sp == SP_DEFENCE) {
                defender.cur_sp += preliminary_damage / 3;
                if(m_report_stream) {
                    *m_report_stream << "  (" << defender.name
                                     << " has avoided " << preliminary_damage
                                     << " potential damage and gains "
                                     << (preliminary_damage / 3)
                                     << "SP due to defence)" << std::endl;
                }
            }

            // Enforce maximum SP.
            if(attacker.cur_sp > 6) {
                attacker.cur_sp = 6;
            }
            if(defender.cur_sp > 6) {
                defender.cur_sp = 6;
            }

            // Attack failed only if it was not a draw with a counter.
            if(final_test_result != 0) {
                // Update control system.
                attacker.actrl->updateAfterMove(attacker, defender, attack_move, false);
                return false;
            }
        }
        else {
            // Failed to defend. If it was a counter, record that it failed.
            if(!counter_move.isWait()) {
                // Update control system.
                defender.dctrl->updateAfterMove(defender, attacker, counter_move, false);                
            }
        }
    }

    // At this point we know we can deal damage, regardless of whether we have
    // been interrupted.

    // NOW the attacker decides whether to use UltraHardness.
    int actual_damage = preliminary_damage;
    if(attacker.cur_sp > 0 && attack_move.hasUltraHardness()) {
        // DECISION POINT.
        if(attacker.actrl->shouldSpendSPToBoostDamage(
                attacker, defender, m_far, attack_move)){
            if(m_report_stream) {
                *m_report_stream << "  (" << attacker.name
                                 << " spends 1SP to boost damage)" << std::endl;
            }
            attacker.cur_sp -= 1;
            actual_damage = attack_move.damage(at_test + at_test_bonus,
                                               attacker.at, attacker.df, 1);
        }
    }

    // Soak
    if(!attack_move.hasSmash()) {
        actual_damage -= defender.df;
    }
    if(actual_damage < 0) {
        actual_damage = 0;
    }

    // Report resolution
    if(m_report_stream) {
        if(can_defend) {
            *m_report_stream << "... and hits for " << actual_damage 
                             << " damage!" << std::endl;
        } else {
            *m_report_stream << "... and auto-hits for " << actual_damage 
                             << " damage!" << std::endl;
        }
    }

    // Apply damage.
    defender.cur_life -= actual_damage;
    
    // Apply SP gain due to damage, both for attacker and defender
    if(defender.sp == SP_WOUND && actual_damage > 0) {
        defender.cur_sp += actual_damage / 3;
        if(m_report_stream) {
            *m_report_stream << "  (" << defender.name
                             << " gains " << (actual_damage / 3)
                             << "SP due to wounds)" << std::endl;
        }
    }
    if(attacker.sp == SP_DAMAGE && actual_damage > 0) {
        attacker.cur_sp += actual_damage / 3;
        if(m_report_stream) {
            *m_report_stream << "  (" << attacker.name
                             << " gains " << (actual_damage / 3)
                             << "SP due to damage)" << std::endl;
        }
    }

    // Enforce maximum SP.
    if(attacker.cur_sp > 6) {
        attacker.cur_sp = 6;
    }
    if(defender.cur_sp > 6) {
        defender.cur_sp = 6;
    }
    
    // Apply state modifiers, post resolution.
    attack_move.modifyStatePostResolution(m_far, attacker.air, attacker.down,
                                          defender.air, defender.down);

    // Apply combo points.
    attacker.cur_combo += attack_move.comboPoints();

    // Update control system.
    attacker.actrl->updateAfterMove(attacker, defender, attack_move, true);

    // We were successful even if we had a counter draw.
    return true;
}


}
}

