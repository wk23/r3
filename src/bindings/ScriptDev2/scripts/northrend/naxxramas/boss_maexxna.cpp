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
SDName: Boss_Maexxna
SD%Complete: 80
SDComment: this needs review, and rewrite of the webwrap ability
SDCategory: Naxxramas
EndScriptData */

#include "precompiled.h"
#include "naxxramas.h"

// Spells and Mobs used in Maexxna encounter
enum
{
    SPELL_WEBWRAP1        = 28673,                       // Strange spells that may knockback target to the wall
    SPELL_WEBWRAP2        = 54127,                       //      and trigger SPELL_WEBWRAP_SELF
    SPELL_WEBWRAP_SELF    = 28622,                       // Spell is normally used by the webwrap on the wall NOT by Maexxna

    SPELL_WEBSPRAY        = 29484,
    SPELL_WEBSPRAY_H      = 54125,
    SPELL_POISONSHOCK     = 28741,
    SPELL_POISONSHOCK_H   = 54122,
    SPELL_NECROTICPOISON  = 28776,
    SPELL_FRENZY          = 54123,
    SPELL_FRENZY_H        = 54124,

    MOB_WEBWRAP           = 16486,
    MOB_SPIDERLING        = 17055
};

// Positions for Web Wrap cocoons (near the wall)
static const uint32 MAX_PLAYERS_WEB_WRAP = 3;

WorldLocation WWlocs[MAX_PLAYERS_WEB_WRAP] = 
{
    WorldLocation(533,3502.164,-3832.138,305.178,1.570),
    WorldLocation(533,3544.651,-3850.027,299.068,2.356),
    WorldLocation(533,3561.365,-3884.543,297.819,3.141)
};

// Cocoon AI. It frees player from SPELL_WEBWRAP_SELF when cocoon dies
struct MANGOS_DLL_DECL mob_webwrapAI : public ScriptedAI
{
    mob_webwrapAI(Creature *c) : ScriptedAI(c) {victimGUID = 0; Reset();}
    uint64 victimGUID;
    void Reset()
    {
        victimGUID = 0;
    }

    void Aggro(Unit *who)
    {
        return;
    }

    void SetVictim(Unit* victim)
    {
        if (victim)
        {
			victimGUID = victim->GetGUID();
            m_creature->AddThreat(victim, 1.0f);
            m_creature->GetMotionMaster()->MovePoint(0,victim->GetPositionX(),victim->GetPositionY(),victim->GetPositionZ());
            victim->SetDisplayId(0);
        }
    }

    void JustDied(Unit* Killer)
    {
        Unit* victim = Unit::GetUnit((*m_creature), victimGUID);
        if (victim)
        {
            victim->RemoveAurasDueToSpell(SPELL_WEBWRAP_SELF);
            victim->DeMorph();
            victimGUID = 0;
        }
    }

    void UpdateAI(const uint32 diff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (m_creature->isAlive())
                m_creature->DealDamage(m_creature,m_creature->GetHealth(), NULL, DIRECT_DAMAGE, SPELL_SCHOOL_MASK_NORMAL, NULL, false);
            return;
        }
        if (m_creature->getVictim()->HasAura(SPELL_WEBSPRAY) || m_creature->getVictim()->HasAura(SPELL_WEBSPRAY_H))
        {
            m_creature->getVictim()->RemoveAurasDueToSpell(SPELL_WEBSPRAY);
            m_creature->getVictim()->RemoveAurasDueToSpell(SPELL_WEBSPRAY_H);
            m_creature->getVictim()->CastSpell(m_creature->getVictim(), SPELL_WEBWRAP_SELF, true);
        }
        m_creature->GetMotionMaster()->MovePoint(0,m_creature->getVictim()->GetPositionX(),m_creature->getVictim()->GetPositionY(),m_creature->getVictim()->GetPositionZ());
        m_creature->AddThreat(m_creature->getVictim(), 1.0f);
    }
};

