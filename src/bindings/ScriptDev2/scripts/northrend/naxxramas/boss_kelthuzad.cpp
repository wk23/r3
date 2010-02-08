/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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
 */

/* ScriptData
SDName: Boss_KelThuzud
SD%Complete: 0
SDComment: 90%
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

//when shappiron dies. dialog between kel and lich king (in this order)
#define SAY_SAPP_DIALOG1            -1533084
#define SAY_SAPP_DIALOG2_LICH       -1533085
#define SAY_SAPP_DIALOG3            -1533086
#define SAY_SAPP_DIALOG4_LICH       -1533087
#define SAY_SAPP_DIALOG5            -1533088

//when cat dies
#define SAY_CAT_DIED                -1533089

//when each of the 4 wing bosses dies
#define SAY_TAUNT1                  -1533090
#define SAY_TAUNT2                  -1533091
#define SAY_TAUNT3                  -1533092
#define SAY_TAUNT4                  -1533093

#define SAY_SUMMON_MINIONS          -1533105                //start of phase 1

#define SAY_AGGRO1                  -1533094                //start of phase 2
#define SAY_AGGRO2                  -1533095
#define SAY_AGGRO3                  -1533096

#define SAY_SLAY1                   -1533097
#define SAY_SLAY2                   -1533098

#define SAY_DEATH                   -1533099

#define SAY_CHAIN1                  -1533100
#define SAY_CHAIN2                  -1533101
#define SAY_FROST_BLAST             -1533102

#define SAY_REQUEST_AID             -1533103                //start of phase 3
#define SAY_ANSWER_REQUEST          -1533104                //lich king answer

#define SAY_SPECIAL1_MANA_DET       -1533106
#define SAY_SPECIAL3_MANA_DET       -1533107
#define SAY_SPECIAL2_DISPELL        -1533108

//***THIS SCRIPTS IS UNDER DEVELOPMENT***
/*
DATA.
This script has been made with info taken from wowwikki... so there are things wrong...
like spell timers and Says. Also there's another major problem as there is no aggroed list
I cannot make Kel'thuzad to target specific party members, that is needed for spells like
Mana Detonation... so what I'm doing untill now is just to cast everything on my main aggroed
target. Sorry for him.
Another bug is that there are spells that are actually NOT working... I have to implement
them first.
Need DISPELL efect
I also don't know the emotes
*/

//Positional defines
#define ADDX_LEFT_FAR               3783.272705
#define ADDY_LEFT_FAR               -5062.697266
#define ADDZ_LEFT_FAR               143.711203
#define ADDO_LEFT_FAR               3.617599

#define ADDX_LEFT_MIDDLE            3730.291260
#define ADDY_LEFT_MIDDLE            -5027.239258
#define ADDZ_LEFT_MIDDLE            143.956909
#define ADDO_LEFT_MIDDLE            4.461900

#define ADDX_LEFT_NEAR              3683.868652
#define ADDY_LEFT_NEAR              -5057.281250
#define ADDZ_LEFT_NEAR              143.183884
#define ADDO_LEFT_NEAR              5.237086

#define ADDX_RIGHT_FAR              3759.355225
#define ADDY_RIGHT_FAR              -5174.128418
#define ADDZ_RIGHT_FAR              143.802383
#define ADDO_RIGHT_FAR              2.170104

#define ADDX_RIGHT_MIDDLE           370.724365
#define ADDY_RIGHT_MIDDLE           -5185.123047
#define ADDZ_RIGHT_MIDDLE           143.928024
#define ADDO_RIGHT_MIDDLE           1.309310

#define ADDX_RIGHT_NEAR             3665.121094
#define ADDY_RIGHT_NEAR             -5138.679199
#define ADDZ_RIGHT_NEAR             143.183212
#define ADDO_RIGHT_NEAR             0.604023

#define WALKX_LEFT_FAR              3754.431396
#define WALKY_LEFT_FAR              -5080.727734
#define WALKZ_LEFT_FAR              142.036316
#define WALKO_LEFT_FAR              3.736189

