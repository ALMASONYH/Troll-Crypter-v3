// Troll Crypter v3 - Mason Group / Freemasonry
// Developed by ABOLHB

#include "ui/main_window.h"
#include "core/orchestrator.h"
#include <GLFW/glfw3.h>
#include <cstdio>
#include <algorithm>
#include <cmath>

#ifdef _WIN32
#include <windows.h>
#include <commdlg.h>
#include <shellapi.h>
#endif

namespace troll {

static const float CORNER_R = 14.0f;
static const float FADE_SPEED = 3.0f;
static const float RESIZE_SPEED = 10.0f;
static const int W = 860;
static const int MAX_H = 860;
static const float BASE_H = 395.0f;

void MainWindow::render() {
    GLFWwindow* win = (GLFWwindow*)glfw_win_;
    float dt = ImGui::GetIO().DeltaTime;

    if (fade_alpha_ < 1.0f) {
        fade_alpha_ += dt * FADE_SPEED;
        if (fade_alpha_ > 1.0f) fade_alpha_ = 1.0f;
    }

    float target_h = BASE_H;
    if (mode_ == 3) target_h += 185.0f;
    {
        std::lock_guard<std::mutex> lk(log_mutex_);
        if (!log_entries_.empty()) target_h += 170.0f;
    }
    if (show_result_) target_h += 65.0f;

    float diff = target_h - anim_h_;
    if (std::abs(diff) > 0.5f) {
        anim_h_ += diff * std::min(dt * RESIZE_SPEED, 0.3f);
    } else {
        anim_h_ = target_h;
    }

    float vis_h = anim_h_;

    ImGuiViewport* vp = ImGui::GetMainViewport();
    ImVec2 wp = vp->WorkPos;
    ImVec2 ws = vp->WorkSize;

    ImDrawList* bgdl = ImGui::GetBackgroundDrawList();
    ImU32 bgcol = ImGui::ColorConvertFloat4ToU32(
        ImVec4(0.055f, 0.035f, 0.090f, fade_alpha_));
    bgdl->AddRectFilled(wp, ImVec2(wp.x + (float)W, wp.y + vis_h), bgcol, CORNER_R);

    ImU32 bordercol = ImGui::ColorConvertFloat4ToU32(
        ImVec4(0.20f, 0.12f, 0.32f, 0.35f * fade_alpha_));
    bgdl->AddRect(wp, ImVec2(wp.x + (float)W, wp.y + vis_h), bordercol, CORNER_R, 0, 1.5f);

    ImGui::SetNextWindowPos(wp);
    ImGui::SetNextWindowSize(ImVec2((float)W, vis_h));

    ImGui::PushStyleVar(ImGuiStyleVar_WindowRounding, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowBorderSize, 0.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(22, 8));
    ImGui::PushStyleColor(ImGuiCol_WindowBg, ImVec4(0, 0, 0, 0));

    ImGui::Begin("##main", nullptr,
        ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize |
        ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoBringToFrontOnFocus | ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse | ImGuiWindowFlags_NoBackground);

    ImGui::PushStyleVar(ImGuiStyleVar_Alpha, fade_alpha_);

    render_titlebar();
    ImGui::Spacing();
    render_header();
    ImGui::Spacing();
    render_file_picker();
    ImGui::Spacing();
    render_mode_selector();
    if (mode_ == 3) render_custom_panel();
    ImGui::Spacing();
    render_action_buttons();
    render_progress();
    render_log();
    render_result();

    float fy = vis_h - 22;
    ImGui::SetCursorPosY(fy);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.24f, 0.20f, 0.34f, 1.0f));
    ImGui::Text("The result of 4 years of work on this project  |  ");
    ImGui::PopStyleColor();
    ImGui::SameLine(0, 0);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.40f, 0.30f, 0.60f, 1.0f));
    ImVec2 tp = ImGui::GetCursorScreenPos();
    ImGui::Text("Developed by ABOLHB");
    ImVec2 te = ImVec2(tp.x + ImGui::CalcTextSize("Developed by ABOLHB").x, tp.y + ImGui::GetTextLineHeight());
    if (ImGui::IsMouseHoveringRect(tp, te)) {
        ImGui::SetMouseCursor(ImGuiMouseCursor_Hand);
        ImGui::GetWindowDrawList()->AddLine(
            ImVec2(tp.x, te.y), ImVec2(te.x, te.y),
            ImGui::ColorConvertFloat4ToU32(ImVec4(0.40f, 0.30f, 0.60f, 1.0f)));
        if (ImGui::IsMouseClicked(0)) {
#ifdef _WIN32
            ShellExecuteA(nullptr, "open", "https://abolhb.com", nullptr, nullptr, SW_SHOWNORMAL);
#else
            system("xdg-open https://abolhb.com &");
#endif
        }
    }
    ImGui::PopStyleColor();

    ImGui::SameLine(0, 0);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.24f, 0.20f, 0.34f, 1.0f));
    ImGui::Text("  |  Freemasonry");
    ImGui::PopStyleColor();

    ImGui::PopStyleVar();
    ImGui::End();
    ImGui::PopStyleColor();
    ImGui::PopStyleVar(3);
}

