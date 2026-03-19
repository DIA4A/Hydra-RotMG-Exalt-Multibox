#pragma once
#include <HydraPeer.h>

struct alignas(64) LeaderBroadcastState
{
	// TODO: move this back into HydraPeer
	volatile long sequence; // seqlock: odd = writing, even = stable

	float posX, posY;

	bool shouldShoot;
	float shootX, shootY;

	uint32_t leaderWorldId;
	char leaderServerAddress[64];
	int32_t leaderObjectId;
	char leaderName[64];
	bool allowCrossWorldConnections;

	bool bShouldRequestTome = false;
	float flRequestTomeThreshold = 0.f;
	bool bNotifyBagTypes[6] = {};
	bool bShowBagContents = false;
	bool bInteract = false;
	int32_t nOptimizationMode = 0;
};

struct alignas(64) PeerState
{
	float speed;
	int32_t objectId;
	uint32_t worldId;
	volatile long isSlowed;
	volatile long isConnectedInGame;
	int32_t inventory[28];
	int32_t backpackSlots;
};

	enum PoCCommandType : uint32_t
	{
		CmdNexus = HydraIPC::CmdUserDefined,
		CmdUsePortal,
		CmdCurrentWorld,
		CmdIngameDisconnect,
		CmdForceNexus,
		CmdChangeServer,
		CmdServerIPConnect,
		CmdUnplug,
		CmdLoadConfig,
		CmdRunCommand,
		CmdChatMessage,
		CmdFollowTarget,
		CmdRequestTome,
		CmdSwapout,
		CmdUseAbility,
		CmdInformObjectId,
		CmdBagDrop,
		CmdPing,
		CmdPong
	};


namespace Cmd
{
	struct UsePortal
	{
		int32_t nObjectId = 0;
	};

	struct CurrentWorld
	{
		uint32_t uWorldId = 0;
	};

	struct ChangeServer
	{
		int32_t nServerIndex = 0;
	};

	struct ServerIPConnect
	{
		char szServerIP[64] = {};
	};

	struct Unplug
	{
		bool bEnable = false;
	};

	struct LoadConfig
	{
		int32_t nConfigIndex = 0;
	};

	struct RunCommand
	{
		char szCommand[512] = {};
	};

	struct ChatMessage
	{
		char szMessage[256] = {};
	};

	struct FollowTarget
	{
		int32_t nObjectId = 0;
	};

	struct Swapout
	{
		int32_t nSwapToObjectType = -1;
	};

	struct InformObjectId
	{
		int32_t nObjectId = 0;
	};

	struct UseAbility
	{
		float flX = 0.f;
		float flY = 0.f;
	};

	struct BagDrop
	{
		int32_t nBagType;
		int32_t nBagObjectType;
		int32_t nItems[8];
		float flX, flY;
	};

struct Ping
{
	long long nTimestamp;
};

}


struct MultiboxPeer : public HydraIPC::Peer<LeaderBroadcastState, PeerState>
{
#pragma region Command Wrappers

	inline void Nexus(HydraIPC::PeerMask uTargetMask = HydraIPC::TargetAll)
	{
		Send(CmdNexus, uTargetMask);
	}

	inline void UsePortal(int32_t nObjectId, HydraIPC::PeerMask uTargetMask = HydraIPC::TargetAll)
	{
		Cmd::UsePortal cmdUsePortal;
		cmdUsePortal.nObjectId = nObjectId;
		Send(CmdUsePortal, cmdUsePortal, uTargetMask);
	}

	inline void CurrentWorld(uint32_t uWorldId, HydraIPC::PeerMask uTargetMask = HydraIPC::TargetAll)
	{
		Cmd::CurrentWorld cmdCurrentWorld;
		cmdCurrentWorld.uWorldId = uWorldId;
		Send(CmdCurrentWorld, cmdCurrentWorld, uTargetMask);
	}

	inline void ForceNexus(HydraIPC::PeerMask uTargetMask = HydraIPC::TargetAll)
	{
		Send(CmdForceNexus, uTargetMask);
	}

	inline void IngameDisconnect(HydraIPC::PeerMask uTargetMask = HydraIPC::TargetAll)
	{
		Send(CmdIngameDisconnect, uTargetMask);
	}

	inline void ChangeServer(int32_t nServerIndex, HydraIPC::PeerMask uTargetMask = HydraIPC::TargetAll)
	{
		Cmd::ChangeServer cmdChangeServer;
		cmdChangeServer.nServerIndex = nServerIndex;
		Send(CmdChangeServer, cmdChangeServer, uTargetMask);
	}

	inline void ServerIPConnect(const char* szIP, HydraIPC::PeerMask uTargetMask = HydraIPC::TargetAll)
	{
		Cmd::ServerIPConnect cmdServerIPConnect;
		strncpy_s(cmdServerIPConnect.szServerIP, szIP, sizeof(cmdServerIPConnect.szServerIP) - 1);
		Send(CmdServerIPConnect, cmdServerIPConnect, uTargetMask);
	}

	inline void SetUnplug(bool bEnable, HydraIPC::PeerMask uTargetMask = HydraIPC::TargetAll)
	{
		Cmd::Unplug cmdUnplug;
		cmdUnplug.bEnable = bEnable;
		Send(CmdUnplug, cmdUnplug, uTargetMask);
	}

	inline void LoadConfig(int32_t nConfigIndex, HydraIPC::PeerMask uTargetMask = HydraIPC::TargetAll)
	{
		Cmd::LoadConfig cmdLoadConfig;
		cmdLoadConfig.nConfigIndex = nConfigIndex;
		Send(CmdLoadConfig, cmdLoadConfig, uTargetMask);
	}

