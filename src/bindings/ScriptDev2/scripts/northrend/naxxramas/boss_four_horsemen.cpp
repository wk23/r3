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
SDName: Boss_Four_Horsemen
SD%Complete: 75
SDComment: Lady Blaumeux, Thane Korthazz, Sir Zeliek, Baron Rivendare
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

//all horsemen
#define SPELL_SHIELDWALL            29061
#define SPELL_BESERK                26662

//lady blaumeux
#define SAY_BLAU_AGGRO              -1533044
#define SAY_BLAU_TAUNT1             -1533045
#define SAY_BLAU_TAUNT2             -1533046
#define SAY_BLAU_TAUNT3             -1533047
#define SAY_BLAU_SPECIAL            -1533048
#define SAY_BLAU_SLAY               -1533049
#define SAY_BLAU_DEATH              -1533050

#define SPELL_MARK_OF_BLAUMEUX      28833
#define SPELL_UNYILDING_PAIN        57381
#define SPELL_VOIDZONE              28863
#define H_SPELL_VOIDZONE            57463
#define SPELL_SHADOW_BOLT           57374
#define H_SPELL_SHADOW_BOLT         57464

#define C_SPIRIT_OF_BLAUMEUX        16776

struct MANGOS_DLL_DECL boss_lady_blaumeuxAI : public Scripted_NoMovementAI
{
    boss_lady_blaumeuxAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    // X: 2460,656982 Y: -2953,094238 Z: 241,275406 Orient 0,591030
    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Mark_Timer;
    uint32 VoidZone_Timer;
    uint32 ShadowBolt_Timer;
    uint32 Condemnation_Timer;
    bool ShieldWall1;
    bool ShieldWall2;
    Player* target;
    Player* m_void_target;

