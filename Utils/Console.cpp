#include "Console.h"
#include "../dllmain.h"
#include <HydraPeer.h>

namespace Console
{
	void Init()
	{
		AllocConsole();
		FILE* pFile = nullptr;
		freopen_s(&pFile, "CONOUT$", "w", stdout);
		freopen_s(&pFile, "CONIN$", "r", stdin);
		SetConsoleTitleA("Hydra IPC PoC");
	}

	void Log(const char* szFormat, ...)
	{
		int32_t nSlot = g_Peer.GetMySlotIndex();
		if (nSlot >= 0)
		{
			printf("[Peer %d] ", nSlot);
		}
		else
		{
			printf("[Peer ?] ");
		}

		va_list args;
		va_start(args, szFormat);
		vprintf(szFormat, args);
		va_end(args);
		printf("\n");
	}

	void Shutdown()
	{
		FreeConsole();
	}
}