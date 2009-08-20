/* Copyright (C) 2006 - 2009 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
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

#include "precompiled.h"


//emotes
#define SAY_SUMMON_MINIONS          -1533105

#define SAY_REQUEST_AID             -1533103
#define SAY_ANSWER_REQUEST          -1533104

#define SAY_AGGRO1                  -1533094
#define SAY_AGGRO2                  -1533095
#define SAY_AGGRO3                  -1533096

#define SAY_DEATH                   -1533099

//summoned creatures
#define CR_SKELETON     16427
#define CR_BANSHEE      16429
#define CR_ABOMINATION  16428
#define CR_GUARDIAN     16441

//Kel'Thuzad spells
#define SP_FROSTBOLT        28478
#define H_SP_FROSTBOLT      55802
#define SP_FROSTBOLT_VOLLEY 28479
#define H_SP_FROSTBOLT_VOLLEY   55807
#define SP_FROST_BLAST      27808
#define SP_DETONATE_MANA    27819
#define SP_SHADOW_FISSURE   27810

#define MAX_MOBS_PHASE1     64


struct MANGOS_DLL_DECL boss_kelthuzadAI : public ScriptedAI
{
    boss_kelthuzadAI(Creature* c) : ScriptedAI(c)
    {
        Heroic = m_creature->GetMap()->IsHeroic();
        MaxGuardians = Heroic ? 4 : 2;
        Reset();
    }

    //Variables
    bool Heroic;
    bool stopped;
    uint32 phase;
    uint32 Phase1_Timer;
    //
    uint32 Skeleton_Timer;
    uint32 Abomination_Timer;
    uint32 Banshee_Timer;
    //
    uint32 Frostbolt_Timer;
    uint32 FrostboltV_Timer;
    uint32 FrostBlast_Timer;
    //uint32 DetonateMana_Timer;
    //uint32 ShadowFissure_Timer;
    //
    uint64 Guardians[4];
    uint32 Guardian_Count;
    uint64 Phase1Mobs[MAX_MOBS_PHASE1];
    uint32 Phase1Mobs_Count;
    uint32 MaxGuardians;
    uint32 Guardian_Timer;
    //adds coords
    float AddX[6];
    float AddY[6];
    float AddZ[6];

    void Reset()
    {
        phase = 1;
        stopped = false;
        Phase1_Timer = 220000; // 3m 40s

        AddX[0] = 3783.272705; AddY[0] = -5062.697266; AddZ[0] = 143.711203; //left far
        AddX[1] = 3759.355225; AddY[1] = -5174.128418; AddZ[1] = 143.802383; //right far
        AddX[2] = 3730.291260; AddY[2] = -5027.239258; AddZ[2] = 143.956909; //left middle
        AddX[3] = 3700.724365; AddY[3] = -5185.123047; AddZ[3] = 143.928024; //right middle
        AddX[4] = 3683.868652; AddY[4] = -5057.281250; AddZ[4] = 143.183884; //left near
        AddX[5] = 3665.121094; AddY[5] = -5138.679199; AddZ[5] = 143.183212; //right near

        Skeleton_Timer = 3000;
        Abomination_Timer = 30000;
        Banshee_Timer = 15000;

        Frostbolt_Timer = 5000 + rand()%10000;
        FrostboltV_Timer = 15000 + rand()%5000;
        FrostBlast_Timer = 30000 + rand()%30000;
        //DetonateMana_Timer = 30000 + rand()%30000;
        //ShadowFissure_Timer = 15000 + rand()%25000;

        Guardian_Timer = 8000;
        Guardian_Count = 0;
        Guardians[0] = 0;
        Guardians[1] = 0;
        Guardians[2] = 0;
        Guardians[3] = 0;
        Phase1Mobs_Count = 0;
        for(int i=0; i<64; i++) Phase1Mobs[i]=0;
    }

    void Aggro(Unit *unit)
    {
        DoScriptText(SAY_SUMMON_MINIONS, m_creature);

    }

    void JustDied(Unit* Killer) //despawn guardians here
    {
        DoScriptText(SAY_DEATH, m_creature);
        for(int i=0; i<4; i++)
        {
            if(Guardians[i])
            {
                Unit* guard = Unit::GetUnit((*m_creature), Guardians[i]);
                if(guard && guard->isAlive())
                    guard->DealDamage(guard, guard->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim())
            return;

        if(phase==1)
        {
            if(!stopped)
            {
                m_creature->GetMotionMaster()->Clear(false);
                m_creature->GetMotionMaster()->MoveIdle();
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                stopped = true;
            }

            if(Phase1_Timer < diff)
            {
                switch(rand()%3)
                {
                    case 0: DoScriptText(SAY_AGGRO1, m_creature); break;
                    case 1: DoScriptText(SAY_AGGRO2, m_creature); break;
                    case 2: DoScriptText(SAY_AGGRO3, m_creature); break;
                }
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->GetMotionMaster()->Clear(false);
                m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());

                //despawn mobs
                for(int i=0; i<Phase1Mobs_Count; i++)
                {
                    if(Phase1Mobs[i])
                    {
                        Unit *mob = Unit::GetUnit((*m_creature), Phase1Mobs[i]);
                        if(mob) mob->RemoveFromWorld();
                    }
                }

                phase = 2;
            }
            else Phase1_Timer -= diff;

            if(Skeleton_Timer < diff)
            {
                int i = irand(0,5);
                Unit *mob = NULL;
                //mob = DoSpawnCreature(CR_SKELETON, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
                mob = m_creature->SummonCreature(CR_SKELETON, AddX[i], AddY[i], AddZ[i], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
                Unit *target = SelectUnit(SELECT_TARGET_RANDOM,0);
                mob->AddThreat(target, 1.0f);
                mob->GetMotionMaster()->MoveChase(target);
                Skeleton_Timer = 3000;
            }
            else Skeleton_Timer -= diff;

            if(Abomination_Timer < diff && Phase1Mobs_Count <= MAX_MOBS_PHASE1)
            {
                int i = irand(0,5);
                Unit *mob = NULL;
                //mob = DoSpawnCreature(CR_ABOMINATION, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
                mob = m_creature->SummonCreature(CR_ABOMINATION, AddX[i], AddY[i], AddZ[i], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
                Unit *target = SelectUnit(SELECT_TARGET_RANDOM,0);
                mob->AddThreat(target, 1.0f);
                mob->GetMotionMaster()->MoveChase(target);
                Abomination_Timer = Heroic ? (20000+rand()%15000) : (30000+rand()%15000);

                Phase1Mobs[Phase1Mobs_Count] = mob->GetGUID();
                Phase1Mobs_Count++;
            }
            else Abomination_Timer -= diff;

            if(Banshee_Timer < diff && Phase1Mobs_Count <= MAX_MOBS_PHASE1)
            {
                int i = irand(0,5);
                Unit *mob = NULL;
                //mob = DoSpawnCreature(CR_BANSHEE, 0, 0, 0, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
                mob = m_creature->SummonCreature(CR_BANSHEE, AddX[i], AddY[i], AddZ[i], 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
                Unit *target = SelectUnit(SELECT_TARGET_RANDOM,0);
                mob->AddThreat(target, 1.0f);
                mob->GetMotionMaster()->MoveChase(target);
                Banshee_Timer = Heroic ? (12000+rand()%10000) : (15000+rand()%15000);

                Phase1Mobs[Phase1Mobs_Count] = mob->GetGUID();
                Phase1Mobs_Count++;
            }
            else Banshee_Timer -= diff;
        }

        if(phase==2 || phase==3)
        {
            //Frostbolt Volley
            if(FrostboltV_Timer < diff)
            {
                DoCast(m_creature->getVictim(), Heroic ? H_SP_FROSTBOLT_VOLLEY : SP_FROSTBOLT_VOLLEY);
                FrostboltV_Timer = 15000 + rand()%5000;
            }
            else FrostboltV_Timer -= diff;

            //Frost Blast
            if(FrostBlast_Timer < diff)
            {
                if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                    DoCast(target, SP_FROST_BLAST);
                FrostBlast_Timer = 20000 + rand()%30000;
            }
            else FrostBlast_Timer -= diff;

            /*if(ShadowFissure_Timer < diff)
            {
                if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                    DoCast(target, SP_SHADOW_FISSURE);
                ShadowFissure_Timer = 15000 + rand()%25000;
            }
            else ShadowFissure_Timer -= diff;*/

            //Detonate Mana
            /*if(DetonateMana_Timer < diff)
            {
                if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                    DoCast(target, SP_DETONATE_MANA);
                DetonateMana_Timer = 30000 + rand()%30000;
            }
            else DetonateMana_Timer -= diff;*/

            //Frostbolt
            if(Frostbolt_Timer < diff)
            {
                DoCast(m_creature->getVictim(), Heroic ? H_SP_FROSTBOLT : SP_FROSTBOLT);
                Frostbolt_Timer = 5000 + rand()%10000;
            }
            else Frostbolt_Timer -= diff;
        }

        if (phase!=3 && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 40) 
        {
            //DoScriptText(SAY_REQUEST_AID, m_creature);
            DoScriptText(SAY_ANSWER_REQUEST, m_creature);
            phase=3;
        }

        if(phase==3)
        {
            if(Guardian_Count < MaxGuardians)
            {
                if(Guardian_Timer < diff)
                {
                    Unit *guard = NULL;
                    guard = DoSpawnCreature(CR_GUARDIAN, 40, 0, 5, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 10000);
                    if(guard)
                    {
                        Unit *target = SelectUnit(SELECT_TARGET_RANDOM,0);
                        guard->AddThreat(target, 1.0f);
                        guard->GetMotionMaster()->MoveChase(m_creature);
                        Guardians[Guardian_Count] = guard->GetGUID();
                    }
                    Guardian_Count++;
                    Guardian_Timer = 8000;
                }
                else Guardian_Timer -= diff;
            }
        }

        

        if(phase!=1) DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_kelthuzadAI(Creature *_Creature)
{
    return new boss_kelthuzadAI (_Creature);
}

void AddSC_boss_kelthuzad()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_kelthuzad";
    newscript->GetAI = &GetAI_boss_kelthuzadAI;
    newscript->RegisterSelf();
}
