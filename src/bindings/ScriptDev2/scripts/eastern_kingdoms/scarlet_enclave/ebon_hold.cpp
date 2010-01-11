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

/* ScriptData
SDName: Ebon_Hold
SD%Complete: 80
SDComment: Quest support: 12848, 12733, 12739(and 12742 to 12750), 12727
SDCategory: Ebon Hold
EndScriptData */

/* ContentData
npc_a_special_surprise
npc_death_knight_initiate
npc_unworthy_initiate_anchor
npc_unworthy_initiate
go_acherus_soul_prison
EndContentData */

#include "precompiled.h"
#include "escort_ai.h"

/*######
## npc_a_special_surprise
######*/

enum SpecialSurprise
{
    SAY_EXEC_START_1            = -1609025,                 // speech for all
    SAY_EXEC_START_2            = -1609026,
    SAY_EXEC_START_3            = -1609027,
    SAY_EXEC_PROG_1             = -1609028,
    SAY_EXEC_PROG_2             = -1609029,
    SAY_EXEC_PROG_3             = -1609030,
    SAY_EXEC_PROG_4             = -1609031,
    SAY_EXEC_PROG_5             = -1609032,
    SAY_EXEC_PROG_6             = -1609033,
    SAY_EXEC_PROG_7             = -1609034,
    SAY_EXEC_NAME_1             = -1609035,
    SAY_EXEC_NAME_2             = -1609036,
    SAY_EXEC_RECOG_1            = -1609037,
    SAY_EXEC_RECOG_2            = -1609038,
    SAY_EXEC_RECOG_3            = -1609039,
    SAY_EXEC_RECOG_4            = -1609040,
    SAY_EXEC_RECOG_5            = -1609041,
    SAY_EXEC_RECOG_6            = -1609042,
    SAY_EXEC_NOREM_1            = -1609043,
    SAY_EXEC_NOREM_2            = -1609044,
    SAY_EXEC_NOREM_3            = -1609045,
    SAY_EXEC_NOREM_4            = -1609046,
    SAY_EXEC_NOREM_5            = -1609047,
    SAY_EXEC_NOREM_6            = -1609048,
    SAY_EXEC_NOREM_7            = -1609049,
    SAY_EXEC_NOREM_8            = -1609050,
    SAY_EXEC_NOREM_9            = -1609051,
    SAY_EXEC_THINK_1            = -1609052,
    SAY_EXEC_THINK_2            = -1609053,
    SAY_EXEC_THINK_3            = -1609054,
    SAY_EXEC_THINK_4            = -1609055,
    SAY_EXEC_THINK_5            = -1609056,
    SAY_EXEC_THINK_6            = -1609057,
    SAY_EXEC_THINK_7            = -1609058,
    SAY_EXEC_THINK_8            = -1609059,
    SAY_EXEC_THINK_9            = -1609060,
    SAY_EXEC_THINK_10           = -1609061,
    SAY_EXEC_LISTEN_1           = -1609062,
    SAY_EXEC_LISTEN_2           = -1609063,
    SAY_EXEC_LISTEN_3           = -1609064,
    SAY_EXEC_LISTEN_4           = -1609065,
    SAY_PLAGUEFIST              = -1609066,
    SAY_EXEC_TIME_1             = -1609067,
    SAY_EXEC_TIME_2             = -1609068,
    SAY_EXEC_TIME_3             = -1609069,
    SAY_EXEC_TIME_4             = -1609070,
    SAY_EXEC_TIME_5             = -1609071,
    SAY_EXEC_TIME_6             = -1609072,
    SAY_EXEC_TIME_7             = -1609073,
    SAY_EXEC_TIME_8             = -1609074,
    SAY_EXEC_TIME_9             = -1609075,
    SAY_EXEC_TIME_10            = -1609076,
    SAY_EXEC_WAITING            = -1609077,
    EMOTE_DIES                  = -1609078,

    NPC_PLAGUEFIST              = 29053
};

