/* ScriptData
SDName: Boss_Prince_Keleseth
SD%Complete:
SDComment:
SDCategory: Utgarde Keep
EndScriptData */

#include "precompiled.h"
#include "def_utgarde_keep.h"

#define SPELL_SHADOW_BOLT_N                      43667
#define SPELL_SHADOW_BOLT_H                      59389
#define SPELL_FROST_TOMB                         48400
#define SPELL_DECREPIFY_N                        42702
#define SPELL_DECREPIFY_H                        59397
#define SPELL_SCOURGE_RESSURRECTION              42704
#define MOB_FROST_TOMB                           23965
#define MOB_VRYKUL_SKELETON                      23970

#define SAY_AGGRO                                -1574000
#define SAY_FROST_TOMB                           -1574001
#define SAY_SKELETONS                            -1574002
#define SAY_KILL                                 -1574003
#define SAY_DEATH                                -1574004

#define SKELETONSPAWN_Z                          42.87

float SkeletonSpawnPoint[5][2]=
{
    {162.85, 243.43},
    {165.72, 247.25},
    {167.56, 249.06},
    {169.60, 251.48},
    {173.72, 253.47},
};

uint64 FrostTombVictimGUID;

struct MANGOS_DLL_DECL mob_frost_tombAI : public ScriptedAI
{
    mob_frost_tombAI(Creature *c) : ScriptedAI(c)
    {
        Reset();
    }

    void Reset()
	{
        m_creature->ApplySpellImmune(0, IMMUNITY_SCHOOL, SPELL_SCHOOL_MASK_FROST, true);
	}

	void AttackStart(Unit* who) {}
    
	void MoveInLineOfSight(Unit* who) {}

    void JustDied(Unit* killer)  
    {
        if (FrostTombVictimGUID)
            if (Unit* FrostTombVictim = Unit::GetUnit((*m_creature),FrostTombVictimGUID))
                FrostTombVictim->RemoveAurasDueToSpell(SPELL_FROST_TOMB); 
    }

	void UpdateAI(const uint32 diff)
    {
        if (FrostTombVictimGUID)
            if (Unit* FrostTombVictim = Unit::GetUnit((*m_creature),FrostTombVictimGUID))
				if (!FrostTombVictim->HasAura(SPELL_FROST_TOMB))
					m_creature->DealDamage(m_creature, m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false); 
	}
};

struct MANGOS_DLL_DECL  boss_kelesethAI : public ScriptedAI
{
    boss_kelesethAI(Creature *c) : ScriptedAI(c)
    {
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
        Reset();
        HeroicMode = c->GetMap()->IsHeroic();
    }
    
    ScriptedInstance* pInstance;

    uint32 FrostTombTimer;
    uint32 SummonSkeletonsTimer;
    uint32 RespawnSkeletonsTimer;
    uint32 ShadowBoltTimer;
    bool Skeletons;
    bool HeroicMode;
    bool RespawnSkeletons;
    
    void Reset()
    {
        ShadowBoltTimer = 0;
        SummonSkeletonsTimer = 5000;
        FrostTombTimer = 18000;
        Skeletons = false;
		FrostTombVictimGUID = 0;
        if (pInstance)
            pInstance->SetData(EVENT_KELESETH, NOT_STARTED);
    }

    void KilledUnit(Unit *victim)
    {
        if(victim == m_creature)
            return;

        DoScriptText(SAY_KILL, m_creature);
    }

    void JustDied(Unit* killer)  
    {
        if (pInstance)
            pInstance->SetData(EVENT_KELESETH, DONE);

        DoScriptText(SAY_DEATH, m_creature);
    }

    void Aggro(Unit* who) 
    {
        if (pInstance)
            pInstance->SetData(EVENT_KELESETH, IN_PROGRESS);

        DoScriptText(SAY_AGGRO, m_creature);
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim() )
            return;