#define WALKX_LEFT_MIDDLE           3724.396484
#define WALKY_LEFT_MIDDLE           -5061.330566
#define WALKZ_LEFT_MIDDLE           142.032700
#define WALKO_LEFT_MIDDLE           4.564785

#define WALKX_LEFT_NEAR             3687.158424
#define WALKY_LEFT_NEAR             -5076.834473
#define WALKZ_LEFT_NEAR             142.017319
#define WALKO_LEFT_NEAR             5.237086

#define WALKX_RIGHT_FAR             3687.571777
#define WALKY_RIGHT_FAR             -5126.831055
#define WALKZ_RIGHT_FAR             142.017807
#define WALKO_RIGHT_FAR             0.604023

#define WALKX_RIGHT_MIDDLE          3707.990733
#define WALKY_RIGHT_MIDDLE          -5151.450195
#define WALKZ_RIGHT_MIDDLE          142.032562
#define WALKO_RIGHT_MIDDLE          1.376855

#define WALKX_RIGHT_NEAR            3739.500000
#define WALKY_RIGHT_NEAR            -5141.883989
#define WALKZ_RIGHT_NEAR            142.0141130
#define WALKO_RIGHT_NEAR            2.121412

//spells to be casted
#define SPELL_FROST_BOLT            28478
#define H_SPELL_FROST_BOLT          55802
#define SPELL_FROST_BOLT_NOVA       28479
#define H_SPELL_FROST_BOLT_NOVA     55807

#define SPELL_CHAINS_OF_KELTHUZAD   28410                   //casted spell should be 28408. Also as of 303, heroic only
#define SPELL_MANA_DETONATION       27819
#define SPELL_SHADOW_FISURE         27810
#define SPELL_FROST_BLAST           27808

float AddNpc[7][4]=
{
    {3763.591553, -5074.789062, 143.182022, 3.698747},
    {3727.601074, -5049.215332, 143.281937, 4.492000},
    {3685.690430, -5062.457520, 143.179794, 5.501236},
    {3660.546875, -5096.548828, 143.284775, 6.029805},
    {3668.957764, -5139.075684, 143.181763, 0.743290},
    {3704.820312, -5164.700684, 143.285233, 1.363758},
    {3749.049805, -5155.220703, 143.182693, 2.124021},
};

float AddLocations[42][4]=
{
    // banshe
    {3757.66, -5150.64, 143.565, 2.0745},
    {3760,    -5158.89, 143.591, 2.0745},
    {3742.31, -5160.77, 143.431, 2.06665},
    {3701.89, -5163.56, 143.385, 1.15951},
    {3666.62, -5150.67, 143.652, 0.561034},
    {3668.46, -5141.2,  143.18,  0.546897},
    {3662.76, -5135.29, 143.354, 0.351333},
    {3659.97, -5093.71, 143.321, 6.00542},
    {3677.67, -5064.14, 143.294, 5.22316},
    {3691.84, -5055.26, 143.229, 5.55617},
    {3725.44, -5037.12, 143.668, 4.49668},
    {3729.52, -5049.37, 143.336, 4.40243},
    // aboms
    {3750.27, -5158.44, 143.184, 2.16641},
    {3712.03, -5170.32, 143.687, 1.36845},
    {3704.83, -5175.21, 143.573, 1.36845},
    {3706.34, -5167.82, 143.416, 1.36845},
    {3696.52, -5165.81, 143.639, 1.36845},
    {3663.72, -5144.91, 143.339, 0.656883},
    {3683.92, -5059.68, 143.182, 5.37085},
    {3676.37, -5056.48, 143.462, 5.37085},
    {3685.22, -5049.63, 143.474, 5.37085},
    {3735.63, -5042.05, 143.622, 4.36632},
    {3722.02, -5042.97, 143.63,  4.75588},
    {3767.75, -5071.23, 143.229, 3.64297},
    // skelets
    {3761.25, -5065.75, 143.558, 3.92887},
    {3772.62, -5066.32, 143.461, 4.0671},
    {3753.59, -5155.48, 143.336, 2.17114},
    {3745.85, -5160.78, 143.385, 2.17114},
    {3702.45, -5166.98, 143.377, 1.03231},
    {3664.42, -5138.49, 143.18,  0.57835},
    {3670.32, -5146.32, 143.453, 0.587775},
    {3660.21, -5087.45, 143.606, 6.12091},
    {3649.8,  -5090.29, 143.428, 1.6677},
    {3650.77, -5098.65, 143.557, 2.88978},
    {3657.9,  -5098.26, 143.412, 4.62787},
    {3655.47, -5091.61, 143.441, 4.62787},
    {3681.22, -5057.02, 143.181, 5.45725},
    {3728.03, -5044.25, 143.371, 4.56739},
    {3733.38, -5046.63, 143.527, 4.56739},
    {3773.44, -5072.12, 143.389, 5.44939},
    {3766.86, -5067.02, 143.449, 4.0671},
    {3769.91, -5075.19, 143.327, 3.74979},
};

