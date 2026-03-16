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

#define WIN32_LEAN_AND_MEAN
#include <Windows.h>
#include <cstdio>
#include <cstdarg>
#include <chrono>

#include <HydraPeer.h>
using namespace HydraIPC;

#include <MinHook.h>

#include "il2cpp/il2cpp.h"
#include "SDK/SDK.h"
#include "Utils/Console.h"
#include "Utils/Window.h"

static constexpr int PURGE_INTERVAL_MS = 5000; // Purge stale peers every 5s
static constexpr int STATUS_INTERVAL_MS = 3000; // Print status every 3s
static constexpr int REQUEST_TOME_INTERVAL_MS = 1000; // Request tome every 1s

// user defined commands to utilize the library without altering it
#pragma region User Defined Commands
enum TestCommands : uint32_t
{
	CmdPing = CmdUserDefined,
	CmdPong,
};

struct PingPayload
{
	long long nTimestamp;
};
#pragma endregion

volatile bool g_bRunning = false;
HANDLE g_hMainThread = NULL;
long long g_nLastPurge = 0;
long long g_nLastStatus = 0;
bool g_bWasLeader = false;

HWND g_hGameWindow = NULL;
WNDPROC g_pOriginalWndProc = nullptr;

bool g_bShouldRequestTome = false;
float g_flRequestTomeThreshold = 0.5f;
bool g_bNotifyBagTypes[6] = {};
long long g_nLastTomeRequest = 0;

long long GetCurrentMillis()
{
	return ((std::chrono::milliseconds)std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch())).count();
}

#define AsDeclType(x, y) ((decltype(&x))y)
#define AsOriginal(x) AsDeclType(x, x##Original)

#define TOSTRMACRO2(x) #x
#define TOSTRMACRO(x) TOSTRMACRO2(x)

