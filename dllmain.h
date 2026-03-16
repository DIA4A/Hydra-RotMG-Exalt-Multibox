#pragma once
#include <HydraPeer.h>

struct alignas(64) LeaderBroadcastState
{
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

inline HydraIPC::Peer<LeaderBroadcastState, PeerState> g_Peer;

