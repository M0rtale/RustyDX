#include "Game-Data.h"
#include <string>
#include <sstream>
#include <fstream>
#include <iostream>

std::vector<Player> Game_Data::_Player_Vector = std::vector<Player>();
std::vector<Player> Game_Data::_Obj_Vector = std::vector<Player>();
std::vector<Player> Game_Data::_Animal_Vector = std::vector<Player>();
std::vector<Player> Game_Data::_Turret_Vector = std::vector<Player>();
std::vector<Player> Game_Data::_Vehicle_Vector = std::vector<Player>();
ULONG64 Game_Data::_Camera_Addr = 0;
Player Game_Data::localplayer = Player();

ULONG64 _Test_ULONG64 = 0;
INT16 _Test_INT16 = 0;

ULONG64 mono_field_static_get_value(ULONG64 klass, ULONG offset)
{
	ULONG64 vTable = Memory::_This->Read_Memory<ULONG64>(klass + 0xD0);
	if (vTable != NULL)
	{
		vTable = Memory::_This->Read_Memory<ULONG64>(vTable + 0x8);
		if (vTable != NULL)
		{
			ULONG vTableSize = 0x40 + Memory::_This->Read_Memory<ULONG>(klass + 0x5C) * 0x8;
			return Memory::_This->Read_Memory<ULONG64>(vTable + vTableSize + offset);
		}
	}
	return 0;
}

ULONG64 getBaseNetworked()
{
	ULONG64 ObjectManager = Memory::_This->Read_Memory(Memory::_This->Base + RUST_OBJECTMANAGER, _Test_ULONG64);
	ULONG64 taggedObjectsPtr = Memory::_This->Read_Memory(ObjectManager + OBJECTMANAGER_TAGGEDLIST, _Test_ULONG64);
	ULONG64 firstObject = Memory::_This->Read_Memory(ObjectManager + OBJECTMANAGER_TAGGEDLISTFIRST, _Test_ULONG64);

	ULONG64 BaseNetworkable;

	while (firstObject != taggedObjectsPtr)
	{
		Player currentPlayer = Player();
		currentPlayer.BaseObject = taggedObjectsPtr;
		currentPlayer.GameObject = Memory::_This->Read_Memory(taggedObjectsPtr + BASEENT_GAMEOBJECT, _Test_ULONG64);
		currentPlayer.tag = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_TAG, _Test_INT16);

		if (currentPlayer.tag == 5)
		{
			//mainCamera
			ULONG64 objClass = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
			ULONG64 entity = Memory::_This->Read_Memory(objClass + OBJECTCLASS_ENTITYPTR, _Test_ULONG64);
			ULONG64 transform = Memory::_This->Read_Memory(objClass + OBJECTCLASS_TRANSFORM, _Test_ULONG64);

			Game_Data::_Camera_Addr = entity;

			currentPlayer.VisualState = Memory::_This->Read_Memory(transform + TRANSFORM_VISUALSTATE, _Test_ULONG64);
		}

		if (currentPlayer.tag == 6) // found a player, which is networked
		{
			ULONG64 objClass = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
			ULONG64 entity = Memory::_This->Read_Memory(objClass + OBJECTCLASS_ENTITYPTR, _Test_ULONG64);
			ULONG64 base = Memory::_This->Read_Memory(entity + ENTITYPTR_BASEENT, _Test_ULONG64);

			//need the first
			ULONG64 temp1 = Memory::_This->Read_Memory(base, _Test_ULONG64);
			temp1 = Memory::_This->Read_Memory(temp1, _Test_ULONG64);
			ULONG64 temp2 = Memory::_This->Read_Memory(temp1 + 0x30, _Test_ULONG64);
			ULONG64 temp3 = Memory::_This->Read_Memory(temp2 + 0x30, _Test_ULONG64);
			ULONG64 temp4 = Memory::_This->Read_Memory(temp3 + 0x30, _Test_ULONG64);

			BaseNetworkable = mono_field_static_get_value(temp4, 0);

			//again, need the first
			BaseNetworkable = Memory::_This->Read_Memory(BaseNetworkable, _Test_ULONG64);
			return BaseNetworkable;
			//ULONG64 t1 = Memory::_This->Read_Memory(BaseNetworkable + 0x10, _Test_ULONG64);
			//ULONG64 t2 = Memory::_This->Read_Memory(t1 + 0x28, _Test_ULONG64);
			//networked_list = Memory::_This->Read_Memory(t2 + 0x10, _Test_ULONG64);
		}
		taggedObjectsPtr = Memory::_This->Read_Memory(taggedObjectsPtr + TAGGEDLIST_NEXTTAGGEDENT, _Test_ULONG64);
	}
	return 0;
}