    void Reset()
    {
        Mark_Timer = 24000;                                 // First Horsemen Mark is applied at 20 sec.
        VoidZone_Timer = 12000;                             // right
        ShadowBolt_Timer = 5000;
        Condemnation_Timer = 4000;
        ShieldWall1 = true;
        ShieldWall2 = true;
        target = NULL;
        m_void_target = NULL;

        if (m_pInstance)
        {
            m_pInstance->SetData(ENCOUNT_FOURHORSEMAN, NOT_STARTED);

            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_KORTHAZZ))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_RIVENDARE))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_ZELIEK))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();
        }
    }

    void Aggro(Unit *who)
    {
        DoScriptText(SAY_BLAU_AGGRO, m_creature);

        if (m_pInstance)
        {
            m_pInstance->SetData(ENCOUNT_FOURHORSEMAN, IN_PROGRESS);
            
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_KORTHAZZ))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_RIVENDARE))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_ZELIEK))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();

            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_KORTHAZZ))))
                if (pTemp->isAlive() && !pTemp->getVictim())
                    pTemp->AI()->AttackStart(who);
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_RIVENDARE))))
                if (pTemp->isAlive() && !pTemp->getVictim())
                    pTemp->AI()->AttackStart(who);
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_ZELIEK))))
                if (pTemp->isAlive() && !pTemp->getVictim())
                    pTemp->AI()->AttackStart(who);
        }
    }

    void KilledUnit(Unit* Victim)
    {
        DoScriptText(SAY_BLAU_SLAY, m_creature);
    }

    void EnterCombat(Unit *who)
    {
        m_creature->GetMap()->CreatureRelocation(m_creature, 2460.656f, -2953.094f, 241.275f, 0.591f);
    }

    Player* GetNearTarget()
    {
        Map::PlayerList const &PlList = m_pInstance->instance->GetPlayers();
        if (PlList.isEmpty())
            return NULL;
        for(Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
        {
            if (Player* pPlayer = i->getSource())
            {
                if (pPlayer->isAlive())
                {
                    if(pPlayer->GetDistance((m_creature)) <= 45.0f)
                        return pPlayer;
                }
            }
        }
        
        return NULL;
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_BLAU_DEATH, m_creature);

        if (m_pInstance)
        {
            bool HorsemenDead = true;
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_KORTHAZZ))))
                if (pTemp->isAlive())
                    HorsemenDead = false;
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_RIVENDARE))))
                if (pTemp->isAlive())
                    HorsemenDead = false;
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_ZELIEK))))
                if (pTemp->isAlive())
                    HorsemenDead = false;

            if (HorsemenDead)
            {
                m_pInstance->SetData(ENCOUNT_FOURHORSEMAN, DONE);
                GameObject* pGo = GetClosestGameObjectWithEntry(m_creature,!m_bIsRegularMode?193426:181366,200.0f);

                if (pGo)
                    m_pInstance->DoRespawnGameObject(pGo->GetGUID(),604800);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Mark of Blaumeux
        if (Mark_Timer < diff)
        {
            m_creature->InterruptNonMeleeSpells(false);
            DoCast(m_creature,SPELL_MARK_OF_BLAUMEUX,true);
            Mark_Timer = 12000;
        }else Mark_Timer -= diff;

        if (target && target->isAlive() && m_creature->GetDistance(target) <= 45.0f)
        {
            if (m_creature->GetUInt64Value(UNIT_FIELD_TARGET) != target->GetGUID())
                m_creature->SetUInt64Value(UNIT_FIELD_TARGET, target->GetGUID());
            if (ShadowBolt_Timer < diff)
            {
                DoCast(target, !m_bIsRegularMode ? H_SPELL_SHADOW_BOLT : SPELL_SHADOW_BOLT);
                ShadowBolt_Timer = 2100;
            }else ShadowBolt_Timer -= diff;
        }
        else
        {
            target = GetNearTarget();

            if (!target || (target && m_creature->GetDistance(target) <= 45.0f))
            {
                if (Condemnation_Timer <= diff)
                {
                    m_creature->InterruptNonMeleeSpells(false);
                    DoCast(m_creature,57381);
                    Condemnation_Timer = 2100;
                }else Condemnation_Timer -= diff;
            }
        }

        // Void Zone
        if (VoidZone_Timer < diff)
        {
            m_void_target  = GetNearTarget();
            if (m_void_target)
            {
                m_creature->InterruptNonMeleeSpells(false);
                DoCast(m_void_target, !m_bIsRegularMode ? H_SPELL_VOIDZONE : SPELL_VOIDZONE,true);
            }
            VoidZone_Timer = 15000;
        }else VoidZone_Timer -= diff;

        //DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_lady_blaumeux(Creature* pCreature)
{
    return new boss_lady_blaumeuxAI(pCreature);
}

//baron rivendare
#define SAY_RIVE_AGGRO1             -1533065
#define SAY_RIVE_AGGRO2             -1533066
#define SAY_RIVE_AGGRO3             -1533067
#define SAY_RIVE_SLAY1              -1533068
#define SAY_RIVE_SLAY2              -1533069
#define SAY_RIVE_SPECIAL            -1533070
#define SAY_RIVE_TAUNT1             -1533071
#define SAY_RIVE_TAUNT2             -1533072
#define SAY_RIVE_TAUNT3             -1533073
#define SAY_RIVE_DEATH              -1533074

#define SPELL_MARK_OF_RIVENDARE     28834
#define SPELL_UNHOLY_SHADOW         28882
#define H_SPELL_UNHOLY_SHADOW       57369

#define C_SPIRIT_OF_RIVENDARE       0                       //creature entry not known yet

struct MANGOS_DLL_DECL boss_rivendare_naxxAI : public ScriptedAI
{
    boss_rivendare_naxxAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Mark_Timer;
    uint32 UnholyShadow_Timer;
    bool ShieldWall1;
    bool ShieldWall2;

    void Reset()
    {
        Mark_Timer = 24000;
        UnholyShadow_Timer = 15000;
        ShieldWall1 = true;
        ShieldWall2 = true;

        if (m_pInstance)
        {
            m_pInstance->SetData(ENCOUNT_FOURHORSEMAN, NOT_STARTED);

            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_KORTHAZZ))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_BLAUMEUX))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_ZELIEK))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();
        }
    }

    void Aggro(Unit *who)
    {
        switch(rand()%3)
        {
        case 0: DoScriptText(SAY_RIVE_AGGRO1, m_creature); break;
        case 1: DoScriptText(SAY_RIVE_AGGRO2, m_creature); break;
        case 2: DoScriptText(SAY_RIVE_AGGRO3, m_creature); break;
        }

        if (m_pInstance)
        {
            m_pInstance->SetData(ENCOUNT_FOURHORSEMAN, IN_PROGRESS);

            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_KORTHAZZ))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_BLAUMEUX))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_ZELIEK))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();

            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_KORTHAZZ))))
                if (pTemp->isAlive() && !pTemp->getVictim())
                    pTemp->AI()->AttackStart(who);
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_BLAUMEUX))))
                if (pTemp->isAlive() && !pTemp->getVictim())
                    pTemp->AI()->AttackStart(who);
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_ZELIEK))))
                if (pTemp->isAlive() && !pTemp->getVictim())
                    pTemp->AI()->AttackStart(who);
        }
    }

    void KilledUnit(Unit* Victim)
    {
        switch(rand()%2)
        {
        case 0: DoScriptText(SAY_RIVE_SLAY1, m_creature); break;
        case 1: DoScriptText(SAY_RIVE_SLAY2, m_creature); break;
        }
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_RIVE_DEATH, m_creature);

        if (m_pInstance)
        {
            bool HorsemenDead = true;
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_KORTHAZZ))))
                if (pTemp->isAlive())
                    HorsemenDead = false;
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_BLAUMEUX))))
                if (pTemp->isAlive())
                    HorsemenDead = false;
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_ZELIEK))))
                if (pTemp->isAlive())
                    HorsemenDead = false;

            if (HorsemenDead)
            {
                m_pInstance->SetData(ENCOUNT_FOURHORSEMAN, DONE);
                GameObject* pGo = GetClosestGameObjectWithEntry(m_creature,!m_bIsRegularMode?193426:181366,200.0f);
                
                if (pGo)
                    m_pInstance->DoRespawnGameObject(pGo->GetGUID(),604800);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Mark of Blaumeux
        if (Mark_Timer < diff)
        {
            m_creature->InterruptNonMeleeSpells(false);
            DoCast(m_creature->getVictim(),SPELL_MARK_OF_RIVENDARE,true);
            Mark_Timer = 15000;
        }else Mark_Timer -= diff;

        if (UnholyShadow_Timer < diff)
        {
            m_creature->InterruptNonMeleeSpells(false);
            DoCast(m_creature->getVictim(), !m_bIsRegularMode ? H_SPELL_UNHOLY_SHADOW : SPELL_UNHOLY_SHADOW);
            UnholyShadow_Timer = 15000;
        }else UnholyShadow_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_rivendare_naxx(Creature* pCreature)
{
    return new boss_rivendare_naxxAI(pCreature);
}

//thane korthazz
#define SAY_KORT_AGGRO              -1533051
#define SAY_KORT_TAUNT1             -1533052
#define SAY_KORT_TAUNT2             -1533053
#define SAY_KORT_TAUNT3             -1533054
#define SAY_KORT_SPECIAL            -1533055
#define SAY_KORT_SLAY               -1533056
#define SAY_KORT_DEATH              -1533057

#define SPELL_MARK_OF_KORTHAZZ      28832
#define SPELL_METEOR                26558                   // m_creature->getVictim() auto-area spell but with a core problem

#define C_SPIRIT_OF_KORTHAZZ        16778

struct MANGOS_DLL_DECL boss_thane_korthazzAI : public ScriptedAI
{
    boss_thane_korthazzAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Mark_Timer;
    uint32 Meteor_Timer;
    bool ShieldWall1;
    bool ShieldWall2;

    void Reset()
    {
        Mark_Timer = 24000;                                 // First Horsemen Mark is applied at 20 sec.
        Meteor_Timer = 30000;                               // wrong
        ShieldWall1 = true;
        ShieldWall2 = true;

        if (m_pInstance)
        {
            m_pInstance->SetData(ENCOUNT_FOURHORSEMAN, NOT_STARTED);

            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_RIVENDARE))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_BLAUMEUX))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_ZELIEK))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();
        }
    }

    void Aggro(Unit *who)
    {
        DoScriptText(SAY_KORT_AGGRO, m_creature);

        if (m_pInstance)
        {
            m_pInstance->SetData(ENCOUNT_FOURHORSEMAN, IN_PROGRESS);

            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_RIVENDARE))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_BLAUMEUX))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_ZELIEK))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();

            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_RIVENDARE))))
                if (pTemp->isAlive() && !pTemp->getVictim())
                    pTemp->AI()->AttackStart(who);
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_BLAUMEUX))))
                if (pTemp->isAlive() && !pTemp->getVictim())
                    pTemp->AI()->AttackStart(who);
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_ZELIEK))))
                if (pTemp->isAlive() && !pTemp->getVictim())
                    pTemp->AI()->AttackStart(who);
        }
    }

    void KilledUnit(Unit* Victim)
    {
        DoScriptText(SAY_KORT_SLAY, m_creature);

        if (m_pInstance)
        {
            bool HorsemenDead = true;
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_RIVENDARE))))
                if (pTemp->isAlive())
                    HorsemenDead = false;
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_BLAUMEUX))))
                if (pTemp->isAlive())
                    HorsemenDead = false;
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_ZELIEK))))
                if (pTemp->isAlive())
                    HorsemenDead = false;

            if (HorsemenDead)
                m_pInstance->SetData(ENCOUNT_FOURHORSEMAN, DONE);
        }
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_KORT_DEATH, m_creature);

        if (m_pInstance)
        {
            bool HorsemenDead = true;
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_ZELIEK))))
                if (pTemp->isAlive())
                    HorsemenDead = false;
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_RIVENDARE))))
                if (pTemp->isAlive())
                    HorsemenDead = false;
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_BLAUMEUX))))
                if (pTemp->isAlive())
                    HorsemenDead = false;

            if (HorsemenDead)
            {
                m_pInstance->SetData(ENCOUNT_FOURHORSEMAN, DONE);

                GameObject* pGo = GetClosestGameObjectWithEntry(m_creature,!m_bIsRegularMode?193426:181366,200.0f);

                if (pGo)
                    m_pInstance->DoRespawnGameObject(pGo->GetGUID(),604800);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Mark of Korthazz
        if (Mark_Timer < diff)
        {
            m_creature->InterruptNonMeleeSpells(false);
            DoCast(m_creature->getVictim(),SPELL_MARK_OF_KORTHAZZ,true);
            Mark_Timer = 12000;
        }else Mark_Timer -= diff;

        // Meteor
        if (Meteor_Timer < diff)
        {
            DoCast(m_creature->getVictim(),SPELL_METEOR);
            Meteor_Timer = 15000 + rand()%2000;
        }else Meteor_Timer -= diff;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_thane_korthazz(Creature* pCreature)
{
    return new boss_thane_korthazzAI(pCreature);
}

//sir zeliek
#define SAY_ZELI_AGGRO              -1533058
#define SAY_ZELI_TAUNT1             -1533059
#define SAY_ZELI_TAUNT2             -1533060
#define SAY_ZELI_TAUNT3             -1533061
#define SAY_ZELI_SPECIAL            -1533062
#define SAY_ZELI_SLAY               -1533063
#define SAY_ZELI_DEATH              -1533064

#define SPELL_MARK_OF_ZELIEK        28835
#define SPELL_HOLY_WRATH            28883
#define H_SPELL_HOLY_WRATH          57466
#define SPELL_HOLY_BOLT             57376
#define H_SPELL_HOLY_BOLT           57465

#define C_SPIRIT_OF_ZELIREK         16777

struct MANGOS_DLL_DECL boss_sir_zeliekAI : public Scripted_NoMovementAI
{
    boss_sir_zeliekAI(Creature* pCreature) : Scripted_NoMovementAI(pCreature)
    {
        m_pInstance = (ScriptedInstance*)pCreature->GetInstanceData();
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();
    }

    // X: 2520,967285 Y: -2890,136475 Z: 241,275116 Orient: 4,082907
    ScriptedInstance* m_pInstance;
    bool m_bIsRegularMode;

    uint32 Mark_Timer;
    uint32 HolyWrath_Timer;
    uint32 HolyBolt_Timer;
    uint32 Condemnation_Timer;
    Player* target;
    Player* m_chain_target;
    bool ShieldWall1;
    bool ShieldWall2;

    void Reset()
    {
        Mark_Timer = 24000;                                 // First Horsemen Mark is applied at 24 sec.
        HolyWrath_Timer = 12000;                            // right
        HolyBolt_Timer = 5000;
        Condemnation_Timer = 4000;
        ShieldWall1 = true;
        ShieldWall2 = true;
        target = NULL;
        m_chain_target = NULL;

        if (m_pInstance)
        {
            m_pInstance->SetData(ENCOUNT_FOURHORSEMAN, NOT_STARTED);

            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_KORTHAZZ))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_RIVENDARE))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_BLAUMEUX))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();
        }
    }

    void Aggro(Unit *who)
    {
        DoScriptText(SAY_ZELI_AGGRO, m_creature);

        if (m_pInstance)
        {
            m_pInstance->SetData(ENCOUNT_FOURHORSEMAN, IN_PROGRESS);

            // stupid check. need to avoid bug with one-pull boss.
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_KORTHAZZ))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_RIVENDARE))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_BLAUMEUX))))
                if (!pTemp->isAlive())
                    pTemp->Respawn();

            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_KORTHAZZ))))
                if (pTemp->isAlive() && !pTemp->getVictim())
                    pTemp->AI()->AttackStart(who);
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_RIVENDARE))))
                if (pTemp->isAlive() && !pTemp->getVictim())
                    pTemp->AI()->AttackStart(who);
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_BLAUMEUX))))
                if (pTemp->isAlive() && !pTemp->getVictim())
                    pTemp->AI()->AttackStart(who);
        }
    }

    Player* GetNearTarget()
    {
        Map::PlayerList const &PlList = m_pInstance->instance->GetPlayers();
        if (PlList.isEmpty())
            return NULL;
        for(Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
        {
            if (Player* pPlayer = i->getSource())
            {
                if (pPlayer->isAlive())
                {
                    if(pPlayer->GetDistance((m_creature)) <= 45.0f)
                        return pPlayer;
                }
            }
        }

        return NULL;
    }

    void EnterCombat(Unit *who)
    {
        m_creature->GetMap()->CreatureRelocation(m_creature, 2520.967f, -2890.136f, 241.275f, 4.082f);
    }

    void KilledUnit(Unit* Victim)
    {
        DoScriptText(SAY_ZELI_SLAY, m_creature);
    }

    void JustDied(Unit* Killer)
    {
        DoScriptText(SAY_ZELI_DEATH, m_creature);

        if (m_pInstance)
        {
            bool HorsemenDead = true;
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_KORTHAZZ))))
                if (pTemp->isAlive())
                    HorsemenDead = false;
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_RIVENDARE))))
                if (pTemp->isAlive())
                    HorsemenDead = false;
            if (Creature* pTemp = ((Creature*)Unit::GetUnit((*m_creature), m_pInstance->GetData64(GUID_BLAUMEUX))))
                if (pTemp->isAlive())
                    HorsemenDead = false;

            if (HorsemenDead)
            {
                m_pInstance->SetData(ENCOUNT_FOURHORSEMAN, DONE);

                GameObject* pGo = GetClosestGameObjectWithEntry(m_creature,!m_bIsRegularMode?193426:181366,200.0f);

                if (pGo)
                    m_pInstance->DoRespawnGameObject(pGo->GetGUID(),604800);
            }
        }
    }

    void UpdateAI(const uint32 diff)
    {
        //Return since we have no target
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Mark of Zeliek
        if (Mark_Timer < diff)
        {
            m_creature->InterruptNonMeleeSpells(false);
            DoCast(m_creature,SPELL_MARK_OF_ZELIEK,true);
            Mark_Timer = 12000;
        }else Mark_Timer -= diff;

        if (target && target->isAlive() && m_creature->GetDistance(target) <= 45.0f)
        {
            if (m_creature->GetUInt64Value(UNIT_FIELD_TARGET) != target->GetGUID())
                m_creature->SetUInt64Value(UNIT_FIELD_TARGET, target->GetGUID());
            if (HolyBolt_Timer < diff)
            {
                DoCast(target, !m_bIsRegularMode ? H_SPELL_HOLY_BOLT : SPELL_HOLY_BOLT);
                HolyBolt_Timer = 2100;
            }else HolyBolt_Timer -= diff;
        }
        else
        {
            target = GetNearTarget();

            if (!target || (target && m_creature->GetDistance(target) <= 45.0f))
            {
                if (Condemnation_Timer <= diff)
                {
                    m_creature->InterruptNonMeleeSpells(false);
                    DoCast(m_creature,57377);
                    Condemnation_Timer = 2100;
                }else Condemnation_Timer -= diff;
            }
        }

        // Holy Wrath
        if (HolyWrath_Timer < diff)
        {
            m_chain_target = GetNearTarget();
            if (m_chain_target)
            {
                m_creature->InterruptNonMeleeSpells(false);
                DoCast(m_chain_target, !m_bIsRegularMode ? H_SPELL_HOLY_WRATH : SPELL_HOLY_WRATH);
            }
            HolyWrath_Timer = 12000;
        }else HolyWrath_Timer -= diff;

        //DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_sir_zeliek(Creature* pCreature)
{
    return new boss_sir_zeliekAI(pCreature);
}

struct MANGOS_DLL_DECL mob_void_zoneAI : public Scripted_NoMovementAI
{
    mob_void_zoneAI(Creature *c) : Scripted_NoMovementAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
        m_bIsRegularMode = c->GetMap()->IsRegularDifficulty();
        Reset();
    }

    ScriptedInstance* pInstance;
    bool m_bIsRegularMode;
    uint32 m_despawn_timer;

    void Reset()
    {
        m_despawn_timer = 75000;
    }

    void UpdateAI(const uint32 diff) 
    {
        if (!(m_creature->HasAura(!m_bIsRegularMode?39003:36120)))
            DoCast(m_creature,!m_bIsRegularMode?39003:36120,true);

        if (m_despawn_timer <= diff)
        {
            m_creature->ForcedDespawn();
        }else m_despawn_timer -= diff;
    }
};

CreatureAI* GetAI_mob_void_zone(Creature *_Creature)
{
    return new mob_void_zoneAI (_Creature);
}

void AddSC_boss_four_horsemen()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_lady_blaumeux";
    newscript->GetAI = &GetAI_boss_lady_blaumeux;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_rivendare_naxx";
    newscript->GetAI = &GetAI_boss_rivendare_naxx;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_thane_korthazz";
    newscript->GetAI = &GetAI_boss_thane_korthazz;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "boss_sir_zeliek";
    newscript->GetAI = &GetAI_boss_sir_zeliek;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_void_zone";
    newscript->GetAI = &GetAI_mob_void_zone;
    newscript->RegisterSelf();
}