static const int MAX_SPIDERLINGS = 16;
struct MANGOS_DLL_DECL boss_maexxnaAI : public ScriptedAI
{
    boss_maexxnaAI(Creature *c) : ScriptedAI(c)
	{
        m_bIsRegularMode = c->GetMap()->IsRegularDifficulty();
		pInstance = ((ScriptedInstance*)c->GetInstanceData());
        for (int i = 0; i < MAX_PLAYERS_WEB_WRAP; i++)
            WWplayers[i] = 0;
        for (int i = 0; i < MAX_SPIDERLINGS; i++)
            guidSpiderlings[i] = 0;
		Reset();
	}

	ScriptedInstance *pInstance;
    bool m_bIsRegularMode;

    bool m_ach_10ppl;
    bool m_ach_25ppl;
    bool m_ach_arahna_10;
    bool m_ach_arahna_25;
    uint32 m_count_ppl;
    uint32 Ach_Timer;
    time_t m_arahna_timer;

    uint32 WebWrap_Timer;
    uint32 WebSpray_Timer;
    uint32 PoisonShock_Timer;
    uint32 NecroticPoison_Timer;
    uint32 SummonSpiderling_Timer;
    bool Enraged;

    uint32 WWplayers[MAX_PLAYERS_WEB_WRAP];
    uint32 WWplayersFlyTimer[MAX_PLAYERS_WEB_WRAP];
    uint32 WWplayersFlyTimer2[MAX_PLAYERS_WEB_WRAP];

    uint64 guidSpiderlings[MAX_SPIDERLINGS];
    uint32 Spiderlings_count;

    void Reset()
    {
        m_ach_10ppl = true;
        m_ach_25ppl = true;
        m_ach_arahna_10 = false;
        m_ach_arahna_25 = false;
        m_count_ppl = 0;
        Ach_Timer = 10000;

        WebWrap_Timer = 20000;                              //20 sec init, 40 sec normal
        WebSpray_Timer = 40000;                             //40 seconds
        PoisonShock_Timer = 20000;                          //20 seconds
        NecroticPoison_Timer = 30000;                       //30 seconds
        SummonSpiderling_Timer = 30000;                     //30 sec init, 40 sec normal
        Enraged = false;

        // Remove all Spiderlings
        Spiderlings_count = 0;
        /*for (int i = 0; i < MAX_SPIDERLINGS; i++)
        {
            if (Unit* pUnit = Unit::GetUnit((*m_creature), guidSpiderlings[i]))
                pUnit->AddObjectToRemoveList();
            guidSpiderlings[i] = 0;
        }*/
        
        for (int i = 0; i < MAX_PLAYERS_WEB_WRAP; i++)
            WWplayers[i] = 0;

		if(pInstance)
			m_creature->isDead() ? (pInstance->SetData(ENCOUNT_MAEXXNA, DONE)):(pInstance->SetData(ENCOUNT_MAEXXNA, NOT_STARTED));
    }

    void Aggro(Unit* who)
    {
        //Close the room for boss fight
        if(pInstance)
            pInstance->SetData(ENCOUNT_MAEXXNA, IN_PROGRESS);

        CheckAch();
    }

