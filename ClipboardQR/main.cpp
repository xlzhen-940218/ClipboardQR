#include <windows.h>
#include <algorithm>

#define IDI_ICON1 101
#include <string>
#include "qrcodegen.hpp"

#pragma comment(linker,"\"/manifestdependency:type='win32' \
name='Microsoft.Windows.Common-Controls' version='6.0.0.0' \
processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")

#define WM_TRAYICON (WM_USER + 1)
#define IDM_EXIT 1001

using namespace qrcodegen;

// Global variables
HINSTANCE hInst;
NOTIFYICONDATA nid;
std::string qrText = "No Clipboard Data";

// Get clipboard text and convert to UTF-8 encoding
std::string GetClipboardText() {
    std::string utf8Text = "";
    if (OpenClipboard(nullptr)) {
        // Request Unicode (UTF-16) format clipboard data
        HANDLE hData = GetClipboardData(CF_UNICODETEXT);
        if (hData != nullptr) {
            wchar_t* pwszText = static_cast<wchar_t*>(GlobalLock(hData));
            if (pwszText != nullptr) {
                // 1. Get number of bytes needed for UTF-8 conversion (return value includes terminating '\0')
                int utf8Len = WideCharToMultiByte(CP_UTF8, 0, pwszText, -1, nullptr, 0, nullptr, nullptr);
                if (utf8Len > 0) {
                    // 2. Resize std::string and perform conversion
                    utf8Text.resize(utf8Len - 1);
                    WideCharToMultiByte(CP_UTF8, 0, pwszText, -1, &utf8Text[0], utf8Len, nullptr, nullptr);
                }
                GlobalUnlock(hData);
            }
        }
        CloseClipboard();
    }
    return utf8Text;
}

// Initialize system tray
void InitTrayIcon(HWND hWnd) {
    memset(&nid, 0, sizeof(NOTIFYICONDATA));
    nid.cbSize = sizeof(NOTIFYICONDATA);
    nid.hWnd = hWnd;
    nid.uID = 1;
    nid.uFlags = NIF_ICON | NIF_MESSAGE | NIF_TIP;
    nid.uCallbackMessage = WM_TRAYICON;

    // 尝试直接从程序运行目录读取名为 "qrcode.ico" 的文件
    nid.hIcon = (HICON)LoadImageA(nullptr, "qrcode.ico", IMAGE_ICON, 16, 16, LR_LOADFROMFILE | LR_DEFAULTCOLOR);

    if (nid.hIcon == nullptr) {
        nid.hIcon = LoadIcon(nullptr, IDI_APPLICATION); // 兜底
    }

    if (nid.hIcon == nullptr) {
        nid.hIcon = (HICON)LoadImage(hInst, MAKEINTRESOURCE(IDI_ICON1), IMAGE_ICON, 32, 32, LR_DEFAULTCOLOR);
    }

    // ---------- 添加以下代码作为兜底 ----------
    if (nid.hIcon == nullptr) {
        // 如果依然为 nullptr，说明资源文件缺失，强制加载 Windows 默认的应用程序图标
        nid.hIcon = LoadIcon(nullptr, IDI_APPLICATION);
    }
    // ------------------------------------------

    lstrcpy(nid.szTip, TEXT("Clipboard QR"));
    Shell_NotifyIcon(NIM_ADD, &nid);
}

