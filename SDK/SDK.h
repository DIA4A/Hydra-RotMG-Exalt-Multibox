/*
 * Hydra IPC PoC
 * Copyright (c) 2026 DIA4A
 *
 * This source code is licensed under the MIT license found in the
 * LICENSE file in the root directory of this project.
 *
 * ==============================================================================
 * DISCLAIMER: EDUCATIONAL PURPOSE ONLY
 * ==============================================================================
 * This Proof of Concept (PoC) is provided strictly for educational and
 * demonstrative purposes to showcase the Hydra IPC library's capabilities.
 * Interacting with or modifying a game's runtime memory via hooks may violate
 * the software's Terms of Service (ToS) or End User License Agreement (EULA).
 * * The author does not condone or support the use of this code for malicious
 * activities, cheating, or gaining an unfair advantage in multiplayer environments.
 * By using this code, you agree that you take full responsibility for your
 * actions. The author is not responsible for any damages, account bans, or
 * legal repercussions that may occur from compiling, executing, or modifying
 * this Proof of Concept.
 * ==============================================================================
 */

#pragma once
#include <cstdint>
#include "../il2cpp/il2cpp.h"
#include "ClassMacros.h"
#include "NameMapping.h"
#include "Interfaces.h"

namespace SDK
{
	// partial list
	enum ConditionEffects : uint64_t
	{
		None = 0 << 0,
		Dead = 1 << 0,
		Quiet = 1 << 1,
		Weak = 1 << 2,
		Slowed = 1 << 3,
		Sick = 1 << 4,
		Dazed = 1 << 5,
		Stunned = 1 << 6,
		Blind = 1 << 7,
		Hallucinating = 1 << 8,
		Drunk = 1 << 9,
		Confused = 1 << 10,
		StunImmume = 1 << 11,
		Invisible = 1 << 12,
		Paralyzed = 1 << 13,
		Speedy = 1 << 14,
		Bleeding = 1 << 15,
		ArmorBreakImmune = 1 << 16,
		Healing = 1 << 17,
		Damaging = 1 << 18,
		Berserk = 1 << 19,
		Paused = 1 << 20,
		Stasis = 1 << 21,
		StasisImmune = 1 << 22,
		Invincible = 1 << 23,
		Invulnerable = 1 << 24,
		Armored = 1 << 25,
		ArmorBroken = 1 << 26,
		Hexed = 1 << 27,
		NinjaSpeedy = 1 << 28,
		Unstable = 1 << 29,
		Darkness = 1 << 30
	};

	enum OptionType : int
	{
		UseWeaponAttack = 9,
		UseSpecialAbility = 10,
		InteractBuy = 11,
		EscapeToNexus = 12,
		InteractWithGameview = 103,
	};

	template <typename T>
	struct Il2CppArray
	{
		Il2CppObject obj;
		Il2CppArrayBounds* bounds;
		il2cpp_array_size_t max_length;
		T m_Items[0];
	};

	template <typename T>
	struct Il2CppList
	{
		Il2CppObject obj;
		Il2CppArray<T>* _items;
		int32_t _size;
		int32_t _version;
		Il2CppObject* _syncRoot;
	};

	struct Vector2 {
		float x;
		float y;
	};

	struct Vector3 {
		float x;
		float y;
		float z;
	};

	struct System_String {
		Il2CppObject obj;
		int32_t _stringLength;
		uint16_t _firstChar;
	};

	inline std::string WideToAnsiFast(const std::wstring& wstr)
	{
		return std::string(wstr.begin(), wstr.end());
	}

	inline std::string FromSystemString(SDK::System_String* pString)
	{
		return WideToAnsiFast(std::wstring((wchar_t*)&pString->_firstChar, pString->_stringLength));
	}

	struct Server {
		Il2CppObject obj;
		struct System_String* name;
		struct System_String* address;
		int32_t port;
		struct BFJLEEMBDFJ* latLong;
		float usage;
		bool isAdminOnly;
	};

	struct GameController
	{
		METHOD(GetGameId, GameControllerClass, int32_t);
	};

	struct MapViewService
	{
		FIELD(GameController*, _gameController, MapViewServiceClass);
		FIELD(Server*, _server, MapViewServiceClass);
		FIELD(bool, playerExist, MapViewServiceClass);
	};

	struct BasicMapObject
	{
		FIELD(int32_t, objectId, BasicMapObjectClass);
		FIELD(bool, isMe, BasicMapObjectClass);
		FIELD(float, x, BasicMapObjectClass);
		FIELD(float, y, BasicMapObjectClass);
		FIELD(MapViewService*, _mapViewService, BasicMapObjectClass);
	};

	struct MapObject : BasicMapObject
	{
		FIELD(System_String*, _name, MapObjectClass);
		FIELD(int32_t, hp, MapObjectClass);
		FIELD(int32_t, maxHp, MapObjectClass);
		FIELD(Il2CppList<int32_t>*, equipment, MapObjectClass);
		FIELD(Il2CppArray<uint32_t>*, conditions, MapObjectClass);

		uint64_t GetFullConditions()
		{
			return conditions()->m_Items[0] | conditions()->m_Items[1] << 31;
		}

		bool HasCondition(int test)
		{
			return GetFullConditions() & test;
		}
	};

	struct Character : MapObject
	{

	};

	struct Player : Character
	{
		FIELD(float, _speed, PlayerClass);
		METHOD(Update, PlayerClass, void, PARAMETERS(int32_t time, int32_t delta));
		METHOD(MoveTo, PlayerClass, void, PARAMETERS(float x, float y));
	};

	struct ObjectProperties
	{
		FIELD(System_String*, id, ObjectPropertiesClass);
		FIELD(int32_t, slotTypeElementValue, ObjectPropertiesClass);
	};

	struct EquipmentManager
	{
		STATIC_METHOD(GetObjectProperties, EquipmentManagerClass, ObjectProperties*, PARAMETERS(EquipmentManager* unused, int32_t itemId));
	};

	struct Input
	{
		STATIC_METHOD(GetKey, InputClass, bool, PARAMETERS(int32_t key));
		STATIC_METHOD(GetKeyDown, InputClass, bool, PARAMETERS(int32_t key));
		STATIC_METHOD(GetKeyUp, InputClass, bool, PARAMETERS(int32_t key));
		STATIC_METHOD(GetMousePosition, InputClass, Vector3*, PARAMETERS(Vector3* retvalue));
	};

	struct InputManager
	{
		METHOD(Shooting, InputManagerClass, void);
		METHOD(GetMousePosition, InputManagerClass, Vector2);
	};

	struct SettingsManager
	{
		METHOD(GetKeyCode, SettingsManagerClass, int32_t, PARAMETERS(int32_t setting));
	};

	struct ApplicationManager
	{
		STATIC_METHOD_NO_PARAM(GetInstance, ApplicationManagerClass, ApplicationManager*);
		STATIC_METHOD_NO_PARAM(GetExists, ApplicationManagerClass, bool);
		FIELD(MapViewService*, _mapViewService, ApplicationManagerClass);
		FIELD(SettingsManager*, _settingsManager, ApplicationManagerClass);
		FIELD(InputManager*, _inputManager, ApplicationManagerClass);
	};
}
