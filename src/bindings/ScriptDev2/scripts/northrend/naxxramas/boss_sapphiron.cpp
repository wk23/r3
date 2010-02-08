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
SDName: Boss_Sapphiron
SD%Complete: 0
SDComment: Place Holder
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

#define EMOTE_BREATH            -1533082
#define EMOTE_ENRAGE            -1533083

#define SPELL_FROST_AURA        28531
#define SPELL_FROST_AURA_H      55799
#define SPELL_CLEAVE            19983
#define SPELL_TAIL_SWEEP        55697
#define SPELL_TAIL_SWEEP_H      55696
#define SPELL_SUMMON_BLIZZARD   28560
#define SPELL_LIFE_DRAIN        28542
#define SPELL_LIFE_DRAIN_H      55665
#define SPELL_ICEBOLT           28522
#define SPELL_FROST_BREATH      29318
#define SPELL_FROST_EXPLOSION   28524
#define SPELL_FROST_MISSILE     30101
#define SPELL_BERSERK           26662
#define SPELL_DIES              29357

#define SPELL_CHILL             28547
#define SPELL_CHILL_H           55699

#define MOB_BLIZZARD            16474
#define GO_ICEBLOCK             181247

#define ACHIEVEMENT_THE_HUNDRED_CLUB    2146
#define ACHIEVEMENT_THE_HUNDRED_CLUB_H  2147
#define MAX_FROST_RESISTANCE            100
typedef std::map<uint64, uint64> IceBlockMap;

