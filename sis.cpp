#include <windows.h>
#include <string>
#include <ctime>

#define IDT_TIMER 1

std::string GetCurrentDateTime() {
    time_t now = time(0);
    struct tm* localTime = localtime(&now);
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);
    return buffer;
}

void UpdateDisplayedTime(HWND hwnd) {
    std::string dateTime = GetCurrentDateTime();
    SetDlgItemTextA(hwnd, 1, dateTime.c_str());
}

void DisplaySystemInformation(HWND hwnd) {
    OSVERSIONINFO osvi = { sizeof(OSVERSIONINFO) };
    GetVersionEx(&osvi);
    std::string osVersion = "Operational System Version: " + std::to_string(osvi.dwMajorVersion) + "." +
                            std::to_string(osvi.dwMinorVersion);

    char sysDir[MAX_PATH];
    GetSystemDirectory(sysDir, MAX_PATH);
    std::string systemDirectory = "System Directory: " + std::string(sysDir);

    char computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(computerName);
    GetComputerName(computerName, &size);
    std::string computerNameStr = "Computer Name: " + std::string(computerName);

    char userName[MAX_PATH];
    DWORD userNameSize = sizeof(userName);
    GetUserName(userName, &userNameSize);
    std::string userNameStr = "Current User Name: " + std::string(userName);

    std::string message = osVersion + "\n" + systemDirectory + "\n" +
                          computerNameStr + "\n" + userNameStr;

    MessageBox(hwnd, message.c_str(), "System Information", MB_OK | MB_ICONINFORMATION);
}

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        case WM_CREATE: {
            HWND hEdit = CreateWindowA(
                "EDIT",
                NULL,
                WS_CHILD | WS_VISIBLE | WS_BORDER | ES_AUTOHSCROLL | ES_READONLY,
                10, 10, 200, 25,
                hwnd,
                (HMENU)1,
                ((LPCREATESTRUCT)lParam)->hInstance,
                NULL
            );

            UpdateDisplayedTime(hwnd);

            CreateWindowA(
                "BUTTON",
                "Set time",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                10, 45, 100, 25,
                hwnd,
                (HMENU)2,
                ((LPCREATESTRUCT)lParam)->hInstance,
                NULL
            );

            CreateWindowA(
                "BUTTON",
                "System Info",
                WS_CHILD | WS_VISIBLE | BS_PUSHBUTTON,
                120, 45, 150, 25,
                hwnd,
                (HMENU)3,
                ((LPCREATESTRUCT)lParam)->hInstance,
                NULL
            );

            SetTimer(hwnd, IDT_TIMER, 1000, NULL);
            return 0;
        }
        case WM_TIMER: {
            if (wParam == IDT_TIMER) {
                UpdateDisplayedTime(hwnd);
            }
            return 0;
        }
        case WM_COMMAND: {
            if (LOWORD(wParam) == 2) {
                std::string newTime;
                char buffer[80];
                GetDlgItemTextA(hwnd, 1, buffer, sizeof(buffer));
                newTime = buffer;

                SYSTEMTIME st;
                sscanf(newTime.c_str(), "%d-%d-%d %d:%d:%d", &st.wYear, &st.wMonth, &st.wDay, &st.wHour, &st.wMinute, &st.wSecond);

                SetLocalTime(&st);
            }
            else if (LOWORD(wParam) == 3) {
                DisplaySystemInformation(hwnd);
            }
            return 0;
        }
        case WM_DESTROY: {
            KillTimer(hwnd, IDT_TIMER);
            PostQuitMessage(0);
            return 0;
        }
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    const char CLASS_NAME[] = "SampleWindowClass";

    WNDCLASSA wc = {};
    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    RegisterClassA(&wc);

    HWND hwnd = CreateWindowExA(
        0,
        CLASS_NAME,
        "System's data",
        WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 150,
        NULL,
        NULL,
        hInstance,
        NULL
    );

    if (hwnd == NULL) {
        return 0;
    }

    ShowWindow(hwnd, nCmdShow);

    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
