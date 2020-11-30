#include <stdio.h>
#include <windows.h>
#include <TlHelp32.h>
#include <tchar.h>

int main(int argc, char* argv[])
{
	HANDLE hSnapshot, hProcess, hThread;
	HMODULE hMod;
	DWORD PPid = 0;
	LPVOID DllBuf;
	LPTHREAD_START_ROUTINE hModAddr;
	LPCTSTR DllPath = "injection.dll";

	// 프로세스 정보를 담을 구조체
	PROCESSENTRY32 pe32 = { 0 };
	pe32.dwSize = sizeof(PROCESSENTRY32);

	// 프로세스 정보를 얻음
	hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPALL, 0);
	if (Process32First(hSnapshot, &pe32))
	{
		do
		{
			//대상 프로세스와 같은 프로세스일 경우 PID를 얻음
			if (!_tcsicmp(pe32.szExeFile, TEXT("notepad.exe")))
			{
				PPid = pe32.th32ProcessID;
				printf("Find notepad.exe\nPid : %d\n\n", PPid);
				break;
			}
		} while (Process32Next(hSnapshot, &pe32));
		CloseHandle(hSnapshot);
	}

	// 얻은 PID를 이용해 대상 프로세스인 notepad.exe의 핸들을 구함
	if (!(hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, PPid)))
		printf("OpenProcess Failed\n");
	printf("- Handle : %x\n", hProcess);

	// 대상 프로세스 메모리에 dll 크기만큼 메모리 할당
	if (!(DllBuf = VirtualAllocEx(hProcess, NULL, lstrlen(DllPath) + 1, MEM_COMMIT, PAGE_READWRITE)))
		printf("VirtualAllocEx Failed\n");
	printf("- Virtual Memory : %x\n", DllBuf);

	// 할당한 메모리에 injection.dll 경로를 씀
	if (!(WriteProcessMemory(hProcess, DllBuf, (LPVOID)DllPath, lstrlen(DllPath) + 1, NULL)))
		printf("WriteProcessMemory Failed\n");

	// kernel32.dll 핸들을 얻음
	hMod = GetModuleHandle("kernel32.dll");
	if(!hMod) printf("GetModuleHandle Failed, error code: %d\n", GetLastError());
	printf("- kernel32.dll : %x\n", hMod);

	// kernel32.dll의 LoadLibraryA() API 주소를 구함
	hModAddr = (LPTHREAD_START_ROUTINE)GetProcAddress(hMod, "LoadLibraryA");
	if (!hModAddr) printf("GetModuleHandle Failed, error code: %d\n", GetLastError());
	printf("- kernel32.LoadLibraryA : %x\n", hModAddr);

	// notepad.exe 프로세스에 스레드를 실행
	if (!(hThread = CreateRemoteThread(hProcess, NULL, 0, hModAddr, DllBuf, 0, NULL)))
		printf("CreateRemoteThread Failed, error code: %d\n", GetLastError());
	WaitForSingleObject(hThread, INFINITE);
	printf("- Thread Handle : %x\n\n", hThread);

	CloseHandle(hThread);
	CloseHandle(hProcess);

	system("pause");
	return TRUE;
}