struct MANGOS_DLL_DECL boss_sapphironAI : public ScriptedAI
{
    boss_sapphironAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
		m_pInstance = ((ScriptedInstance*)pCreature->GetInstanceData());
		m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
		Reset();
    }

	ScriptedInstance* m_pInstance;
	IceBlockMap iceblocks;
	bool m_bIsRegularMode;
    uint32 Icebolt_Count;
    uint32 Icebolt_Timer;
    uint32 FrostBreath_Timer;
    uint32 FrostBreath_Explo;
    uint32 FrostAura_Timer;
    uint32 LifeDrain_Timer;
    uint32 Blizzard_Timer;
	uint32 Cleave_Timer;
	uint32 Tail_Timer;
    uint32 Fly_Timer;
    uint32 Beserk_Timer;
    uint32 phase;
    bool landoff;
    uint32 land_Timer;
	bool CanTheHundredClub;
	uint32 CheckFrostResistTimer;

    void Reset()
    {
        FrostAura_Timer = 2000;
        FrostBreath_Timer = 2500;
        FrostBreath_Explo = 8000;
        LifeDrain_Timer = 24000;
        Blizzard_Timer = 20000;
		Cleave_Timer = 5000+rand()%10000;
		Tail_Timer = 5000+rand()%10000;
        Fly_Timer = 45000;
        Icebolt_Timer = 5000;
        land_Timer = 4000;
        Beserk_Timer = 900000;
        phase = 1;
        Icebolt_Count = 0;
        landoff = false;
		CanTheHundredClub = true;
		CheckFrostResistTimer = 5000;
        ClearIceBlock();

        if (m_pInstance)
            m_pInstance->SetData(ENCOUNT_SAPPHIRON, NOT_STARTED);
    }

	void CheckPlayersFrostResist()
	{
		if(CanTheHundredClub && m_pInstance)
		{
			Map::PlayerList const &players = m_pInstance->instance->GetPlayers();
			for (Map::PlayerList::const_iterator itr = players.begin(); itr != players.end(); ++itr)
			{
				if(itr->getSource()->GetResistance(SPELL_SCHOOL_FROST) > MAX_FROST_RESISTANCE)
				{
					CanTheHundredClub = false;
					break;
				}
			}
		}
	}

	void CastExplosion()
	{
		m_creature->SetInCombatWithZone(); // make sure everyone is in threatlist
		std::vector<Unit*> targets;
		ThreatList::const_iterator i = m_creature->getThreatManager().getThreatList().begin();
		for (; i != m_creature->getThreatManager().getThreatList().end(); ++i)
		{
			Unit *pTarget = (*i)->getTarget();
			if (pTarget->GetTypeId() != TYPEID_PLAYER)
				continue;

			if (pTarget->HasAura(SPELL_ICEBOLT))
			{
				pTarget->ApplySpellImmune(0, IMMUNITY_ID, SPELL_FROST_EXPLOSION, true);
				targets.push_back(pTarget);
				continue;
			}

			for (IceBlockMap::iterator itr = iceblocks.begin(); itr != iceblocks.end(); ++itr)
			{
				if (GameObject* pGo = GameObject::GetGameObject(*m_creature, itr->second))
				{
					if (pGo->IsInBetween(m_creature, pTarget, 3.0f)
						&& m_creature->GetExactDist2d(pTarget->GetPositionX(), pTarget->GetPositionY()) - m_creature->GetExactDist2d(pGo->GetPositionX(), pGo->GetPositionY()) < 5.0f)
					{
						pTarget->ApplySpellImmune(0, IMMUNITY_ID, SPELL_FROST_EXPLOSION, true);
						targets.push_back(pTarget);
						break;
					}
				}
			}
		}

		m_creature->CastSpell(m_creature, SPELL_FROST_EXPLOSION, true);

		for (std::vector<Unit*>::iterator itr = targets.begin(); itr != targets.end(); ++itr)
			(*itr)->ApplySpellImmune(0, IMMUNITY_ID, SPELL_FROST_EXPLOSION, false);
	}
	
    void ClearIceBlock()
    {
        for (IceBlockMap::iterator itr = iceblocks.begin(); itr != iceblocks.end(); ++itr)
        {
            if (Unit* pPlayer = Unit::GetUnit(*m_creature,itr->first))
                pPlayer->RemoveAurasDueToSpell(SPELL_ICEBOLT);
            if (GameObject* pGo = GameObject::GetGameObject(*m_creature, itr->second))
                pGo->Delete();
        }
        iceblocks.clear();
    }

	void SpellHitTarget(Unit *pTarget, const SpellEntry *spell)
	{
		if (spell->Id == SPELL_ICEBOLT)
		{
			IceBlockMap::iterator itr = iceblocks.find(pTarget->GetGUID());
			if (itr != iceblocks.end() && !itr->second)
			{
				if (GameObject *iceblock = m_creature->SummonGameObject(GO_ICEBLOCK, pTarget->GetPositionX(), pTarget->GetPositionY(), pTarget->GetPositionZ(), 0, 0, 0, 0, 0, 25000))
					itr->second = iceblock->GetGUID();
			}
		}
	}

    void MovementInform(uint32, uint32 id)
    {
        if (id == 1)
        {
            m_creature->HandleEmoteCommand(EMOTE_ONESHOT_LIFTOFF);
            m_creature->AddMonsterMoveFlag(MONSTER_MOVE_LEVITATING);
            m_creature->SendMovementFlagUpdate();
            m_creature->GetMotionMaster()->Clear(false);
            m_creature->GetMotionMaster()->MoveIdle();
        }
            
    }

    void Aggro(Unit* who)
    {
        if (m_pInstance)
            m_pInstance->SetData(ENCOUNT_SAPPHIRON, IN_PROGRESS);
    }

    void JustDied(Unit* Killer)
    {
        if (m_pInstance)
            m_pInstance->SetData(ENCOUNT_SAPPHIRON, DONE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

		if (!m_pInstance)
			return;

		if (m_bIsRegularMode && !m_creature->HasAura(SPELL_FROST_AURA))
			DoCast(m_creature,SPELL_FROST_AURA,true);

		if (!m_bIsRegularMode && !m_creature->HasAura(SPELL_FROST_AURA_H))
			DoCast(m_creature,SPELL_FROST_AURA_H,true);

        if(CanTheHundredClub)
        {
            if (CheckFrostResistTimer <= diff)
            {
                CheckPlayersFrostResist();
                CheckFrostResistTimer = (rand() % 5 + 5) * 1000;
            } else CheckFrostResistTimer -= diff;
        }

        if (phase == 1)
        {
            if (LifeDrain_Timer <= diff)
            {
                if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
					DoCast(target,!m_bIsRegularMode ? SPELL_LIFE_DRAIN_H : SPELL_LIFE_DRAIN);
                LifeDrain_Timer = 24000;
            }else LifeDrain_Timer -= diff;

            if (Blizzard_Timer <= diff)
            {
                if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,0))
                    m_creature->SummonCreature(MOB_BLIZZARD,target->GetPositionX(),target->GetPositionY(),target->GetPositionZ(),0,TEMPSUMMON_CORPSE_DESPAWN,600000);
                Blizzard_Timer = 20000;
            }else Blizzard_Timer -= diff;

			if (Cleave_Timer <= diff)
            {
                DoCast(m_creature->getVictim(),SPELL_CLEAVE);
                Cleave_Timer = 5000+rand()%10000;
            }else Cleave_Timer -= diff;

            if (Tail_Timer <= diff)
            {
                DoCast(m_creature->getVictim(),!m_bIsRegularMode?SPELL_TAIL_SWEEP_H:SPELL_TAIL_SWEEP);
                Tail_Timer = 5000+rand()%10000;
            }else Tail_Timer -= diff;

            if (m_creature->GetHealth()*100 / m_creature->GetMaxHealth() > 10)
            {
                if (Fly_Timer <= diff)
                {
                    phase = 2;
                    float x, y, z, o;
                    m_creature->GetRespawnCoord(x, y, z, &o);
                    m_creature->GetMotionMaster()->MovePoint(1, x, y, z);
                    m_creature->InterruptNonMeleeSpells(false);
                    Icebolt_Timer = 4000;
                    Icebolt_Count = 0;
                    landoff = false;
                }else Fly_Timer -= diff;
            }
        }

        if (phase == 2)
        {
            if (Icebolt_Timer < diff && Icebolt_Count < (!m_bIsRegularMode?3:2))
            {
                std::vector<Unit*> targets;
                ThreatList::const_iterator i = m_creature->getThreatManager().getThreatList().begin();
                for (; i != m_creature->getThreatManager().getThreatList().end(); ++i)
                    if ((*i)->getTarget()->GetTypeId() == TYPEID_PLAYER && !(*i)->getTarget()->HasAura(SPELL_ICEBOLT))
                        targets.push_back((*i)->getTarget());

                if (targets.empty())
                    Icebolt_Count = (!m_bIsRegularMode?3:2);
                else
                {
                    std::vector<Unit*>::iterator itr = targets.begin();
                    advance(itr, rand()%targets.size());
                    iceblocks.insert(std::make_pair((*itr)->GetGUID(), 0));
                    DoCast(*itr, SPELL_ICEBOLT);
                    m_creature->AddMonsterMoveFlag(MONSTER_MOVE_LEVITATING);
                    m_creature->SendMovementFlagUpdate();
                    ++Icebolt_Count;
                }

                Icebolt_Timer = 4000;
            }else Icebolt_Timer -= diff;

            if (Icebolt_Count == (!m_bIsRegularMode?3:2) && !landoff)
            {
                if (FrostBreath_Timer <= diff)
                {
                    DoScriptText(EMOTE_BREATH, m_creature);
                    DoCast(m_creature,SPELL_FROST_MISSILE);
                    m_creature->AddMonsterMoveFlag(MONSTER_MOVE_LEVITATING);
                    m_creature->SendMovementFlagUpdate();
                    land_Timer = 5000;
                    FrostBreath_Timer = 6000;
                }else FrostBreath_Timer -= diff;

                if (FrostBreath_Explo <= diff)
                {
                    CastExplosion();
                    ClearIceBlock();
                    FrostBreath_Explo = 8000;
                    landoff = true;
                }else FrostBreath_Explo -= diff;
            }

            if (landoff)
            {
                if (land_Timer <= diff)
                {
                    phase = 1;
                    m_creature->HandleEmoteCommand(EMOTE_ONESHOT_LAND);
                    m_creature->RemoveMonsterMoveFlag(MONSTER_MOVE_LEVITATING);
                    m_creature->SendMovementFlagUpdate();
                    m_creature->GetMotionMaster()->Clear(false);
                    m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
                    Fly_Timer = 67000;
                }else land_Timer -= diff;
            }
        }

        if (Beserk_Timer <= diff)
        {
            DoScriptText(EMOTE_ENRAGE, m_creature);
            DoCast(m_creature,SPELL_BERSERK,true);
            Beserk_Timer = 300000;
        }else Beserk_Timer -= diff;

        if (phase!=2)
            DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_boss_sapphiron(Creature* pCreature)
{
    return new boss_sapphironAI(pCreature);
}

