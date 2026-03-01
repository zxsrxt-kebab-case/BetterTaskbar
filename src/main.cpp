#include <windows.h>
#include <shellapi.h>
#include <string>
#include <cmath>
#include <chrono>

enum menu_ids
{
    id_tray_exit = 1001,
    id_tray_anim_toggle = 1002,
    id_tray_speed_fast = 1003,
    id_tray_speed_slow = 1004,
    id_tray_anim_linear = 1005,
    id_tray_anim_ease_out = 1006,
    id_tray_anim_ease_in_out = 1007,
    id_tray_anim_bounce = 1008
};

enum class anim_type
{
    linear,
    ease_out,
    ease_in_out,
    bounce
};

struct config
{
    bool use_animation = true;
    float anim_duration = 0.25f;
    anim_type current_anim = anim_type::ease_out;
    bool is_running = true;
} g_cfg;

struct system_state
{
    HWND h_taskbar = nullptr;
    int screen_height = 0;
    int taskbar_h = 30;

    int target_y = 0;
    float start_y = 0.0f;
    float current_y = 0.0f;

    bool is_animating = false;
    std::chrono::steady_clock::time_point anim_start;
} g_sys;

float ease_linear(float t)
{
    return t;
}

float ease_out_cubic(float t)
{
    const float f = t - 1.0f;
    return f * f * f + 1.0f;
}

float ease_in_out_cubic(float t)
{
    return t < 0.5f ? 4.0f * t * t * t : 1.0f - std::pow(-2.0f * t + 2.0f, 3.0f) / 2.0f;
}

//rep gemini
float ease_out_bounce(float t)
{
    const float n1 = 7.5625f;
    const float d1 = 2.75f;
    if (t < 1.0f / d1)
    {
        return n1 * t * t;
    }
    if (t < 2.0f / d1)
    {
        t -= 1.5f / d1;
        return n1 * t * t + 0.75f;
    }
    if (t < 2.5f / d1)
    {
        t -= 2.25f / d1;
        return n1 * t * t + 0.9375f;
    }
    {
        t -= 2.625f / d1;
        return n1 * t * t + 0.984375f;
    }
}

//main func wrapper
void move_taskbar(const int y)
{
    const auto w = GetSystemMetrics(SM_CXSCREEN);
    SetWindowPos(g_sys.h_taskbar, HWND_TOPMOST, 0, y, w, g_sys.taskbar_h,
                 SWP_NOACTIVATE | SWP_NOSENDCHANGING | SWP_SHOWWINDOW | SWP_NOOWNERZORDER);
}

//helper for check is win start menu opened
bool is_start_focused()
{
    const auto foreground = GetForegroundWindow();
    if (!foreground) return false;
    char class_name[256];
    GetClassNameA(foreground, class_name, sizeof(class_name));
    const std::string cl = class_name;
    return (cl == "Windows.UI.Core.CoreWindow" || cl == "DV2ControlHost");
}

void show_context_menu(HWND hwnd)
{
    POINT pt;
    GetCursorPos(&pt);
    const auto h_menu = CreatePopupMenu();

    AppendMenuA(h_menu, g_cfg.use_animation ? MF_CHECKED : MF_UNCHECKED, id_tray_anim_toggle, "Enable Animation");
    AppendMenuA(h_menu, MF_SEPARATOR, 0, nullptr);

    AppendMenuA(h_menu, g_cfg.anim_duration < 0.2f ? MF_CHECKED : MF_UNCHECKED, id_tray_speed_fast, "Speed: Fast");
    AppendMenuA(h_menu, g_cfg.anim_duration >= 0.2f ? MF_CHECKED : MF_UNCHECKED, id_tray_speed_slow, "Speed: Smooth");
    AppendMenuA(h_menu, MF_SEPARATOR, 0, nullptr);

    AppendMenuA(h_menu, g_cfg.current_anim == anim_type::linear ? MF_CHECKED : MF_UNCHECKED, id_tray_anim_linear, "Anim: Linear");
    AppendMenuA(h_menu, g_cfg.current_anim == anim_type::ease_out ? MF_CHECKED : MF_UNCHECKED, id_tray_anim_ease_out, "Anim: Ease-Out");
    AppendMenuA(h_menu, g_cfg.current_anim == anim_type::ease_in_out ? MF_CHECKED : MF_UNCHECKED, id_tray_anim_ease_in_out, "Anim: Ease-In-Out");
    AppendMenuA(h_menu, g_cfg.current_anim == anim_type::bounce ? MF_CHECKED : MF_UNCHECKED, id_tray_anim_bounce, "Anim: Bounce");

    AppendMenuA(h_menu, MF_SEPARATOR, 0, nullptr);
    AppendMenuA(h_menu, MF_STRING, id_tray_exit, "Exit");

    SetForegroundWindow(hwnd);
    TrackPopupMenu(h_menu, TPM_BOTTOMALIGN | TPM_LEFTALIGN, pt.x, pt.y, 0, hwnd, nullptr);
    DestroyMenu(h_menu);
}