ULONG64 getCam()
{
	std::vector<Player> _Player_Vector_Temp;

	ULONG64 ObjectManager = Memory::_This->Read_Memory(Memory::_This->Base + RUST_OBJECTMANAGER, _Test_ULONG64);
	ULONG64 taggedObjectsPtr = Memory::_This->Read_Memory(ObjectManager + OBJECTMANAGER_TAGGEDLIST, _Test_ULONG64);
	ULONG64 firstObject = Memory::_This->Read_Memory(ObjectManager + OBJECTMANAGER_TAGGEDLISTFIRST, _Test_ULONG64);

	while (firstObject != taggedObjectsPtr)
	{
		Player currentPlayer = Player();
		currentPlayer.BaseObject = taggedObjectsPtr;
		currentPlayer.GameObject = Memory::_This->Read_Memory(taggedObjectsPtr + BASEENT_GAMEOBJECT, _Test_ULONG64);
		currentPlayer.tag = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_TAG, _Test_INT16);

		if (currentPlayer.tag == 5)
		{
			//mainCamera
			ULONG64 objClass = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
			ULONG64 entity = Memory::_This->Read_Memory(objClass + OBJECTCLASS_ENTITYPTR, _Test_ULONG64);
			ULONG64 transform = Memory::_This->Read_Memory(objClass + OBJECTCLASS_TRANSFORM, _Test_ULONG64);

			return entity;
		}
		taggedObjectsPtr = Memory::_This->Read_Memory(taggedObjectsPtr + TAGGEDLIST_NEXTTAGGEDENT, _Test_ULONG64);
	}
	return 0;
}

Player getLocal(ULONG64 UnknownEnt, ULONG64 length)
{
	Player currentPlayer = Player();

	ULONG64 temp1 = Memory::_This->Read_Memory(UnknownEnt + UNKNOWNENTITY_FIRST, _Test_ULONG64);
	ULONG64 obj = Memory::_This->Read_Memory(temp1 + FIRST_GAMEOBJ, _Test_ULONG64);

	currentPlayer.GameObject = obj;
	currentPlayer.tag = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_TAG, _Test_INT16);
	ULONG64 objectNamePor = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTNAMEPTR, _Test_ULONG64);
	char world_name_char[100] = { 0 };
	Memory::_This->Read_Memory(objectNamePor, world_name_char, 50);
	//printf("The name is: %s at mem pos %llu\n", world_name_char, objectNamePor);
	if (strstr(world_name_char, "LocalPlayer"))
	{
		//other stuff
		//printf("\nlocal local at position %llu", obj);
		Game_Data::localplayer = currentPlayer;
		currentPlayer.name = world_name_char;
		currentPlayer.objName = "Localplayer";
		ULONG64 objClass = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
		ULONG64 entity = Memory::_This->Read_Memory(objClass + OBJECTCLASS_ENTITYPTR, _Test_ULONG64);
		ULONG64 base = Memory::_This->Read_Memory(entity + ENTITYPTR_BASEENT, _Test_ULONG64);
		INT _Test_INT = 0;
		INT flag = Memory::_This->Read_Memory(base + BASEENT_FLAG, _Test_INT);
		ULONG64 transform = Memory::_This->Read_Memory(objClass + OBJECTCLASS_TRANSFORM, _Test_ULONG64);
		currentPlayer.VisualState = Memory::_This->Read_Memory(transform + TRANSFORM_VISUALSTATE, _Test_ULONG64);
		currentPlayer.flag = flag; // todo: fix this
		return currentPlayer;
	}
	//printf("You fucked up!");
}