struct MANGOS_DLL_DECL npc_a_special_surpriseAI : public ScriptedAI
{
    npc_a_special_surpriseAI(Creature *pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint32 m_uiExecuteSpeech_Timer;
    uint32 m_uiExecuteSpeech_Counter;
    uint64 m_uiPlayerGUID;

    void Reset()
    {
        m_uiExecuteSpeech_Timer = 0;
        m_uiExecuteSpeech_Counter = 0;
        m_uiPlayerGUID = 0;
    }

    bool MeetQuestCondition(Unit* pPlayer)
    {
        switch(m_creature->GetEntry())
        {
            case 29061:                                     // Ellen Stanbridge
                if (((Player*)pPlayer)->GetQuestStatus(12742) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29072:                                     // Kug Ironjaw
                if (((Player*)pPlayer)->GetQuestStatus(12748) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29067:                                     // Donovan Pulfrost
                if (((Player*)pPlayer)->GetQuestStatus(12744) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29065:                                     // Yazmina Oakenthorn
                if (((Player*)pPlayer)->GetQuestStatus(12743) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29071:                                     // Antoine Brack
                if (((Player*)pPlayer)->GetQuestStatus(12750) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29032:                                     // Malar Bravehorn
                if (((Player*)pPlayer)->GetQuestStatus(12739) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29068:                                     // Goby Blastenheimer
                if (((Player*)pPlayer)->GetQuestStatus(12745) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29073:                                     // Iggy Darktusk
                if (((Player*)pPlayer)->GetQuestStatus(12749) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29074:                                     // Lady Eonys
                if (((Player*)pPlayer)->GetQuestStatus(12747) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
            case 29070:                                     // Valok the Righteous
                if (((Player*)pPlayer)->GetQuestStatus(12746) == QUEST_STATUS_INCOMPLETE)
                    return true;
                break;
        }

        return false;
    }

    void MoveInLineOfSight(Unit* pWho)
    {
        if (m_uiPlayerGUID || pWho->GetTypeId() != TYPEID_PLAYER || !pWho->IsWithinDist(m_creature, INTERACTION_DISTANCE))
            return;

        if (MeetQuestCondition(pWho))
            m_uiPlayerGUID = pWho->GetGUID();
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiPlayerGUID && !m_creature->getVictim() && m_creature->isAlive())
        {
            if (m_uiExecuteSpeech_Timer < uiDiff)
            {
                Player* pPlayer = (Player*)Unit::GetUnit(*m_creature, m_uiPlayerGUID);

                if (!pPlayer)
                {
                    Reset();
                    return;
                }

                //TODO: simplify text's selection

                switch(pPlayer->getRace())
                {
                    case RACE_HUMAN:
                        switch(m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_1, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_5, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_1, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_5, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_7, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_1, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_6, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->setDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_ORC:
                        switch(m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_1, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_6, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_1, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_7, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_8, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_1, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_8, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->setDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_DWARF:
                        switch(m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_2, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_2, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_3, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_2, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_5, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_2, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_3, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->setDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_NIGHTELF:
                        switch(m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_1, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_5, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_1, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_6, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_2, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_1, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_7, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->setDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_UNDEAD_PLAYER:
                        switch(m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_1, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_3, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_4, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_3, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_1, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_3, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_4, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->setDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_TAUREN:
                        switch(m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_1, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_1, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_5, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_8, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_9, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_1, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_9, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->setDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_GNOME:
                        switch(m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_1, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_4, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_1, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_4, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_6, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_1, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_5, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->setDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_TROLL:
                        switch(m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_3, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_7, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_2, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_6, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_9, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_10, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_4, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_10, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->setDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_BLOODELF:
                        switch(m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_1, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_1, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_1, m_creature, pPlayer); break;
                            //case 5: //unknown
                            case 6: DoScriptText(SAY_EXEC_THINK_3, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_1, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_1, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->setDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                    case RACE_DRAENEI:
                        switch(m_uiExecuteSpeech_Counter)
                        {
                            case 0: DoScriptText(SAY_EXEC_START_1, m_creature, pPlayer); break;
                            case 1: m_creature->SetStandState(UNIT_STAND_STATE_STAND); break;
                            case 2: DoScriptText(SAY_EXEC_PROG_1, m_creature, pPlayer); break;
                            case 3: DoScriptText(SAY_EXEC_NAME_1, m_creature, pPlayer); break;
                            case 4: DoScriptText(SAY_EXEC_RECOG_2, m_creature, pPlayer); break;
                            case 5: DoScriptText(SAY_EXEC_NOREM_1, m_creature, pPlayer); break;
                            case 6: DoScriptText(SAY_EXEC_THINK_4, m_creature, pPlayer); break;
                            case 7: DoScriptText(SAY_EXEC_LISTEN_1, m_creature, pPlayer); break;
                            case 8:
                                if (Creature* pPlaguefist = GetClosestCreatureWithEntry(m_creature, NPC_PLAGUEFIST, 85.0f))
                                    DoScriptText(SAY_PLAGUEFIST, pPlaguefist, pPlayer);
                                break;
                            case 9:
                                DoScriptText(SAY_EXEC_TIME_2, m_creature, pPlayer);
                                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                                m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_OOC_NOT_ATTACKABLE);
                                break;
                            case 10: DoScriptText(SAY_EXEC_WAITING, m_creature, pPlayer); break;
                            case 11:
                                DoScriptText(EMOTE_DIES, m_creature);
                                m_creature->setDeathState(JUST_DIED);
                                m_creature->SetHealth(0);
                                return;
                        }
                        break;
                }

                if (m_uiExecuteSpeech_Counter >= 9)
                    m_uiExecuteSpeech_Timer = 15000;
                else
                    m_uiExecuteSpeech_Timer = 7000;

                ++m_uiExecuteSpeech_Counter;
            }
            else
                m_uiExecuteSpeech_Timer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_a_special_surprise(Creature* pCreature)
{
    return new npc_a_special_surpriseAI(pCreature);
}

/*######
## npc_death_knight_initiate
######*/

enum
{
    SAY_DUEL_A                  = -1609016,
    SAY_DUEL_B                  = -1609017,
    SAY_DUEL_C                  = -1609018,
    SAY_DUEL_D                  = -1609019,
    SAY_DUEL_E                  = -1609020,
    SAY_DUEL_F                  = -1609021,
    SAY_DUEL_G                  = -1609022,
    SAY_DUEL_H                  = -1609023,
    SAY_DUEL_I                  = -1609024,

    SPELL_DUEL                  = 52996,
    SPELL_DUEL_TRIGGERED        = 52990,
    SPELL_DUEL_VICTORY          = 52994,
    SPELL_DUEL_FLAG             = 52991,

    QUEST_DEATH_CHALLENGE       = 12733,
    FACTION_HOSTILE             = 2068
};

int32 m_auiRandomSay[] =
{
    SAY_DUEL_A, SAY_DUEL_B, SAY_DUEL_C, SAY_DUEL_D, SAY_DUEL_E, SAY_DUEL_F, SAY_DUEL_G, SAY_DUEL_H, SAY_DUEL_I
};

#define GOSSIP_ACCEPT_DUEL      "I challenge you, death knight!"

struct MANGOS_DLL_DECL npc_death_knight_initiateAI : public ScriptedAI
{
    npc_death_knight_initiateAI(Creature* pCreature) : ScriptedAI(pCreature) { Reset(); }

    uint64 m_uiDuelerGUID;
    uint32 m_uiDuelTimer;
    bool m_bIsDuelInProgress;

    void Reset()
    {
        if (m_creature->getFaction() != m_creature->GetCreatureInfo()->faction_A)
            m_creature->setFaction(m_creature->GetCreatureInfo()->faction_A);

        m_creature->SetFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_15);

        m_uiDuelerGUID = 0;
        m_uiDuelTimer = 5000;
        m_bIsDuelInProgress = false;
    }

    void AttackedBy(Unit* pAttacker)
    {
        if (m_creature->getVictim())
            return;

        if (m_creature->IsFriendlyTo(pAttacker))
            return;

        AttackStart(pAttacker);
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (!m_bIsDuelInProgress && pSpell->Id == SPELL_DUEL_TRIGGERED)
        {
            m_uiDuelerGUID = pCaster->GetGUID();
            m_bIsDuelInProgress = true;
        }
    }

    void DamageTaken(Unit* pDoneBy, uint32 &uiDamage)
    {
        if (m_bIsDuelInProgress && uiDamage > m_creature->GetHealth())
        {
            uiDamage = 0;

            if (Unit* pUnit = Unit::GetUnit(*m_creature, m_uiDuelerGUID))
                m_creature->CastSpell(pUnit, SPELL_DUEL_VICTORY, true);

            //possibly not evade, but instead have end sequenze
            EnterEvadeMode();
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
        {
            if (m_bIsDuelInProgress)
            {
                if (m_uiDuelTimer < uiDiff)
                {
                    m_creature->setFaction(FACTION_HOSTILE);

                    if (Unit* pUnit = Unit::GetUnit(*m_creature, m_uiDuelerGUID))
                        AttackStart(pUnit);
                }
                else
                    m_uiDuelTimer -= uiDiff;
            }
            return;
        }

        // TODO: spells

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_death_knight_initiate(Creature* pCreature)
{
    return new npc_death_knight_initiateAI(pCreature);
}

bool GossipHello_npc_death_knight_initiate(Player* pPlayer, Creature* pCreature)
{
    if (pPlayer->GetQuestStatus(QUEST_DEATH_CHALLENGE) == QUEST_STATUS_INCOMPLETE)
    {
        pPlayer->ADD_GOSSIP_ITEM(GOSSIP_ICON_CHAT, GOSSIP_ACCEPT_DUEL, GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF + 1);
        pPlayer->SEND_GOSSIP_MENU(13433, pCreature->GetGUID());
        return true;
    }
    return false;
}

bool GossipSelect_npc_death_knight_initiate(Player* pPlayer, Creature* pCreature, uint32 uiSender, uint32 uiAction)
{
    if (uiAction == GOSSIP_ACTION_INFO_DEF+1)
    {
        pPlayer->CLOSE_GOSSIP_MENU();

        if (npc_death_knight_initiateAI* pInitiateAI = dynamic_cast<npc_death_knight_initiateAI*>(pCreature->AI()))
        {
            if (pInitiateAI->m_bIsDuelInProgress)
                return true;
        }

        pCreature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_UNK_15);

        int32 uiSayId = rand()% (sizeof(m_auiRandomSay)/sizeof(int32));
        DoScriptText(m_auiRandomSay[uiSayId], pCreature, pPlayer);

        pCreature->CastSpell(pPlayer, SPELL_DUEL, false);
        pCreature->CastSpell(pPlayer, SPELL_DUEL_FLAG, true);
    }
    return true;
}

/*######
## npc_koltira_deathweaver
######*/

enum eKoltira
{
    SAY_BREAKOUT1                   = -1609079,
    SAY_BREAKOUT2                   = -1609080,
    SAY_BREAKOUT3                   = -1609081,
    SAY_BREAKOUT4                   = -1609082,
    SAY_BREAKOUT5                   = -1609083,
    SAY_BREAKOUT6                   = -1609084,
    SAY_BREAKOUT7                   = -1609085,
    SAY_BREAKOUT8                   = -1609086,
    SAY_BREAKOUT9                   = -1609087,
    SAY_BREAKOUT10                  = -1609088,

    SPELL_KOLTIRA_TRANSFORM         = 52899,
    SPELL_ANTI_MAGIC_ZONE           = 52894,

    QUEST_BREAKOUT                  = 12727,

    NPC_CRIMSON_ACOLYTE             = 29007,
    NPC_HIGH_INQUISITOR_VALROTH     = 29001,
    NPC_KOLTIRA_ALT                 = 28447,

    //not sure about this id
    //NPC_DEATH_KNIGHT_MOUNT          = 29201,
    MODEL_DEATH_KNIGHT_MOUNT        = 25278
};

struct MANGOS_DLL_DECL npc_koltira_deathweaverAI : public npc_escortAI
{
    npc_koltira_deathweaverAI(Creature *pCreature) : npc_escortAI(pCreature) { Reset(); }

    uint32 m_uiWave;
    uint32 m_uiWave_Timer;
    uint64 m_uiValrothGUID;

    void Reset()
    {
        if (!HasEscortState(STATE_ESCORT_ESCORTING))
        {
            m_uiWave = 0;
            m_uiWave_Timer = 3000;
            m_uiValrothGUID = 0;
        }
    }

    void WaypointReached(uint32 uiPointId)
    {
        switch(uiPointId)
        {
            case 0:
                DoScriptText(SAY_BREAKOUT1, m_creature);
                break;
            case 1:
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                break;
            case 2:
                m_creature->SetStandState(UNIT_STAND_STATE_STAND);
                //m_creature->UpdateEntry(NPC_KOLTIRA_ALT); //unclear if we must update or not
                DoCast(m_creature, SPELL_KOLTIRA_TRANSFORM);
                break;
            case 3:
                SetEscortPaused(true);
                m_creature->SetStandState(UNIT_STAND_STATE_KNEEL);
                DoScriptText(SAY_BREAKOUT2, m_creature);
                DoCast(m_creature, SPELL_ANTI_MAGIC_ZONE);  // cast again that makes bubble up
                break;
            case 4:
                SetRun(true);
                break;
            case 9:
                m_creature->Mount(MODEL_DEATH_KNIGHT_MOUNT);
                break;
            case 10:
                m_creature->Unmount();
                break;
        }
    }

    void JustSummoned(Creature* pSummoned)
    {
        if (Player* pPlayer = GetPlayerForEscort())
        {
            pSummoned->AI()->AttackStart(pPlayer);
            pSummoned->AddThreat(m_creature);
        }

        if (pSummoned->GetEntry() == NPC_HIGH_INQUISITOR_VALROTH)
            m_uiValrothGUID = pSummoned->GetGUID();
    }

    void SummonAcolyte(uint32 uiAmount)
    {
        for(uint32 i = 0; i < uiAmount; ++i)
            m_creature->SummonCreature(NPC_CRIMSON_ACOLYTE, 1642.329, -6045.818, 127.583, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 5000);
    }

    void UpdateEscortAI(const uint32 uiDiff)
    {
        if (HasEscortState(STATE_ESCORT_PAUSED))
        {
            if (m_uiWave_Timer < uiDiff)
            {
                switch(m_uiWave)
                {
                    case 0:
                        DoScriptText(SAY_BREAKOUT3, m_creature);
                        SummonAcolyte(3);
                        m_uiWave_Timer = 20000;
                        break;
                    case 1:
                        DoScriptText(SAY_BREAKOUT4, m_creature);
                        SummonAcolyte(3);
                        m_uiWave_Timer = 20000;
                        break;
                    case 2:
                        DoScriptText(SAY_BREAKOUT5, m_creature);
                        SummonAcolyte(4);
                        m_uiWave_Timer = 20000;
                        break;
                    case 3:
                        DoScriptText(SAY_BREAKOUT6, m_creature);
                        m_creature->SummonCreature(NPC_HIGH_INQUISITOR_VALROTH, 1642.329, -6045.818, 127.583, 0.0f, TEMPSUMMON_TIMED_DESPAWN_OUT_OF_COMBAT, 1000);
                        m_uiWave_Timer = 1000;
                        break;
                    case 4:
                    {
                        Unit* pTemp = Unit::GetUnit(*m_creature, m_uiValrothGUID);

                        if (!pTemp || !pTemp->isAlive())
                        {
                            DoScriptText(SAY_BREAKOUT8, m_creature);
                            m_uiWave_Timer = 5000;
                        }
                        else
                        {
                            m_uiWave_Timer = 2500;
                            return;                         //return, we don't want m_uiWave to increment now
                        }
                        break;
                    }
                    case 5:
                        DoScriptText(SAY_BREAKOUT9, m_creature);
                        m_creature->RemoveAurasDueToSpell(SPELL_ANTI_MAGIC_ZONE);
                        m_uiWave_Timer = 2500;
                        break;
                    case 6:
                        DoScriptText(SAY_BREAKOUT10, m_creature);
                        SetEscortPaused(false);
                        break;
                }

                ++m_uiWave;
            }
            else
                m_uiWave_Timer -= uiDiff;
        }

        if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
            return;

        DoMeleeAttackIfReady();
    }
};

CreatureAI* GetAI_npc_koltira_deathweaver(Creature* pCreature)
{
    return new npc_koltira_deathweaverAI(pCreature);
}

bool QuestAccept_npc_koltira_deathweaver(Player* pPlayer, Creature* pCreature, const Quest* pQuest)
{
    if (pQuest->GetQuestId() == QUEST_BREAKOUT)
    {
        pCreature->SetStandState(UNIT_STAND_STATE_STAND);

        if (npc_koltira_deathweaverAI* pEscortAI = dynamic_cast<npc_koltira_deathweaverAI*>(pCreature->AI()))
            pEscortAI->Start(false, false, pPlayer->GetGUID(), pQuest);
    }
    return true;
}

/*######
##
######*/

enum
{
    SAY_START                       = -1609000,             // 8 texts in total, GetTextId() generates random with this as base
    SAY_AGGRO                       = -1609008,             // 8 texts in total, GetTextId() generates random with this as base

    //SPELL_CHAINED_PESANT_LH         = 54602,              // not used. possible it determine side, where to go get "weapon"
    //SPELL_CHAINED_PESANT_RH         = 54610,
    SPELL_CHAINED_PESANT_CHEST      = 54612,
    SPELL_CHAINED_PESANT_BREATH     = 54613,
    SPELL_INITIATE_VISUAL           = 51519,

    SPELL_BLOOD_STRIKE              = 52374,
    SPELL_DEATH_COIL                = 52375,
    SPELL_ICY_TOUCH                 = 52372,
    SPELL_PLAGUE_STRIKE             = 52373,

    NPC_ANCHOR                      = 29521,
    FACTION_MONSTER                 = 16,

    PHASE_INACTIVE_OR_COMBAT        = 0,
    PHASE_DRESSUP                   = 1,
    PHASE_ACTIVATE                  = 2
};

struct DisplayToSpell
{
    uint32 m_uiDisplayId;
    uint32 m_uiSpellToNewDisplay;
};

DisplayToSpell m_aDisplayToSpell[] =
{
    {25354, 51520},                                         // human M
    {25355, 51534},                                         // human F
    {25356, 51538},                                         // dwarf M
    {25357, 51541},                                         // draenei M
    {25358, 51535},                                         // nelf M
    {25359, 51539},                                         // gnome M
    {25360, 51536},                                         // nelf F
    {25361, 51537},                                         // dwarf F
    {25362, 51540},                                         // gnome F
    {25363, 51542},                                         // draenei F
    {25364, 51543},                                         // orc M
    {25365, 51546},                                         // troll M
    {25366, 51547},                                         // tauren M
    {25367, 51549},                                         // forsaken M
    {25368, 51544},                                         // orc F
    {25369, 51552},                                         // belf F
    {25370, 51545},                                         // troll F
    {25371, 51548},                                         // tauren F
    {25372, 51550},                                         // forsaken F
    {25373, 51551}                                          // belf M
};

/*######
## npc_unworthy_initiate_anchor
######*/

struct MANGOS_DLL_DECL npc_unworthy_initiate_anchorAI : public ScriptedAI
{
    npc_unworthy_initiate_anchorAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_uiMyInitiate = 0;
        Reset();
    }

    uint64 m_uiMyInitiate;

    void Reset() { }

    void NotifyMe(Unit* pSource)
    {
        Creature* pInitiate = (Creature*)Unit::GetUnit(*m_creature, m_uiMyInitiate);

        if (pInitiate && pSource)
        {
            pInitiate->SetLootRecipient(pSource);
            m_creature->CastSpell(pInitiate,SPELL_CHAINED_PESANT_BREATH,true);
        }
    }

    void RegisterCloseInitiate(uint64 uiGuid)
    {
        m_uiMyInitiate = uiGuid;
    }
};

CreatureAI* GetAI_npc_unworthy_initiate_anchor(Creature* pCreature)
{
    return new npc_unworthy_initiate_anchorAI(pCreature);
}

/*######
## npc_unworthy_initiate
######*/

struct MANGOS_DLL_DECL npc_unworthy_initiateAI : public ScriptedAI
{
    npc_unworthy_initiateAI(Creature* pCreature) : ScriptedAI(pCreature)
    {
        m_pToTransform = NULL;

        uint32 uiDisplayCount = sizeof(m_aDisplayToSpell)/sizeof(DisplayToSpell);

        for (uint8 i=0; i<uiDisplayCount; ++i)
        {
            // check if we find a match, if not, it's NULL and produce error when used
            if (m_aDisplayToSpell[i].m_uiDisplayId == pCreature->GetDisplayId())
            {
                m_pToTransform = &m_aDisplayToSpell[i];
                break;
            }
        }

        m_uiNormFaction = pCreature->getFaction();
        Reset();
    }

    DisplayToSpell* m_pToTransform;

    uint32 m_uiNormFaction;
    uint32 m_uiAnchorCheckTimer;
    uint32 m_uiPhase;
    uint32 m_uiPhaseTimer;
    uint32 m_uiBloodStrike_Timer;
    uint32 m_uiDeathCoil_Timer;
    uint32 m_uiIcyTouch_Timer;
    uint32 m_uiPlagueStrike_Timer;

    void Reset()
    {
        if (m_creature->getFaction() != m_uiNormFaction)
            m_creature->setFaction(m_uiNormFaction);

        m_uiAnchorCheckTimer = 5000;
        m_uiPhase = PHASE_INACTIVE_OR_COMBAT;
        m_uiPhaseTimer = 7500;
        m_uiBloodStrike_Timer = 4000;
        m_uiDeathCoil_Timer = 6000;
        m_uiIcyTouch_Timer = 2000;
        m_uiPlagueStrike_Timer = 5000;
    }

    void JustReachedHome()
    {
        SetAnchor();
    }

    int32 GetTextId()
    {
        return m_uiPhase == PHASE_DRESSUP ? SAY_START-rand()%8 : SAY_AGGRO-rand()%8;
    }

    void SetAnchor()
    {
        if (Creature* pAnchor = GetClosestCreatureWithEntry(m_creature, NPC_ANCHOR, INTERACTION_DISTANCE*2))
        {
            ((npc_unworthy_initiate_anchorAI*)pAnchor->AI())->RegisterCloseInitiate(m_creature->GetGUID());

            pAnchor->CastSpell(m_creature, SPELL_CHAINED_PESANT_CHEST, false);

            m_uiAnchorCheckTimer = 0;
            return;
        }

        m_uiAnchorCheckTimer = 5000;
    }

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pSpell->Id == SPELL_CHAINED_PESANT_BREATH)
        {
            pCaster->InterruptNonMeleeSpells(true);
            m_creature->SetStandState(UNIT_STAND_STATE_STAND);

            m_uiPhase = PHASE_DRESSUP;

            if (Player* pSource = m_creature->GetLootRecipient())
                DoScriptText(GetTextId(), m_creature, pSource);
        }
    }

    void UpdateAI(const uint32 uiDiff)
    {
        if (m_uiAnchorCheckTimer && m_uiAnchorCheckTimer < uiDiff)
            SetAnchor();
        else
            m_uiAnchorCheckTimer -= uiDiff;

        if (m_uiPhase == PHASE_INACTIVE_OR_COMBAT)
        {
            if (!m_creature->SelectHostileTarget() || !m_creature->getVictim())
                return;

            if (m_uiBloodStrike_Timer < uiDiff)
            {
                DoCast(m_creature->getVictim(),SPELL_BLOOD_STRIKE);
                m_uiBloodStrike_Timer = 9000;
            }else m_uiBloodStrike_Timer -= uiDiff;

            if (m_uiDeathCoil_Timer < uiDiff)
            {
                DoCast(m_creature->getVictim(),SPELL_DEATH_COIL);
                m_uiDeathCoil_Timer = 8000;
            }else m_uiDeathCoil_Timer -= uiDiff;

            if (m_uiIcyTouch_Timer < uiDiff)
            {
                DoCast(m_creature->getVictim(),SPELL_ICY_TOUCH);
                m_uiIcyTouch_Timer = 8000;
            }else m_uiIcyTouch_Timer -= uiDiff;

            if (m_uiPlagueStrike_Timer < uiDiff)
            {
                DoCast(m_creature->getVictim(),SPELL_PLAGUE_STRIKE);
                m_uiPlagueStrike_Timer = 8000;
            }else m_uiPlagueStrike_Timer -= uiDiff;

            DoMeleeAttackIfReady();
        }
        else
        {
            if (m_uiPhaseTimer < uiDiff)
            {
                if (m_uiPhase == PHASE_DRESSUP)
                {
                    if (m_pToTransform)
                    {
                        m_creature->CastSpell(m_creature, m_pToTransform->m_uiSpellToNewDisplay, true);
                        m_creature->CastSpell(m_creature, SPELL_INITIATE_VISUAL, false);
                    }
                    else
                        error_log("SD2: npc_unworthy_initiate does not have any spell associated with model");

                    m_uiPhase = PHASE_ACTIVATE;
                }
                else
                {
                    m_creature->setFaction(FACTION_MONSTER);

                    m_uiPhase = PHASE_INACTIVE_OR_COMBAT;

                    if (Player* pTarget = m_creature->GetLootRecipient())
                    {
                        DoScriptText(GetTextId(), m_creature, pTarget);
                        AttackStart(pTarget);
                    }
                }

                m_uiPhaseTimer = 5000;
            }
            else
                m_uiPhaseTimer -= uiDiff;
        }
    }
};

CreatureAI* GetAI_npc_unworthy_initiate(Creature* pCreature)
{
    return new npc_unworthy_initiateAI(pCreature);
}

/*######
## go_acherus_soul_prison
######*/

bool GOHello_go_acherus_soul_prison(Player* pPlayer, GameObject* pGo)
{
    if (Creature* pAnchor = GetClosestCreatureWithEntry(pGo, NPC_ANCHOR, INTERACTION_DISTANCE))
        ((npc_unworthy_initiate_anchorAI*)pAnchor->AI())->NotifyMe(pPlayer);

    return false;
}

/*######
## npc_salanar_the_horseman
######*/
enum salanar
{
    SPELL_REALM_OF_SHADOWS          = 52275,
    //SPELL_DEATH_RACE_COMPLETE       = 52361,
    SPELL_HORSEMANS_CALL            = 52362, // not working
    NPC_ACHERUS_DEATHCHARGER        = 28782,
    NPC_DARK_RIDER_OF_ACHERUS       = 28768
};

bool GossipHello_npc_salanar_the_horseman(Player* pPlayer, Creature* pCreature)
{
    if (pCreature->isQuestGiver())
        pPlayer->PrepareQuestMenu( pCreature->GetGUID() );

    if (pPlayer->GetQuestStatus(12687) == QUEST_STATUS_INCOMPLETE)
        pPlayer->ADD_GOSSIP_ITEM( 0, "Send me into the Realm of Shadows.", GOSSIP_SENDER_MAIN, GOSSIP_ACTION_INFO_DEF+1);

    //pPlayer->SEND_GOSSIP_MENU(pCreature->GetNpcTextId(), pCreature->GetGUID());
    pPlayer->SEND_GOSSIP_MENU(pPlayer->GetGossipTextId(pCreature), pCreature->GetGUID());

    return true;
}

bool GossipSelect_npc_salanar_the_horseman(Player* pPlayer, Creature *pCreature, uint32 uiSender, uint32 uiAction)
{
    switch (uiAction)
    {
        case GOSSIP_ACTION_INFO_DEF+1:
            pPlayer->CLOSE_GOSSIP_MENU();
            pPlayer->CastSpell(pPlayer, SPELL_REALM_OF_SHADOWS, true);
            pPlayer->SummonCreature(NPC_DARK_RIDER_OF_ACHERUS, pCreature->GetPositionX(), pCreature->GetPositionY(), pCreature->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 180000);
            break;
    }
    return true;
}

/*######
## Mob Dark Rider of Acherus
######*/
enum darkrider
{
    //SPELL_ICY_TOUCH             = 52372,
    //SPELL_BLOOD_STRIKE          = 52374,
    SPELL_PLAGUE_STRIKE2        = 50688,
    SPELL_THROW                 = 52356,
    SPELL_DEATH_RACE_COMPLETE   = 52361
};
// 52693
struct MANGOS_DLL_DECL mob_dark_rider_of_acherusAI : public ScriptedAI
{
    mob_dark_rider_of_acherusAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 uiBlood_strike_timer;
    uint32 uiIcy_touch_timer;
    uint32 uiPlague_strike_timer;
    uint32 uiThrow_timer;
    uint64 uiPlayerGUID;

    void Reset()
    {
        uiBlood_strike_timer = 3000;
        uiIcy_touch_timer = 4000;
        uiPlague_strike_timer = 5000;
        uiThrow_timer = 10000;
        uiPlayerGUID = 0;
    }

    void Aggro(Unit* who)
    {
        uiPlayerGUID = who->GetGUID();
    }

    void UpdateAI(const uint32 diff) 
    {
        if (uiBlood_strike_timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_BLOOD_STRIKE);
            uiBlood_strike_timer = 5000 + rand()%1000;
        }else uiBlood_strike_timer -= diff;

        if (uiIcy_touch_timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_ICY_TOUCH);
            uiIcy_touch_timer = 6000 + rand()%1000;
        }else uiIcy_touch_timer -= diff;

        if (uiPlague_strike_timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_PLAGUE_STRIKE2);
            uiPlague_strike_timer = 12000 + rand()%1000;
        }else uiPlague_strike_timer -= diff;

        if (uiThrow_timer < diff)
        {
            DoCast(m_creature->getVictim(), SPELL_THROW);
            uiThrow_timer = 10000 + rand()%1000;
        }else uiThrow_timer -= diff;

        DoMeleeAttackIfReady();
    }
    void JustDied(Unit* killer)
    {
        if (Unit* pPlayer = Unit::GetUnit(*m_creature, uiPlayerGUID))
            pPlayer->CastSpell(pPlayer, SPELL_DEATH_RACE_COMPLETE, true);
    }
};


/*######
## Mob scarlet miner
######*/
enum scarletminer
{
    SPELL_GIFT_OF_THE_HARVESTER_MISSILE = 52481,
    NPC_SCARLET_GHOUL                   = 28845
};

struct MANGOS_DLL_DECL mob_scarlet_minerAI : public ScriptedAI
{
    mob_scarlet_minerAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        // hack spell 52481
        SpellEntry *TempSpell = (SpellEntry*)GetSpellStore()->LookupEntry(SPELL_GIFT_OF_THE_HARVESTER_MISSILE);
        if (TempSpell && TempSpell->EffectImplicitTargetB[0] != 16)
        {
            TempSpell->EffectImplicitTargetB[0] = 16;
            TempSpell->EffectImplicitTargetB[1] = 87;
            TempSpell->EffectImplicitTargetB[2] = 16;
        }
    }

    void Reset() {}

    void SpellHit(Unit* pCaster, const SpellEntry* pSpell)
    {
        if (pCaster->GetTypeId() == TYPEID_PLAYER && m_creature->isAlive() && pSpell->Id == SPELL_GIFT_OF_THE_HARVESTER_MISSILE)
        {
            if(((Player*)pCaster)->GetQuestStatus(12698) == QUEST_STATUS_INCOMPLETE)
            {
                // spell 52490 Scarlet Miner Ghoul Transform doesn't work, hack it
                Unit* pGhoul = m_creature->SummonCreature(NPC_SCARLET_GHOUL, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ(), 0, TEMPSUMMON_TIMED_DESPAWN, 60000);
                ((Player*)pCaster)->KilledMonsterCredit(NPC_SCARLET_GHOUL,pGhoul->GetGUID());
                m_creature->setDeathState(JUST_DIED);
                m_creature->RemoveCorpse();
            }
        }
    }
};

/*######
## Mob scarlet courier
######*/
// use 28957 Scarlet Crusader Test Dummy Guy to start it
enum scarletcourier
{
    SAY_TREE1                          = -1609120,
    SAY_TREE2                          = -1609121,
    SPELL_SHOOT                        = 52818,
    GO_INCONSPICUOUS_TREE              = 191144,
    NPC_SCARLET_COURIER                = 29076
};

struct MANGOS_DLL_DECL mob_scarlet_courierAI : public ScriptedAI
{
    mob_scarlet_courierAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 uiStage;
    uint32 uiStage_timer;
    uint64 pPlayer;

    void Reset() {
        uiStage = 0;
        uiStage_timer = 3000;
        pPlayer = 0;
    }

    void MovementInform(uint32 type, uint32 id)
    {
        if(type != POINT_MOTION_TYPE)
                return;

        switch(id)
        {
            case 0:
                uiStage = 1;
                break;
            case 1:
                uiStage = 2;
                break;
        }
    }

    void UpdateAI(const uint32 diff) 
    {
        if (uiStage_timer < diff)
        {
            switch(uiStage)
            {
                case 1:
                {
                    m_creature->GetMotionMaster()->Clear(false);
                    m_creature->GetMotionMaster()->MoveIdle();
                    m_creature->AddMonsterMoveFlag(MONSTER_MOVE_WALK);

                    if (GameObject* treeGO = GetClosestGameObjectWithEntry(m_creature, GO_INCONSPICUOUS_TREE, 40.0f))
                    {
                        DoScriptText(SAY_TREE1, m_creature);
                        m_creature->GetMotionMaster()->MovePoint(1, treeGO->GetPositionX(), treeGO->GetPositionY(), treeGO->GetPositionZ());
                    }
                    uiStage = 0;
                } break;
                case 2:
                {
                    m_creature->GetMotionMaster()->Clear(false);
                    m_creature->GetMotionMaster()->MoveIdle();
                    DoScriptText(SAY_TREE2, m_creature);
                    m_creature->Unmount();
                    //who->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
                    uiStage = 0;
                } break;
            }
            uiStage_timer = 3000;
        }else uiStage_timer -= diff;

        DoMeleeAttackIfReady();
    }
};
struct MANGOS_DLL_DECL mob_scarlet_courier_controllerAI : public ScriptedAI
{
    mob_scarlet_courier_controllerAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    bool bAmbush_overlook;

    void Reset() {
        bAmbush_overlook = false;
    }

    void UpdateAI(const uint32 diff) 
    {
        GameObject* treeGO = GetClosestGameObjectWithEntry(m_creature, GO_INCONSPICUOUS_TREE, 40.0f);
        if(treeGO && bAmbush_overlook == false)
        {
            Creature* pCourier = m_creature->SummonCreature(NPC_SCARLET_COURIER, 1461.65, -6010.34, 116.369, 0, TEMPSUMMON_TIMED_DESPAWN, 180000);
            pCourier->RemoveMonsterMoveFlag(MONSTER_MOVE_WALK);
            pCourier->Mount(14338); // not sure about this id
            pCourier->GetMotionMaster()->MovePoint(0, m_creature->GetPositionX(), m_creature->GetPositionY(), m_creature->GetPositionZ());
            bAmbush_overlook = true;
        }
        if(!treeGO && bAmbush_overlook == true)
            bAmbush_overlook = false;
    }
};

/*######
## Mob High Inquisitor Valroth
######*/
enum valroth
{
    SAY_VALROTH1                      = -1609122,
    SAY_VALROTH2                      = -1609123,
    SAY_VALROTH3                      = -1609124,
    SAY_VALROTH4                      = -1609125,
    SAY_VALROTH5                      = -1609126,
    SAY_VALROTH6                      = -1609127,
    SPELL_RENEW                       = 38210,
    SPELL_INQUISITOR_PENANCE          = 52922,
    SPELL_VALROTH_SMITE               = 52926,
    SPELL_SUMMON_VALROTH_REMAINS      = 52929
};

/*######
## quest How To Win Friends And Influence Enemies
######*/
enum win_friends
{
    SAY_PERSUADE1                     = -1609101,
    SAY_PERSUADE2                     = -1609102,
    SAY_PERSUADE3                     = -1609103,
    SAY_PERSUADE4                     = -1609104,
    SAY_PERSUADE5                     = -1609105,
    SAY_PERSUADE6                     = -1609106,
    SAY_PERSUADE7                     = -1609107,
    SAY_CRUSADER1                     = -1609108,
    SAY_CRUSADER2                     = -1609109,
    SAY_CRUSADER3                     = -1609110,
    SAY_CRUSADER4                     = -1609111,
    SAY_CRUSADER5                     = -1609112,
    SAY_CRUSADER6                     = -1609113,
    SAY_PERSUADED1                    = -1609114,
    SAY_PERSUADED2                    = -1609115,
    SAY_PERSUADED3                    = -1609116,
    SAY_PERSUADED4                    = -1609117,
    SAY_PERSUADED5                    = -1609118,
    SAY_PERSUADED6                    = -1609119,
    SPELL_PERSUASIVE_STRIKE           = 52781
};

struct MANGOS_DLL_DECL npc_crusade_persuadedAI : public ScriptedAI
{
    npc_crusade_persuadedAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 uiSpeech_timer;
    uint32 uiSpeech_counter;
    uint32 uiCrusade_faction;
    uint64 uiPlayerGUID;

    void Reset()
    {
        uiSpeech_timer = 0;
        uiSpeech_counter = 0;
        uiCrusade_faction = 0;
        uiPlayerGUID = 0;
    }

    void SpellHit(Unit *caster, const SpellEntry *spell)
    {
        if (caster->GetTypeId() == TYPEID_PLAYER && m_creature->isAlive() && spell->Id == SPELL_PERSUASIVE_STRIKE && uiSpeech_counter == 0)
        {
            if(((Player*)caster)->GetQuestStatus(12720) == QUEST_STATUS_INCOMPLETE)
            {
                if (rand()%100 > 90) // chance
                {
                    uiPlayerGUID = ((Player*)caster)->GetGUID();
                    uiCrusade_faction = m_creature->getFaction();
                    uiSpeech_timer = 1000;
                    uiSpeech_counter = 1;
                    m_creature->setFaction(35);
                }
                else if (uiSpeech_counter == 0)
                {
                    switch(rand()%6)
                    {
                        case 0: DoScriptText(SAY_PERSUADE1, caster);break;
                        case 1: DoScriptText(SAY_PERSUADE2, caster);break;
                        case 2: DoScriptText(SAY_PERSUADE3, caster);break;
                        case 3: DoScriptText(SAY_PERSUADE4, caster);break;
                        case 4: DoScriptText(SAY_PERSUADE5, caster);break;
                        case 5: DoScriptText(SAY_PERSUADE6, caster);break;
                        case 6: DoScriptText(SAY_PERSUADE7, caster);break;
                    }
                    switch(rand()%5)
                    {
                        case 0: DoScriptText(SAY_CRUSADER1, m_creature);break;
                        case 1: DoScriptText(SAY_CRUSADER2, m_creature);break;
                        case 2: DoScriptText(SAY_CRUSADER3, m_creature);break;
                        case 3: DoScriptText(SAY_CRUSADER4, m_creature);break;
                        case 4: DoScriptText(SAY_CRUSADER5, m_creature);break;
                        case 5: DoScriptText(SAY_CRUSADER6, m_creature);break;
                    }
                }
            }
        }
    }

    void UpdateAI(const uint32 diff) 
    {
        if (uiSpeech_counter >= 1 && uiSpeech_counter <= 6)
            if (uiSpeech_timer < diff)
            {
                m_creature->CombatStop(true);
                m_creature->StopMoving();
                Unit* pPlayer = Unit::GetUnit(*m_creature, uiPlayerGUID);

                switch(uiSpeech_counter)
                {
                    case 1: DoScriptText(SAY_PERSUADED1, m_creature); uiSpeech_timer = 8000; uiSpeech_counter++; break;
                    case 2: DoScriptText(SAY_PERSUADED2, m_creature); uiSpeech_timer = 8000; uiSpeech_counter++; break;
                    case 3: DoScriptText(SAY_PERSUADED3, m_creature); uiSpeech_timer = 8000; uiSpeech_counter++; break;
                    case 4: DoScriptText(SAY_PERSUADED4, m_creature); uiSpeech_timer = 8000; uiSpeech_counter++; break;
                    case 5: DoScriptText(SAY_PERSUADED5, pPlayer); uiSpeech_timer = 8000; uiSpeech_counter++; break;
                    case 6:
                        DoScriptText(SAY_PERSUADED6, m_creature);
                        m_creature->setFaction(uiCrusade_faction);
                        //m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NON_ATTACKABLE);
                        //m_creature->RemoveFlag(UNIT_FIELD_FLAGS, UNIT_FLAG_NOT_SELECTABLE);
                        uiSpeech_timer = 0;
                        uiCrusade_faction = 0;
                        uiSpeech_counter++;
                        AttackStart(pPlayer);
                        if(((Player*)pPlayer)->GetQuestStatus(12720) == QUEST_STATUS_INCOMPLETE)
                            ((Player*)pPlayer)->AreaExploredOrEventHappens(12720);
                        break;
                }
            }else uiSpeech_timer -= diff;
        else
            DoMeleeAttackIfReady();
    }
};

struct MANGOS_DLL_DECL mob_high_inquisitor_valrothAI : public ScriptedAI
{
    mob_high_inquisitor_valrothAI(Creature *pCreature) : ScriptedAI(pCreature)
    {
        Reset();
    }

    uint32 uiRenew_timer;
    uint32 uiInquisitor_Penance_timer;
    uint32 uiValroth_Smite_timer;

    void Reset()
    {
        uiRenew_timer = 1000;
        uiInquisitor_Penance_timer = 2000;
        uiValroth_Smite_timer = 1000;
    }

    void Aggro(Unit* who)
    {
        DoScriptText(SAY_VALROTH2, m_creature);
        DoCast(who, SPELL_VALROTH_SMITE);
    }

    void UpdateAI(const uint32 diff)
    {
        if (uiRenew_timer < diff)
        {
            Shout();
            DoCast(m_creature, SPELL_RENEW);
            uiRenew_timer = 1000 + rand()%5000;
        }else uiRenew_timer -= diff;

        if (uiInquisitor_Penance_timer < diff)
        {
            Shout();
            DoCast(m_creature->getVictim(), SPELL_INQUISITOR_PENANCE);
            uiInquisitor_Penance_timer = 2000 + rand()%5000;
        }else uiInquisitor_Penance_timer -= diff;

        if (uiValroth_Smite_timer < diff)
        {
            Shout();
            DoCast(m_creature->getVictim(), SPELL_VALROTH_SMITE);
            uiValroth_Smite_timer = 1000 + rand()%5000;
        }else uiValroth_Smite_timer -= diff;

        DoMeleeAttackIfReady();
    }

    void Shout()
    {
        switch(rand()%20)
        {
            case 0: DoScriptText(SAY_VALROTH3, m_creature);break;
            case 1: DoScriptText(SAY_VALROTH4, m_creature);break;
            case 2: DoScriptText(SAY_VALROTH5, m_creature);break;
        }
    }

    void JustDied(Unit* killer)
    {
        DoScriptText(SAY_VALROTH6, m_creature);
        killer->CastSpell(m_creature, SPELL_SUMMON_VALROTH_REMAINS, true);
    }
};


CreatureAI* GetAI_mob_dark_rider_of_acherus(Creature* pCreature)
{
    return new mob_dark_rider_of_acherusAI (pCreature);
}

CreatureAI* GetAI_mob_scarlet_miner(Creature* pCreature)
{
    return new mob_scarlet_minerAI (pCreature);
}

CreatureAI* GetAI_mob_scarlet_courier_controller(Creature* pCreature)
{
    return new mob_scarlet_courier_controllerAI (pCreature);
}

CreatureAI* GetAI_mob_scarlet_courier(Creature* pCreature)
{
    return new mob_scarlet_courierAI (pCreature);
}

CreatureAI* GetAI_mob_high_inquisitor_valroth(Creature* pCreature)
{
    return new mob_high_inquisitor_valrothAI (pCreature);
}

CreatureAI* GetAI_npc_crusade_persuaded(Creature* pCreature)
{
    return new npc_crusade_persuadedAI (pCreature);
}

void AddSC_ebon_hold()
{
    Script *newscript;

    newscript = new Script;
    newscript->Name = "npc_a_special_surprise";
    newscript->GetAI = &GetAI_npc_a_special_surprise;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_death_knight_initiate";
    newscript->GetAI = &GetAI_npc_death_knight_initiate;
    newscript->pGossipHello = &GossipHello_npc_death_knight_initiate;
    newscript->pGossipSelect = &GossipSelect_npc_death_knight_initiate;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_koltira_deathweaver";
    newscript->GetAI = &GetAI_npc_koltira_deathweaver;
    newscript->pQuestAccept = &QuestAccept_npc_koltira_deathweaver;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_unworthy_initiate";
    newscript->GetAI = &GetAI_npc_unworthy_initiate;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_unworthy_initiate_anchor";
    newscript->GetAI = &GetAI_npc_unworthy_initiate_anchor;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "go_acherus_soul_prison";
    newscript->pGOHello = &GOHello_go_acherus_soul_prison;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_salanar_the_horseman";
    newscript->pGossipHello =  &GossipHello_npc_salanar_the_horseman;
    newscript->pGossipSelect = &GossipSelect_npc_salanar_the_horseman;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_dark_rider_of_acherus";
    newscript->GetAI = &GetAI_mob_dark_rider_of_acherus;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_scarlet_miner";
    newscript->GetAI = &GetAI_mob_scarlet_miner;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_scarlet_courier_controller";
    newscript->GetAI = &GetAI_mob_scarlet_courier_controller;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_scarlet_courier";
    newscript->GetAI = &GetAI_mob_scarlet_courier;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "mob_high_inquisitor_valroth";
    newscript->GetAI = &GetAI_mob_high_inquisitor_valroth;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "npc_crusade_persuaded";
    newscript->GetAI = &GetAI_npc_crusade_persuaded;
    newscript->RegisterSelf();
}