void MainWindow::render_titlebar() {
    GLFWwindow* win = (GLFWwindow*)glfw_win_;
    float w = ImGui::GetContentRegionAvail().x;

    ImVec2 wp = ImGui::GetWindowPos();

    ImDrawList* bg = ImGui::GetBackgroundDrawList();
    bg->AddRectFilled(
        wp,
        ImVec2(wp.x + (float)W, wp.y + 40),
        IM_COL32(10, 6, 18, (int)(255 * fade_alpha_)),
        CORNER_R, ImDrawFlags_RoundCornersTop
    );
    bg->AddLine(
        ImVec2(wp.x + 1, wp.y + 40),
        ImVec2(wp.x + (float)W - 1, wp.y + 40),
        IM_COL32(40, 25, 65, (int)(120 * fade_alpha_))
    );

    ImGui::SetCursorPosY(12);
    ImGui::SetCursorPosX(16);
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.486f, 0.228f, 0.929f, 1.0f));
    ImGui::Text("Troll Crypter v3");
    ImGui::PopStyleColor();
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.24f, 0.20f, 0.34f, 1.0f));
    ImGui::Text("2022~2024");
    ImGui::PopStyleColor();

    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 4.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(0, 0));

    ImGui::SetCursorPos(ImVec2(w + 4, 10));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.85f,0.18f,0.28f,0.7f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.48f,0.42f,0.60f,1));
    if (ImGui::Button("X##cl", ImVec2(22, 22))) { if (win) glfwSetWindowShouldClose(win, GLFW_TRUE); }
    ImGui::PopStyleColor(3);

    ImGui::SetCursorPos(ImVec2(w - 24, 10));
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0,0,0,0));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.3f,0.2f,0.5f,0.5f));
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.48f,0.42f,0.60f,1));
    if (ImGui::Button("_##mn", ImVec2(22, 22))) { if (win) glfwIconifyWindow(win); }
    ImGui::PopStyleColor(3);

    ImGui::PopStyleVar(2);

    ImVec2 mp = ImGui::GetMousePos();
    bool in_tb = mp.y >= wp.y && mp.y <= wp.y + 40 && mp.x < wp.x + W - 60;
    static bool drag = false;
    static double ddx = 0, ddy = 0;
    if (in_tb && ImGui::IsMouseClicked(0)) { drag = true; if (win) glfwGetCursorPos(win, &ddx, &ddy); }
    if (drag && ImGui::IsMouseDown(0) && win) {
        double mx, my; glfwGetCursorPos(win, &mx, &my);
        int wx, wy; glfwGetWindowPos(win, &wx, &wy);
        glfwSetWindowPos(win, wx+(int)(mx-ddx), wy+(int)(my-ddy));
    }
    if (ImGui::IsMouseReleased(0)) drag = false;

    ImGui::SetCursorPosY(44);
}

void MainWindow::render_header() {
    float w = ImGui::GetContentRegionAvail().x;
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.486f, 0.228f, 0.929f, 1.0f));
    const char* t = "PYTHON CRYPTER";
    ImGui::SetCursorPosX((w - ImGui::CalcTextSize(t).x) * 0.5f);
    ImGui::Text("%s", t);
    ImGui::PopStyleColor();

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.32f, 0.27f, 0.42f, 1.0f));
    const char* s = "AES-256-GCM  |  ChaCha20-Poly1305  |  scrypt KDF";
    ImGui::SetCursorPosX((w - ImGui::CalcTextSize(s).x) * 0.5f);
    ImGui::Text("%s", s);
    ImGui::PopStyleColor();
}

