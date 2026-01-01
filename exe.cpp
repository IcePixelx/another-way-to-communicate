int main(int argc, char* argv[])
{
    UINT myWindowMessageCode = RegisterWindowMessageW(L"myWindowMessage");
    assert(myWindowMessageCode != 0);

    HWND windowHandle = FindWindowW(nullptr, L"myTargetWindow");
    assert(windowHandle != nullptr);

    DWORD tid = GetWindowThreadProcessId(windowHandle, nullptr);
    assert(tid != 0);

    // Don't call DllMain for the module or resolve any other module dependencies, we just need the virtual address for 'GetMsgHook'.
    HMODULE moduleHandle = LoadLibraryExA("myModule.dll", nullptr, LOAD_LIBRARY_SEARCH_APPLICATION_DIR | DONT_RESOLVE_DLL_REFERENCES);
    assert(moduleHandle != nullptr);

    HOOKPROC hookExportAddr = reinterpret_cast<HOOKPROC>(GetProcAddress(moduleHandle, "GetMsgHook"));
    assert(hookExportAddr != nullptr);

    HHOOK hookHandle = SetWindowsHookEx(WH_GETMESSAGE, hookExportAddr, moduleHandle, tid);
    assert(hookHandle != nullptr);

    // Don't need the module anymore, free it.
    FreeLibrary(moduleHandle);

    // Do things..

    // Clean-up hook from target.
    UnhookWindowsHookEx(hookHandle);
    return 0;
}