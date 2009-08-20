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
SDName: Instance_Naxxramas
SD%Complete: 15%
SDComment: Spider Wing GOs implemented
SDCategory: Naxxramas
EndScriptData */

/* Originally based on BroodWyrm scripts. Modified by danbst*/

#include "precompiled.h"
#include "def_naxxramas.h"

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

struct MANGOS_DLL_DECL instance_naxxramas : public ScriptedInstance
{
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

    uint64 guid_anubrekhan;
    uint64 guid_faerlina;
    uint64 guid_maexxna;

    uint32 Encounters[ENCOUNTERS];

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
    void CloseDoor(notDirectGO &GO) { Close(GO); Enable(GO); }
    void OpenDoor(notDirectGO &GO) { Open(GO); Disable(GO); }
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
        guid_anubrekhan    = 0;
        guid_faerlina      = 0;
        guid_maexxna       = 0;   

        for(uint8 i = 0; i < ENCOUNTERS; i++)
            Encounters[i] = NOT_STARTED;
    }

    //Currently we will check bosses only for Spider Wing
    void OnCreatureCreate(Creature *creature, uint32 creature_entry)
    {
        switch(creature_entry)
        {
            //Spiderwing ------------------------------------
            case 15953: guid_faerlina   = creature->GetGUID();   break;
            case 15956: guid_anubrekhan = creature->GetGUID();   break;
            case 15952: guid_maexxna    = creature->GetGUID();   break;
        }
    }

    //Currently we will check GOs only for Spider Wing
    void OnObjectCreate(GameObject *go)
    {
        switch(go->GetEntry())
        {
            //Spiderwing ------------------------------------
            case 181126: go_anubrekhan_door. Init(go);           break;
            case 181195: go_anubrekhan_gate.Init(go);            break;
            case 181167: go_faerlina_door.Init(go);              break;
            case 181235: go_faerlina_web.Init(go);               break;
            case 181209: go_maexxna_outerweb.Init(go);           break;
            case 181197: go_maexxna_innerweb.Init(go);           break;
            case 181233: go_spiderwing_eye_boss.Init(go);        break;
            case 181212: go_spiderwing_eye_ramp.Init(go);        break;
            case 181577: go_spiderwing_portal.Init(go);          break;
        }
    }

    uint64 GetData64(uint32 identifier)
    {
        switch (identifier)
        {
            //Spiderwing -------------------------------------
            case GUID_ANUBREKHAN: return guid_anubrekhan;        break;
            case GUID_FAERLINA:   return guid_faerlina;          break;
            case GUID_MAEXXNA:    return guid_maexxna;           break;
            default:
                return 0;
        }
    }

    void SetData(uint32 type, uint32 data)
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
        }

        if (data == DONE)
        {
            OUT_SAVE_INST_DATA;

            std::ostringstream saveStream;
            saveStream << Encounters[0] << " " << Encounters[1] << " " << Encounters[2];

            str_data = saveStream.str();

            SaveToDB();
            OUT_SAVE_INST_DATA_COMPLETE;
        }
    }

    uint32 GetData(uint32 type)
    {
        switch (type)
        {
            //Spiderwing ------------------------------------
            case ENCOUNT_ANUBREKHAN:    return Encounters[0]; break;
            case ENCOUNT_FAERLINA:      return Encounters[1]; break;
            case ENCOUNT_MAEXXNA:       return Encounters[2]; break;

            default: return 0;
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

        std::istringstream loadStream(in);
        loadStream >> Encounters[0] >> Encounters[1] >> Encounters[2];
        for(uint32 i = 0; i < ENCOUNTERS; i++)
        {
            if (Encounters[i] == IN_PROGRESS)               // Do not load an encounter as "In Progress" - reset it instead.
                Encounters[i] = NOT_STARTED;
            SetData(i,Encounters[i]);
        }
        OUT_LOAD_INST_DATA_COMPLETE;
    }
};

InstanceData* GetInstanceData_naxxramas(Map* map)
{
    return new instance_naxxramas(map);
}

void AddSC_instance_naxxramas()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_naxxramas";
    newscript->GetInstanceData = &GetInstanceData_naxxramas;
    newscript->RegisterSelf();
}