void MainWindow::render_file_picker() {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.46f, 0.40f, 0.58f, 1.0f));
    ImGui::Text("Target File");
    ImGui::PopStyleColor();

    float avail = ImGui::GetContentRegionAvail().x;
    ImGui::PushItemWidth(avail - 88);
    char buf[1024] = {};
    strncpy(buf, file_path_.c_str(), sizeof(buf) - 1);
    if (ImGui::InputText("##fp", buf, sizeof(buf))) file_path_ = buf;
    ImGui::PopItemWidth();
    ImGui::SameLine();
    ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.20f,0.10f,0.38f,1));
    ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.486f,0.228f,0.929f,1));
    if (ImGui::Button("Browse", ImVec2(80, 0))) open_file_dialog();
    ImGui::PopStyleColor(2);
}

void MainWindow::render_mode_selector() {
    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.46f, 0.40f, 0.58f, 1.0f));
    ImGui::Text("Protection Mode");
    ImGui::PopStyleColor();
    ImGui::Spacing();

    struct MI { const char* n; const char* d; ImVec4 c; };
    MI modes[] = {
        {"Quick",    "2 layers | AES-256 | Fast",    ImVec4(0.3f,0.75f,0.5f,1)},
        {"Standard", "3 layers | AES + ChaCha20",    ImVec4(0.486f,0.228f,0.929f,1)},
        {"Paranoid", "5 layers | 256-bit | Anti-VM",  ImVec4(0.92f,0.28f,0.38f,1)},
        {"Custom",   "Configure everything manually", ImVec4(0.85f,0.65f,0.20f,1)},
    };

    float avail = ImGui::GetContentRegionAvail().x;
    float gap = 6.0f;
    float cw = (avail - gap * 3) / 4.0f;
    float font_sz = ImGui::GetFontSize();
    float small = font_sz * 0.85f;

    for (int i = 0; i < 4; i++) {
        if (i > 0) ImGui::SameLine(0, gap);
        bool sel = (mode_ == i);
        ImGui::PushStyleColor(ImGuiCol_ChildBg, sel ? ImVec4(0.13f,0.07f,0.24f,1) : ImVec4(0.058f,0.034f,0.088f,1));
        ImGui::PushStyleColor(ImGuiCol_Border, sel ? modes[i].c : ImVec4(0.14f,0.09f,0.22f,0.4f));
        ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, sel ? 2.0f : 1.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
        ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(8, 6));
        char id[8]; snprintf(id, 8, "##m%d", i);
        ImGui::BeginChild(id, ImVec2(cw, 48), true,
            ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

        ImGui::PushStyleColor(ImGuiCol_Text, modes[i].c);
        ImGui::Text("%s", modes[i].n);
        ImGui::PopStyleColor();

        ImGui::SetCursorPosY(ImGui::GetCursorPosY() - 2);
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.34f,0.30f,0.44f,1));
        ImGui::SetWindowFontScale(small / font_sz);
        ImGui::Text("%s", modes[i].d);
        ImGui::SetWindowFontScale(1.0f);
        ImGui::PopStyleColor();
        if (ImGui::IsWindowHovered() && ImGui::IsMouseClicked(0)) mode_ = i;
        ImGui::EndChild();
        ImGui::PopStyleVar(3);
        ImGui::PopStyleColor(2);
    }
}

void MainWindow::render_custom_panel() {
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.048f,0.028f,0.074f,1));
    ImGui::PushStyleColor(ImGuiCol_Border, ImVec4(0.18f,0.11f,0.30f,0.5f));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 8.0f);
    ImGui::PushStyleVar(ImGuiStyleVar_ChildBorderSize, 1.0f);

    ImGui::BeginChild("##cust", ImVec2(0, 175), true,
        ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse);

    ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.85f,0.65f,0.20f,1));
    ImGui::Text("Custom Configuration");
    ImGui::PopStyleColor();

    ImGui::PushStyleVar(ImGuiStyleVar_FramePadding, ImVec2(4, 2));
    ImGui::PushStyleVar(ImGuiStyleVar_ItemSpacing, ImVec2(8, 4));

    float total_w = ImGui::GetContentRegionAvail().x;
    float left_w = total_w * 0.50f;

    ImGui::BeginGroup();
    ImGui::PushItemWidth(left_w - 100);
    ImGui::SliderInt("Layers", &custom_layers_, 2, 7);
    ImGui::SliderInt("KDF Rounds", &custom_kdf_, 100, 2000);
    const char* kb[] = {"64-bit","128-bit","256-bit"};
    ImGui::Combo("Key Size", &custom_keybits_, kb, 3);
    ImGui::SliderInt("Junk Intensity", &custom_junk_, 1, 10);
    ImGui::PopItemWidth();
    ImGui::EndGroup();

    ImGui::SameLine(left_w + 60);

    ImGui::BeginGroup();
    ImGui::Checkbox("ChaCha20 Dual Cipher", &custom_chacha_);
    ImGui::Checkbox("Integrity Check", &custom_integrity_);
    ImGui::Checkbox("Anti-Debug", &custom_antidebug_);
    ImGui::Checkbox("Time Check", &custom_timecheck_);
    ImGui::Checkbox("Anti-VM", &custom_antivm_);
    ImGui::EndGroup();

    ImGui::PopStyleVar(2);

    ImGui::EndChild();
    ImGui::PopStyleVar(2);
    ImGui::PopStyleColor(2);
}

