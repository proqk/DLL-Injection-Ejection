//홈페이지 다운로드하는 dll

#include <stdio.h>
#include <windows.h>

#pragma comment(lib, "urlmon.lib")
#define DEF_MYBLOG_ADDR ("http://foxtrotin.tistory.com/")
#define DEF_INDEX_PATH  ("c:\\index.html")

//urlmon!URLDownloadToFile() API를 호출하여 foxtrotin.tistory.com을 다운받는다
DWORD WINAPI ThreadProc(LPVOID lParam)
{
	URLDownloadToFile(NULL, DEF_MYBLOG_ADDR, DEF_INDEX_PATH, 0, NULL);
	return 0;
}

//프로세스에 DLL인젝션이 발생하면 DLLMain() 호출
BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	HANDLE hThread = NULL;

	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH: //DLL이 로딩될 때
		hThread = CreateThread(NULL, 0, ThreadProc, NULL, 0, NULL); //ThreadProc 스레드를 실행한다
		CloseHandle(hThread);
	}

	return TRUE;
}