#define CREATEHOOK(x, func) \
if (func) \
{ \
	if (MH_CreateHook(func, &x, (LPVOID*)&x##Original) != MH_OK) \
	{ \
		MessageBoxA(NULL, "Failed To Create Hook [" #x "] At Line " TOSTRMACRO(__LINE__), "Test", MB_ICONERROR | MB_OK); \
		exit(2); \
	} \
} \
else \
{ \
	MessageBoxA(NULL, "Function Provided For Hook [" #x "] At Line " TOSTRMACRO(__LINE__) " Is Null", "Test", MB_ICONERROR | MB_OK); \
	exit(2); \
}

SDK::ApplicationManager* g_pApplicationManager = nullptr;
SDK::MapViewService* g_pMapViewService = nullptr;
SDK::SettingsManager* g_pSettingsManager = nullptr;
int GetGameId()
{
	if (!g_pMapViewService || !g_pMapViewService->_gameController())
	{
		return 0;
	}
	return g_pMapViewService->_gameController()->GetGameId()();
}

SDK::Player* g_pPlayer = nullptr;
int32_t nLastKnownObjectId = 0;

SDK::Vector2 g_vecShootPosition;
bool g_bIsShooting = false;
bool g_bForceShoot = false;
bool g_bStopShoot = false;
SDK::Vector2 g_vecShootPositionOverride;

bool g_bIsUsingAbility = false;
bool g_bForceUseAbility = false;
bool g_bOverrideAbilityPosition = false;
SDK::Vector2 g_vecAbilityPositionOverride;

bool g_bIsShowingBagContents = false;
bool g_bForceShowBagContents = false;
bool g_bStopShowBagContents = false;

bool g_bIsEscapingToNexus = false;
bool g_bShouldEscapeToNexus = false;

bool g_bIsInteracting = false;
long long g_nForceInteractUntil = 0;

#pragma region Hydra Peer Logic

void RegisterHandlers()
{
	Peer::RegisterHandler(CmdNexus, [](uint32_t uSender, const void*, uint32_t)
		{
			Console::Log("CmdNexus from slot %u", uSender);
			g_bShouldEscapeToNexus = true;
		});

	Peer::On<Cmd::UseAbility>(CmdUseAbility, [](uint32_t uSender, const Cmd::UseAbility& cmd)
		{
			g_vecAbilityPositionOverride = { cmd.flX, cmd.flY };
			g_bForceUseAbility = true;
		});

	// too lazy to do the sending of the packet, if you imagine it hard enough in your head you can get the picture of how'd it work
	Peer::On<Cmd::BagDrop>(CmdBagDrop, [](uint32_t uSender, const Cmd::BagDrop& cmd)
		{
			Console::Log("Bag dropped from slot %u: type=%d at (%.1f, %.1f)",
				uSender, cmd.nBagType, cmd.flX, cmd.flY);
		});

	Peer::RegisterHandler(CmdRequestTome, [](uint32_t uSender, const void*, uint32_t)
		{
			Console::Log("Tome request from slot %u", uSender);
			if (g_pMapViewService && g_pMapViewService->playerExist() && g_pPlayer)
			{
				SDK::Il2CppList<int32_t>* equipment = g_pPlayer->equipment();
				if (equipment && equipment->_size > 0 && equipment->_items->m_Items[1] != -1)
				{
					SDK::ObjectProperties* pProperties = SDK::EquipmentManager::GetObjectProperties()(nullptr, equipment->_items->m_Items[1]);
					if (pProperties->slotTypeElementValue() == 4/*tome slot type*/)
					{
						g_vecAbilityPositionOverride = { g_pPlayer->x(), -g_pPlayer->y() };
						g_bForceUseAbility = true;
					}
				}
			}
		});

	Peer::On<PingPayload>(CmdPing, [](uint32_t uSender, const PingPayload& ping)
		{
			Console::Log("Ping from slot %u (ts=%u) -> Pong", uSender, ping.nTimestamp);

			PingPayload pong;
			pong.nTimestamp = GetCurrentMillis();
			Peer::Send(CmdPong, pong, TargetSlot(uSender));
		});

	Peer::On<PingPayload>(CmdPong, [](uint32_t uSender, const PingPayload& pong)
		{
			long long nTimeDelta = GetCurrentMillis() - pong.nTimestamp;
			Console::Log("Pong from slot %u (RTT ~%u ms)", uSender, nTimeDelta);
		});

	Peer::SetDefaultHandler([](uint32_t uSender, uint32_t uType, const void*, uint32_t)
		{
			Console::Log("Unhandled command 0x%04X from slot %u", uType, uSender);
		});
}

void Update(SDK::Player* player)
{
	Peer::UpdateSpeed(player->_speed());
	Peer::UpdateObjectId(player->objectId());
	Peer::UpdateWorldId(GetGameId());
	Peer::UpdateSlowed(player->HasCondition(SDK::Slowed));
	Peer::UpdateConnectedInGame(g_pMapViewService->playerExist());

	if (g_bShouldRequestTome)
	{
		long long nCurrentTime = GetCurrentMillis();
		float flHpPerc = float(player->hp()) / float(player->maxHp());
		if (flHpPerc <= g_flRequestTomeThreshold && nCurrentTime - g_nLastTomeRequest > REQUEST_TOME_INTERVAL_MS)
		{
			Peer::RequestTome();
			g_nLastTomeRequest = nCurrentTime;
		}
	}
}

void LeaderTick(SDK::Player* player)
{
	int nWorldId = GetGameId();

	Peer::BeginStateUpdate();
	{
		auto& s = Peer::State();

		s.posX = player->x();
		s.posY = player->y();

		s.shouldShoot = g_bIsShooting;
		s.shootX = g_vecShootPosition.x;
		s.shootY = g_vecShootPosition.y;

		s.leaderWorldId = nWorldId;
		std::string strServerAddress = SDK::FromSystemString(g_pMapViewService->_server()->address);
		strncpy_s(s.leaderServerAddress, strServerAddress.c_str(), strServerAddress.size() + 1);

		s.leaderObjectId = player->objectId();
		std::string strLeaderName = SDK::FromSystemString(player->_name());
		strncpy_s(s.leaderName, strLeaderName.c_str(), strLeaderName.size() + 1);

		s.allowCrossWorldConnections = false;

		s.bShouldRequestTome = true;
		s.flRequestTomeThreshold = 0.5f;
		memset(s.bNotifyBagTypes, 0, sizeof(s.bNotifyBagTypes));
		s.bShowBagContents = g_bIsShowingBagContents;
		s.bInteract = g_bIsInteracting;
		g_bIsInteracting = false;
	}
	Peer::EndStateUpdate();

	int32_t nSpeed = Peer::GetMinPeerSpeed(nWorldId);
	if (Peer::IsAnyPeerSlowed(nWorldId))
	{
		nSpeed = 10;
	}
	player->_speed() = nSpeed;

	long long nCurrentTime = GetCurrentMillis();
	if (nCurrentTime - g_nLastPurge > PURGE_INTERVAL_MS)
	{
		Peer::PurgeStale();
		Peer::RefreshPeerCount();
		g_nLastPurge = nCurrentTime;
	}
}

void FollowerTick(SDK::Player* player)
{
	LeaderBroadcastState state;
	if (Peer::PollState(state) && state.leaderWorldId == GetGameId())
	{
		float flDeltaX = state.posX - player->x();
		float flDeltaY = state.posY - player->y();
		float flDist = sqrtf(flDeltaX * flDeltaX + flDeltaY * flDeltaY);
		if (flDist < 5.f)
		{
			if (flDist > 1.5f)
			{
				constexpr float stepSize = 0.1f;
				state.posX = player->x() + (flDeltaX / flDist) * stepSize;
				state.posY = player->y() + (flDeltaY / flDist) * stepSize;
			}
			player->MoveTo()(state.posX, state.posY);
		}

		if (g_bForceShoot && !state.shouldShoot && !g_bIsShooting)
		{
			g_bStopShoot = true;
		}
		g_bForceShoot = state.shouldShoot;
		g_vecShootPositionOverride = { state.shootX, state.shootY };

		if (g_bForceShowBagContents && !state.bShowBagContents && !g_bIsShowingBagContents)
		{
			g_bStopShowBagContents = true;
		}
		g_bForceShowBagContents = state.bShowBagContents;

		if (state.bInteract)
		{
			g_nForceInteractUntil = GetCurrentMillis() + 200;
		}
		g_bShouldRequestTome = state.bShouldRequestTome;
		g_flRequestTomeThreshold = state.flRequestTomeThreshold;
		memcpy(g_bNotifyBagTypes, state.bNotifyBagTypes, sizeof(g_bNotifyBagTypes));
	}
	else
	{
		if (g_bForceShoot)
		{
			g_bForceShoot = false;
			if (!g_bIsShooting)
			{
				g_bStopShoot = true;
			}
		}
	}

	long long nCurrentTime = GetCurrentMillis();
	if (nCurrentTime - g_nLastPurge > PURGE_INTERVAL_MS)
	{
		Peer::WatchdogCheckLeader();
		g_nLastPurge = nCurrentTime;
	}
}

void PrintStatus()
{
	long long nCurrentTime = GetCurrentMillis();
	if (nCurrentTime - g_nLastStatus < STATUS_INTERVAL_MS)
	{
		return;
	}
	g_nLastStatus = nCurrentTime;

	bool bIsLeader = Peer::IsLeader();
	int32_t nPeers = Peer::GetPeerCount();
	int32_t nLeaderSlot = Peer::GetLeaderSlot();

	Console::Log("Status: %s | leader=slot %d | peers=%d",
		bIsLeader ? "LEADER" : "FOLLOWER", nLeaderSlot, nPeers);
}

#pragma endregion



#pragma region Hooks

void* Player$$UpdateOriginal = nullptr;
bool __stdcall Player$$Update(SDK::Player* thisptr, int time, int elapsed, const MethodInfo* method)
{
	g_pMapViewService = thisptr->_mapViewService();

	bool bResult = AsOriginal(Player$$Update)(thisptr, time, elapsed, method);
	if (!bResult || !thisptr->isMe())
	{
		return bResult;
	}

	g_pPlayer = thisptr;

	bool bIsLeader = Peer::IsLeader();
	if (bIsLeader != g_bWasLeader)
	{
		Console::Log("Role changed: %s -> %s",
			g_bWasLeader ? "LEADER" : "FOLLOWER",
			bIsLeader ? "LEADER" : "FOLLOWER");
		g_bWasLeader = bIsLeader;
	}

	Update(thisptr);

	if (bIsLeader)
	{
		LeaderTick(thisptr);
	}
	else
	{
		FollowerTick(thisptr);
	}

	PrintStatus();

	return bResult;
}

bool g_bInShooting = false;

void* InputManager$$ShootingOriginal = nullptr;
void __stdcall InputManager$$Shooting(SDK::InputManager* thisptr, const MethodInfo* method)
{
	g_bInShooting = true;
	AsOriginal(InputManager$$Shooting)(thisptr, method);
	g_bInShooting = false;
}

void* InputManager$$GetMousePositionOriginal = nullptr;
SDK::Vector2 __stdcall InputManager$$GetMousePosition(SDK::InputManager* thisptr, const MethodInfo* method)
{
	SDK::Vector2 vecMousePosition = AsOriginal(InputManager$$GetMousePosition)(thisptr, method);

	if (g_bForceUseAbility)
	{
		vecMousePosition.x = g_vecAbilityPositionOverride.x;
		vecMousePosition.y = g_vecAbilityPositionOverride.y;
	}

	return vecMousePosition;
}

void* UnityEngine_Input$$GetKeyOriginal = nullptr;
bool UnityEngine_Input$$GetKey(int nKey, const MethodInfo* method)
{
	bool bIsKeyPressed = AsOriginal(UnityEngine_Input$$GetKey)(nKey, method);

	if (!g_pSettingsManager)
	{
		g_pSettingsManager = g_pApplicationManager->_settingsManager();
	}

	if (g_bInShooting)
	{
		if (nKey == g_pSettingsManager->GetKeyCode()(SDK::UseWeaponAttack))
		{
			g_bIsShooting = bIsKeyPressed;
			if (g_bForceShoot)
			{
				bIsKeyPressed = true;
			}
		}
	}

	if (nKey == g_pSettingsManager->GetKeyCode()(SDK::InteractWithGameview))
	{
		g_bIsShowingBagContents = bIsKeyPressed;
		if (g_bForceShowBagContents)
		{
			g_bStopShowBagContents = false;
			bIsKeyPressed = true;
		}
	}

	return bIsKeyPressed;
}

void* UnityEngine_Input$$GetKeyDownOriginal = nullptr;
bool UnityEngine_Input$$GetKeyDown(int nKey, const MethodInfo* method)
{
	bool bIsKeyDown = AsOriginal(UnityEngine_Input$$GetKeyDown)(nKey, method);

	if (!g_pSettingsManager)
	{
		g_pSettingsManager = g_pApplicationManager->_settingsManager();
	}

	if (g_bInShooting)
	{
		if (nKey == g_pSettingsManager->GetKeyCode()(SDK::UseSpecialAbility))
		{
			if (bIsKeyDown && !g_bIsUsingAbility && Peer::IsLeader())
			{
				SDK::Vector2 vecMousePosition = g_pApplicationManager->_inputManager()->GetMousePosition()();
				Peer::UseAbility(vecMousePosition.x, vecMousePosition.y);
			}
			g_bIsUsingAbility = bIsKeyDown;
			if (g_bForceUseAbility)
			{
				bIsKeyDown = true;
			}
		}
	}

	if (nKey == g_pSettingsManager->GetKeyCode()(SDK::InteractWithGameview))
	{
		g_bIsShowingBagContents = bIsKeyDown;
		if (g_bForceShowBagContents)
		{
			g_bStopShowBagContents = false;
			bIsKeyDown = true;
		}
	}

	if (nKey == g_pSettingsManager->GetKeyCode()(SDK::InteractBuy))
	{
		if (bIsKeyDown)
		{
			g_bIsInteracting = true;
		}

		if (g_nForceInteractUntil != 0)
		{
			if (GetCurrentMillis() <= g_nForceInteractUntil)
			{
				bIsKeyDown = true;
			}
			else
			{
				g_nForceInteractUntil = 0;
			}
		}
	}

	if (nKey == g_pSettingsManager->GetKeyCode()(SDK::EscapeToNexus))
	{
		if (bIsKeyDown && !g_bIsEscapingToNexus && Peer::IsLeader())
		{
			Peer::Nexus();
		}

		g_bIsEscapingToNexus = bIsKeyDown;
		if (g_bShouldEscapeToNexus)
		{
			g_bShouldEscapeToNexus = false;
			bIsKeyDown = true;
		}
	}

	return bIsKeyDown;
}

void* UnityEngine_Input$$GetKeyUpOriginal = nullptr;
bool UnityEngine_Input$$GetKeyUp(int nKey, const MethodInfo* method)
{
	bool bIsKeyUp = AsOriginal(UnityEngine_Input$$GetKeyUp)(nKey, method);

	if (!g_pSettingsManager)
	{
		g_pSettingsManager = g_pApplicationManager->_settingsManager();
	}

	if (g_bInShooting)
	{
		if (nKey == g_pSettingsManager->GetKeyCode()(SDK::UseWeaponAttack))
		{
			if (g_bStopShoot)
			{
				g_bStopShoot = false;
				bIsKeyUp = true;
			}
		}

		if (nKey == g_pSettingsManager->GetKeyCode()(SDK::UseSpecialAbility))
		{
			if (g_bForceUseAbility)
			{
				g_bForceUseAbility = false;
				bIsKeyUp = true;
			}
		}
	}

	if (nKey == g_pSettingsManager->GetKeyCode()(SDK::InteractWithGameview))
	{
		if (g_bStopShowBagContents)
		{
			bIsKeyUp = true;
		}
	}

	return bIsKeyUp;
}

void* UnityEngine_Input$$GetMousePositionOriginal = nullptr;
SDK::Vector3* UnityEngine_Input$$GetMousePosition(SDK::Vector3* retvalue, const MethodInfo* method)
{
	SDK::Vector3* pMouseWorldPosition = AsOriginal(UnityEngine_Input$$GetMousePosition)(retvalue, method);

	if (g_bInShooting)
	{
		g_vecShootPosition = { pMouseWorldPosition->x, pMouseWorldPosition->y };
		if (g_bForceShoot)
		{
			pMouseWorldPosition->x = g_vecShootPositionOverride.x;
			pMouseWorldPosition->y = g_vecShootPositionOverride.y;
		}
	}

	return pMouseWorldPosition;
}

LRESULT CALLBACK HookedWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_ACTIVATEAPP && wParam == TRUE)
	{
		if (Peer::IsJoined() && !Peer::IsLeader())
		{
			Peer::ClaimLeadership();
			Console::Log("WM_ACTIVATEAPP -> claimed leadership");
		}
	}
	if (uMsg == WM_DESTROY)
	{
		if (Peer::IsJoined())
		{
			Peer::Leave();
			Console::Log("WM_DESTROY -> left hive");
		}
	}

	return CallWindowProcA(g_pOriginalWndProc, hWnd, uMsg, wParam, lParam);
}

#pragma endregion

inline void HookWndProc()
{
	if (g_hGameWindow)
	{
		return;
	}

	long long m_nWindowSearchStartTime = GetCurrentMillis();
	g_hGameWindow = FindMainWindow(GetCurrentProcessId());
	while (!g_hGameWindow)
	{
		if (GetCurrentMillis() - m_nWindowSearchStartTime > 10000)
		{
			MessageBoxA(NULL, "Failed To Find Window In Time", "Error", MB_ICONERROR | MB_OK);
			exit(3);
		}

		Sleep(100);
		g_hGameWindow = FindMainWindow(GetCurrentProcessId());
	}

	g_pOriginalWndProc = (WNDPROC)SetWindowLongPtrA(g_hGameWindow, GWLP_WNDPROC, (LONG_PTR)HookedWndProc);
	Console::Log("WndProc hooked (HWND=%p)", g_hGameWindow);
}

inline void UnhookWndProc()
{
	if (g_hGameWindow && g_pOriginalWndProc)
	{
		SetWindowLongPtrA(g_hGameWindow, GWLP_WNDPROC, (LONG_PTR)g_pOriginalWndProc);
		Console::Log("WndProc unhooked");
		g_pOriginalWndProc = nullptr;
		g_hGameWindow = NULL;
	}
}

DWORD WINAPI MainThread(LPVOID)
{
	RegisterHandlers();

	char szName[32];
	sprintf_s(szName, "Peer_%u", GetCurrentProcessId());

	if (!Peer::Join(szName, GetCurrentProcessId()))
	{
		Console::Log("Failed to join hive!");
		return 1;
	}

	Console::Log("Joined hive as '%s' (slot %d)", szName, Peer::GetMySlotIndex());

	if (Peer::IsLeader())
	{
		Console::Log("First peer, auto claimed leadership");
		g_bWasLeader = true;
	}

	InitializeInterfaces();

	{
		Il2Cpp::ThreadAttacher pThread;
		while (!SDK::ApplicationManager::GetExists()())
		{
			Sleep(100);
		}

		g_pApplicationManager = SDK::ApplicationManager::GetInstance()();
	}

	HookWndProc();

	if (MH_Initialize() != MH_OK)
	{
		MessageBoxA(NULL, "Failed To Initialize MinHook", "Error", MB_ICONERROR | MB_OK);
		exit(1);
	}

	CREATEHOOK(Player$$Update, SDK::Player::UpdateMethodInfo()->methodPointer);

	CREATEHOOK(InputManager$$Shooting, SDK::InputManager::ShootingMethodInfo()->methodPointer);
	CREATEHOOK(InputManager$$GetMousePosition, SDK::InputManager::GetMousePositionMethodInfo()->methodPointer);

	CREATEHOOK(UnityEngine_Input$$GetKey, SDK::Input::GetKeyMethodInfo()->methodPointer);
	CREATEHOOK(UnityEngine_Input$$GetKeyDown, SDK::Input::GetKeyDownMethodInfo()->methodPointer);
	CREATEHOOK(UnityEngine_Input$$GetKeyUp, SDK::Input::GetKeyUpMethodInfo()->methodPointer);
	CREATEHOOK(UnityEngine_Input$$GetMousePosition, SDK::Input::GetMousePositionMethodInfo()->methodPointer);

	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
	{
		MessageBoxA(NULL, "Failed To Enable Hooks", "Error", MB_ICONERROR | MB_OK);
		exit(4);
	}

	return 0;
}

BOOL APIENTRY DllMain(HMODULE hModule, DWORD dwReason, LPVOID)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		DisableThreadLibraryCalls(hModule);
		Console::Init();

		g_bRunning = true;
		g_hMainThread = CreateThread(NULL, 0, MainThread, NULL, 0, NULL);

		if (!g_hMainThread)
		{
			Console::Log("Failed to create main thread");
			Console::Shutdown();
			return FALSE;
		}

		Console::Log("DLL loaded (PID %u)", GetCurrentProcessId());
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		g_bRunning = false;

		if (g_hMainThread)
		{
			WaitForSingleObject(g_hMainThread, 3000);
			CloseHandle(g_hMainThread);
			g_hMainThread = NULL;
		}

		UnhookWndProc();
		Console::Shutdown();
	}

	return TRUE;
}
