/* Copyright (C) 2006 - 2010 ScriptDev2 <https://scriptdev2.svn.sourceforge.net/>
 * This program is free software licensed under GPL version 2
 * Please see the included DOCS/LICENSE.TXT for more information */

/* Originally based on BroodWyrm scripts. Modified by danbst*/

#ifndef DEF_NAXXRAMAS_H
#define DEF_NAXXRAMAS_H

//GO system
enum
{
    GO_DOOR_STATE_OPEN = 0,
    GO_DOOR_STATE_CLOSE = 1
};

//Encounters
#define ENCOUNTERS         15
enum
{
    //Spiderwing
    ENCOUNT_ANUBREKHAN   = 0,
    ENCOUNT_FAERLINA     = 1,
    ENCOUNT_MAEXXNA      = 2,

    //Abomination Wing
    ENCOUNT_PATCHWERK    = 3,
    ENCOUNT_GROBBULUS    = 4,
    ENCOUNT_GLUTH        = 5,
    ENCOUNT_THADDIUS     = 6,

    //Deathknight Wing
    ENCOUNT_RAZUVIOUS    = 7,
    ENCOUNT_GOTHIK       = 8,
    ENCOUNT_FOURHORSEMAN = 9,

    //Plague Wing
    ENCOUNT_NOTH         = 10,
    ENCOUNT_HEIGAN       = 11,
    ENCOUNT_LOATHEB      = 12,

    //Frostwyrm Lair
    ENCOUNT_SAPPHIRON    = 13,
    ENCOUNT_KELTHUZAD    = 14
};

//Bosses
enum
{
    //Spiderwing
    GUID_ANUBREKHAN   = 100,
    GUID_FAERLINA     = 101,
    GUID_MAEXXNA      = 102,
    GUID_WORSHIPPER1  = 20,
    GUID_WORSHIPPER2  = 21,
    GUID_WORSHIPPER3  = 22,
    GUID_WORSHIPPER4  = 23,

    //Abomination Wing
    GUID_PATCHWERK    = 103,
    GUID_GROBBULUS    = 104,
    GUID_GLUTH        = 105,
    GUID_THADDIUS     = 600,
    GUID_FEUGEN       = 601,
    GUID_STALAGG      = 602,

    //Deathknight Wing
    GUID_RAZUVIOUS    = 107,
    GUID_GOTHIK       = 108,
    GUID_KORTHAZZ     = 901,
    GUID_BLAUMEUX     = 902,
    GUID_RIVENDARE    = 903,
    GUID_ZELIEK       = 904,

    //Plague Wing
    GUID_NOTH          = 110,
    GUID_HEIGAN       = 111,
    GUID_LOATHEB      = 112,

    //Frostwyrm Lair
    GUID_SAPPHIRON    = 113,
    GUID_KELTHUZAD    = 114
};

//Doors and other gameobject
enum
{
    //Spiderwing
    GO_ANUBREKHAN_DOOR  = 1000,
    GO_ANUBREKHAN_GATE  = 1001,
    GO_FAERLINA_DOOR    = 1002,
    GO_FAERLINA_WEB     = 1003,
    GO_MAEXXNA_OUTERWEB = 1004,
    GO_MAEXXNA_INNERWEB = 1005,
    GO_SPIDERWING_EYE_BOSS = 1006,
    GO_SPIDERWING_EYE_RAMP = 1007,
    GO_SPIDERWING_PORTAL   = 1008,
    
    //Abomination Wing
    GO_PATCHWORK_EXIT   = 2000,
    GO_GLUTH_EXIT       = 2001,
    GO_THADDIUS_DOOR    = 2002,

    //Plague Wing
    GO_NOTH_ENTRY       = 3000,
    GO_NOTH_EXIT        = 3001,
    GO_HEIGAN_ENTRY     = 3002,
    GO_HEIGAN_EXIT      = 3003,
    GO_HEIGAN_EXITGATE  = 3004,
    GO_LOATHEB_DOOR     = 3005,

    //Deathknight Wing
    GO_VACCUUM_ENTER      = 4000,
    GO_VACCUUM_EXIT       = 4001,
    GO_VACCUUM_COMBAT     = 4002,
    GO_DEATHKNIGHT_DOOR   = 4003,
    GO_FOURHORSEMEN_CHEST = 4004,
    
    //Frostwyrm Lair
    GO_FROSTWYRM_WATERFALL  = 5000,
    GO_KELTHUZAD_DOOR       = 5001,
	GO_SAPPHIRON_BIRTH      = 5002
};

#endif