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

#include "Interfaces.h"
#include "SDK.h"

#define NOT_FOUND_ASSERT(x) if (!x) RaiseError(MakeNullClassError(#x));

#define FindClass(name, image) \
	name = image->GetClass(NameMapping::name::CLASS_NAMESPACE.c_str(), NameMapping::name::CLASS_NAME); \
	NOT_FOUND_ASSERT(name);

void InitializeInterfaces()
{
	Sleep(1000);

	while ((uintptr_t)Il2Cpp::Domain::Get() < 0x100)
	{
		Sleep(1);
	}

	Il2Cpp::Domain* pIl2cppDomain = Il2Cpp::Domain::Get();
	if (!pIl2cppDomain)
	{
		MessageBoxA(NULL, "Failed to get Il2CppDomain", "Error", MB_ICONERROR | MB_OK);
	}

	Il2Cpp::Image* pMainImage = pIl2cppDomain->OpenAssembly("Assembly-CSharp.dll")->GetImage();
	Il2Cpp::Image* pInputImage = pIl2cppDomain->OpenAssembly("UnityEngine.InputLegacyModule.dll")->GetImage();

	FindClass(BasicMapObjectClass, pMainImage);
	FindClass(MapObjectClass, pMainImage);
	FindClass(CharacterClass, pMainImage);
	FindClass(PlayerClass, pMainImage);
	FindClass(MapViewServiceClass, pMainImage);
	FindClass(GameControllerClass, pMainImage);
	FindClass(InputManagerClass, pMainImage);
	FindClass(SettingsManagerClass, pMainImage);
	FindClass(ApplicationManagerClass, pMainImage);
	FindClass(ObjectPropertiesClass, pMainImage);
	FindClass(EquipmentManagerClass, pMainImage);
	FindClass(InputClass, pInputImage);
}