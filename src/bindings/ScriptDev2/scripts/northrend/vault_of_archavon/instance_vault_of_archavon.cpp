
#include "precompiled.h"


struct MANGOS_DLL_DECL instance_vault_of_archavon : public ScriptedInstance
{
    instance_vault_of_archavon(Map* pMap) : ScriptedInstance(pMap)
    {
        Initialize();
    };

    uint64 mTempestMinionGUID[4];
    uint64 mEmalonGUID;
    int minion;

    void Initialize()
    {
        minion = 0;
        mTempestMinionGUID[0] = 0;
        mTempestMinionGUID[1] = 0;
        mTempestMinionGUID[2] = 0;
        mTempestMinionGUID[3] = 0;
        mEmalonGUID = 0;
    }

    void OnCreatureCreate(Creature* pCreature)
    {
        if(pCreature->GetEntry() == 33998)
        {
            mTempestMinionGUID[minion] = pCreature->GetGUID();
            minion++;
            if(minion==4) minion=0;
        }

        if(pCreature->GetEntry() == 33993)
            mEmalonGUID = pCreature->GetGUID();
    }

    void OnObjectCreate(GameObject* pGo)
    {
    }

    void SetData(uint32 uiType, uint32 uiData)
    {
    }

    uint32 GetData(uint32 uiType)
    {
        return 0;
    }

    uint64 GetData64(uint32 uiData)
    {
        switch(uiData)
        {
            case 0: return mTempestMinionGUID[0];
            case 1: return mTempestMinionGUID[1];
            case 2: return mTempestMinionGUID[2];
            case 3: return mTempestMinionGUID[3];
            case 4: return mEmalonGUID;
            default: return 0;
        }
    }
};

InstanceData* GetInstanceData_instance_vault_of_archavon(Map* pMap)
{
    return new instance_vault_of_archavon(pMap);
}

void AddSC_instance_vault_of_archavon()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "instance_vault_of_archavon";
    newscript->GetInstanceData = &GetInstanceData_instance_vault_of_archavon;
    newscript->RegisterSelf();
}