struct MANGOS_DLL_DECL boss_kelthuzadAI : public ScriptedAI
{
    boss_kelthuzadAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        GuardiansOfIcecrown[0] = 0;
        GuardiansOfIcecrown[1] = 0;
        GuardiansOfIcecrown[2] = 0;
        GuardiansOfIcecrown[3] = 0;
        GuardiansOfIcecrown[4] = 0;
        GuardiansOfIcecrown_Count = 0;
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    uint64 GuardiansOfIcecrown[5];
    uint64 MobsArround[42];
    bool m_bIsRegularMode;

    uint32 AbomsTimer;
    uint32 BansheTimer;
    uint32 SkeletalTimer;

    uint32 GuardiansOfIcecrown_Count;
    uint32 GuardiansOfIcecrown_Timer;
    uint32 FrostBolt_Timer;
    uint32 FrostBoltNova_Timer;
    uint32 ChainsOfKelthuzad_Timer;
    uint32 ManaDetonation_Timer;
    uint32 ShadowFisure_Timer;
    uint32 FrostBlast_Timer;
    uint32 ChainsOfKelthuzad_Targets;
    uint32 Phase1_Timer;
    bool Phase1;
    bool Phase2;
    bool Phase3;

    void Reset()
    {
        FrostBolt_Timer = (rand()%60)*1000;                 //It won't be more than a minute without cast it
        FrostBoltNova_Timer = 15000;                        //Cast every 15 seconds
        ChainsOfKelthuzad_Timer = (rand()%30+30)*1000;      //Cast no sooner than once every 30 seconds
        ManaDetonation_Timer = 20000;                       //Seems to cast about every 20 seconds
        ShadowFisure_Timer = 25000;                         //25 seconds
        FrostBlast_Timer = (rand()%30+30)*1000;             //Random time between 30-60 seconds
        GuardiansOfIcecrown_Timer = 5000;                   //5 seconds for summoning each Guardian of Icecrown in phase 3
        GuardiansOfIcecrown_Count = 0;

        SkeletalTimer = 3000; // 71
        AbomsTimer = 10000; // 8
        BansheTimer = 10000; // 8

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);

        for(int i=0; i<5; i++)
        {
            if (GuardiansOfIcecrown[i])
            {
                //delete creature
                if (Creature* pGuardian = (Creature*)Unit::GetUnit(*m_creature, GuardiansOfIcecrown[i]))
                {
                    if (pGuardian->isAlive())
                        pGuardian->ForcedDespawn();
                }

                GuardiansOfIcecrown[i] = 0;
            }
        }

        for(int i=0; i<42; i++)
        {
            if (MobsArround[i])
            {
                //delete creature
                if (Creature* pGuardian = (Creature*)Unit::GetUnit(*m_creature, MobsArround[i]))
                {
                    if (pGuardian->isAlive())
                        pGuardian->ForcedDespawn();
                }

                MobsArround[i] = 0;
            }
        }

