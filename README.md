# another-way-to-communicate

Creating named shared memory & pipes can be a hassle to setup right for interprocess communications on Windows.
It might also be overkill to set them up for the amount of data through-put you might have or it's one-way communication to the target process.

A pretty common injection technique is utilizing SetWindowsHookEx, more information on that here https://learn.microsoft.com/en-us/windows/win32/winmsg/using-hooks
So we already have a channel available where we can technically receive window message communication from.

With RegisterWindowMessage we can register a new window message code that is unique throughout the system, more information on that function here https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-registerwindowmessagew

Combining the two things we can have a one-way communication channel to our injected module in the target process.

The catch is now the following, we technically only have 16 bytes available to us to send information to our injected module in the target process.
wParam and lParam allows us each to send 8 bytes of data via PostMessage, more information here https://learn.microsoft.com/en-us/windows/win32/api/winuser/nf-winuser-postmessagew

You could register multiple window message codes for individual messages you might wanna transmit to your injected module.
Or you split those 16 bytes up into individual bits, little example below.

```cpp
constexpr uint32_t MESSAGE_BITS = 2;
constexpr uint32_t MESSAGE_COUNT = (1 << MESSAGE_BITS) - 1;
enum class eMessages : uint32_t
{
    HELLO = 0,
    INVOKECOMMAND,

    _COUNT
};
static_assert(static_cast<uint32_t>(eMessages::_COUNT) <= MESSAGE_COUNT, "Budget exceeded.");

struct WindowMessage
{
    eMessages msg;
};

static inline void ReadWindowMessage(CBitBuf* const buf, WindowMessage* const msg)
{
    buf->readBits(&msg->msg, MESSAGE_BITS);
}

static inline void WriteWindowMessage(CBitBuf* const buf, WindowMessage* const msg)
{
    buf->writeBits(&msg->msg, MESSAGE_BITS);
}

static void RunWindowCommand(MSG* const msg)
{
    CBitBuf buf;
    buf.setBuffer(&msg->wParam);
    buf.setBufferSize(sizeof(msg->wParam));

    WindowMessage windowMsg;
    ReadWindowMessage(&buf, &windowMsg);

    // ...
}

extern "C" __declspec(dllexport) LRESULT __stdcall GetMsgHook(int nCode, WPARAM wParam, LPARAM lParam)
{
    if (nCode >= 0)
    {
        MSG* const msg = reinterpret_cast<MSG*>(lParam);
        if (msg->message == wmCode)
        {
            CBitBuf buf;
            buf.setBuffer(&msg->wParam);
            buf.setBufferSize(sizeof(msg->wParam));

            WindowMessage windowMsg{};
            ReadWindowMessage(&buf, &windowMsg);
            // ...
        }
    }

    return CallNextHookEx(0, nCode, wParam, lParam);
}

void SendHelloWindowCommand()
{
    WindowMessage windowMsg;
    windowMsg.msg = eMessages::HELLO;

    WPARAM wParam;

    CBitBuf bitBuf;
    bitBuf.setBuffer(reinterpret_cast<uint8_t*>(&wParam));
    bitBuf.setBufferSize(sizeof(WPARAM));
    bitBuf.writeBits(&cmd.msg, MESSAGE_BITS);

    PostMessage(windowHandle, myWindowMessageCode, wParam, 0);
}
```

***DISCLAIMER***
All code in this repository is just pseudo and is not guranteed to compile.
