#pragma once
#include "../Utils/CStr.h"
#define GAME_STRING(x) (std::string(CIPHER("QrpnTnzrf.EbgZT.", -13)) + CIPHER(x, 0))

constexpr inline int IgnoreArgCount = -1;

#define SET_CLASS_NAME(x, y) inline std::string CLASS_NAMESPACE = x; inline constexpr const char* CLASS_NAME = y;
#define ADD_FIELD(x, y) inline constexpr const char* x = y;
#define ADD_METHOD(x, y, argcount) inline constexpr const char* Method_##x = y; inline constexpr int Method_##x##_ArgCount = argcount;

namespace NameMapping
{
	namespace BasicMapObjectClass
	{
		SET_CLASS_NAME("", "KJMONHENJEN");
		ADD_FIELD(objectId, "HHPOJBFICAH");
		ADD_FIELD(isMe, "IOGIPIKIDPB");
		ADD_FIELD(x, "CLFEOFKBNEJ");
		ADD_FIELD(y, "PKEECFNFEIO");
		ADD_FIELD(_mapViewService, "EFAGNJBDCOM");
	}

	namespace MapObjectClass
	{
		SET_CLASS_NAME("", "LKHPPBEGNOM");
		ADD_FIELD(_name, "DPGEBOCBKEF");
		ADD_FIELD(hp, "ABCPKBGJPEP");
		ADD_FIELD(maxHp, "OADOHPKBPJB");
		ADD_FIELD(equipment, "KELCBLCGGPB");
		ADD_FIELD(conditions, "COHCKAPOLCA");
	}

	namespace CharacterClass
	{
		SET_CLASS_NAME("", "PMMFLLAIPGN");
	}

	namespace PlayerClass
	{
		SET_CLASS_NAME("", "FKALGHJIADI");
		ADD_FIELD(_speed, "BHJFNEAHAOE");
		ADD_METHOD(Update, "GJFKGLJEGKO", IgnoreArgCount);
		ADD_METHOD(MoveTo, "DGLCONCOIBO", IgnoreArgCount);
	}

	namespace MapViewServiceClass
	{
		SET_CLASS_NAME("", "HJMBOMEHGDJ");
		ADD_FIELD(_gameController, "MEGKICBOJBK");
		ADD_FIELD(_server, "AGPKCIALHHA");
		ADD_FIELD(playerExist, "AHOOBGKDIJP");
	}

	namespace GameControllerClass
	{
		SET_CLASS_NAME(GAME_STRING("Managers.Game"), "GameController");
		ADD_METHOD(GetGameId, "KJPGIIHLENO", IgnoreArgCount);
	}

	namespace InputManagerClass
	{
		SET_CLASS_NAME(GAME_STRING("Managers.Options"), "InputManager");
		ADD_METHOD(Shooting, "MACNANOGMBL", IgnoreArgCount);
		ADD_METHOD(GetMousePosition, "GetMousePosition", IgnoreArgCount);
	}

	namespace ObjectPropertiesClass
	{
		SET_CLASS_NAME(GAME_STRING("Objects.Map.Data"), "ObjectProperties");
		ADD_FIELD(id, "id");
		ADD_FIELD(slotTypeElementValue, "slotTypeElementValue");
	}

	namespace EquipmentManagerClass
	{
		SET_CLASS_NAME(GAME_STRING("Managers.Equipment"), "EquipmentManager");
		ADD_METHOD(GetObjectProperties, "LCNCGOGJIFB", IgnoreArgCount);
	}

	namespace InputClass
	{
		SET_CLASS_NAME("UnityEngine", "Input");
		ADD_METHOD(GetKey, "GetKey", IgnoreArgCount);
		ADD_METHOD(GetKeyDown, "GetKeyDown", IgnoreArgCount);
		ADD_METHOD(GetKeyUp, "GetKeyUp", IgnoreArgCount);
		ADD_METHOD(GetMousePosition, "get_mousePosition", IgnoreArgCount);
	}

	namespace SettingsManagerClass
	{
		SET_CLASS_NAME(GAME_STRING("UI.Managers"), "SettingsManager");
		ADD_METHOD(GetKeyCode, "GetKeyCode", IgnoreArgCount);
	}

	namespace ApplicationManagerClass
	{
		SET_CLASS_NAME(GAME_STRING("Managers"), "ApplicationManager");
		ADD_FIELD(_mapViewService, "<CHDFAEBMILI>k__BackingField");
		ADD_FIELD(_settingsManager, "<BBJMMGABMFM>k__BackingField");
		ADD_FIELD(_inputManager, "<NHOKMNKKNMN>k__BackingField");
		ADD_METHOD(GetInstance, "get_instance", 0);
		ADD_METHOD(GetExists, "get_exists", 0);
	}
}