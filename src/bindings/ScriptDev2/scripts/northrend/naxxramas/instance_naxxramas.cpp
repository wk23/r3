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
SDName: Instance_Naxxramas
SD%Complete: 15%
SDComment: Spider Wing GOs implemented
SDCategory: Naxxramas
EndScriptData */

/* Originally based on BroodWyrm scripts. Modified by danbst*/

#include "precompiled.h"
#include "naxxramas.h"

/* Naxxramas encounters:
Boss Anub'Rekhan           ENCOUNT_ANUBREKHAN
Boss Grand Widow Faerlina  ENCOUNT_FAERLINA
Boss Maexxna               ENCOUNT_MAEXXNA
*/

// struct notDirectGO - used to handle gameobjects, that are not yet created
// How it works. If some function (Open() for example) tries to handle GO, it checks property ***tempGo***.
// if NULL - means object not created yet, so it has to set property ***set*** to false and define other properties (***state***, ***flagSet***, ***flagRemove***)
// Now, when object will be created by notDirectGO.Init() - the new state will be set
struct notDirectGO
{
    GameObject* tempGo;
    uint32 flagSet, flagRemove;
    uint8 lastFlagChange;
    GOState state;
    bool set;

    notDirectGO()
    {
        flagSet = flagRemove = 0;
        state = GO_STATE_ACTIVE;
        set = true;
        tempGo = NULL;
        lastFlagChange = 0;
    }
    void Init(GameObject *go)
    {
        tempGo = go;
        if (!set && go)
        {
            if (lastFlagChange == 0)
            {
                go->SetFlag(GAMEOBJECT_FLAGS,flagSet);
                go->RemoveFlag(GAMEOBJECT_FLAGS,flagRemove);
            }else
            {
                go->RemoveFlag(GAMEOBJECT_FLAGS,flagRemove);
                go->SetFlag(GAMEOBJECT_FLAGS,flagSet);
            }
            go->SetGoState(state);
            set = true;
        }
    }
};

class MANGOS_DLL_DECL instance_naxxramas : public ScriptedInstance
{
public:
    instance_naxxramas(Map *Map) : ScriptedInstance(Map) {Initialize();};

    std::string str_data;

    notDirectGO go_anubrekhan_door;
    notDirectGO go_anubrekhan_gate;
    notDirectGO go_faerlina_door;
    notDirectGO go_faerlina_web;
    notDirectGO go_maexxna_outerweb;
    notDirectGO go_maexxna_innerweb;
    notDirectGO go_spiderwing_eye_boss;
    notDirectGO go_spiderwing_eye_ramp;
    notDirectGO go_spiderwing_portal;

    notDirectGO go_noth_door;
    notDirectGO go_noth_exit;
    notDirectGO go_heigan_entry;
    notDirectGO go_heigan_outerdoor;
    notDirectGO go_heigan_exitgate;
    notDirectGO go_loatheb_door;
    notDirectGO go_plaguewing_eye_boss;
    notDirectGO go_plaguewing_eye_ramp;
    notDirectGO go_plaguewing_portal;

    notDirectGO go_grobblulus_door;
    notDirectGO go_gluth_door;
    notDirectGO go_thaddius_door;
    notDirectGO go_abom_eye_boss;
    notDirectGO go_abom_eye_ramp;
    notDirectGO go_naxx_portal;

    notDirectGO go_vaccuum_combat_gate;
    notDirectGO go_vaccuum_exit_gate;
    notDirectGO go_vaccuum_enter_gate;

    notDirectGO go_horsemans_door;
    notDirectGO go_sapphiron_birth;
    notDirectGO go_kelthuzad_door;
    notDirectGO go_sapphiron_door;

    uint64 guid_anubrekhan;
    uint64 guid_faerlina;
    uint64 guid_maexxna;
    uint64 guid_heigan;
    uint64 m_uiworshipper1GUID;
    uint64 m_uiworshipper2GUID;
    uint64 m_uiworshipper3GUID;
    uint64 m_uiworshipper4GUID;
    uint8 m_uiworshipper;
    uint64 guid_stalagg;
    uint64 guid_feugen;
    uint64 guid_thaddius;
    uint64 guid_loatheb;

    uint64 m_uiZeliekGUID;
    uint64 m_uiKorthazzGUID;
    uint64 m_uiBlaumeuxGUID;
    uint64 m_uiRivendareGUID;