LRESULT CALLBACK window_proc(HWND hwnd, UINT msg, WPARAM wparam, LPARAM lparam)
{
    if (msg == WM_USER + 1 && lparam == WM_RBUTTONUP)
        show_context_menu(hwnd);

    else if (msg == WM_COMMAND)
    {
        switch (LOWORD(wparam))
        {
            case id_tray_exit: g_cfg.is_running = false; break;
            case id_tray_anim_toggle: g_cfg.use_animation = !g_cfg.use_animation; break;
            case id_tray_speed_fast: g_cfg.anim_duration = 0.15f; break;
            case id_tray_speed_slow: g_cfg.anim_duration = 0.40f; break;
            case id_tray_anim_linear: g_cfg.current_anim = anim_type::linear; break;
            case id_tray_anim_ease_out: g_cfg.current_anim = anim_type::ease_out; break;
            case id_tray_anim_ease_in_out: g_cfg.current_anim = anim_type::ease_in_out; break;
            case id_tray_anim_bounce: g_cfg.current_anim = anim_type::bounce; break;
            default:
                break;
        }
    }
    return DefWindowProc(hwnd, msg, wparam, lparam);
}

int APIENTRY WinMain(HINSTANCE h_inst, HINSTANCE, LPSTR, int)
{
    g_sys.h_taskbar = FindWindowA("Shell_TrayWnd", nullptr);
    if (!g_sys.h_taskbar) return 1;

    g_sys.screen_height = GetSystemMetrics(SM_CYSCREEN);
    const auto exp_y = g_sys.screen_height - g_sys.taskbar_h;
    const auto hid_y = g_sys.screen_height - 1;

    g_sys.target_y = hid_y;
    g_sys.current_y = static_cast<float>(hid_y);

    WNDCLASS wc = {0};
    wc.lpfnWndProc = window_proc;
    wc.hInstance = h_inst;
    wc.lpszClassName = "BetterTaskBarCls";
    RegisterClass(&wc);
    const auto h_wnd = CreateWindow(wc.lpszClassName, nullptr, 0, 0, 0, 0, 0, nullptr, nullptr, h_inst, nullptr);

    NOTIFYICONDATA nid = { sizeof(nid), h_wnd, 1, NIF_ICON | NIF_MESSAGE | NIF_TIP, WM_USER + 1, LoadIcon(nullptr, IDI_APPLICATION) };
    strcpy_s(nid.szTip, "BetterTaskBar");
    Shell_NotifyIcon(NIM_ADD, &nid);

    MSG msg;
    while (g_cfg.is_running)
    {
        while (PeekMessage(&msg, nullptr, 0, 0, PM_REMOVE))
        {
            TranslateMessage(&msg);
            DispatchMessage(&msg);
        }

        POINT pt;
        GetCursorPos(&pt);

        RECT real_rect;
        GetWindowRect(g_sys.h_taskbar, &real_rect);

        const int wanted_y = (pt.y >= g_sys.screen_height - g_sys.taskbar_h ||
                              GetAsyncKeyState(VK_LWIN) & 0x8000 ||
                              GetAsyncKeyState(VK_RWIN) & 0x8000 ||
                              is_start_focused()) ? exp_y : hid_y;

        if (wanted_y != g_sys.target_y)
        {
            g_sys.start_y = g_sys.current_y;
            g_sys.target_y = wanted_y;
            g_sys.anim_start = std::chrono::steady_clock::now();
            g_sys.is_animating = true;
        }

        if (g_sys.is_animating && g_cfg.use_animation)
        {
            const auto now = std::chrono::steady_clock::now();
            const std::chrono::duration<float> elapsed = now - g_sys.anim_start;

            float t = elapsed.count() / g_cfg.anim_duration;

            if (t >= 1.0f)
            {
                t = 1.0f;
                g_sys.is_animating = false;
            }

            float eased_t = t;
            switch (g_cfg.current_anim)
            {
                case anim_type::linear: eased_t = ease_linear(t); break;
                case anim_type::ease_out: eased_t = ease_out_cubic(t); break;
                case anim_type::ease_in_out: eased_t = ease_in_out_cubic(t); break;
                case anim_type::bounce: eased_t = ease_out_bounce(t); break;
            }

            g_sys.current_y = std::lerp(g_sys.start_y, static_cast<float>(g_sys.target_y), eased_t);
            move_taskbar(static_cast<int>(std::round(g_sys.current_y)));
            Sleep(10);
        }
        else
        {

            g_sys.current_y = static_cast<float>(g_sys.target_y);
            g_sys.is_animating = false;


            //window hide bugfix
            if (real_rect.top != g_sys.target_y)
            {
                move_taskbar(g_sys.target_y);
            }

            Sleep(20);
        }
    }

    Shell_NotifyIcon(NIM_DELETE, &nid);
    move_taskbar(exp_y);
    return 0;
}