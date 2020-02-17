/**
 * MaNGOS is a full featured server for World of Warcraft, supporting
 * the following clients: 1.12.x, 2.4.3, 3.3.5a, 4.3.4a and 5.4.8
 *
 * Copyright (C) 2005-2020 MaNGOS <https://getmangos.eu>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 *
 * World of Warcraft, and all World of Warcraft or Warcraft art, images,
 * and lore are copyrighted by Blizzard Entertainment, Inc.
 */

#ifndef MANGOS_DBCSFRM_H
#define MANGOS_DBCSFRM_H

const char Achievementfmt[]= "niiissiiiiisiix";                     // 5.3.0 Checked
const char AchievementCriteriafmt[]= "niiiiiiiixsiiiiixxxxxxxx";    // 5.3.0 Checked
const char AreaTableEntryfmt[]= "iiinixxxxxisiiiiixxxxxxxxx";
const char AreaGroupEntryfmt[] = "niiiiiii";                        // 5.3.0 Checked
const char AreaTriggerEntryfmt[]= "nifffxxxfffff";                  // TODO: 5.3.0 DIFFERENT
const char ArmorLocationfmt[]="nfffff";                             // 5.3.0 Checked
const char AuctionHouseEntryfmt[]= "niiix";                         // 5.3.0 Checked
const char BankBagSlotPricesEntryfmt[] = "ni";                      // 5.3.0 Checked
const char BarberShopStyleEntryfmt[]= "nixxxiii";                   // TODO: 5.3.0 DIFFERENT
const char BattlemasterListEntryfmt[]="niiiiiiiiixsiiiiiiii";       // TODO: 5.3.0 DIFFERENT
const char CharStartOutfitEntryfmt[]= "diiiiiiiiiiiiiiiiiiiiiiiiixxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";    // TODO: 5.3.0 DIFFERENT
const char CharTitlesEntryfmt[]="nxsxix";                           // 5.3.0 Checked
const char ChatChannelsEntryfmt[]="iixsx";                          // 5.3.0 Checked
// ChatChannelsEntryfmt, index not used (more compact store)
const char ChrClassesEntryfmt[]= "nixsxxxixiiiii";                  // TODO: 5.3.0 DIFFERENT
const char ChrRacesEntryfmt[]="nxixiixixxxxixsxxxxxixxx";           // TODO: 5.3.0 DIFFERENT
const char ChrClassesXPowerTypesfmt[]="nii";                        // 5.3.0 Checked
const char CinematicSequencesEntryfmt[] = "nxxxxxxxxx";             // 5.3.0 Checked
const char CreatureDisplayInfofmt[]="nxxifxxxxxxxxxxxx";            // TODO: 5.3.0 DIFFERENT
const char CreatureDisplayInfoExtrafmt[] = "nixxxxxxxxxxxxxxxxxxx"; // 5.3.0 Checked
const char CreatureFamilyfmt[]="nfifiiiiixsx";                      // 5.3.0 Checked
const char CreatureModelDatafmt[] = "nxxxxxxxxxxxxxxffxxxxxxxxxxxxxx";
const char CreatureSpellDatafmt[] = "niiiixxxx";                    // 5.3.0 Checked
const char CreatureTypefmt[]="nxx";                                 // 5.3.0 Checked
const char CurrencyTypesfmt[]="nisxxxxiiix";                        // TODO: 5.3.0 DIFFERENT
const char DestructibleModelDataFmt[] = "nixxxixxxxixxxxixxxxxxxx"; // 5.3.0 Checked
const char DungeonEncounterfmt[]="niiiisxx";                        // 5.3.0 Checked
const char DurabilityCostsfmt[] = "niiiiiiiiiiiiiiiiiiiiiiiiiiiii"; // TODO: 5.3.0 DIFFERENT
const char DurabilityQualityfmt[] = "nf";                           // TODO: 5.3.0 DIFFERENT
const char EmotesEntryfmt[]="nxxiiixx";                             // 5.3.0 Checked
const char EmotesTextEntryfmt[] = "nxixxxxxxxxxxxxxxxx";            // 5.3.0 Checked
const char FactionEntryfmt[]="niiiiiiiiiiiiiiiiiiffixsxx";          // TODO: 5.3.0 DIFFERENT
const char FactionTemplateEntryfmt[] = "niiiiiiiiiiiii";            // 5.3.0 Checked
const char GameObjectDisplayInfofmt[]="nsxxxxxxxxxxffffffxxx";      // 5.3.0 Checked
const char GemPropertiesEntryfmt[]="nixxix";                        // 5.3.0 Checked
const char GlyphPropertiesfmt[] = "niii";                           // 5.3.0 Checked
const char GlyphSlotfmt[] = "nii";                                  // 5.3.0 Checked
const char GtBarberShopCostBasefmt[]="xf";                          // 5.3.0 Checked
const char GtCombatRatingsfmt[]="xf";                               // 5.3.0 Checked
const char GtChanceToMeleeCritBasefmt[]="xf";                       // 5.3.0 Checked
const char GtChanceToMeleeCritfmt[]="xf";                           // 5.3.0 Checked
const char GtChanceToSpellCritBasefmt[]="xf";                       // 5.3.0 Checked
const char GtChanceToSpellCritfmt[]="xf";                           // 5.3.0 Checked
const char GtOCTRegenHPfmt[]="xf";
const char GtOCTBaseHPByClassfmt[]="df";                            // 5.3.0 Checked
const char GtOCTBaseMPByClassfmt[]="df";                            // 5.3.0 Checked
const char GtOCTClassCombatRatingScalarfmt[] = "df";                // 5.3.0 Checked

