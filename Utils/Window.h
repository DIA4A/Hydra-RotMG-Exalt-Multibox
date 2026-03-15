#pragma once
#include <Windows.h>

struct WindowHandleQueryData
{
	DWORD m_dwProcessId = 0;
	HWND m_hWindowHandle = NULL;
};

__declspec(noinline) inline BOOL CALLBACK EnumWindowsCallback(HWND hWindowHandle, LPARAM lParam)
{
	WindowHandleQueryData& m_pWindowHandleQuery = *(WindowHandleQueryData*)lParam;

	DWORD m_dwProcessId = 0;
	GetWindowThreadProcessId(hWindowHandle, &m_dwProcessId);

	if (m_pWindowHandleQuery.m_dwProcessId != m_dwProcessId || GetWindow(hWindowHandle, GW_OWNER) != (HWND)0 || hWindowHandle == GetConsoleWindow())
	{
		return TRUE;
	}

	m_pWindowHandleQuery.m_hWindowHandle = hWindowHandle;
	return FALSE;
}

inline HWND FindMainWindow(const DWORD& dwProcessId)
{
	WindowHandleQueryData pWindowHandleQuery;
	pWindowHandleQuery.m_dwProcessId = dwProcessId;
	pWindowHandleQuery.m_hWindowHandle = NULL;

	EnumWindows(EnumWindowsCallback, (LPARAM)&pWindowHandleQuery);
	return pWindowHandleQuery.m_hWindowHandle;
}