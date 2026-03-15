#pragma once
#include <iostream>
#include <Windows.h>

namespace Console
{
	void Init();

	void Log(const char* szFormat, ...);

	void Shutdown();
}