    EncounterData<ENCOUNTERS> Encounters;

/****  Door System - need review ****/
    //Open/Close or Show/Hide everything that has two states
    void Open(notDirectGO &GO)
    {
        GO.state = GO_STATE_ACTIVE;
        GO.set = GO.tempGo ? true : false;
        if(GO.tempGo)
            GO.tempGo->SetGoState(GO_STATE_ACTIVE);
    }
    void Close(notDirectGO &GO)
    {
        GO.state = GO_STATE_READY;
        GO.set = GO.tempGo ? true : false;
        if(GO.tempGo)
            GO.tempGo->SetGoState(GO_STATE_READY);
    }
    void CloseDoor(notDirectGO &GO) { Close(GO); Enable(GO);}
    void OpenDoor(notDirectGO &GO) { Open(GO); Disable(GO);}
    //Enable/Disable GO (for interaction)
    void Disable(notDirectGO &GO)
    {
        GO.flagSet = GO_FLAG_INTERACT_COND | GO_FLAG_IN_USE;
        GO.lastFlagChange = 1;
        GO.set = GO.tempGo ? true : false;
        if (GO.tempGo)
            GO.tempGo->SetFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND | GO_FLAG_IN_USE);
    }
    void Enable(notDirectGO &GO)
    {
        GO.flagRemove = GO_FLAG_INTERACT_COND | GO_FLAG_IN_USE;
        GO.lastFlagChange = 0;
        GO.set = GO.tempGo ? true : false;
        if (GO.tempGo)
            GO.tempGo->RemoveFlag(GAMEOBJECT_FLAGS, GO_FLAG_INTERACT_COND | GO_FLAG_IN_USE);
    }
