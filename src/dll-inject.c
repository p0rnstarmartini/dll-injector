#include <windows.h>
#include <stdio.h>

int inject(DWORD pid, const char *dllPath)
{
    HANDLE hProcess;
    LPVOID pDllPath;
    HANDLE hThread;

    hProcess = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pid);
    if (!hProcess)
    {
        printf("Failed to open process\n");
        return 0;
    }

    pDllPath = VirtualAllocEx(
        hProcess,
        NULL,
        strlen(dllPath) + 1,
        MEM_COMMIT | MEM_RESERVE,
        PAGE_READWRITE
    );

    if (!pDllPath)
    {
        printf("Failed to allocate memory\n");
        return 0;
    }

    WriteProcessMemory(
        hProcess,
        pDllPath,
        dllPath,
        strlen(dllPath) + 1,
        NULL
    );

    hThread = CreateRemoteThread(
        hProcess,
        NULL,
        0,
        (LPTHREAD_START_ROUTINE)GetProcAddress(
            GetModuleHandle("kernel32.dll"),
            "LoadLibraryA"
        ),
        pDllPath,
        0,
        NULL
    );

    if (!hThread)
    {
        printf("Failed to create remote thread\n");
        return 0;
    }

    CloseHandle(hThread);
    CloseHandle(hProcess);

    printf("DLL injected successfully\n");
    return 1;
}

int main()
{
    DWORD pid;
    char dllPath[MAX_PATH];

    printf("Enter process ID: ");
    scanf("%lu", &pid);

    printf("Enter DLL path: ");
    scanf("%s", dllPath);

    inject(pid, dllPath);

    return 0;
}