/*######
## Mob Blizzard
######*/

struct MANGOS_DLL_DECL mob_BlizzardAI : public ScriptedAI
{
    mob_BlizzardAI(Creature* pCreature) : ScriptedAI(pCreature) 
    {
        m_bIsRegularMode = pCreature->GetMap()->IsRegularDifficulty();
        Reset();         
    }

    uint32 m_uiDespawn;
    uint32 m_uiBlizzTimer;
    bool m_bIsRegularMode;

    void Aggro(Unit* who)
    {
        DoStopAttack();
        SetCombatMovement(false);
    }

    void Reset()
    {
        m_uiDespawn = 21000;
        m_uiBlizzTimer = 1000;
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiBlizzTimer <= uiDiff)
        {
            DoCast(m_creature,!m_bIsRegularMode ? 55699 : 28547);
            m_uiBlizzTimer = 10000;
        }else m_uiBlizzTimer -= uiDiff;

        if (m_uiDespawn <= uiDiff)
            m_creature->ForcedDespawn();
        else m_uiDespawn -= uiDiff;
    }
};

CreatureAI* GetAI_mob_Blizzard(Creature* pCreature)
{
    return new mob_BlizzardAI(pCreature);
}

void AddSC_boss_sapphiron()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "boss_sapphiron";
    newscript->GetAI = &GetAI_boss_sapphiron;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_Blizzard";
    newscript->GetAI = &GetAI_mob_Blizzard;
    newscript->RegisterSelf();
}