        Phase1_Timer = 228000;                              //Phase 1 lasts 3 minutes and 48 seconds
        Phase1 = true;
        Phase2 = false;
        Phase3 = false;
        m_creature->HandleEmoteCommand(EMOTE_ONESHOT_NONE);

        if (m_pInstance)
            m_pInstance->SetData(ENCOUNT_KELTHUZAD, NOT_STARTED);
    }

    void KilledUnit()
    {
        if (rand()%2)
            DoScriptText(SAY_SLAY1, m_creature);
        else
            DoScriptText(SAY_SLAY2, m_creature);
    }

    void JustDied(Unit* Killer)
    {
        if (m_pInstance)
            m_pInstance->SetData(ENCOUNT_KELTHUZAD, DONE);

        DoScriptText(SAY_DEATH, m_creature);

        for(int i=0; i<5; i++)
        {
            if (GuardiansOfIcecrown[i])
            {
                Creature* pGuardian = (Creature*)Unit::GetUnit(*m_creature, GuardiansOfIcecrown[i]);

                if (!pGuardian || !pGuardian->isAlive())
                    continue;

                pGuardian->CombatStop();

                float Walk_Pos_X = 0.0f;
                float Walk_Pos_Y = 0.0f;
                float Walk_Pos_Z = 0.0f;

                switch(rand()%6)
                {
                    case 0:
                        Walk_Pos_X = ADDX_LEFT_FAR;
                        Walk_Pos_Y = ADDY_LEFT_FAR;
                        Walk_Pos_Z = ADDZ_LEFT_FAR;
                        break;
                    case 1:
                        Walk_Pos_X = ADDX_LEFT_MIDDLE;
                        Walk_Pos_Y = ADDY_LEFT_MIDDLE;
                        Walk_Pos_Z = ADDZ_LEFT_MIDDLE;
                        break;
                    case 2:
                        Walk_Pos_X = ADDX_LEFT_NEAR;
                        Walk_Pos_Y = ADDY_LEFT_NEAR;
                        Walk_Pos_Z = ADDZ_LEFT_NEAR;
                        break;
                    case 3:
                        Walk_Pos_X = ADDX_RIGHT_FAR;
                        Walk_Pos_Y = ADDY_RIGHT_FAR;
                        Walk_Pos_Z = ADDZ_RIGHT_FAR;
                        break;
                    case 4:
                        Walk_Pos_X = ADDX_RIGHT_MIDDLE;
                        Walk_Pos_Y = ADDY_RIGHT_MIDDLE;
                        Walk_Pos_Z = ADDZ_RIGHT_MIDDLE;
                        break;
                    case 5:
                        Walk_Pos_X = ADDX_RIGHT_NEAR;
                        Walk_Pos_Y = ADDY_RIGHT_NEAR;
                        Walk_Pos_Z = ADDZ_RIGHT_NEAR;
                        break;
                }

                pGuardian->SendMonsterMoveWithSpeed(Walk_Pos_X, Walk_Pos_Y, Walk_Pos_Z);
                pGuardian->ForcedDespawn();
            }
        }
    }

    void SpawnNpc()
    {
        // spawn banshe
        for (int i = 0; i < 12; i++)
        {
            if (Creature* pBanshe = m_creature->SummonCreature(16429,AddLocations[i][0],AddLocations[i][1],AddLocations[i][2],AddLocations[i][3],TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,600000))
                MobsArround[i] = pBanshe->GetGUID();
        }
        
        // spawn aboms
        for (int i = 12; i < 24; i++)
        {
            if (Creature* pAboms = m_creature->SummonCreature(16428,AddLocations[i][0],AddLocations[i][1],AddLocations[i][2],AddLocations[i][3],TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,600000))
                MobsArround[i] = pAboms->GetGUID();
        }

        // spawn skelets
        for (int i = 24; i < 42; i++)
        {
            if (Creature* pSkelets = m_creature->SummonCreature(16427,AddLocations[i][0],AddLocations[i][1],AddLocations[i][2],AddLocations[i][3],TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,600000))
                MobsArround[i] = pSkelets->GetGUID();
        }
    }

    void Aggro(Unit* who)
    {
        DoScriptText(SAY_SUMMON_MINIONS, m_creature);
        SpawnNpc();
        SetCombatMovement(false);
        m_creature->SetInCombatWithZone();

        if (m_pInstance)
            m_pInstance->SetData(ENCOUNT_KELTHUZAD, IN_PROGRESS);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        if (Phase1)
        {
            if (!m_creature->HasAura(29423))
                DoCast(m_creature,29423,true);

            if (SkeletalTimer <= diff)
            {
                uint8 j = (rand()%7);
                if (Creature* pSkelet = m_creature->SummonCreature(16427,AddNpc[j][0],AddNpc[j][1],AddNpc[j][2],AddNpc[j][3],TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,20000))
                    if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                    {
                        pSkelet->SetNoCallAssistance(true);
                        pSkelet->AI()->AttackStart(target);
                    }
                
                SkeletalTimer = 4000;
            }else SkeletalTimer -= diff;

            if (AbomsTimer <= diff)
            {
                uint8 j = (rand()%7);
                if (Creature* pAbom = m_creature->SummonCreature(16428,AddNpc[j][0],AddNpc[j][1],AddNpc[j][2],AddNpc[j][3],TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,20000))
                    if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                    {
                        pAbom->SetNoCallAssistance(true);
                        pAbom->AI()->AttackStart(target);
                    }
                AbomsTimer = 25000;
            }else AbomsTimer -= diff;

            if (BansheTimer <= diff)
            {
                uint8 j = (rand()%7);
                if (Creature* pBanshe = m_creature->SummonCreature(16429,AddNpc[j][0],AddNpc[j][1],AddNpc[j][2],AddNpc[j][3],TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,20000))
                    if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                    {
                        pBanshe->SetNoCallAssistance(true);
                        pBanshe->AI()->AttackStart(target);
                    }

                BansheTimer = 23000;
            }else BansheTimer -= diff;


            if (Phase1_Timer - 15000 <= diff)
            {
                switch(rand()%3)
                {
                case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
                case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
                case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
                }

                for(int i=0; i<42; i++)
                {
                    if (MobsArround[i])
                    {
                        //delete creature
                        if (Creature* pGuardian = (Creature*)Unit::GetUnit(*m_creature, MobsArround[i]))
                            if (pGuardian->isAlive())
                                pGuardian->ForcedDespawn();

                        MobsArround[i] = 0;
                    }
                }
            }

            if (Phase1_Timer <= diff)
            {
                Phase1 = false;
                Phase2 = true;
                
                m_creature->RemoveAurasDueToSpell(29423);
                m_creature->HandleEmoteCommand(EMOTE_ONESHOT_NONE);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                SetCombatMovement(true);
                m_creature->GetMotionMaster()->Clear();
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
            } else Phase1_Timer -= diff;
        }

        if (Phase2)
        {
            //Check for Frost Bolt
            if (FrostBolt_Timer < diff)
            {
                DoCast(m_creature->getVictim(),!m_bIsRegularMode?H_SPELL_FROST_BOLT:SPELL_FROST_BOLT);
                FrostBolt_Timer = (rand()%60)*1000;
            }else FrostBolt_Timer -= diff;

            //Check for Frost Bolt Nova
            if (FrostBoltNova_Timer < diff)
            {
                DoCast(m_creature->getVictim(),!m_bIsRegularMode?H_SPELL_FROST_BOLT_NOVA:SPELL_FROST_BOLT_NOVA);
                FrostBoltNova_Timer = 15000;
            }else FrostBoltNova_Timer -= diff;

            //Check for Chains Of Kelthuzad
            if (ChainsOfKelthuzad_Timer < diff)
            {
                //DoCast(m_creature->getVictim(),SPELL_CHAINS_OF_KELTHUZAD);

                if (!m_bIsRegularMode)
                {

                    if (rand()%2)
                        DoScriptText(SAY_CHAIN1, m_creature);
                    else
                        DoScriptText(SAY_CHAIN2, m_creature);

                    DoResetThreat(); // 50% chance WIPE
                }
                
                ChainsOfKelthuzad_Timer = (rand()%30+30)*1000;
            }else ChainsOfKelthuzad_Timer -= diff;

            //Check for Mana Detonation
            if (ManaDetonation_Timer < diff)
            {
                Unit* pTarget = NULL;
                ThreatList const& t_list = m_creature->getThreatManager().getThreatList();
                std::vector<Unit *> target_list;
                for(ThreatList::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                {
                    pTarget = Unit::GetUnit(*m_creature, (*itr)->getUnitGuid());
                    if (pTarget && pTarget->GetTypeId() == TYPEID_PLAYER && pTarget->getPowerType() == POWER_MANA)
                        target_list.push_back(pTarget);
                    pTarget = NULL;
                }
                if (target_list.size())
                    pTarget = *(target_list.begin()+rand()%target_list.size());

                DoCast(pTarget,SPELL_MANA_DETONATION, true);

                if (rand()%2)
                    DoScriptText(SAY_SPECIAL1_MANA_DET, m_creature);

                ManaDetonation_Timer = 20000;
            }else ManaDetonation_Timer -= diff;

            //Check for Shadow Fissure
            if (ShadowFisure_Timer < diff)
            {
                DoCast(SelectUnit(SELECT_TARGET_RANDOM,0),SPELL_SHADOW_FISURE, true);

                if (rand()%2)
                    DoScriptText(SAY_SPECIAL3_MANA_DET, m_creature);

                ShadowFisure_Timer = 25000;
            }else ShadowFisure_Timer -= diff;

            //Check for Frost Blast
            if (FrostBlast_Timer < diff)
            {
                Unit* pTarget = NULL;
                ThreatList const& t_list = m_creature->getThreatManager().getThreatList();
                std::vector<Unit *> target_list;
                //std::vector<Unit *> targets;
                //std::vector<Unit *> targets_2;
                // find random player target
                for(ThreatList::const_iterator itr = t_list.begin(); itr!= t_list.end(); ++itr)
                {
                    pTarget = Unit::GetUnit(*m_creature, (*itr)->getUnitGuid());
                    if (pTarget && pTarget->GetTypeId() == TYPEID_PLAYER)
                        target_list.push_back(pTarget);
                    pTarget = NULL;
                }
                if (target_list.size())
                    pTarget = *(target_list.begin()+rand()%target_list.size());
                
                /*targets.push_back(pTarget);

                // find near players
                if (Group* pGroup = ((Player*)pTarget)->GetGroup())
                {
                    for(GroupReference *itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
                    {
                        Player *pGroupie = itr->getSource();
                        if (pGroupie && pGroupie != pTarget && pGroupie->IsWithinDistInMap(pTarget,9.5f))
                            targets.push_back(pGroupie);
                    }
                }

                targets_2 = targets;

                // find another players
                for(std::vector<Unit *>::iterator iter = targets_2.begin(); iter!= targets_2.end(); ++iter)
                {
                    Unit *target = *iter;
                    if (target)
                    {
                        if (Group* pGroup = ((Player*)target)->GetGroup())
                        {
                            for(GroupReference *itr = pGroup->GetFirstMember(); itr != NULL; itr = itr->next())
                            {
                                Player *pGroupie = itr->getSource();
                                if (pGroupie && pGroupie != target && pGroupie->IsWithinDistInMap(target,9.5f))
                                    targets.push_back(pGroupie);
                            }
                        }
                    }
                }

                std::set<Unit *> iset(targets.begin(), targets.end());

                for(std::set<Unit *>::iterator iter = iset.begin(); iter!= iset.end(); ++iter)
                {
                    DoCast(*iter,SPELL_FROST_BLAST,true);
                }

                target_list.clear();
                targets.clear();
                targets_2.clear();
                iset.clear();*/

                DoCast(pTarget,SPELL_FROST_BLAST, true);

                if (rand()%2)
                    DoScriptText(SAY_FROST_BLAST, m_creature);

                FrostBlast_Timer = (rand()%30+30)*1000;
            }else FrostBlast_Timer -= diff;
        }

        //start phase 3 when we are 40% health
        if (!Phase3 && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 40)
        {
            Phase3 = true;
            DoScriptText(SAY_REQUEST_AID, m_creature);
            //here Lich King should respond to KelThuzad but I don't know which creature to make talk
            //so for now just make Kelthuzad says it.
            DoScriptText(SAY_ANSWER_REQUEST, m_creature);
        }

        if (Phase3 && (GuardiansOfIcecrown_Count < (!m_bIsRegularMode ? 4:2) ))
        {
            if (GuardiansOfIcecrown_Timer < diff)
            {
                //Summon a Guardian of Icecrown in a random alcove (Creature # 16441)
                //uint32 TimeToWalk;
                Creature* pGuardian = NULL;

                float Walk_Pos_X;
                float Walk_Pos_Y;
                float Walk_Pos_Z;

                switch(rand()%6)
                {
                    case 0:
                        pGuardian = m_creature->SummonCreature(16441,ADDX_LEFT_FAR,ADDY_LEFT_FAR,ADDZ_LEFT_FAR,ADDO_LEFT_FAR,TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,12000);
                        //Setting walk position
                        Walk_Pos_X = WALKX_LEFT_FAR;
                        Walk_Pos_Y = WALKY_LEFT_FAR;
                        Walk_Pos_Z = WALKZ_LEFT_FAR;
                        break;
                    case 1:
                        pGuardian = m_creature->SummonCreature(16441,ADDX_LEFT_MIDDLE,ADDY_LEFT_MIDDLE,ADDZ_LEFT_MIDDLE,ADDO_LEFT_MIDDLE,TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,12000);
                        //Start moving guardian towards the center of the room
                        Walk_Pos_X = WALKX_LEFT_MIDDLE;
                        Walk_Pos_Y = WALKY_LEFT_MIDDLE;
                        Walk_Pos_Z = WALKZ_LEFT_MIDDLE;
                        break;
                    case 2:
                        pGuardian = m_creature->SummonCreature(16441,ADDX_LEFT_NEAR,ADDY_LEFT_NEAR,ADDZ_LEFT_NEAR,ADDO_LEFT_NEAR,TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,12000);
                        //Start moving guardian towards the center of the room
                        Walk_Pos_X = WALKX_LEFT_NEAR;
                        Walk_Pos_Y = WALKY_LEFT_NEAR;
                        Walk_Pos_Z = WALKZ_LEFT_NEAR;
                        break;
                    case 3:
                        pGuardian = m_creature->SummonCreature(16441,ADDX_RIGHT_FAR,ADDY_RIGHT_FAR,ADDZ_RIGHT_FAR,ADDO_RIGHT_FAR,TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,12000);
                        //Start moving guardian towards the center of the room
                        Walk_Pos_X = WALKX_RIGHT_FAR;
                        Walk_Pos_Y = WALKY_RIGHT_FAR;
                        Walk_Pos_Z = WALKZ_RIGHT_FAR;
                        break;
                    case 4:
                        pGuardian = m_creature->SummonCreature(16441,ADDX_RIGHT_MIDDLE,ADDY_RIGHT_MIDDLE,ADDZ_RIGHT_MIDDLE,ADDO_RIGHT_MIDDLE,TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,12000);
                        //Start moving guardian towards the center of the room
                        Walk_Pos_X = WALKX_RIGHT_MIDDLE;
                        Walk_Pos_Y = WALKY_RIGHT_MIDDLE;
                        Walk_Pos_Z = WALKZ_RIGHT_MIDDLE;
                        break;
                    case 5:
                        pGuardian = m_creature->SummonCreature(16441,ADDX_RIGHT_NEAR,ADDY_RIGHT_NEAR,ADDZ_RIGHT_NEAR,ADDO_RIGHT_NEAR,TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN,12000);
                        //Start moving guardian towards the center of the room
                        Walk_Pos_X = WALKX_RIGHT_NEAR;
                        Walk_Pos_Y = WALKY_RIGHT_NEAR;
                        Walk_Pos_Z = WALKZ_RIGHT_NEAR;
                        break;
                }

                if (pGuardian)
                {
                    //if we find no one to figth walk to the center
                    if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                        pGuardian->AI()->AttackStart(target);

                    //Safe storing of creatures
                    GuardiansOfIcecrown[GuardiansOfIcecrown_Count] = pGuardian->GetGUID();

                    //Update guardian count
                    GuardiansOfIcecrown_Count++;
                }

                //5 seconds until summoning next guardian
                GuardiansOfIcecrown_Timer = 5000;
            }else GuardiansOfIcecrown_Timer -= diff;
        }

        if (!Phase1)
            DoMeleeAttackIfReady();
    }
};

