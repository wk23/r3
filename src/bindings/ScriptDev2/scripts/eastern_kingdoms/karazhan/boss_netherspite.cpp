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
SDName: Boss_Netherspite
SD%Complete: 0
SDComment: Place Holder
SDCategory: Karazhan
EndScriptData */

#include "precompiled.h"

enum
{
    EMOTE_PHASE_PORTAL          = -1532089,
    EMOTE_PHASE_BANISH          = -1532090,

    SPELL_NETHERBURN_AURA       = 30522,
    SPELL_VOIDZONE              = 37014,                    //Probably won't work
    SPELL_BERSERK               = 26662,
    SPELL_NETHERBREATH          = 36631,

    //Beams (no idea how these are going to work in Mangos)
    SPELL_DOMINANCE_ENEMY       = 30423,
    SPELL_DOMINANCE_SELF        = 30468,
    SPELL_PERSEVERANCE_ENEMY    = 30421,
    SPELL_PERSEVERANCE_SELF     = 30466,
    SPELL_SERENITY_ENEMY        = 30422,
    SPELL_SERENITY_SELF         = 30467
};