const char GtOCTHpPerStaminafmt[]="df";                             // 5.3.0 Checked
const char GtOCTRegenHPfmt[]="xf";
const char GtRegenHPPerSptfmt[]="xf";
const char GtRegenMPPerSptfmt[]="xf";                               // 5.3.0 Checked
const char GtSpellScalingfmt[]="df";                                // 5.3.0 Checked
const char GtOCTBaseHPByClassfmt[]="df";
const char GtOCTBaseMPByClassfmt[]="df";
const char Holidaysfmt[] = "nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"; // 5.3.0 Checked
const char ItemArmorQualityfmt[]="nfffffffi";                       // 5.3.0 Checked
const char ItemArmorShieldfmt[]="nifffffff";                        // 5.3.0 Checked
const char ItemArmorTotalfmt[]="niffff";                            // 5.3.0 Checked
const char ItemBagFamilyfmt[]="nx";                                 // 5.3.0 Checked
const char ItemClassfmt[]="nixxfs";                                 // TODO: 5.3.0 DIFFERENT
// const char ItemDisplayTemplateEntryfmt[]="nxxxxxxxxxxixxxxxxxxxxx";
// const char ItemCondExtCostsEntryfmt[]="xiii";
const char ItemDamagefmt[]="nfffffffi";
const char ItemLimitCategoryEntryfmt[]="nxii";                      // 5.3.0 Checked
const char ItemRandomPropertiesfmt[]="nxiiiiis";                    // 5.3.0 Checked
const char ItemRandomSuffixfmt[]="nsxiiiiiiiiii";                   // 5.3.0 Checked
const char ItemReforgefmt[]="nifif";
const char ItemSetEntryfmt[]="dsxxxxxxxxxxxxxxxxxiiiiiiiiiiiiiiiiii"; // 5.3.0 Checked
const char LfgDungeonsEntryfmt[] = "isiiiiifiiifsisisiiii";
const char LiquidTypefmt[] = "nxxixixxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx"; // 5.3.0 Checked
const char LockEntryfmt[] = "niiiiiiiiiiiiiiiiiiiiiiiixxxxxxxx";    // 5.3.0 Checked
const char MailTemplateEntryfmt[]="nxs";                            // 5.3.0 Checked
const char MapEntryfmt[]="nsiiiisissififfiiiii";                    // TODO: 5.3.0 DIFFERENT
const char MapDifficultyEntryfmt[]="niisiis";                       // 5.3.0 Checked
const char MovieEntryfmt[]="nxxx";
const char MountCapabilityfmt[]="niiiiiii";
const char MountTypefmt[]="niiiiiiiiiiiiiiiiiiiiiiii";
const char NumTalentsAtLevelfmt[]="df";
const char OverrideSpellDatafmt[]="niiiiiiiiiixx";
const char QuestFactionRewardfmt[] = "niiiiiiiiii";
const char QuestSortEntryfmt[]="nx";                                // 5.3.0 Checked
const char QuestXPLevelfmt[] = "niiiiiiiiii";                       // 5.3.0 Checked
const char Phasefmt[]="nii";                                        // 5.3.0 Checked
const char PowerDisplayfmt[] = "nixxxx";
const char PvPDifficultyfmt[] = "diiiii";
const char RandomPropertiesPointsfmt[] = "niiiiiiiiiiiiiii";        // 5.3.0 Checked
const char ScalingStatDistributionfmt[]="niiiiiiiiiiiiiiiiiiiixi";  // 5.3.0 Checked
const char ScalingStatValuesfmt[]="iniiiiiiiiiiiiiiiiiiiixxxxxxxxxxxxxxxxxxxxxxxxx";
const char SkillLinefmt[]="nisxixi";
const char SkillLineAbilityfmt[] = "niiiixxiiiiiix";
const char SkillRaceClassInfofmt[]="diiiiixxx";
const char SoundEntriesfmt[]="nissssssssssssssssssssssxxxxxxxxxxx";
const char SpellCastTimefmt[]="niii";                               // 5.3.0 Checked
const char SpellDurationfmt[] = "niii";                             // 5.3.0 Checked
const char SpellDifficultyfmt[] = "niiii";
//                          1        10        20        30        40        50        60        70        80        90        100       110       120       130       140       150       160
const char SpellEntryfmt[]="niiiiiiiiiiiiiiifiiiissxxiixxixiiiiiiixiiiiiiiix";
const char SpellAuraOptionsEntryfmt[]="diiii";
const char SpellAuraRestrictionsEntryfmt[]="diiiiiiii";
const char SpellCastingRequirementsEntryfmt[]="dixxixi";            // 5.3.0 Checked
const char SpellCategoriesEntryfmt[]="diiiiii";
const char SpellClassOptionsEntryfmt[]="dxiiiix";
const char SpellCooldownsEntryfmt[]="diii";
//                                1        10        20        30
const char SpellEffectEntryfmt[]="difiiiffiiiiiifiifiiiiiiiix";
const char SpellEquippedItemsEntryfmt[]="diii";
const char SpellInterruptsEntryfmt[]="dixixi";
const char SpellLevelsEntryfmt[]="diii";
const char SpellPowerEntryfmt[]="diiiiixf";
const char SpellReagentsEntryfmt[]="diiiiiiiiiiiiiiii";
const char SpellScalingEntryfmt[]="diiiiffffffffffi";
const char SpellShapeshiftEntryfmt[]="dixixx";                      // 5.3.0 Checked
const char SpellTargetRestrictionsEntryfmt[]="dfiiii";
const char SpellTotemsEntryfmt[]="diiii";                           // 5.3.0 Checked
const char SpellFocusObjectfmt[]="nx";
const char SpellItemEnchantmentfmt[]="nxiiiiiixxxiiisiiiiiiix";
const char SpellItemEnchantmentConditionfmt[] = "nbbbbbxxxxxbbbbbbbbbbiiiiiXXXXX"; // 5.3.0 Checked
const char SpellRadiusfmt[]="nfff";
const char SpellRangefmt[]="nffffixx";                              // 5.3.0 Checked
const char SpellRuneCostfmt[] = "niiii";
const char SpellShapeshiftFormfmt[]="nxxiixiiixxiiiiiiiixx";        // 5.3.0 Checked
//const char StableSlotPricesfmt[] = "ni"; // removed
const char SummonPropertiesfmt[] = "niiiii";                        // 5.3.0 Checked
const char TalentEntryfmt[]="niiiiiiiiixxixxxxxx";
const char TalentTabEntryfmt[]="nxxiiixxiii";
const char TalentTreePrimarySpellsfmt[]="diix";
const char TaxiNodesEntryfmt[]="nifffsiixxx";                       // 5.3.0 Checked
const char TaxiPathEntryfmt[] = "niii";                             // 5.3.0 Checked
const char TaxiPathNodeEntryfmt[] = "diiifffiiii";                  // 5.3.0 Checked
const char TotemCategoryEntryfmt[]="nxii";                          // 5.3.0 Checked
const char TransportAnimationEntryfmt[]="diixxxx";                  // 5.3.0 Checked
const char VehicleEntryfmt[]="niffffiiiiiiiifffffffffffffffssssfifiixx";
const char VehicleSeatEntryfmt[]="niiffffffffffiiiiiifffffffiiifffiiiiiiiffiiiiixxxxxxxxxxxxxxxxxxxx"; // 5.3.0 Checked
const char WMOAreaTableEntryfmt[]="niiixxxxxiixxxx";                // 5.3.0 Checked
const char WorldMapAreaEntryfmt[]="xinxffffixxxxx";                 // 5.3.0 Checked
const char WorldMapOverlayEntryfmt[]="nxiiiixxxxxxxxx";
const char WorldSafeLocsEntryfmt[]="nifffx";
const char WorldPvPAreaEnrtyfmt[]="niiiiii";

#endif
