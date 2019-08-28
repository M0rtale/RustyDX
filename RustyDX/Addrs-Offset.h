#pragma once

#include "XorStr.h"

//#define GET_ENT_ID 
//#define _DEBUG_PRINT

#define RUST_OBJECTMANAGER 0x15FF218

#define OBJECTMANAGER_TAGGEDLISTFIRST 0x0
#define OBJECTMANAGER_TAGGEDLIST 0x8
#define OBJECTMANAGER_ACTIVELIST 0x18
#define OBJECTMANAGER_ACTIVELISTLAST 0x10

#define TAGGEDLIST_NEXTTAGGEDENT 0x8
#define BASEENT_GAMEOBJECT 0x10

#define GAMEOBJECT_OBJECTCLASS 0x30
#define GAMEOBJECT_OBJECTNAMEPTR 0x60
#define GAMEOBJECT_TAG 0x54

#define OBJECTCLASS_TRANSFORM 0x8
#define OBJECTCLASS_ENTITYPTR 0x18

#define ENTITYPTR_BASEENT 0x28

#define TRANSFORM_VISUALSTATE 0x38

#define VISUALSTATE_POSITION 0xb0

#define ENTITYPTR_MATRIX 0x2E4

#define BASEENT_NAME 0x4B0
#define BASEENT_MODEL 0x80
#define BASEENT_FLAG 0x528

#define MODEL_TRANSFORM 0x40

#define TRANSFORM_BONE 0x20

#define BONE_WORLD 0x10

#define BASENETWORKABLE_CLIENTENTITIES 0x10
#define CLIENTENTITIES_ENTITY_LIST 0x28
#define ENTITY_LIST_SIZE 0x18
#define ENTITY_LIST_OBJECTS_LIST 0x10
#define OBJECTS_LIST_OFFSET 0x20 // first is localplayer
#define UNKNOWNENTITY_FIRST 0x10
#define FIRST_GAMEOBJ 0x30 // regular game obj.

//& sets state, | 

enum PlayerFlags
{
	Unused1 = 1,
	Unused2 = 2,
	IsAdmin = 4,
	ReceivingSnapshot = 8,
	Sleeping = 16,
	Spectating = 32,
	Wounded = 64,
	IsDeveloper = 128,
	Connected = 256,
	VoiceMuted = 512,
	ThirdPersonViewmode = 1024,
	EyesViewmode = 2048,
	ChatMute = 4096,
	NoSprint = 8192,
	Aiming = 16384,
	DisplaySash = 32768,
	Relaxed = 65536,
	SafeZone = 131072,
	Workbench1 = 1048576,
	Workbench2 = 2097152,
	Workbench3 = 4194304
};