	inline void RunCommand(const char* szCommand, HydraIPC::PeerMask uTargetMask = HydraIPC::TargetAll)
	{
		Cmd::RunCommand cmdRunCommand;
		strncpy_s(cmdRunCommand.szCommand, szCommand, sizeof(cmdRunCommand.szCommand) - 1);
		Send(CmdRunCommand, cmdRunCommand, uTargetMask);
	}

	inline void ChatMessage(const char* szMessage, HydraIPC::PeerMask uTargetMask = HydraIPC::TargetAll)
	{
		Cmd::ChatMessage cmdChatMessage;
		strncpy_s(cmdChatMessage.szMessage, szMessage, sizeof(cmdChatMessage.szMessage) - 1);
		Send(CmdChatMessage, cmdChatMessage, uTargetMask);
	}

	inline void FollowTarget(int32_t nObjectId, HydraIPC::PeerMask uTargetMask = HydraIPC::TargetAll)
	{
		Cmd::FollowTarget cmdFollowTarget;
		cmdFollowTarget.nObjectId = nObjectId;
		Send(CmdFollowTarget, cmdFollowTarget, uTargetMask);
	}

	inline void RequestTome(HydraIPC::PeerMask uTargetMask = HydraIPC::TargetAll)
	{
		Send(CmdRequestTome, uTargetMask);
	}

	inline void Swapout(int32_t nSwapToObjectType, HydraIPC::PeerMask uTargetMask = HydraIPC::TargetAll)
	{
		Cmd::Swapout cmdSwapout;
		cmdSwapout.nSwapToObjectType = nSwapToObjectType;
		Send(CmdSwapout, cmdSwapout, uTargetMask);
	}

	inline void UseAbility(float flX, float flY, HydraIPC::PeerMask uTargetMask = HydraIPC::TargetAll)
	{
		Cmd::UseAbility cmdUseAbility;
		cmdUseAbility.flX = flX;
		cmdUseAbility.flY = flY;
		Send(CmdUseAbility, cmdUseAbility, uTargetMask);
	}

	inline void SendObjectId(int32_t nObjectId, HydraIPC::PeerMask uTargetMask = HydraIPC::TargetAll)
	{
		Cmd::InformObjectId cmdInformObjectId;
		cmdInformObjectId.nObjectId = nObjectId;
		Send(CmdInformObjectId, cmdInformObjectId, uTargetMask);
	}

#pragma endregion

#pragma region Peer State Writing

	inline void UpdateSpeed(float fSpeed)
	{
		auto* pSlot = GetMySlot();
		if (pSlot)
		{
			pSlot->state.speed = fSpeed;
		}
	}

	inline void UpdateObjectId(int32_t nObjectId)
	{
		auto* pSlot = GetMySlot();
		if (pSlot)
		{
			pSlot->state.objectId = nObjectId;
		}
	}

	inline void UpdateWorldId(uint32_t uWorldId)
	{
		auto* pSlot = GetMySlot();
		if (pSlot)
		{
			pSlot->state.worldId = uWorldId;
		}
	}

	inline void UpdateSlowed(bool bIsSlowed)
	{
		auto* pSlot = GetMySlot();
		if (pSlot)
		{
			InterlockedExchange(&pSlot->state.isSlowed, bIsSlowed ? 1 : 0);
		}
	}

	inline void UpdateConnectedInGame(bool bConnected)
	{
		auto* pSlot = GetMySlot();
		if (pSlot)
		{
			InterlockedExchange(&pSlot->state.isConnectedInGame, bConnected ? 1 : 0);
		}
	}

	inline void UpdateInventory(const int32_t* pInventory, int32_t nCount, int32_t nBackpackSlots)
	{
		auto* pSlot = GetMySlot();
		if (!pSlot)
		{
			return;
		}
		int32_t nCopyCount = nCount < 28 ? nCount : 28;
		memcpy(pSlot->state.inventory, pInventory, nCopyCount * sizeof(int32_t));
		pSlot->state.backpackSlots = nBackpackSlots;
	}

#pragma endregion

#pragma region Peer State Utility

	inline int32_t GetMinPeerSpeed(uint32_t uWorldId)
	{
		int32_t nMinSpeed = 1000;
		ForEachPeer([&](int, auto& slot) -> bool
			{
				if (slot.state.worldId != uWorldId)
				{
					return true;
				}
				int32_t nSpeed = (int32_t)slot.state.speed;
				if (nSpeed == 0)
				{
					nSpeed = 10;
				}
				if (nSpeed < nMinSpeed)
				{
					nMinSpeed = nSpeed;
				}
				return true;
			});
		return nMinSpeed;
	}

	inline bool IsAnyPeerSlowed(uint32_t uWorldId)
	{
		bool bSlowed = false;
		ForEachPeer([&](int, auto& slot) -> bool
			{
				if (slot.state.worldId == uWorldId && slot.state.isSlowed)
				{
					bSlowed = true;
					return false;
				}
				return true;
			});
		return bSlowed;
	}

	inline void SnakeFollow(int32_t nServerObjectId)
	{
		int32_t nPrevObjectId = nServerObjectId;
		ForEachFollower([&](int nSlotIndex, auto& slot) -> bool
			{
				Cmd::FollowTarget cmdFollowTarget;
				cmdFollowTarget.nObjectId = nPrevObjectId;
				Send(CmdFollowTarget, cmdFollowTarget, HydraIPC::TargetSlot(nSlotIndex));
				nPrevObjectId = slot.state.objectId;
				return true;
			});
	}

#pragma endregion
};


inline MultiboxPeer g_Peer;

