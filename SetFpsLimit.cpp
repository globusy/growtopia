void SetFpsLimitExternal(float fps_limit) {
    if (!game->phandle) {
        std::cout << "Invalid process handle!\n";
        return;
    }

    uintptr_t function_address = game->base_address + 0xD1EBE0; // offset of BaseApp::SetFpsLimit function

    uintptr_t allocated_memory = (uintptr_t)VirtualAllocEx(game->phandle, nullptr, sizeof(float), MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE);
    if (!allocated_memory) {
        std::cerr << "Memory allocation failed\n";
        return;
    }

    if (!WriteProcessMemory(game->phandle, (LPVOID)allocated_memory, &fps_limit, sizeof(float), nullptr)) {
        std::cout << "Failed to write FPS limit to memory\n";
        VirtualFreeEx(game->phandle, (LPVOID)allocated_memory, 0, MEM_RELEASE);
        return;
    }

    HANDLE hThread = CreateRemoteThread(game->phandle, nullptr, 0, (LPTHREAD_START_ROUTINE)function_address, (LPVOID)allocated_memory, 0, nullptr);

    if (!hThread) {
        std::cout << "Failed to create remote thread\n";
    }
    else {
        WaitForSingleObject(hThread, INFINITE);
        CloseHandle(hThread);
    }

    VirtualFreeEx(game->phandle, (LPVOID)allocated_memory, 0, MEM_RELEASE);
}