// Draw QR code
void DrawQRCode(HDC hdc, const std::string& text, RECT clientRect) {
    if (text.empty()) return;

    try {
        QrCode qr = QrCode::encodeText(text.c_str(), QrCode::Ecc::LOW);
        int size = qr.getSize();

        // Calculate scaling and centering offset
        int scale = (clientRect.right / (size + 4) < clientRect.bottom / (size + 4)) ? 
                    clientRect.right / (size + 4) : clientRect.bottom / (size + 4);
        if (scale < 1) scale = 1;
        int offsetX = (clientRect.right - size * scale) / 2;
        int offsetY = (clientRect.bottom - size * scale) / 2;

        HBRUSH blackBrush = CreateSolidBrush(RGB(0, 0, 0));
        HBRUSH whiteBrush = CreateSolidBrush(RGB(255, 255, 255));

        FillRect(hdc, &clientRect, whiteBrush); // White background

        for (int y = 0; y < size; y++) {
            for (int x = 0; x < size; x++) {
                RECT r = { offsetX + x * scale, offsetY + y * scale,
                           offsetX + (x + 1) * scale, offsetY + (y + 1) * scale };
                FillRect(hdc, &r, qr.getModule(x, y) ? blackBrush : whiteBrush);
            }
        }
        DeleteObject(blackBrush);
        DeleteObject(whiteBrush);
    }
    catch (...) {
        TextOutA(hdc, 10, 10, "Failed to generate QR", 21);
    }
}

// Window procedure function
LRESULT CALLBACK WndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
    switch (message) {
    case WM_CREATE:
        qrText = GetClipboardText();
        if (qrText.empty()) qrText = "Clipboard is empty";
        InitTrayIcon(hWnd);
        break;

    case WM_PAINT: {
        PAINTSTRUCT ps;
        HDC hdc = BeginPaint(hWnd, &ps);
        RECT rect;
        GetClientRect(hWnd, &rect);
        DrawQRCode(hdc, qrText, rect);
        EndPaint(hWnd, &ps);
        break;
    }

    case WM_CLOSE:
        // Intercept close button, hide window instead of exiting
        ShowWindow(hWnd, SW_HIDE);
        return 0;

    case WM_TRAYICON:
        if (lParam == WM_RBUTTONUP) {
            // Right-click popup menu
            POINT pt;
            GetCursorPos(&pt);
            HMENU hMenu = CreatePopupMenu();
            AppendMenu(hMenu, MF_STRING, IDM_EXIT, TEXT("Exit (&X)"));
            SetForegroundWindow(hWnd); // Prevent menu from losing focus
            TrackPopupMenu(hMenu, TPM_RIGHTBUTTON, pt.x, pt.y, 0, hWnd, nullptr);
            DestroyMenu(hMenu);
        }
        else if (lParam == WM_LBUTTONDBLCLK) {
            // Double-click tray to restore window and re-read clipboard
            qrText = GetClipboardText();
            InvalidateRect(hWnd, nullptr, TRUE); // Trigger redraw
            ShowWindow(hWnd, SW_RESTORE);
            SetForegroundWindow(hWnd);
        }
        break;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDM_EXIT) {
            Shell_NotifyIcon(NIM_DELETE, &nid);
            PostQuitMessage(0);
        }
        break;

    case WM_DESTROY:
        Shell_NotifyIcon(NIM_DELETE, &nid);
        PostQuitMessage(0);
        break;

    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Main function
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nCmdShow) {
    hInst = hInstance;
    const wchar_t* CLASS_NAME = L"QRCodeAppClass";

    WNDCLASSW wc = { 0 };
    wc.lpfnWndProc = WndProc;
    wc.hInstance = hInstance;
    wc.lpszClassName = CLASS_NAME;
    wc.hCursor = LoadCursor(nullptr, IDC_ARROW);
    wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 1);

    // Load custom ICO icon (for window top-left corner and taskbar)
    wc.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_ICON1));

    RegisterClassW(&wc);

    HWND hWnd = CreateWindowExW(
        0, CLASS_NAME, L"Clipboard QR", WS_OVERLAPPEDWINDOW,
        CW_USEDEFAULT, CW_USEDEFAULT, 400, 420,
        nullptr, nullptr, hInstance, nullptr
    );

    if (hWnd == nullptr) return 0;

    ShowWindow(hWnd, nCmdShow);
    UpdateWindow(hWnd);

    MSG msg;
    while (GetMessage(&msg, nullptr, 0, 0)) {
        TranslateMessage(&msg);
        DispatchMessage(&msg);
    }

    // Cast to int to eliminate C4244 warning
    return (int)msg.wParam;
}
