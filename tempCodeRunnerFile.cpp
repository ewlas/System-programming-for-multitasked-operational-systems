#include <windows.h>
#include <string>
#include <ctime>

#define IDT_TIMER 1

// Функція для отримання поточної дати та часу у вигляді рядка
std::string GetCurrentDateTime() {
    // Отримання поточного часу
    time_t now = time(0);
    struct tm* localTime = localtime(&now);

    // Форматування часу у вигляді рядка
    char buffer[80];
    strftime(buffer, sizeof(buffer), "%Y-%m-%d %H:%M:%S", localTime);

    return buffer;
}

// Функція для оновлення відображеного часу
void UpdateDisplayedTime(HWND hwnd) {
    // Отримання поточної дати та часу
    std::string dateTime = GetCurrentDateTime();

    // Оновлення відображеного часу
    SetDlgItemTextA(hwnd, 1, dateTime.c_str());
}

// Функція для відображення системної інформації у вікні повідомлень
void DisplaySystemInformation(HWND hwnd) {
    // Отримання версії операційної системи
    OSVERSIONINFO osvi = { sizeof(OSVERSIONINFO) };
    GetVersionEx(&osvi);
    std::string osVersion = "Operational System Version: " + std::to_string(osvi.dwMajorVersion) + "." +
                            std::to_string(osvi.dwMinorVersion);

    // Отримання каталогу системи
    char sysDir[MAX_PATH];
    GetSystemDirectory(sysDir, MAX_PATH);
    std::string systemDirectory = "System Directory: " + std::string(sysDir);

    // Отримання імені комп'ютера
    char computerName[MAX_COMPUTERNAME_LENGTH + 1];
    DWORD size = sizeof(computerName);
    GetComputerName(computerName, &size);
    std::string computerNameStr = "Computer Name: " + std::string(computerName);

    // Отримання поточного імені користувача
    char userName[MAX_PATH];
    DWORD userNameSize = sizeof(userName);
    GetUserName(userName, &userNameSize);
    std::string userNameStr = "Current User Name: " + std::string(userName);

    // Складання повідомлення з системною інформацією
    std::string message = osVersion + "\n" + systemDirectory + "\n" +
                          computerNameStr + "\n" + userNameStr;

    // Відображення вікна повідомлення
    MessageBox(hwnd, message.c_str(), "System Information", MB_OK | MB_ICONINFORMATION);
}

// Процедура вікна
LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
    switch (uMsg) {
        // Обробка створення вікна
        case WM_CREATE: {
            // Створення поля редагування для відображення дати та часу
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

            // Встановлення початкового тексту поля редагування на поточну дату та час
            UpdateDisplayedTime(hwnd);

            // Додавання кнопки для встановлення часу
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

            // Додавання кнопки для відображення інформації про систему
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

            // Налаштування таймера для періодичного оновлення часу
            SetTimer(hwnd, IDT_TIMER, 1000, NULL); // Оновлення кожну секунду

            return 0;
        }
        // Обробка події таймера
        case WM_TIMER: {
            if (wParam == IDT_TIMER) {
                // Оновлення відображеного часу при спрацьовуванні таймера
                UpdateDisplayedTime(hwnd);
            }
            return 0;
        }
        // Обробка повідомлень про команди
        case WM_COMMAND: {
            if (LOWORD(wParam) == 2) {
                // Якщо натиснута кнопка "Встановити час", встановити системний час
                std::string newTime;
                char buffer[80];
                GetDlgItemTextA(hwnd, 1, buffer, sizeof(buffer));
                newTime = buffer;

                // Конвертування newTime у формат SYSTEMTIME
                SYSTEMTIME st;
                sscanf(newTime.c_str(), "%d-%d-%d %d:%d:%d", &st.wYear, &st.wMonth, &st.wDay, &st.wHour, &st.wMinute, &st.wSecond);

                // Встановлення системного часу
                SetLocalTime(&st);
            }
            else if (LOWORD(wParam) == 3) {
                // Якщо натиснута кнопка "Інформація про систему", відобразити інформацію про систему
                DisplaySystemInformation(hwnd);
            }
            return 0;
        }
        // Обробка знищення вікна
        case WM_DESTROY: {
            // Зупинка таймера при знищенні вікна
            KillTimer(hwnd, IDT_TIMER);
            PostQuitMessage(0);
            return 0;
        }
        // Обробка інших повідомлень
        default:
            return DefWindowProc(hwnd, uMsg, wParam, lParam);
    }
}

// Головна функція
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    // Реєстрація класу вікна
    const char CLASS_NAME[] = "SampleWindowClass";

    WNDCLASSA wc = {};

    wc.lpfnWndProc = WindowProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;

    RegisterClassA(&wc);

    // Створення вікна
    HWND hwnd = CreateWindowExA(
        0,                                  // Додаткові стилі вікна
        CLASS_NAME,                         // Клас вікна
        "System's data",              // Текст вікна
        WS_OVERLAPPEDWINDOW,                // Стилі вікна

        // Розмір та положення
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 150,

        NULL,       // Батьківське вікно
        NULL,       // Меню
        hInstance,  // Дескриптор екземпляру
        NULL        // Додаткові дані додатка
    );

    // Перевірка успішності створення вікна
    if (hwnd == NULL) {
        return 0;
    }

    // Показ вікна
    ShowWindow(hwnd, nCmdShow);

    // Цикл повідомлень
    MSG msg = {};
    while (GetMessage(&msg, NULL, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    return 0;
}
