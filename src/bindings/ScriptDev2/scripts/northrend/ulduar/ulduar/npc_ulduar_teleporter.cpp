#include "precompiled.h"

#define BASE_CAMP    200
#define GROUNDS      201
#define FORGE        202
#define SCRAPYARD    203
#define ANTECHAMBER  204
#define WALKWAY      205
#define CONSERVATORY 206

bool GossipHello_npc_ulduar_teleporter(Player *player, Creature *creature)
{
    {
    player->ADD_GOSSIP_ITEM(0, "Перенестись в Главный лагерь экспедиции", GOSSIP_SENDER_MAIN, BASE_CAMP);
    player->ADD_GOSSIP_ITEM(0, "Перенестись на Плац", GOSSIP_SENDER_MAIN, GROUNDS);
    player->ADD_GOSSIP_ITEM(0, "Перенестись в Гигантскую кузню", GOSSIP_SENDER_MAIN, FORGE);
    player->ADD_GOSSIP_ITEM(0, "Перенестись на Мусорную свалку", GOSSIP_SENDER_MAIN, SCRAPYARD);
    player->ADD_GOSSIP_ITEM(0, "Перенестись в Вестибюль", GOSSIP_SENDER_MAIN, ANTECHAMBER);
    player->ADD_GOSSIP_ITEM(0, "Перенестись на Разрушенную аллею", GOSSIP_SENDER_MAIN, WALKWAY);
    player->ADD_GOSSIP_ITEM(0, "Перенестись в Консерваторию", GOSSIP_SENDER_MAIN, CONSERVATORY);
    }
    player->SEND_GOSSIP_MENU(DEFAULT_GOSSIP_MESSAGE, creature->GetGUID());

    return true;
}

bool GossipSelect_npc_ulduar_teleporter(Player *player, Creature *creature, uint32 sender, uint32 action)
{
    if(sender != GOSSIP_SENDER_MAIN) return true;
    if(!player->getAttackers().empty()) return true;

    switch(action)
    {
    case BASE_CAMP:
        player->TeleportTo(603, -706.122, -92.6024, 429.876, 0);
        player->CLOSE_GOSSIP_MENU(); break;
    case GROUNDS:
        player->TeleportTo(603, 131.248, -35.3802, 409.804, 0);
        player->CLOSE_GOSSIP_MENU(); break;
    case FORGE:
        player->TeleportTo(603, 553.233, -12.3247, 409.679, 0);
        player->CLOSE_GOSSIP_MENU(); break;
    case SCRAPYARD:
        player->TeleportTo(603, 926.292, -11.4635, 418.595, 0);
        player->CLOSE_GOSSIP_MENU(); break;
    case ANTECHAMBER:
        player->TeleportTo(603, 1498.09, -24.246, 420.967, 0);
        player->CLOSE_GOSSIP_MENU(); break;
    case WALKWAY:
        player->TeleportTo(603, 1859.45, -24.1, 448.9, 0); 
        player->CLOSE_GOSSIP_MENU(); break;
    case CONSERVATORY:
        player->TeleportTo(603, 2086.27, -24.3134, 421.239, 0);
        player->CLOSE_GOSSIP_MENU(); break;
    }

    return true;
}

void AddSC_npc_ulduar_teleporter()
{
    Script *newscript;
    newscript = new Script;
    newscript->Name = "npc_ulduar_teleporter";
    newscript->pGossipHello = &GossipHello_npc_ulduar_teleporter;
    newscript->pGossipSelect = &GossipSelect_npc_ulduar_teleporter;
    newscript->RegisterSelf();
}