        if (ShadowBoltTimer < diff)
        {
			if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0))
                DoCast(target, HeroicMode ? SPELL_SHADOW_BOLT_H : SPELL_SHADOW_BOLT_N);
            ShadowBoltTimer = 10000;
        }else ShadowBoltTimer -= diff;

        if (!Skeletons)
            if ((SummonSkeletonsTimer < diff))
            {
                DoScriptText(SAY_SKELETONS, m_creature);
                for (uint8 i = 0; i < 5; ++i)
                {
                    Creature* Skeleton = m_creature->SummonCreature(MOB_VRYKUL_SKELETON, SkeletonSpawnPoint[i][0], SkeletonSpawnPoint[i][1] , SKELETONSPAWN_Z, 0, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1000);
					Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 0);
                    if (Skeleton && target)
						Skeleton->AI()->AttackStart(target);
                }
                Skeletons = true;
            }else SummonSkeletonsTimer -= diff;

		if (FrostTombTimer < diff)
        {
            Unit* target = SelectUnit(SELECT_TARGET_RANDOM, 1);
            if (target && target->GetTypeId() == TYPEID_PLAYER)
            {
                Creature* FrostTomb = m_creature->SummonCreature(MOB_FROST_TOMB, target->GetPositionX(), target->GetPositionY(), target->GetPositionZ(), 0, TEMPSUMMON_TIMED_OR_CORPSE_DESPAWN, 16000);
                if (FrostTomb)
                {
                    FrostTomb->CastSpell(target, SPELL_FROST_TOMB, true);
					FrostTombVictimGUID = target->GetGUID();
                    DoScriptText(SAY_FROST_TOMB, m_creature);
                }
            }
            FrostTombTimer = 17000;
        }else FrostTombTimer -= diff;

        DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL  mob_vrykul_skeletonAI : public ScriptedAI
{
    mob_vrykul_skeletonAI(Creature *c) : ScriptedAI(c)
	{
        pInstance = ((ScriptedInstance*)c->GetInstanceData());
		Reset();
        HeroicMode = c->GetMap()->IsHeroic();
	}

    ScriptedInstance* pInstance;

	uint32 Resurrect_Timer;
	uint32 Resurrection_Delay_Timer;
    uint32 Decrepify_Timer;

    bool HeroicMode;
	bool isDead;
	bool Resurrection_Delay;

    void Reset()
    {
        Decrepify_Timer = 10000 + rand()%20000;
        isDead = false;
		Resurrection_Delay = false;
    }

    void DamageTaken(Unit *done_by, uint32 &damage)
    {
		if (damage <= m_creature->GetHealth())
			return;
		if (pInstance && pInstance->GetData(EVENT_KELESETH) == IN_PROGRESS)
		{
			damage = 0;
			m_creature->InterruptNonMeleeSpells(false);
			m_creature->SetHealth(0);
			m_creature->StopMoving();
			m_creature->ClearComboPointHolders();
			m_creature->RemoveAllAurasOnDeath();
			m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_20_PERCENT, false);
			m_creature->ModifyAuraState(AURA_STATE_HEALTHLESS_35_PERCENT, false);
			m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
			m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
			m_creature->ClearAllReactives();
			m_creature->SetUInt64Value(UNIT_FIELD_TARGET, 0); 
			m_creature->GetMotionMaster()->Clear(); 
			m_creature->GetMotionMaster()->MoveIdle();
			m_creature->SetStandState(UNIT_STAND_STATE_DEAD);
			Resurrect_Timer = 12000;
			isDead = true;
		}
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostilTarget() || !m_creature->getVictim() )
            return;

		if (Resurrection_Delay)
		{
			if (Resurrection_Delay_Timer < diff)
			{
				isDead = false;
				Resurrection_Delay = false;
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
				m_creature->GetMotionMaster()->Clear();
				m_creature->GetMotionMaster()->MoveChase(m_creature->getVictim());
			}else Resurrection_Delay_Timer -= diff;
			return;
		}

		if (isDead)
		{
			if (Resurrect_Timer < diff)
			{
				m_creature->SetStandState(UNIT_STAND_STATE_STAND);
				DoCast(m_creature, SPELL_SCOURGE_RESSURRECTION, true);
				Resurrection_Delay = true;
				Resurrection_Delay_Timer = 3000;
			}else Resurrect_Timer -= diff;
			return;
		}

		if (Decrepify_Timer < diff)
		{
			DoCast(m_creature->getVictim(), HeroicMode ? SPELL_DECREPIFY_H : SPELL_DECREPIFY_N);
			Decrepify_Timer = 30000;
		}else Decrepify_Timer -= diff;

		DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_frost_tomb(Creature *_Creature)
{
    return new mob_frost_tombAI(_Creature);
}

CreatureAI* GetAI_boss_keleseth(Creature *_Creature)
{
    return new boss_kelesethAI (_Creature);
}

CreatureAI* GetAI_mob_vrykul_skeleton(Creature *_Creature)
{
    return new mob_vrykul_skeletonAI (_Creature);
}

void AddSC_boss_keleseth()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name="boss_keleseth";
    newscript->GetAI = &GetAI_boss_keleseth;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_frost_tomb";
    newscript->GetAI = &GetAI_mob_frost_tomb;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name="mob_vrykul_skeleton";
    newscript->GetAI = &GetAI_mob_vrykul_skeleton;
    newscript->RegisterSelf();
}