bool Game_Data::Hook_Start()
{
	_Memory->Init();
	_Rusty_Addr = Memory::_This->Get_Model_Addr();

	BaseNetworkable = getBaseNetworked();

	_Camera_Addr = getCam();

	return true;
}

void Game_Data::Get_Data(bool box, bool animal, bool turret, bool vehicle)
{
	if (BaseNetworkable == 0) // this shit is constant
	{
		BaseNetworkable = getBaseNetworked();
	}

	_Camera_Addr = getCam();

	//printf("BaseNetwork: %llu, Camera: %llu\n", BaseNetworkable, _Camera_Addr);

	ULONG64 clientEntities = Memory::_This->Read_Memory(BaseNetworkable + BASENETWORKABLE_CLIENTENTITIES, _Test_ULONG64);
	ULONG64 Entity_list = Memory::_This->Read_Memory(clientEntities + CLIENTENTITIES_ENTITY_LIST, _Test_ULONG64);

	//printf("finished reading list\n");

	ULONG64 size = Memory::_This->Read_Memory(Entity_list + ENTITY_LIST_SIZE, _Test_ULONG64);
	ULONG64 list = Memory::_This->Read_Memory(Entity_list + ENTITY_LIST_OBJECTS_LIST, _Test_ULONG64);
	list += 0x20; // skip some junk

	//printf("before copying\n");
	_Networked_List = (ULONG64*)malloc(size * sizeof(ULONG64));

	//printf("Nice, you didn't fuck up\n");

	Memory::_This->Read_Memory(list, _Networked_List, size * sizeof(ULONG64));
	_Networked_size = size;

	//printf("copying successful, size: %llu _Networked_List first: %llu\n", size, _Networked_List[0]);


	localplayer = getLocal(_Networked_List[0], _Networked_size);

	//walk the list

	//first is localplayer
	_Player_Vector.clear();
	_Obj_Vector.clear();
	_Animal_Vector.clear();
	_Turret_Vector.clear();
	_Vehicle_Vector.clear();
	for (ULONG64 i = 1; i < size; i++)
	{
		Player currentPlayer = Player();

		ULONG64 temp1 = Memory::_This->Read_Memory(_Networked_List[i] + UNKNOWNENTITY_FIRST, _Test_ULONG64);
		ULONG64 obj = Memory::_This->Read_Memory(temp1 + FIRST_GAMEOBJ, _Test_ULONG64);

		currentPlayer.GameObject = obj;
		currentPlayer.tag = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_TAG, _Test_INT16);

		if (currentPlayer.tag == 6)
		{
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
			_Player_Vector.push_back(currentPlayer);
		}
		else if (currentPlayer.tag == 5) printf("Hey! I Found a Camera in this LIST!\n");
		else
		{

			if (animal)
			{
				char world_name_char[100] = { 0 };
				ULONG64 objectNamePor = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTNAMEPTR, _Test_ULONG64);
				Memory::_This->Read_Memory(objectNamePor, world_name_char, 50);
				if (strstr(world_name_char, "rust.ai/agents/boar"))
				{
					//printf("found crate");
					ULONG64 objClass = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
					ULONG64 transform = Memory::_This->Read_Memory(objClass + OBJECTCLASS_TRANSFORM, _Test_ULONG64);
					currentPlayer.VisualState = Memory::_This->Read_Memory(transform + TRANSFORM_VISUALSTATE, _Test_ULONG64);
					currentPlayer.objName = "boar";
					_Animal_Vector.push_back(currentPlayer);
					continue;
				}

				if (strstr(world_name_char, "rust.ai/agents/bear"))
				{
					ULONG64 objClass = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
					ULONG64 transform = Memory::_This->Read_Memory(objClass + OBJECTCLASS_TRANSFORM, _Test_ULONG64);
					currentPlayer.VisualState = Memory::_This->Read_Memory(transform + TRANSFORM_VISUALSTATE, _Test_ULONG64);
					currentPlayer.objName = "bear";
					_Animal_Vector.push_back(currentPlayer);
					continue;
				}
			}
			// vehicles/boats/rhib  vehicles/boats/rowboat
			if (vehicle)
			{
				char world_name_char[100] = { 0 };
				ULONG64 objectNamePor = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTNAMEPTR, _Test_ULONG64);
				Memory::_This->Read_Memory(objectNamePor, world_name_char, 50);
				if (strstr(world_name_char, "vehicles/boats/rhib"))
				{
					ULONG64 objClass = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
					ULONG64 transform = Memory::_This->Read_Memory(objClass + OBJECTCLASS_TRANSFORM, _Test_ULONG64);
					currentPlayer.VisualState = Memory::_This->Read_Memory(transform + TRANSFORM_VISUALSTATE, _Test_ULONG64);
					currentPlayer.objName = "Speed Boat";
					_Vehicle_Vector.push_back(currentPlayer);
					continue;
				}

				if (strstr(world_name_char, "vehicles/boats/rowboat"))
				{
					ULONG64 objClass = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
					ULONG64 transform = Memory::_This->Read_Memory(objClass + OBJECTCLASS_TRANSFORM, _Test_ULONG64);
					currentPlayer.VisualState = Memory::_This->Read_Memory(transform + TRANSFORM_VISUALSTATE, _Test_ULONG64);
					currentPlayer.objName = "Dinghy";
					_Vehicle_Vector.push_back(currentPlayer);
					continue;
				}
			}

			if (turret)
			{
				char world_name_char[100] = { 0 };
				ULONG64 objectNamePor = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTNAMEPTR, _Test_ULONG64);
				Memory::_This->Read_Memory(objectNamePor, world_name_char, 50);
				if (strstr(world_name_char, "prefabs/npc/autoturret"))
				{
					ULONG64 objClass = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
					ULONG64 transform = Memory::_This->Read_Memory(objClass + OBJECTCLASS_TRANSFORM, _Test_ULONG64);
					currentPlayer.VisualState = Memory::_This->Read_Memory(transform + TRANSFORM_VISUALSTATE, _Test_ULONG64);
					currentPlayer.objName = "Auto Turret";
					_Turret_Vector.push_back(currentPlayer);
					continue;
				}

				if (strstr(world_name_char, "prefabs/npc/flame turret"))
				{
					ULONG64 objClass = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
					ULONG64 transform = Memory::_This->Read_Memory(objClass + OBJECTCLASS_TRANSFORM, _Test_ULONG64);
					currentPlayer.VisualState = Memory::_This->Read_Memory(transform + TRANSFORM_VISUALSTATE, _Test_ULONG64);
					currentPlayer.objName = "Flame Turret";
					_Turret_Vector.push_back(currentPlayer);
					continue;
				}
			}

			if (box)
			{
				char world_name_char[100] = { 0 };
				ULONG64 objectNamePor = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTNAMEPTR, _Test_ULONG64);
				Memory::_This->Read_Memory(objectNamePor, world_name_char, 50);
