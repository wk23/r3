/*
 * Copyright (C) 2005-2009 MaNGOS <http://getmangos.com/>
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
 */

#include "AggressorAI.h"
#include "Errors.h"
#include "Creature.h"
#include "ObjectAccessor.h"
#include "VMapFactory.h"
#include "World.h"

#include <list>

int
AggressorAI::Permissible(const Creature *creature)
{
    // have some hostile factions, it will be selected by IsHostileTo check at MoveInLineOfSight
    if( !creature->isCivilian() && !creature->IsNeutralToAll() )
        return PERMIT_BASE_PROACTIVE;

    return PERMIT_BASE_NO;
}

AggressorAI::AggressorAI(Creature *c) : CreatureAI(c), i_victimGuid(0), i_state(STATE_NORMAL), i_tracker(TIME_INTERVAL_LOOK)
{
}

void
AggressorAI::MoveInLineOfSight(Unit *u)
{
    // Ignore Z for flying creatures
    if( !m_creature->canFly() && m_creature->GetDistanceZ(u) > CREATURE_Z_ATTACK_RANGE )
        return;

    if( !m_creature->hasUnitState(UNIT_STAT_STUNNED) && u->isTargetableForAttack() &&
        ( m_creature->IsHostileTo( u ) /*|| u->getVictim() && m_creature->IsFriendlyTo( u->getVictim() )*/ ) &&
        u->isInAccessablePlaceFor(m_creature) )
    {
        float attackRadius = m_creature->GetAttackDistance(u);
        if(m_creature->IsWithinDistInMap(u, attackRadius) && m_creature->IsWithinLOSInMap(u) )
        {
            if(!m_creature->getVictim())
            {
                AttackStart(u);
                u->RemoveSpellsCausingAura(SPELL_AURA_MOD_STEALTH);
            }
            else if(sMapStore.LookupEntry(m_creature->GetMapId())->IsDungeon())
            {
                m_creature->AddThreat(u, 0.0f);
                u->SetInCombatWith(m_creature);
            }
        }
    }
}

void AggressorAI::EnterEvadeMode()
{
    if( !m_creature->isAlive() )
    {
        DEBUG_LOG("Creature stopped attacking cuz his dead [guid=%u]", m_creature->GetGUIDLow());
        i_victimGuid = 0;
        m_creature->CombatStop(true);
        m_creature->DeleteThreatList();
        return;
    }

    Unit* victim = ObjectAccessor::GetUnit(*m_creature, i_victimGuid );

    if( !victim  )
    {
        DEBUG_LOG("Creature stopped attacking because victim is non exist [guid=%u]", m_creature->GetGUIDLow());
    }
    else if( !victim->isAlive() )
    {
        DEBUG_LOG("Creature stopped attacking cuz his victim is dead [guid=%u]", m_creature->GetGUIDLow());
    }
    else if( victim->HasStealthAura() )
    {
        DEBUG_LOG("Creature stopped attacking cuz his victim is stealth [guid=%u]", m_creature->GetGUIDLow());
    }
    else if( victim->isInFlight() )
    {
        DEBUG_LOG("Creature stopped attacking cuz his victim is fly away [guid=%u]", m_creature->GetGUIDLow());
    }
    else
    {
        DEBUG_LOG("Creature stopped attacking due to target out run him [guid=%u]", m_creature->GetGUIDLow());
        //i_state = STATE_LOOK_AT_VICTIM;
        //i_tracker.Reset(TIME_INTERVAL_LOOK);
    }

    if(!m_creature->isCharmed())
    {
        m_creature->RemoveAllAuras();

        // Remove TargetedMovementGenerator from MotionMaster stack list, and add HomeMovementGenerator instead
        if( m_creature->GetMotionMaster()->GetCurrentMovementGeneratorType() == TARGETED_MOTION_TYPE )
            m_creature->GetMotionMaster()->MoveTargetedHome();
    }

    m_creature->DeleteThreatList();
    i_victimGuid = 0;
    m_creature->CombatStop(true);
    m_creature->SetLootRecipient(NULL);
}

void
AggressorAI::UpdateAI(const uint32 /*diff*/)
{
    // update i_victimGuid if m_creature->getVictim() !=0 and changed
    if(!m_creature->SelectHostilTarget() || !m_creature->getVictim())
        return;

    i_victimGuid = m_creature->getVictim()->GetGUID();

    if( m_creature->isAttackReady() )
    {
        if( m_creature->IsWithinDistInMap(m_creature->getVictim(), ATTACK_DISTANCE))
        {
            m_creature->AttackerStateUpdate(m_creature->getVictim());
            m_creature->resetAttackTimer();
        }
    }
}

bool
AggressorAI::IsVisible(Unit *pl) const
{
    return m_creature->IsWithinDist(pl,sWorld.getConfig(CONFIG_SIGHT_MONSTER))
        && pl->isVisibleForOrDetect(m_creature,true);
}

void
AggressorAI::AttackStart(Unit *u)
{
    if( !u )
        return;

    if(m_creature->Attack(u,true))
    {
        //    DEBUG_LOG("Creature %s tagged a victim to kill [guid=%u]", m_creature->GetName(), u->GetGUIDLow());
        i_victimGuid = u->GetGUID();

        m_creature->AddThreat(u, 0.0f);
        m_creature->SetInCombatWith(u);
        u->SetInCombatWith(m_creature);

        m_creature->GetMotionMaster()->MoveChase(u);
    }
}