void MainWindow::render_action_buttons() {
    float avail = ImGui::GetContentRegionAvail().x;
    bool disabled = file_path_.empty() || working_;
    bool has_log;
    {
        std::lock_guard<std::mutex> lk(log_mutex_);
        has_log = show_result_ || !log_entries_.empty();
    }

    float pw = 160.0f, ph = 34.0f, clw = 72.0f;
    float total = has_log ? (pw + 10 + clw) : pw;
    ImGui::SetCursorPosX((avail - total) * 0.5f);

    if (disabled) {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.09f,0.05f,0.15f,0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.09f,0.05f,0.15f,0.5f));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.09f,0.05f,0.15f,0.5f));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.30f,0.26f,0.40f,0.5f));
    } else {
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.486f,0.228f,0.929f,1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.58f,0.34f,0.98f,1));
        ImGui::PushStyleColor(ImGuiCol_ButtonActive, ImVec4(0.42f,0.18f,0.85f,1));
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(1,1,1,1));
    }
    ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
    if (ImGui::Button(working_ ? "ENCRYPTING..." : "PROTECT FILE", ImVec2(pw, ph))) {
        if (!disabled) start_protect();
    }
    ImGui::PopStyleVar();
    ImGui::PopStyleColor(4);

    if (has_log) {
        ImGui::SameLine(0, 10);
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.09f,0.05f,0.16f,1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.18f,0.09f,0.30f,1));
        ImGui::PushStyleVar(ImGuiStyleVar_FrameRounding, 10.0f);
        if (ImGui::Button("Clear", ImVec2(clw, ph))) clear_all();
        ImGui::PopStyleVar();
        ImGui::PopStyleColor(2);
    }
}

void MainWindow::render_progress() {
    if (!working_ && progress_ <= 0.0f) return;
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_PlotHistogram, ImVec4(0.486f,0.228f,0.929f,1));
    ImGui::PushStyleColor(ImGuiCol_FrameBg, ImVec4(0.055f,0.032f,0.085f,1));
    ImGui::ProgressBar(progress_.load(), ImVec2(-1, 14), progress_msg_.c_str());
    ImGui::PopStyleColor(2);
}

void MainWindow::render_log() {
    std::lock_guard<std::mutex> lk(log_mutex_);
    if (log_entries_.empty()) return;
    ImGui::Spacing();
    ImGui::PushStyleColor(ImGuiCol_ChildBg, ImVec4(0.032f,0.018f,0.050f,1));
    ImGui::PushStyleVar(ImGuiStyleVar_ChildRounding, 6.0f);
    ImGui::BeginChild("##log", ImVec2(0, 130), true);
    for (auto& [lv, msg] : log_entries_) {
        ImVec4 c = lv==0 ? ImVec4(0.36f,0.32f,0.46f,1) :
                   lv==1 ? ImVec4(0.30f,0.85f,0.50f,1) :
                           ImVec4(0.92f,0.28f,0.38f,1);
        ImGui::PushStyleColor(ImGuiCol_Text, c);
        ImGui::TextWrapped("%s", msg.c_str());
        ImGui::PopStyleColor();
    }
    if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 4) ImGui::SetScrollHereY(1.0f);
    ImGui::EndChild();
    ImGui::PopStyleVar();
    ImGui::PopStyleColor();
}

void MainWindow::render_result() {
    if (!show_result_) return;
    ImGui::Spacing();
    if (last_result_.success) {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.30f,0.85f,0.50f,1));
        ImGui::Text("Protection Successful!");
        ImGui::PopStyleColor();
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.40f,0.35f,0.52f,1));
        ImGui::Text("Output: %s  |  %zu -> %zu bytes  |  %.2fs",
            last_result_.output_path.c_str(),
            last_result_.original_size, last_result_.output_size,
            last_result_.elapsed_sec);
        ImGui::PopStyleColor();