    void JustDied(Unit* Killer)
    {
        if (!pInstance)
            return;
		//Faerlina is slayed -> open all doors to Maexxna
        pInstance->SetData(ENCOUNT_MAEXXNA, DONE);

        /*if ((time(NULL) - m_arahna_timer)<1200)
        {
            m_ach_arahna_10 = true;
            m_ach_arahna_25 = true;
        }*/

        Map::PlayerList const &PlList = pInstance->instance->GetPlayers();
        if (PlList.isEmpty())
            return;
        for(Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
        {
            if (Player* pPlayer = i->getSource())
            {
                if (!m_creature->IsWithinDistInMap(pPlayer,200))
                    continue;

                if (m_bIsRegularMode && m_ach_10ppl)
                    pPlayer->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE,m_creature->GetEntry(),1,0,0,7148);
                else if (!m_bIsRegularMode && m_ach_25ppl)
                    pPlayer->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE,m_creature->GetEntry(),1,0,0,7161);

                if (m_bIsRegularMode && m_ach_arahna_10)
                    pPlayer->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE,m_creature->GetEntry(),1,0,0,7128);
                else if (!m_bIsRegularMode && m_ach_arahna_25)
                    pPlayer->UpdateAchievementCriteria(ACHIEVEMENT_CRITERIA_TYPE_KILL_CREATURE,m_creature->GetEntry(),1,0,0,7129);
            }
        }

    }

    void JustSummoned(Creature* temp) 
    {
        if (!temp)
            return;

        //Summoned Spiderling will target random player
        //guidSpiderlings[Spiderlings_count++] = temp->GetGUID();
        if (Unit* target = SelectUnit(SELECT_TARGET_RANDOM,1))
        {
            temp->AddThreat(target,0.2f);
            m_creature->SetInCombatWithZone();
        }
    }

    void CheckAch()
    {
        if (!pInstance)
            return;

        m_count_ppl = 0;
        Map::PlayerList const &PlList = pInstance->instance->GetPlayers();
        if (PlList.isEmpty())
            return;
        for(Map::PlayerList::const_iterator i = PlList.begin(); i != PlList.end(); ++i)
        {
            if (Player* pPlayer = i->getSource())
            {
                if (pPlayer->isGameMaster())
                    continue;
                ++m_count_ppl;
            }
        }
        if (m_bIsRegularMode)
        {
            if(m_count_ppl>8)
                m_ach_10ppl = false;
        }
        else
        {
            if(m_count_ppl>20)
                m_ach_25ppl = false;
        }
    }

    void SendPlayerToWall(Unit* target, uint8 placeOnWall)
    {
        if (!target || target->GetTypeId() != TYPEID_PLAYER || WWplayers[placeOnWall])
            return;

        float vsin = sin(target->GetAngle(WWlocs[placeOnWall].coord_x,WWlocs[placeOnWall].coord_y));
        float vcos = cos(target->GetAngle(WWlocs[placeOnWall].coord_x,WWlocs[placeOnWall].coord_y));
        float l = target->GetDistance2d(WWlocs[placeOnWall].coord_x,WWlocs[placeOnWall].coord_y) * 1.4f;
        float h = WWlocs[placeOnWall].coord_z-target->GetPositionZ() + 30.0f;
        h = (h > 0) ? h : 0;

        WorldPacket data(SMSG_MOVE_KNOCK_BACK, (8+4+4+4+4+4));
        data.append(target->GetPackGUID());
        data << uint32(0);                                      // Sequence
        data << float(vcos);                                    // x direction
        data << float(vsin);                                    // y direction
        data << float(sqrt(l*l*10.0f/(h*2)));                   // Horizontal speed
        data << float(-sqrt(2*10.0f*h));                        // Z Movement speed (vertical)

        ((Player*)target)->GetSession()->SendPacket(&data);
        WWplayers[placeOnWall] = target->GetGUID();
        WWplayersFlyTimer[placeOnWall] = 2300;
        WWplayersFlyTimer2[placeOnWall] = 99999999;
    }

    void UpdateAI(const uint32 diff)
    {
        if (!pInstance)
            return;
        /*if (pInstance->GetData(ENCOUNT_ANUBREKHAN) == DONE && !m_arahna_timer)
            m_arahna_timer = time(NULL);*/
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        // Handle flying players, cast webwrap and summon cocoon
        for (int i = 0; i < MAX_PLAYERS_WEB_WRAP; i++)
            if (WWplayers[i])
            {
                Unit* pl = Unit::GetUnit((*m_creature),WWplayers[i]);
                if (pl)
                    pl->SetOrientation(WWlocs[i].orientation);
                if (WWplayersFlyTimer2[i] < diff && pl)
                {
                    if (Creature* Cocoon =  m_creature->SummonCreature(MOB_WEBWRAP,pl->GetPositionX(),pl->GetPositionY(),pl->GetPositionZ(),pl->GetOrientation(),
                                                                TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,60000))
                    {
                        ((mob_webwrapAI*)Cocoon->AI())->SetVictim(pl);
                        WWplayers[i] = 0;
                    }
                }else WWplayersFlyTimer2[i] -= diff;

                if (WWplayersFlyTimer[i] < diff && pl)
                {
                    pl->CastSpell(pl, SPELL_WEBWRAP_SELF, true);
                    WWplayersFlyTimer[i] = 999999999;
                    WWplayersFlyTimer2[i] = 1200;
                }else WWplayersFlyTimer[i] -= diff;
            }

        //WebTrap_Timer
        if (WebWrap_Timer < diff)
        {
            Unit *target = SelectUnit(SELECT_TARGET_RANDOM,1);
            if(target && !target->HasAura(SPELL_WEBWRAP_SELF) && !target->HasAura(SPELL_WEBSPRAY))
                SendPlayerToWall(target,rand()%MAX_PLAYERS_WEB_WRAP);
            WebWrap_Timer = 40000;
        }else WebWrap_Timer -= diff;

        //WebSpray_Timer
        if (WebSpray_Timer < diff)
        {
            DoCast(m_creature->getVictim(), !m_bIsRegularMode ? SPELL_WEBSPRAY_H:SPELL_WEBSPRAY);
            for (int i = 0; i < MAX_PLAYERS_WEB_WRAP; i++)
                if (WWplayers[i])
                {
                    Unit* pl = Unit::GetUnit((*m_creature),WWplayers[i]);
                    if (pl)
                    {
                        pl->RemoveAurasDueToSpell(SPELL_WEBSPRAY);
                        pl->CastSpell(pl,SPELL_WEBWRAP_SELF,true);
                    }
                }
            WebSpray_Timer = 40000;
        }else WebSpray_Timer -= diff;

        //PoisonShock_Timer
        if (PoisonShock_Timer < diff)
        {
            DoCast(m_creature->getVictim(), !m_bIsRegularMode ? SPELL_POISONSHOCK_H:SPELL_POISONSHOCK);
            PoisonShock_Timer = 20000;
        }else PoisonShock_Timer -= diff;

        //NecroticPoison_Timer
        if (NecroticPoison_Timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_NECROTICPOISON);
            NecroticPoison_Timer = 30000;
        }else NecroticPoison_Timer -= diff;

        //SummonSpiderling_Timer
        if (SummonSpiderling_Timer < diff)
        {
            //Cast(m_creature, SPELL_SUMMON_SPIDERLING);
            for (int i = 0; i < 8 ; i++)
            {
                DoSpawnCreature(MOB_SPIDERLING,0,0,0,0,TEMPSUMMON_TIMED_OR_DEAD_DESPAWN,120000);
            }
            SummonSpiderling_Timer = 40000;
        }else SummonSpiderling_Timer -= diff;

        //Enrage if not already enraged and below 30%
        if (!Enraged && (m_creature->GetHealth()*100 / m_creature->GetMaxHealth()) < 30)
        {
            DoCast(m_creature,!m_bIsRegularMode ? SPELL_FRENZY_H:SPELL_FRENZY);
            Enraged = true;
        }

        if (Ach_Timer<diff)
        {
            if (m_bIsRegularMode && m_ach_10ppl)
                CheckAch();
            else if (!m_bIsRegularMode && m_ach_25ppl)
                CheckAch();
            Ach_Timer = 10000;
        }else Ach_Timer -= diff;  

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_mob_webwrap(Creature* _Creature)
{
    return new mob_webwrapAI (_Creature);
}

CreatureAI* GetAI_boss_maexxna(Creature *_Creature)
{
    return new boss_maexxnaAI (_Creature);
}

void AddSC_boss_maexxna()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "boss_maexxna";
    newscript->GetAI = &GetAI_boss_maexxna;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_webwrap";
    newscript->GetAI = &GetAI_mob_webwrap;
    newscript->RegisterSelf();
}