/*######
## Mob Shadow Fissure
######*/

struct MANGOS_DLL_DECL mob_shadow_fisureAI : public ScriptedAI
{
    mob_shadow_fisureAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();         
    }

    uint32 m_uiDespawn;
    uint32 m_uiVoidTimer;
    bool m_bIsRegularMode;

    void Aggro(Unit* who)
    {
        DoStopAttack();
        SetCombatMovement(false);
    }

    void Reset()
    {
        m_uiDespawn = 5000;
        m_uiVoidTimer = 3500;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiVoidTimer <= uiDiff)
        {
            DoCast(m_creature,27812,true);
            m_uiVoidTimer = 3000;
        }else m_uiVoidTimer -= uiDiff;

        if (m_uiDespawn <= uiDiff)
            m_creature->ForcedDespawn();
        else m_uiDespawn -= uiDiff;
    }
};

/*######
## Mob Soul Weaver
######*/

struct MANGOS_DLL_DECL mob_soul_weaverAI : public ScriptedAI
{
    mob_soul_weaverAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();         
    }

    void Reset()
    {
    }


    bool m_bIsRegularMode;

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->HasAura(!m_bIsRegularMode ? 55717 : 28460))
            DoCast(m_creature, !m_bIsRegularMode ? 55717 : 28460, true);

        DoMeleeAttackIfReady();
    }
};

