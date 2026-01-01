UINT myWindowMessageCode = 0;
extern "C" __declspec(dllexport) LRESULT __stdcall GetMsgHook(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)
    {
        MSG* msg = reinterpret_cast<MSG*>(lParam);
        if (msg->message == myWindowMessageCode)
        {
            // Do things..
        }
    }

    return CallNextHookEx(0, nCode, wParam, lParam);
}

void DllAttached()
{
    myWindowMessageCode = RegisterWindowMessageW(L"myWindowMessage");
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
    switch (fdwReason)
    {
    case DLL_PROCESS_ATTACH:
    {
        DllAttached();
        return true;
    }
    case DLL_PROCESS_DETACH:
    {
        // ...
        return true;
    }
    }

    return true;
}