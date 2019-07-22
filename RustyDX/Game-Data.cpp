#include "Game-Data.h"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

std::vector<Player> Game_Data::_Player_Vector = std::vector<Player>();
std::vector<Player> Game_Data::_Obj_Vector = std::vector<Player>();
ULONG64 Game_Data::_Camera_Addr = 0;
Player Game_Data::localplayer = Player();

bool Game_Data::Hook_Start()
{
	_Memory->Init();
	_Rusty_Addr = Memory::_This->Get_Model_Addr();

	return true;
}

int get_data();
int get_Active_List();
std::vector<Player> Game_Data::Get_Data()
{
	CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)get_data, NULL, NULL, NULL);
	//CreateThread(NULL, NULL, (LPTHREAD_START_ROUTINE)get_Active_List, NULL, NULL, NULL); TODO: fix this
	return _Player_Vector;
}

ULONG64 _Test_ULONG64 = 0;
INT16 _Test_INT16 = 0;
INT _Test_INT = 0;

int get_Active_List()
{
	std::vector<Player> _Player_Vector_Temp;
	ULONG64 ObjectManager = Memory::_This->Read_Memory(Memory::_This->Base + RUST_OBJECTMANAGER, _Test_ULONG64);
	ULONG64 activeObjectsPtr = Memory::_This->Read_Memory(ObjectManager + OBJECTMANAGER_ACTIVELIST, _Test_ULONG64);
	ULONG64 firstObject = Memory::_This->Read_Memory(ObjectManager + OBJECTMANAGER_ACTIVELISTLAST, _Test_ULONG64);

	int cycle = 0;

	while (firstObject != activeObjectsPtr && cycle < 200000 && activeObjectsPtr)
	{
		cycle++;
		if (cycle % 1000 == 0)
			printf("cycle %i\n", cycle);
		ULONG64 gameObj = Memory::_This->Read_Memory(activeObjectsPtr + BASEENT_GAMEOBJECT, _Test_ULONG64);
		ULONG64 objectNamePtr = Memory::_This->Read_Memory(gameObj + GAMEOBJECT_OBJECTNAMEPTR, _Test_ULONG64);

		char world_name_char[100] = { 0 };
		Memory::_This->Read_Memory(objectNamePtr, world_name_char, 100);

		//printf(world_name_char);
		//printf("\n");

		if (strstr(world_name_char, "metal-ore") ||
			strstr(world_name_char, "metal-collectable") ||
			strstr(world_name_char, "stone-ore") ||
			strstr(world_name_char, "stone-collectable") ||
			strstr(world_name_char, "tree_stump") ||
			strstr(world_name_char, "supply_drop") ||
			strstr(world_name_char, "cupboard.tool.deployed") ||
			strstr(world_name_char, "stash") ||
			strstr(world_name_char, "trash-pile") ||
			strstr(world_name_char, "loot_barrel") ||
			strstr(world_name_char, "hemp_mature") ||
			strstr(world_name_char, "hemp-collectable.prefab") ||
			strstr(world_name_char, "hemp.entity.prefab") ||
			strstr(world_name_char, "crate_normal") ||
			strstr(world_name_char, "crate_tools") ||
			strstr(world_name_char, "pumpkin.entity") ||
			strstr(world_name_char, "woodbox") ||
			strstr(world_name_char, "corn.entity") ||
			strstr(world_name_char, "box.wooden.large"))
		{
			printf("found obj\n\n\n\n\n");
			Player ent = Player();
			ent.BaseObject = activeObjectsPtr;
			ent.GameObject = gameObj;
			ent.name = world_name_char;
			ULONG64 objClass = Memory::_This->Read_Memory(ent.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
			ULONG64 transform = Memory::_This->Read_Memory(objClass + OBJECTCLASS_TRANSFORM, _Test_ULONG64);
			ent.VisualState = Memory::_This->Read_Memory(transform + TRANSFORM_VISUALSTATE, _Test_ULONG64);
			_Player_Vector_Temp.push_back(ent);
		}

		activeObjectsPtr = Memory::_This->Read_Memory(activeObjectsPtr + TAGGEDLIST_NEXTTAGGEDENT, _Test_ULONG64);
		firstObject = Memory::_This->Read_Memory(ObjectManager + OBJECTMANAGER_ACTIVELISTLAST, _Test_ULONG64);
	}
	Game_Data::_Obj_Vector = _Player_Vector_Temp;
	return 0;
}

int get_data()
{
	std::vector<Player> _Player_Vector_Temp;

	ULONG64 ObjectManager = Memory::_This->Read_Memory(Memory::_This->Base + RUST_OBJECTMANAGER, _Test_ULONG64);
	ULONG64 taggedObjectsPtr = Memory::_This->Read_Memory(ObjectManager + OBJECTMANAGER_TAGGEDLIST, _Test_ULONG64);
	ULONG64 firstObject = Memory::_This->Read_Memory(ObjectManager + OBJECTMANAGER_TAGGEDLISTFIRST, _Test_ULONG64);

	//walk the list
	ULONG64 cycle = 0;
	while (firstObject != taggedObjectsPtr)
	{
		cycle++;
		Player currentPlayer = Player();
		currentPlayer.BaseObject = taggedObjectsPtr;
		currentPlayer.GameObject = Memory::_This->Read_Memory(taggedObjectsPtr + BASEENT_GAMEOBJECT, _Test_ULONG64);
		currentPlayer.tag = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_TAG, _Test_INT16);

		if (currentPlayer.tag == 5 || /*currentPlayer.tag == 20009 ||*/ currentPlayer.tag == 6)
		{
			//printf("\n gogogo %i \n", currentPlayer.tag);

			if (currentPlayer.tag == 0)
			{
				//you fucked up somewhere
				//printf("\nwalk list failed... tag 0 \n");
				break;
			}
			if (currentPlayer.tag == 5)
			{
				//mainCamera
				ULONG64 objClass = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
				ULONG64 entity = Memory::_This->Read_Memory(objClass + OBJECTCLASS_ENTITYPTR, _Test_ULONG64);
				ULONG64 transform = Memory::_This->Read_Memory(objClass + OBJECTCLASS_TRANSFORM, _Test_ULONG64);

				Game_Data::_Camera_Addr = entity;

				currentPlayer.VisualState = Memory::_This->Read_Memory(transform + TRANSFORM_VISUALSTATE, _Test_ULONG64);
			}
			if (currentPlayer.tag == 20009)
			{
				//Corpse
				ULONG64 objClass = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
				//ULONG64 entity =Memory::_This->Read_Memory(objClass + OBJECTCLASS_ENTITYPTR, _Test_ULONG64);
				ULONG64 transform = Memory::_This->Read_Memory(objClass + OBJECTCLASS_TRANSFORM, _Test_ULONG64);

				currentPlayer.VisualState = Memory::_This->Read_Memory(transform + TRANSFORM_VISUALSTATE, _Test_ULONG64);
			}
			//if (currentPlayer.tag == 20011) probably unnecessary
			//{
			//	//SkyDome
			//	ULONG64 objClass =Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
			//	//ULONG64 entity =Memory::_This->Read_Memory(objClass + OBJECTCLASS_ENTITYPTR, _Test_ULONG64);
			//	ULONG64 transform =Memory::_This->Read_Memory(objClass + OBJECTCLASS_TRANSFORM, _Test_ULONG64);

			//	currentPlayer.VisualState =Memory::_This->Read_Memory(transform + TRANSFORM_VISUALSTATE, _Test_ULONG64);
			//}
			if (currentPlayer.tag == 6)
			{
				firstObject = Memory::_This->Read_Memory(ObjectManager, _Test_ULONG64);
				//Player
				ULONG64 objClass = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
				ULONG64 entity = Memory::_This->Read_Memory(objClass + OBJECTCLASS_ENTITYPTR, _Test_ULONG64);
				ULONG64 transform = Memory::_This->Read_Memory(objClass + OBJECTCLASS_TRANSFORM, _Test_ULONG64);

				//printf("\n before local model \n");

				//ULONG64 model =Memory::_This->Read_Memory(entity + ENTITYPTR_MODEL, _Test_ULONG64);
				//ULONG64 modelTransform =Memory::_This->Read_Memory(model + MODEL_TRANSFORM, _Test_ULONG64);
				//ULONG64 head =Memory::_This->Read_Memory(modelTransform + TRANSFORM_BONE + 47 * 8, _Test_ULONG64);
				//ULONG64 headWorld =Memory::_This->Read_Memory(head + BONE_WORLD, _Test_ULONG64);

				//printf("\n before bone func \n");
				//currentPlayer.head = get_bone_pos(headWorld);
				//printf("\n after bone func \n");
				currentPlayer.VisualState = Memory::_This->Read_Memory(transform + TRANSFORM_VISUALSTATE, _Test_ULONG64);

				ULONG64 objectNamePor = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTNAMEPTR, _Test_ULONG64);
				char world_name_char[100] = { 0 };
				Memory::_This->Read_Memory(objectNamePor, world_name_char, 50);
				//printf("The name is: %s at mem pos %llu\n", world_name_char, objectNamePor);
				if (strstr(world_name_char, "LocalPlayer"))
				{
					//other stuff
					//printf("\nlocal local at position %llu", cycle);
					Game_Data::localplayer = currentPlayer;
					currentPlayer.name = world_name_char;
					currentPlayer.objName = "Localplayer";
					ULONG64 base = Memory::_This->Read_Memory(entity + ENTITYPTR_BASEENT, _Test_ULONG64);
					INT flag = Memory::_This->Read_Memory(base + BASEENT_FLAG, _Test_INT);
					currentPlayer.flag = flag; // todo: fix this
					//currentPlayer.position =Memory::_This->Read_Memory(currentPlayer.VisualState + VISUALSTATE_POSITION, _Test_Vector3);
				}
				else if (strstr(world_name_char, "npc"))
				{
					ULONG64 base = Memory::_This->Read_Memory(entity + ENTITYPTR_BASEENT, _Test_ULONG64);
					INT flag = Memory::_This->Read_Memory(base + BASEENT_FLAG, _Test_INT);
					currentPlayer.flag = flag;
					ULONG64 model = Memory::_This->Read_Memory(base + BASEENT_MODEL, _Test_ULONG64);
					ULONG64 modelTransform = Memory::_This->Read_Memory(model + MODEL_TRANSFORM, _Test_ULONG64);
					ULONG64 head = Memory::_This->Read_Memory(modelTransform + TRANSFORM_BONE + 47 * 8, _Test_ULONG64);
					ULONG64 headWorld = Memory::_This->Read_Memory(head + BONE_WORLD, _Test_ULONG64);

					//printf("\n before bone func \n");
					currentPlayer.head = headWorld;

					//printf("\npeople people at position %llu", cycle);
					ULONG64 playerNamePtr = Memory::_This->Read_Memory(base + BASEENT_NAME, _Test_ULONG64);
					wchar_t txt[64] = { 0 };
					Memory::_This->Read_Memory(playerNamePtr + 0x18, txt, 64);
					wstring ws(txt);
					// your new String
					string str(ws.begin(), ws.end());
					currentPlayer.name = str;

					currentPlayer.objName = "NPC";

					//currentPlayer.position =Memory::_This->Read_Memory(currentPlayer.VisualState + VISUALSTATE_POSITION, _Test_Vector3);
				}
				else
				{
					ULONG64 base = Memory::_This->Read_Memory(entity + ENTITYPTR_BASEENT, _Test_ULONG64);
					INT flag = Memory::_This->Read_Memory(base + BASEENT_FLAG, _Test_INT);
					currentPlayer.flag = flag;
					ULONG64 model = Memory::_This->Read_Memory(base + BASEENT_MODEL, _Test_ULONG64);
					ULONG64 modelTransform = Memory::_This->Read_Memory(model + MODEL_TRANSFORM, _Test_ULONG64);
					ULONG64 head = Memory::_This->Read_Memory(modelTransform + TRANSFORM_BONE + 47 * 8, _Test_ULONG64);
					ULONG64 headWorld = Memory::_This->Read_Memory(head + BONE_WORLD, _Test_ULONG64);

					//printf("\n before bone func \n");
					currentPlayer.head = headWorld;

					//printf("\npeople people at position %llu", cycle);
					ULONG64 playerNamePtr = Memory::_This->Read_Memory(base + BASEENT_NAME, _Test_ULONG64);
					wchar_t txt[64] = { 0 };
					Memory::_This->Read_Memory(playerNamePtr + 0x18, txt, 64);
					wstring ws(txt);
					// your new String
					string str(ws.begin(), ws.end());
					currentPlayer.name = str;

					currentPlayer.objName = "Player";

					//currentPlayer.position =Memory::_This->Read_Memory(currentPlayer.VisualState + VISUALSTATE_POSITION, _Test_Vector3);
				}
			}
			_Player_Vector_Temp.push_back(currentPlayer);
		}
		taggedObjectsPtr = Memory::_This->Read_Memory(taggedObjectsPtr + TAGGEDLIST_NEXTTAGGEDENT, _Test_ULONG64);
	}
	//printf("inner cycle: %llu\n", cycle);
	Game_Data::_Player_Vector = _Player_Vector_Temp;
	return 0;
}