#ifdef _DEBUG_PRINT
				printf("%s\n", world_name_char);
#endif

				//if (strstr(world_name_char, "crate"))
				//{
				//	//printf("found crate");
				//	ULONG64 objClass = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
				//	ULONG64 transform = Memory::_This->Read_Memory(objClass + OBJECTCLASS_TRANSFORM, _Test_ULONG64);
				//	currentPlayer.VisualState = Memory::_This->Read_Memory(transform + TRANSFORM_VISUALSTATE, _Test_ULONG64);
				//	currentPlayer.objName = "crate";
				//	_Obj_Vector.push_back(currentPlayer);
				//}
				if (strstr(world_name_char, "woodbox"))
				{
					//printf("found woodbox");
					ULONG64 objClass = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
					ULONG64 transform = Memory::_This->Read_Memory(objClass + OBJECTCLASS_TRANSFORM, _Test_ULONG64);
					currentPlayer.VisualState = Memory::_This->Read_Memory(transform + TRANSFORM_VISUALSTATE, _Test_ULONG64);
					currentPlayer.objName = "woodbox";
					_Obj_Vector.push_back(currentPlayer);
					continue;
				}
				//else if (strstr(world_name_char, "sulfur"))
				//{
				//	//printf("found woodbox");
				//	ULONG64 objClass = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
				//	ULONG64 transform = Memory::_This->Read_Memory(objClass + OBJECTCLASS_TRANSFORM, _Test_ULONG64);
				//	currentPlayer.VisualState = Memory::_This->Read_Memory(transform + TRANSFORM_VISUALSTATE, _Test_ULONG64);
				//	currentPlayer.objName = "sulfur";
				//	_Obj_Vector.push_back(currentPlayer);
				//}
				else if (strstr(world_name_char, "box.wooden"))
				{
					//printf("found box.wooden");
					ULONG64 objClass = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
					ULONG64 transform = Memory::_This->Read_Memory(objClass + OBJECTCLASS_TRANSFORM, _Test_ULONG64);
					currentPlayer.VisualState = Memory::_This->Read_Memory(transform + TRANSFORM_VISUALSTATE, _Test_ULONG64);
					currentPlayer.objName = "box.wooden";
					_Obj_Vector.push_back(currentPlayer);
					continue;
				}
				else if (strstr(world_name_char, "cupboard"))
				{
					//printf("found cupboard");
					ULONG64 objClass = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
					ULONG64 transform = Memory::_This->Read_Memory(objClass + OBJECTCLASS_TRANSFORM, _Test_ULONG64);
					currentPlayer.VisualState = Memory::_This->Read_Memory(transform + TRANSFORM_VISUALSTATE, _Test_ULONG64);
					currentPlayer.objName = "cupboard";
					_Obj_Vector.push_back(currentPlayer);
					continue;
				}
				else if (strstr(world_name_char, "large wood storage"))
				{
					//printf("found cupboard");
					ULONG64 objClass = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
					ULONG64 transform = Memory::_This->Read_Memory(objClass + OBJECTCLASS_TRANSFORM, _Test_ULONG64);
					currentPlayer.VisualState = Memory::_This->Read_Memory(transform + TRANSFORM_VISUALSTATE, _Test_ULONG64);
					currentPlayer.objName = "large wood storage";
					_Obj_Vector.push_back(currentPlayer);
					continue;
				}
			}
		}
	}
	free(_Networked_List);
	//printf("localplayer read");
}