/*######
## Mob Frozen Wastes
######*/

struct MANGOS_DLL_DECL mob_frozen_wastesAI : public ScriptedAI
{
    mob_frozen_wastesAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();         
    }

    bool m_bIsRegularMode;

    void Reset()
    {
    }


    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->HasAura(!m_bIsRegularMode ? 55713 : 28458))
            DoCast(m_creature, !m_bIsRegularMode ? 55713 : 28458, true);

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_frozen_wastes(Creature* pCreature)
{
    return new mob_frozen_wastesAI(pCreature);
}

CreatureAI* GetAI_mob_soul_weaver(Creature* pCreature)
{
    return new mob_soul_weaverAI(pCreature);
}

CreatureAI* GetAI_mob_shadow_fisure(Creature* pCreature)
{
    return new mob_shadow_fisureAI(pCreature);
}

CreatureAI* GetAI_boss_kelthuzadAI(Creature* pCreature)
{
    return new boss_kelthuzadAI(pCreature);
}

void AddSC_boss_kelthuzad()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_kelthuzad";
    newscript->GetAI = &GetAI_boss_kelthuzadAI;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_shadow_fisure";
    newscript->GetAI = &GetAI_mob_shadow_fisure;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_soul_weaver";
    newscript->GetAI = &GetAI_mob_soul_weaver;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_frozen_wastes";
    newscript->GetAI = &GetAI_mob_frozen_wastes;
    newscript->RegisterSelf();
}