//////   End door system   //////

    void Initialize()
    {
        guid_anubrekhan      = 0;
        guid_faerlina        = 0;
        guid_maexxna         = 0;
        m_uiworshipper1GUID  = 0;
        m_uiworshipper2GUID  = 0;
        m_uiworshipper3GUID  = 0;
        m_uiworshipper4GUID  = 0;
        m_uiworshipper       = 0;
        guid_heigan          = 0;
        guid_stalagg         = 0;
        guid_feugen          = 0;
        guid_thaddius        = 0;
        guid_loatheb         = 0;
        m_uiZeliekGUID       = 0;
        m_uiKorthazzGUID     = 0;
        m_uiBlaumeuxGUID     = 0;
        m_uiRivendareGUID    = 0;
    }

    bool IsEncounterInProgress() const
    {
        for(uint8 i = 0; i < Encounters.size(); ++i)
            if (Encounters[i] == IN_PROGRESS)
                return true;
        return false;
    }

    //Currently we will check bosses only for Spider Wing
    void OnCreatureCreate(Creature *creature)
    {
        switch(creature->GetEntry())
        {
            //Spiderwing ------------------------------------
            case 15953: guid_faerlina   = creature->GetGUID();   break;
            case 15956: guid_anubrekhan = creature->GetGUID();   break;
            case 15952: guid_maexxna    = creature->GetGUID();   break;
            case 15936: guid_heigan     = creature->GetGUID();   break;
            case 15929: guid_stalagg    = creature->GetGUID();   break;
            case 15930: guid_feugen     = creature->GetGUID();   break;
            case 15928: guid_thaddius   = creature->GetGUID();   break;
            case 16011: guid_loatheb    = creature->GetGUID();   break;

            case 16064: m_uiKorthazzGUID  = creature->GetGUID();   break;
            case 16065: m_uiBlaumeuxGUID  = creature->GetGUID();   break;
            case 30549: m_uiRivendareGUID = creature->GetGUID();   break;
            case 16063: m_uiZeliekGUID    = creature->GetGUID();   break;

            case 16506:
                ++m_uiworshipper;
                switch (m_uiworshipper)
                {
                case 1:
                    m_uiworshipper1GUID = creature->GetGUID();
                    break;
                case 2:
                    m_uiworshipper2GUID = creature->GetGUID();
                    break;
                case 3:
                    m_uiworshipper3GUID = creature->GetGUID();
                    break;
                case 4:
                    m_uiworshipper4GUID = creature->GetGUID();
                    break;
                case 5:
                    m_uiworshipper = 0;
                    break;
                }
                break;
        }
    }


    //Currently we will check GOs only for Spider Wing
    void OnObjectCreate(GameObject *go)
    {
        switch(go->GetEntry())
        {
            //Spiderwing ------------------------------------
            case 181126: go_anubrekhan_door.Init(go);           break;
            case 181195: go_anubrekhan_gate.Init(go);            break;
            case 194022: go_faerlina_door.Init(go);              break;
            case 181235: go_faerlina_web.Init(go);               break;
            case 181209: go_maexxna_outerweb.Init(go);           break;
            case 181197: go_maexxna_innerweb.Init(go);           break;
            case 181233: go_spiderwing_eye_boss.Init(go);        break;
            case 181212: go_spiderwing_eye_ramp.Init(go);        break;
            case 181575: go_spiderwing_portal.Init(go);          break;
            case 181200: go_noth_door.Init(go);                  break;
            case 181201: go_noth_exit.Init(go);                  break;
            case 181202: go_heigan_entry.Init(go);               break;
            case 181203: go_heigan_outerdoor.Init(go);           break;
            case 181496: go_heigan_exitgate.Init(go);            break;
            case 181241: go_loatheb_door.Init(go);               break;
            case 181231: go_plaguewing_eye_boss.Init(go);        break;
            case 181211: go_plaguewing_eye_ramp.Init(go);        break;
            case 181577: go_plaguewing_portal.Init(go);          break;
            case 181123: go_grobblulus_door.Init(go);            break;
            case 181120: go_gluth_door.Init(go);                 break;
            case 181121: go_thaddius_door.Init(go);              break;
            case 181213: go_abom_eye_ramp.Init(go);              break;
            case 181576: go_naxx_portal.Init(go);                break;
            case 181232: go_abom_eye_boss.Init(go);              break;
            case 181170: go_vaccuum_combat_gate.Init(go);        break;
            case 181124: go_vaccuum_enter_gate.Init(go);         break;
            case 181125: go_vaccuum_exit_gate.Init(go);          break;
            case 181119: go_horsemans_door.Init(go);             break;
			case 181356: go_sapphiron_birth.Init(go);            break;
            case 181228: go_kelthuzad_door.Init(go);             break;
            case 181225: go_sapphiron_door.Init(go);             break;
        }
    }

    uint64 GetData64(uint32 identifier)
    {
        switch (identifier)
        {
            //Spiderwing -------------------------------------
            case GUID_ANUBREKHAN:  return guid_anubrekhan;       break;
            case GUID_FAERLINA:    return guid_faerlina;         break;
            case GUID_MAEXXNA:     return guid_maexxna;          break;
            case GUID_WORSHIPPER1: return m_uiworshipper1GUID;   break;
            case GUID_WORSHIPPER2: return m_uiworshipper2GUID;   break;
            case GUID_WORSHIPPER3: return m_uiworshipper3GUID;   break;
            case GUID_WORSHIPPER4: return m_uiworshipper4GUID;   break;
            case GUID_HEIGAN:      return guid_heigan;           break;
            case GUID_STALAGG:     return guid_stalagg;          break;
            case GUID_FEUGEN:      return guid_feugen;           break;
            case GUID_THADDIUS:    return guid_thaddius;         break;
            case GUID_LOATHEB:     return guid_loatheb;          break;
            case GUID_ZELIEK:      return m_uiZeliekGUID;        break;
            case GUID_KORTHAZZ:    return m_uiKorthazzGUID;      break;
            case GUID_BLAUMEUX:    return m_uiBlaumeuxGUID;      break;
            case GUID_RIVENDARE:   return m_uiRivendareGUID;     break;
            default:
                return 0;
        }
    }

    void SetEncounterData(uint32 type, uint32 data) 
    {
       switch (type)
        {
            //Spiderwing ------------------------------------
            case ENCOUNT_ANUBREKHAN:
                Encounters[ENCOUNT_ANUBREKHAN] = data;
                switch (data)
                {
                    case NOT_STARTED:
                        CloseDoor(go_anubrekhan_gate);
                        OpenDoor(go_anubrekhan_door);
                        Close(go_maexxna_outerweb);
                        break;
                    case IN_PROGRESS:
                        CloseDoor(go_anubrekhan_door);
                        break;
                    case DONE:
                        OpenDoor(go_anubrekhan_gate);
                        OpenDoor(go_anubrekhan_door);
                        if(Encounters[ENCOUNT_FAERLINA] == DONE)   //Impossible, but check
                            Open(go_maexxna_outerweb);
                        break;
                }
                break;
            case ENCOUNT_FAERLINA:
                Encounters[ENCOUNT_FAERLINA] = data;
                switch (data)
                {
                    case NOT_STARTED:
                        CloseDoor(go_faerlina_door);
                        Open(go_faerlina_web);
                        Close(go_maexxna_outerweb);
                        break;
                    case IN_PROGRESS:
                        Close(go_faerlina_web);
                        break;
                    case DONE:
                        OpenDoor(go_faerlina_door);
                        Open(go_faerlina_web);
                        if(Encounters[ENCOUNT_ANUBREKHAN] == DONE)
                            Open(go_maexxna_outerweb);
                        break;
                }
                break;
            case ENCOUNT_MAEXXNA:
                Encounters[ENCOUNT_MAEXXNA] = data;
                switch (data)
                {
                    case NOT_STARTED:
                        Open(go_maexxna_innerweb);
                        Disable(go_spiderwing_portal);
                        Close(go_spiderwing_eye_boss);
                        Close(go_spiderwing_eye_ramp);
                        break;
                    case IN_PROGRESS:
                        Close(go_maexxna_innerweb);
                        break;
                    case DONE:
                        Open(go_maexxna_innerweb);
                        Open(go_spiderwing_eye_boss);
                        Open(go_spiderwing_eye_ramp);
                        Enable(go_spiderwing_portal);
                        break;
                }
                break;
                //Plaguewing ------------------------------------
            case ENCOUNT_NOTH:
                Encounters[ENCOUNT_NOTH] = data;
                switch(data)
                {
                case NOT_STARTED:
                    Open(go_noth_door);
                    Close(go_noth_exit);
                    break;
                case IN_PROGRESS:
                    Close(go_noth_door);
                    break;
                case DONE:
                    Open(go_noth_door);
                    Open(go_noth_exit);
                    break;
                }
                break;
            case ENCOUNT_HEIGAN:
                Encounters[ENCOUNT_HEIGAN] = data;
                switch(data)
                {
                    case NOT_STARTED:
                        Open(go_heigan_entry);
                        Close(go_heigan_outerdoor);
                        Close(go_heigan_exitgate);
                        break;
                    case IN_PROGRESS:
                        Close(go_heigan_entry);
                        break;
                    case DONE:
                        Open(go_heigan_entry);
                        Open(go_heigan_outerdoor);
                        Open(go_heigan_exitgate);
                        break;
                }
                break;
            case ENCOUNT_LOATHEB:
                Encounters[ENCOUNT_LOATHEB] = data;
                switch (data)
                {
                case NOT_STARTED:
                    Open(go_loatheb_door);
                    Disable(go_plaguewing_portal);
                    Close(go_plaguewing_eye_boss);
                    Close(go_plaguewing_eye_ramp);
                    break;
                case IN_PROGRESS:
                    Close(go_loatheb_door);
                    break;
                case DONE:
                    Open(go_loatheb_door);
                    Open(go_plaguewing_eye_boss);
                    Open(go_plaguewing_eye_ramp);
                    Enable(go_plaguewing_portal);
                    break;
                }
                break;
            case ENCOUNT_PATCHWERK:
                Encounters[ENCOUNT_PATCHWERK] = data;
                switch(data)
                {
                case NOT_STARTED:
                    //Close(go_grobblulus_door);
                    break;
                case IN_PROGRESS:
                    break;
                case DONE:
                    Open(go_grobblulus_door);
                    break;
                }
                break;
            case ENCOUNT_GROBBULUS:
                Encounters[ENCOUNT_GROBBULUS] = data;
                switch(data)
                {
                case NOT_STARTED:
                    Open(go_grobblulus_door);
                    break;
                case IN_PROGRESS:
                    Close(go_grobblulus_door);
                    break;
                case DONE:
                    Open(go_grobblulus_door);
                    break;
                }
                break;
            case ENCOUNT_GLUTH:
                Encounters[ENCOUNT_GLUTH] = data;
                switch(data)
                {
                case NOT_STARTED:
                    Close(go_gluth_door);
                    Close(go_thaddius_door);
                    break;
                case DONE:
                    Open(go_gluth_door);
                    Open(go_thaddius_door);
                    break;
                }
                break;
            case ENCOUNT_THADDIUS:
                Encounters[ENCOUNT_THADDIUS] = data;
                switch(data)
                {
                case NOT_STARTED:
                    {
                        Disable(go_naxx_portal);
                        Close(go_abom_eye_boss);
                        Close(go_abom_eye_ramp);
                        if (GetData(ENCOUNT_GLUTH) == 3)
                            Open(go_thaddius_door);
                        break;
                    }
                case IN_PROGRESS:
                    Close(go_thaddius_door);
                    break;
                case DONE:
                    Open(go_abom_eye_boss);
                    Open(go_abom_eye_ramp);
                    Enable(go_naxx_portal);
                    Open(go_thaddius_door);
                    break;
                }
                break;
            case ENCOUNT_GOTHIK:
                Encounters[ENCOUNT_GOTHIK] = data;
                switch(data)
                {
                case NOT_STARTED:
                    Open(go_vaccuum_enter_gate);
                    Open(go_vaccuum_combat_gate);
                    Close(go_vaccuum_exit_gate);
                    break;
                case IN_PROGRESS:
                    Close(go_vaccuum_enter_gate);
                    Close(go_vaccuum_combat_gate);
                    break;
                case DONE:
                    Open(go_vaccuum_combat_gate);
                    Open(go_vaccuum_enter_gate);
                    Open(go_vaccuum_exit_gate);
                    break;
                }
                break;
            case ENCOUNT_FOURHORSEMAN:
                Encounters[ENCOUNT_FOURHORSEMAN] = data;
                switch(data)
                {
                case NOT_STARTED:
                    Open(go_horsemans_door);
                    break;
                case IN_PROGRESS:
                    Close(go_horsemans_door);
                    break;
                case DONE:
                    Open(go_horsemans_door);
                    break;
                }
                break;
            case ENCOUNT_SAPPHIRON:
                Encounters[ENCOUNT_SAPPHIRON] = data;
                switch(data)
                {
                case NOT_STARTED:
                    Close(go_sapphiron_door);
                    Close(go_kelthuzad_door);
                    break;
                case IN_PROGRESS:
                    break;
                case DONE:
                    Open(go_sapphiron_door);
                    Open(go_kelthuzad_door);
                    break;
                }
                break;
            case ENCOUNT_KELTHUZAD:
                Encounters[ENCOUNT_KELTHUZAD] = data;
                switch(data)
                {
                case NOT_STARTED:
                    if (GetData(ENCOUNT_SAPPHIRON) == DONE)
                        Open(go_kelthuzad_door);
                    break;
                case IN_PROGRESS:
                    Close(go_kelthuzad_door);
                    break;
                case DONE:
                    Open(go_kelthuzad_door);
                    break;
                }
                break;
            }
    }

    void SetData(uint32 type, uint32 data)
    {
        SetEncounterData(type, data);
 
        if (data == DONE)
        {
            OUT_SAVE_INST_DATA;

            str_data = Encounters.dumps();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    uint32 GetData(uint32 type)
    {
        try {
            return Encounters[type];
        } catch (EncounterIndexError) {
            return 0;
        }
    }
    
    const char* Save()
    {
        return str_data.c_str();
    }

    void Load(const char* in)
    {
        if (!in)
        {
            OUT_LOAD_INST_DATA_FAIL;
            return;
        }

        OUT_LOAD_INST_DATA(in);

        Encounters.loads(in);

        for(uint32 i = 0; i < ENCOUNTERS; i++)
        {
            if (Encounters[i] == IN_PROGRESS)               // Do not load an encounter as "In Progress" - reset it instead.
                Encounters[i] = NOT_STARTED;
            SetEncounterData(i,Encounters[i]);
        }
        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_naxxramas(Map* map)
{
    return new instance_naxxramas(map);
}

bool AreaTrigger_at_frostwyrm_lair(Player* pPlayer, AreaTriggerEntry* pAt)
{
    if (ScriptedInstance* pInstance = (ScriptedInstance*)pPlayer->GetInstanceData())
    {
        if (pInstance->GetData(ENCOUNT_MAEXXNA) != DONE || pInstance->GetData(ENCOUNT_THADDIUS) != DONE ||
            pInstance->GetData(ENCOUNT_FOURHORSEMAN) != DONE || pInstance->GetData(ENCOUNT_LOATHEB) != DONE)
            return true;

        return false;
    }
    return false;
}

void AddSC_instance_naxxramas()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_naxxramas";
    newscript->GetInstanceData = &GetInstanceData_naxxramas;
    newscript->RegisterSelf();

    newscript = new Script;
    newscript->Name = "at_frostwyrm_lair";
    newscript->pAreaTrigger = &AreaTrigger_at_frostwyrm_lair;
    newscript->RegisterSelf();
}