//메시지창 띄우는 dll

#include <stdio.h>
#include <windows.h>

DWORD WINAPI ShowMessage(LPVOID lParam)
{
	MessageBox(NULL, TEXT("DLL Injection Success!"), TEXT("MessageBox"), MB_OK);
	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	HANDLE hThread = NULL;

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		hThread = CreateThread(NULL, 0, ShowMessage, NULL, 0, NULL);
		CloseHandle(hThread);
	}

	return TRUE;
}
