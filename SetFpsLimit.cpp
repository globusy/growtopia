HANDLE phandle;
void SetFpsLimitExternal(float fps_limit) {
    if (!phandle) {
        std::cout << "Invalid process handle!\n";
        return;
    }

    uintptr_t function_address = 0x140D1EBE0; // address of BaseApp::SetFpsLimit function

    uintptr_t allocated_memory = (uintptr_t)VirtualAllocEx(phandle, nullptr, sizeof(float), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!allocated_memory) {
        std::cout << "Memory allocation failed\n";
        return;
    }

    if (!WriteProcessMemory(phandle, (LPVOID)allocated_memory, &fps_limit, sizeof(float), nullptr)) {
        std::cout << "Failed to write FPS limit to memory\n";
        VirtualFreeEx(phandle, (LPVOID)allocated_memory, 0, MEM_RELEASE);
        return;
    }

    HANDLE hThread = CreateRemoteThread(phandle, nullptr, 0, (LPTHREAD_START_ROUTINE)function_address, (LPVOID)allocated_memory, 0, nullptr);

    if (!hThread) {
        std::cout << "Failed to create remote thread\n";
    }
    else {
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
    }

    VirtualFreeEx(game->phandle, (LPVOID)allocated_memory, 0, MEM_RELEASE);
}
