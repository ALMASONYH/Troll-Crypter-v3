// Troll Crypter v3 - Mason Group / Freemasonry
// Developed by ABOLHB

#pragma once
#include "imgui.h"

namespace troll {

inline void ApplyTrollTheme() {
    ImGuiStyle& s = ImGui::GetStyle();
    ImVec4* c = s.Colors;

    s.WindowRounding = 8.0f;
    s.FrameRounding = 6.0f;
    s.PopupRounding = 6.0f;
    s.ScrollbarRounding = 8.0f;
    s.GrabRounding = 4.0f;
    s.TabRounding = 6.0f;
    s.ChildRounding = 6.0f;

    s.WindowPadding = ImVec2(16, 16);
    s.FramePadding = ImVec2(12, 8);
    s.ItemSpacing = ImVec2(10, 10);
    s.ItemInnerSpacing = ImVec2(8, 6);
    s.ScrollbarSize = 14.0f;
    s.GrabMinSize = 12.0f;

    s.WindowBorderSize = 1.0f;
    s.FrameBorderSize = 0.0f;
    s.PopupBorderSize = 1.0f;

    ImVec4 bg_dark        = ImVec4(0.055f, 0.035f, 0.090f, 1.00f);
    ImVec4 bg_mid         = ImVec4(0.085f, 0.055f, 0.130f, 1.00f);
    ImVec4 bg_light       = ImVec4(0.110f, 0.075f, 0.165f, 1.00f);
    ImVec4 bg_lighter     = ImVec4(0.140f, 0.095f, 0.200f, 1.00f);
    ImVec4 accent         = ImVec4(0.486f, 0.228f, 0.929f, 1.00f);
    ImVec4 accent_hover   = ImVec4(0.580f, 0.340f, 0.980f, 1.00f);
    ImVec4 accent_active  = ImVec4(0.420f, 0.180f, 0.850f, 1.00f);
    ImVec4 accent_dim     = ImVec4(0.350f, 0.160f, 0.680f, 0.60f);
    ImVec4 text_primary   = ImVec4(0.920f, 0.900f, 0.960f, 1.00f);
    ImVec4 text_secondary = ImVec4(0.600f, 0.540f, 0.720f, 1.00f);
    ImVec4 text_disabled  = ImVec4(0.400f, 0.360f, 0.500f, 1.00f);
    ImVec4 border_color   = ImVec4(0.200f, 0.130f, 0.320f, 0.60f);
    ImVec4 success        = ImVec4(0.300f, 0.850f, 0.500f, 1.00f);
    ImVec4 error_col      = ImVec4(0.920f, 0.280f, 0.380f, 1.00f);
    ImVec4 glow           = ImVec4(0.486f, 0.228f, 0.929f, 0.15f);

    c[ImGuiCol_Text]                  = text_primary;
    c[ImGuiCol_TextDisabled]          = text_disabled;
    c[ImGuiCol_WindowBg]              = bg_dark;
    c[ImGuiCol_ChildBg]               = ImVec4(0.065f, 0.042f, 0.100f, 1.00f);
    c[ImGuiCol_PopupBg]               = bg_mid;
    c[ImGuiCol_Border]                = border_color;
    c[ImGuiCol_BorderShadow]          = ImVec4(0, 0, 0, 0);
    c[ImGuiCol_FrameBg]               = bg_mid;
    c[ImGuiCol_FrameBgHovered]        = bg_light;
    c[ImGuiCol_FrameBgActive]         = bg_lighter;
    c[ImGuiCol_TitleBg]               = bg_dark;
    c[ImGuiCol_TitleBgActive]         = ImVec4(0.075f, 0.048f, 0.115f, 1.00f);
    c[ImGuiCol_TitleBgCollapsed]      = bg_dark;
    c[ImGuiCol_MenuBarBg]             = bg_mid;
    c[ImGuiCol_ScrollbarBg]           = bg_dark;
    c[ImGuiCol_ScrollbarGrab]         = accent_dim;
    c[ImGuiCol_ScrollbarGrabHovered]  = accent;
    c[ImGuiCol_ScrollbarGrabActive]   = accent_hover;
    c[ImGuiCol_CheckMark]             = accent_hover;
    c[ImGuiCol_SliderGrab]            = accent;
    c[ImGuiCol_SliderGrabActive]      = accent_hover;
    c[ImGuiCol_Button]                = ImVec4(0.200f, 0.100f, 0.380f, 1.00f);
    c[ImGuiCol_ButtonHovered]         = accent;
    c[ImGuiCol_ButtonActive]          = accent_active;
    c[ImGuiCol_Header]                = ImVec4(0.180f, 0.100f, 0.320f, 0.80f);
    c[ImGuiCol_HeaderHovered]         = accent_dim;
    c[ImGuiCol_HeaderActive]          = accent;
    c[ImGuiCol_Separator]             = border_color;
    c[ImGuiCol_SeparatorHovered]      = accent;
    c[ImGuiCol_SeparatorActive]       = accent_hover;
    c[ImGuiCol_ResizeGrip]            = accent_dim;
    c[ImGuiCol_ResizeGripHovered]     = accent;
    c[ImGuiCol_ResizeGripActive]      = accent_hover;
    c[ImGuiCol_Tab]                   = bg_light;
    c[ImGuiCol_TabHovered]            = accent;
    c[ImGuiCol_TabActive]             = accent_active;
    c[ImGuiCol_TabUnfocused]          = bg_mid;
    c[ImGuiCol_TabUnfocusedActive]    = bg_light;
    c[ImGuiCol_PlotLines]             = accent;
    c[ImGuiCol_PlotLinesHovered]      = accent_hover;
    c[ImGuiCol_PlotHistogram]         = accent;
    c[ImGuiCol_PlotHistogramHovered]  = accent_hover;
    c[ImGuiCol_TextSelectedBg]        = ImVec4(0.486f, 0.228f, 0.929f, 0.35f);
    c[ImGuiCol_DragDropTarget]        = accent_hover;
    c[ImGuiCol_NavHighlight]          = accent;
    c[ImGuiCol_NavWindowingHighlight] = ImVec4(1.0f, 1.0f, 1.0f, 0.12f);
    c[ImGuiCol_NavWindowingDimBg]     = ImVec4(0.0f, 0.0f, 0.0f, 0.50f);
    c[ImGuiCol_ModalWindowDimBg]      = ImVec4(0.0f, 0.0f, 0.0f, 0.60f);

    (void)success;
    (void)error_col;
    (void)glow;
    (void)text_secondary;
}

}