#ifdef _WIN32
        ImGui::SameLine();
        ImGui::PushStyleColor(ImGuiCol_Button, ImVec4(0.14f,0.07f,0.26f,1));
        ImGui::PushStyleColor(ImGuiCol_ButtonHovered, ImVec4(0.486f,0.228f,0.929f,1));
        if (ImGui::SmallButton("Open Folder")) {
            std::string dir = last_result_.output_path;
            auto sep = dir.find_last_of("\\/");
            if (sep != std::string::npos) dir = dir.substr(0, sep);
            ShellExecuteA(nullptr, "explore", dir.c_str(), nullptr, nullptr, SW_SHOWNORMAL);
        }
        ImGui::PopStyleColor(2);
#endif
    } else {
        ImGui::PushStyleColor(ImGuiCol_Text, ImVec4(0.92f,0.28f,0.38f,1));
        ImGui::Text("Error: %s", last_result_.error_msg.c_str());
        ImGui::PopStyleColor();
    }
}

void MainWindow::clear_all() {
    show_result_ = false;
    progress_ = 0.0f;
    progress_msg_.clear();
    std::lock_guard<std::mutex> lk(log_mutex_);
    log_entries_.clear();
}

void MainWindow::add_log(int lv, const std::string& msg) {
    std::lock_guard<std::mutex> lk(log_mutex_);
    log_entries_.push_back({lv, msg});
    if (log_entries_.size() > 100) log_entries_.erase(log_entries_.begin());
}

Config MainWindow::build_config() {
    switch (mode_) {
        case 0: return Config::Quick();
        case 1: return Config::Standard();
        case 2: return Config::Paranoid();
        case 3: {
            Config c;
            c.layers = custom_layers_;
            c.use_chacha = custom_chacha_;
            c.use_integrity = custom_integrity_;
            c.kdf_rounds_min = std::max(100, custom_kdf_ - 100);
            c.kdf_rounds_max = custom_kdf_ + 100;
            int kbv[] = {64, 128, 256};
            c.key_bits = kbv[custom_keybits_];
            c.param_bits = c.key_bits <= 128 ? 64 : 128;
            c.anti_debug = custom_antidebug_;
            c.time_check = custom_timecheck_;
            c.anti_vm = custom_antivm_;
            c.junk_min = std::max(1, custom_junk_ - 1);
            c.junk_max = custom_junk_ + 2;
            c.compress_level = 9;
            return c;
        }
        default: return Config::Standard();
    }
}

void MainWindow::start_protect() {
    if (working_) return;
    clear_all();
    Config cfg = build_config();
    std::string fp = file_path_;
    working_ = true;
    const char* mn[] = {"Quick","Standard","Paranoid","Custom"};
    add_log(0, std::string("[*] Mode: ") + mn[mode_]);
    add_log(0, "[*] Layers: " + std::to_string(cfg.layers) +
               " | Dual: " + (cfg.use_chacha ? "Yes" : "No") +
               " | KDF: " + std::to_string(cfg.kdf_rounds_min) + "-" + std::to_string(cfg.kdf_rounds_max));

    std::thread([this, cfg, fp]() {
        Orchestrator orch(cfg);
        auto r = orch.protect(fp, [this](int cur, int total, const std::string& msg) {
            progress_ = (float)cur / (float)std::max(total, 1);
            progress_msg_ = msg;
            add_log(0, "[*] " + msg);
        });
        last_result_ = r;
        if (r.success) {
            add_log(1, "[+] Protected: " + r.output_path);
            add_log(1, "[+] " + std::to_string(r.original_size) + " -> " + std::to_string(r.output_size) + " bytes");
        } else {
            add_log(2, "[!] " + r.error_msg);
        }
        progress_ = 1.0f;
        show_result_ = true;
        working_ = false;
    }).detach();
}

void MainWindow::open_file_dialog() {
#ifdef _WIN32
    char fn[MAX_PATH] = {};
    OPENFILENAMEA ofn = {};
    ofn.lStructSize = sizeof(ofn);
    ofn.lpstrFilter = "Python Files\0*.py\0All Files\0*.*\0";
    ofn.lpstrFile = fn;
    ofn.nMaxFile = MAX_PATH;
    ofn.Flags = OFN_FILEMUSTEXIST | OFN_PATHMUSTEXIST | OFN_NOCHANGEDIR;
    if (GetOpenFileNameA(&ofn)) file_path_ = fn;
#else
    add_log(0, "[i] Enter path manually.");
#endif
}

}
