/**
 * MaNGOS is a full featured server for World of Warcraft, supporting
 * the following clients: 1.12.x, 2.4.3, 3.3.5a, 4.3.4a and 5.4.8
 *
 * Copyright (C) 2005-2023 MaNGOS <https://getmangos.eu>
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

const char Achievementfmt[]= "niiissiiiiisiix";
const char AchievementCriteriafmt[]="niiiiiiiixsiiiiixxxxxxx";
const char AreaTableEntryfmt[] = "iiinixxxxxxxisiiiiixxxxxxxxxxx";   // TODO: NEED TO CONFIRM THIS
const char AreaGroupEntryfmt[] = "niiiiiii";
const char AreaTriggerEntryfmt[]="nifffxxxfffffxxx";
const char ArmorLocationfmt[]="nfffff";
const char AuctionHouseEntryfmt[]= "niiix";
const char BankBagSlotPricesEntryfmt[] = "ni";
const char BarberShopStyleEntryfmt[]= "nixxxiii";
const char BattlemasterListEntryfmt[]="niiiiiiiiiiiiiiiiixsiiiiiiiixx";   // TODO: NEED TO CONFIRM THIS
const char CharStartOutfitEntryfmt[]= "diiiiiiiiiiiiiiiiiiiiiiiiixxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
const char CharTitlesEntryfmt[]="nxsxix";
const char ChatChannelsEntryfmt[]="iixsx";
// ChatChannelsEntryfmt, index not used (more compact store)
const char ChrClassesEntryfmt[]="nixsxxxixiiiixxxxx";   // TODO: NEED TO CONFIRM THIS
const char ChrRacesEntryfmt[]="nxixiixixxxxixsxxxxxixxxxxxxxxxxxxxx";
const char ChrClassesXPowerTypesfmt[]="nii";
const char CinematicSequencesEntryfmt[] = "nxxxxxxxxx";
const char CreatureDisplayInfofmt[]="nixifxxxxxxxxxxxxxxx";   // TODO: NEED TO CONFIRM THIS
const char CreatureDisplayInfoExtrafmt[] = "nixxxxxxxxxxxxxxxxxxx";
const char CreatureFamilyfmt[]="nfifiiiiixsx";
const char CreatureSpellDatafmt[] = "niiiixxxx";
const char DestructibleModelDataFmt[] = "nixxxixxxxixxxxixxxxixxx";
const char DungeonEncounterfmt[]="niiiisxx";   // TODO: NEED TO CONFIRM THIS
const char CreatureTypefmt[]="nxx";
const char CurrencyTypesfmt[]="nisxxxxiiixx";   // TODO: NEED TO CONFIRM THIS
const char DurabilityCostsfmt[] = "niiiiiiiiiiiiiiiiiiiiiiiiiiiii";
const char DurabilityQualityfmt[] = "nf";
const char EmotesEntryfmt[]="nxxiiixx";
const char EmotesTextEntryfmt[] = "nxixxxxxxxxxxxxxxxx";
const char FactionEntryfmt[]="niiiiiiiiiiiiiiiiiiffixsxxxx";   // TODO: NEED TO CONFIRM THIS
const char FactionTemplateEntryfmt[] = "niiiiiiiiiiiii";
const char GameObjectDisplayInfofmt[]="nsxxxxxxxxxxffffffxxx";
const char GemPropertiesEntryfmt[]="nixxix";
const char GlyphPropertiesfmt[] = "niii";
const char GlyphSlotfmt[] = "nii";
const char GtBarberShopCostBasefmt[]="xf";
const char GtCombatRatingsfmt[]="xf";
const char GtChanceToMeleeCritBasefmt[]="xf";
const char GtChanceToMeleeCritfmt[]="xf";
const char GtChanceToSpellCritBasefmt[]="xf";
const char GtOCTClassCombatRatingScalarfmt[]="df";
const char GtChanceToSpellCritfmt[]="xf";
const char GtOCTHpPerStaminafmt[]="df";
const char GtOCTRegenHPfmt[]="xf";

const char GtRegenHPPerSptfmt[]="xf";
const char GtRegenMPPerSptfmt[]="xf";
const char GtSpellScalingfmt[]="df";
const char GtOCTBaseHPByClassfmt[]="df";
const char GtOCTBaseMPByClassfmt[]="df";
const char Holidaysfmt[] = "nxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
const char ItemClassfmt[]="nifs";
const char ItemArmorQualityfmt[]="nfffffffi";
const char ItemArmorShieldfmt[]="nifffffff";
const char ItemArmorTotalfmt[]="niffff";
const char ItemBagFamilyfmt[]="nx";
// const char ItemDisplayTemplateEntryfmt[]="nxxxxxxxxxxixxxxxxxxxxx";
// const char ItemCondExtCostsEntryfmt[]="xiii";
const char ItemDamagefmt[]="nfffffffi";
const char ItemLimitCategoryEntryfmt[]="nxii";
const char ItemRandomPropertiesfmt[]="nxiiiiis";
const char ItemRandomSuffixfmt[]="nsxiiiiiiiiii";
const char ItemReforgefmt[]="nifif";
const char ItemSetEntryfmt[]="dsxxxxxxxxxxxxxxxxxiiiiiiiiiiiiiiiiii";
const char LiquidTypefmt[] = "nxxixixxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";
const char LockEntryfmt[] = "niiiiiiiiiiiiiiiiiiiiiiiixxxxxxxx";
const char MailTemplateEntryfmt[]="nxs";
const char MapEntryfmt[]="nsiiisissififfiiiii"; // 5.4.8
const char MapDifficultyEntryfmt[]="niisiis";   // TODO: NEED TO CONFIRM THIS
const char MountCapabilityfmt[]="niiiiiii";
const char MountTypefmt[]="niiiiiiiiiiiiiiiiiiiiiiii";
const char MovieEntryfmt[]="nxxxx";
const char NumTalentsAtLevelfmt[]="df";
const char OverrideSpellDatafmt[]="niiiiiiiiiixx";
const char QuestFactionRewardfmt[] = "niiiiiiiiii";
const char QuestSortEntryfmt[]="nx";
const char QuestXPLevelfmt[] = "niiiiiiiiii";
const char Phasefmt[]="nii";
const char PvPDifficultyfmt[] = "diiiii";
const char RandomPropertiesPointsfmt[] = "niiiiiiiiiiiiiii";
const char ScalingStatDistributionfmt[]="niiiiiiiiiiiiiiiiiiiixi";
const char ScalingStatValuesfmt[]="iniiiiiixiiiiiiiiiiiiiixxxxxxxxxxxxxxxxxxxxxxxxxx";   // TODO: NEED TO CONFIRM THIS
const char SkillLinefmt[]="nisxixixx";   // TODO: NEED TO CONFIRM THIS
const char SkillLineAbilityfmt[]="niiiixxiiiiii";
const char SkillRaceClassInfofmt[]="diiiiixx";
const char SoundEntriesfmt[]="nisxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";   // TODO: NEED TO CONFIRM THIS
const char SpellCastTimefmt[]="niii";
const char SpellDurationfmt[] = "niii";
const char SpellDifficultyfmt[] = "niiii";
const char SpellEntryfmt[]="nssxxixxxiiiiiiiiiiiiiixi";
const char SpellAuraOptionsEntryfmt[]="dxxiiiixx";   // TODO: NEED TO CONFIRM THIS
const char SpellAuraRestrictionsEntryfmt[]="dxxiiiiiiii";
const char SpellCastingRequirementsEntryfmt[]="dixxixi";
const char SpellCategoriesEntryfmt[]="dxxiiiiiix";
const char SpellClassOptionsEntryfmt[]="dxiiiix";
const char SpellCooldownsEntryfmt[]="dxxiii";
const char SpellEffectEntryfmt[]="diifiiixfiiiiiifiifiiiiixiiiix";
const char SpellEquippedItemsEntryfmt[]="dxxiii";
const char SpellInterruptsEntryfmt[]="dxxixixi";
const char SpellLevelsEntryfmt[]="dxxiii";
const char SpellPowerEntryfmt[]="xnxiiiiixxxxx";
const char SpellScalingEntryfmt[]="diiiifffi";   // TODO: NEED TO CONFIRM THIS
const char SpellShapeshiftEntryfmt[]="dixixx";
const char SpellTargetRestrictionsEntryfmt[]="dxxfxiiii";
const char SpellTotemsEntryfmt[]="diiii";
const char SpellFocusObjectfmt[]="nx";
const char SpellItemEnchantmentfmt[]="nxiiiiiiiiisiiiixxixxxxxx";   // TODO: NEED TO CONFIRM THIS
const char SpellItemEnchantmentConditionfmt[] = "nbbbbbxxxxxbbbbbbbbbbiiiiiXXXXX";
const char SpellMiscfmt[]="dxxiiiiiiiiiiiiiiiiifiiiii";   // TODO: NEED TO CONFIRM THIS
const char SpellRadiusfmt[]="nfxxx";
const char SpellRangefmt[]="nffffxxx";
const char SpellRuneCostfmt[]="niiixi";
const char SpellShapeshiftFormfmt[]="nxxiixiiixxiiiiiiiixx";
//const char StableSlotPricesfmt[] = "ni"; // removed
const char SummonPropertiesfmt[] = "niiiii";
const char TalentEntryfmt[]="niiiiiiiiixxixxxxxx";
const char TalentTabEntryfmt[]="nxxiiixxiii";
const char TalentTreePrimarySpellsfmt[]="diix";
const char TaxiNodesEntryfmt[]="nifffsiixxxx";   // TODO: NEED TO CONFIRM THIS
const char TaxiPathEntryfmt[] = "niii";
const char TaxiPathNodeEntryfmt[] = "diiifffiiii";
const char TotemCategoryEntryfmt[]="nxii";
const char TransportAnimationEntryfmt[]="diixxxx";
const char VehicleEntryfmt[]="nixffffiiiiiiiifffffffffffffffssssfifiixx";
const char VehicleSeatEntryfmt[]="niiffffffffffiiiiiifffffffiiifffiiiiiiiffiiiiixxxxxxxxxxxxxxxxxxxx";
const char WMOAreaTableEntryfmt[]="niiixxxxxiixxxx";
const char WorldMapAreaEntryfmt[]="xinxffffixxxxx";
const char WorldMapOverlayEntryfmt[]="nxiiiixxxxxxxxxx";
const char WorldSafeLocsEntryfmt[]="nifffxx";
const char WorldPvPAreaEnrtyfmt[]="niiiiii";

#endif