//int get_data()
//{
//	std::vector<Player> _Player_Vector_Temp;
//
//	ULONG64 ObjectManager = Memory::_This->Read_Memory(Memory::_This->Base + RUST_OBJECTMANAGER, _Test_ULONG64);
//	ULONG64 taggedObjectsPtr = Memory::_This->Read_Memory(ObjectManager + OBJECTMANAGER_TAGGEDLIST, _Test_ULONG64);
//	ULONG64 firstObject = Memory::_This->Read_Memory(ObjectManager + OBJECTMANAGER_TAGGEDLISTFIRST, _Test_ULONG64);
//
//	//walk the list
//	ULONG64 cycle = 0;
//	while (firstObject != taggedObjectsPtr)
//	{
//		cycle++;
//		Player currentPlayer = Player();
//		currentPlayer.BaseObject = taggedObjectsPtr;
//		currentPlayer.GameObject = Memory::_This->Read_Memory(taggedObjectsPtr + BASEENT_GAMEOBJECT, _Test_ULONG64);
//		currentPlayer.tag = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_TAG, _Test_INT16);
//
//		if (currentPlayer.tag == 5 || /*currentPlayer.tag == 20009 ||*/ currentPlayer.tag == 6)
//		{
//			//printf("\n gogogo %i \n", currentPlayer.tag);
//
//			if (currentPlayer.tag == 0)
//			{
//				//you fucked up somewhere
//				//printf("\nwalk list failed... tag 0 \n");
//				break;
//			}
//			if (currentPlayer.tag == 5)
//			{
//				//mainCamera
//				ULONG64 objClass = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
//				ULONG64 entity = Memory::_This->Read_Memory(objClass + OBJECTCLASS_ENTITYPTR, _Test_ULONG64);
//				ULONG64 transform = Memory::_This->Read_Memory(objClass + OBJECTCLASS_TRANSFORM, _Test_ULONG64);
//
//				Game_Data::_Camera_Addr = entity;
//
//				currentPlayer.VisualState = Memory::_This->Read_Memory(transform + TRANSFORM_VISUALSTATE, _Test_ULONG64);
//			}
//			if (currentPlayer.tag == 20009)
//			{
//				//Corpse
//				ULONG64 objClass = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
//				//ULONG64 entity =Memory::_This->Read_Memory(objClass + OBJECTCLASS_ENTITYPTR, _Test_ULONG64);
//				ULONG64 transform = Memory::_This->Read_Memory(objClass + OBJECTCLASS_TRANSFORM, _Test_ULONG64);
//
//				currentPlayer.VisualState = Memory::_This->Read_Memory(transform + TRANSFORM_VISUALSTATE, _Test_ULONG64);
//			}
//			//if (currentPlayer.tag == 20011) probably unnecessary
//			//{
//			//	//SkyDome
//			//	ULONG64 objClass =Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
//			//	//ULONG64 entity =Memory::_This->Read_Memory(objClass + OBJECTCLASS_ENTITYPTR, _Test_ULONG64);
//			//	ULONG64 transform =Memory::_This->Read_Memory(objClass + OBJECTCLASS_TRANSFORM, _Test_ULONG64);
//
//			//	currentPlayer.VisualState =Memory::_This->Read_Memory(transform + TRANSFORM_VISUALSTATE, _Test_ULONG64);
//			//}
//			if (currentPlayer.tag == 6)
//			{
//				firstObject = Memory::_This->Read_Memory(ObjectManager, _Test_ULONG64);
//				//Player
//				ULONG64 objClass = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTCLASS, _Test_ULONG64);
//				ULONG64 entity = Memory::_This->Read_Memory(objClass + OBJECTCLASS_ENTITYPTR, _Test_ULONG64);
//				ULONG64 transform = Memory::_This->Read_Memory(objClass + OBJECTCLASS_TRANSFORM, _Test_ULONG64);
//
//				//printf("\n before local model \n");
//
//				//ULONG64 model =Memory::_This->Read_Memory(entity + ENTITYPTR_MODEL, _Test_ULONG64);
//				//ULONG64 modelTransform =Memory::_This->Read_Memory(model + MODEL_TRANSFORM, _Test_ULONG64);
//				//ULONG64 head =Memory::_This->Read_Memory(modelTransform + TRANSFORM_BONE + 47 * 8, _Test_ULONG64);
//				//ULONG64 headWorld =Memory::_This->Read_Memory(head + BONE_WORLD, _Test_ULONG64);
//
//				//printf("\n before bone func \n");
//				//currentPlayer.head = get_bone_pos(headWorld);
//				//printf("\n after bone func \n");
//				currentPlayer.VisualState = Memory::_This->Read_Memory(transform + TRANSFORM_VISUALSTATE, _Test_ULONG64);
//
//				ULONG64 objectNamePor = Memory::_This->Read_Memory(currentPlayer.GameObject + GAMEOBJECT_OBJECTNAMEPTR, _Test_ULONG64);
//				char world_name_char[100] = { 0 };
//				Memory::_This->Read_Memory(objectNamePor, world_name_char, 50);
//				//printf("The name is: %s at mem pos %llu\n", world_name_char, objectNamePor);
//				if (strstr(world_name_char, "LocalPlayer"))
//				{
//					//other stuff
//					//printf("\nlocal local at position %llu", cycle);
//					Game_Data::localplayer = currentPlayer;
//					currentPlayer.name = world_name_char;
//					currentPlayer.objName = "Localplayer";
//					ULONG64 base = Memory::_This->Read_Memory(entity + ENTITYPTR_BASEENT, _Test_ULONG64);
//					INT flag = Memory::_This->Read_Memory(base + BASEENT_FLAG, _Test_INT);
//					currentPlayer.flag = flag; // todo: fix this
//					//currentPlayer.position =Memory::_This->Read_Memory(currentPlayer.VisualState + VISUALSTATE_POSITION, _Test_Vector3);
//				}
//				else if (strstr(world_name_char, "npc"))
//				{
//					ULONG64 base = Memory::_This->Read_Memory(entity + ENTITYPTR_BASEENT, _Test_ULONG64);
//					INT flag = Memory::_This->Read_Memory(base + BASEENT_FLAG, _Test_INT);
//					currentPlayer.flag = flag;
//					ULONG64 model = Memory::_This->Read_Memory(base + BASEENT_MODEL, _Test_ULONG64);
//					ULONG64 modelTransform = Memory::_This->Read_Memory(model + MODEL_TRANSFORM, _Test_ULONG64);
//					ULONG64 head = Memory::_This->Read_Memory(modelTransform + TRANSFORM_BONE + 47 * 8, _Test_ULONG64);
//					ULONG64 headWorld = Memory::_This->Read_Memory(head + BONE_WORLD, _Test_ULONG64);
//
//					//printf("\n before bone func \n");
//					currentPlayer.head = headWorld;
//
//					//printf("\npeople people at position %llu", cycle);
//					ULONG64 playerNamePtr = Memory::_This->Read_Memory(base + BASEENT_NAME, _Test_ULONG64);
//					wchar_t txt[64] = { 0 };
//					Memory::_This->Read_Memory(playerNamePtr + 0x18, txt, 64);
//					wstring ws(txt);
//					// your new String
//					string str(ws.begin(), ws.end());
//					currentPlayer.name = str;
//
//					currentPlayer.objName = "NPC";
//
//					//currentPlayer.position =Memory::_This->Read_Memory(currentPlayer.VisualState + VISUALSTATE_POSITION, _Test_Vector3);
//				}
//				else
//				{
//					ULONG64 base = Memory::_This->Read_Memory(entity + ENTITYPTR_BASEENT, _Test_ULONG64);
//					INT flag = Memory::_This->Read_Memory(base + BASEENT_FLAG, _Test_INT);
//					currentPlayer.flag = flag;
//					ULONG64 model = Memory::_This->Read_Memory(base + BASEENT_MODEL, _Test_ULONG64);
//					ULONG64 modelTransform = Memory::_This->Read_Memory(model + MODEL_TRANSFORM, _Test_ULONG64);
//					ULONG64 head = Memory::_This->Read_Memory(modelTransform + TRANSFORM_BONE + 47 * 8, _Test_ULONG64);
//					ULONG64 headWorld = Memory::_This->Read_Memory(head + BONE_WORLD, _Test_ULONG64);
//
//					//printf("\n before bone func \n");
//					currentPlayer.head = headWorld;
//
//					//printf("\npeople people at position %llu", cycle);
//					ULONG64 playerNamePtr = Memory::_This->Read_Memory(base + BASEENT_NAME, _Test_ULONG64);
//					wchar_t txt[64] = { 0 };
//					Memory::_This->Read_Memory(playerNamePtr + 0x18, txt, 64);
//					wstring ws(txt);
//					// your new String
//					string str(ws.begin(), ws.end());
//					currentPlayer.name = str;
//
//					currentPlayer.objName = "Player";
//
//					//currentPlayer.position =Memory::_This->Read_Memory(currentPlayer.VisualState + VISUALSTATE_POSITION, _Test_Vector3);
//				}
//			}
//			_Player_Vector_Temp.push_back(currentPlayer);
//		}
//		taggedObjectsPtr = Memory::_This->Read_Memory(taggedObjectsPtr + TAGGEDLIST_NEXTTAGGEDENT, _Test_ULONG64);
//	}
//	//printf("inner cycle: %llu\n", cycle);
//	Game_Data::_Player_Vector = _Player_Vector_Temp;
//	return 0;
//}