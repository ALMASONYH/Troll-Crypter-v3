// Troll Crypter v3 - Mason Group / Freemasonry
// Developed by ABOLHB

#pragma once
#include "imgui.h"
#include "core/config.h"
#include "core/types.h"
#include <string>
#include <vector>
#include <mutex>
#include <thread>
#include <atomic>

namespace troll {

class MainWindow {
public:
    void init(void* glfw_window) { glfw_win_ = glfw_window; }
    void render();

private:
    std::string file_path_;
    int mode_ = 3;

    int custom_layers_ = 3;
    bool custom_chacha_ = true;
    bool custom_integrity_ = true;
    int custom_kdf_ = 500;
    int custom_keybits_ = 1;
    bool custom_antidebug_ = true;
    bool custom_timecheck_ = true;
    bool custom_antivm_ = false;
    int custom_junk_ = 4;

    std::atomic<bool> working_{false};
    std::atomic<float> progress_{0.0f};
    std::string progress_msg_;
    std::mutex log_mutex_;
    std::vector<std::pair<int, std::string>> log_entries_;
    ProtectResult last_result_;
    bool show_result_ = false;

    float anim_h_ = 395.0f;
    float fade_alpha_ = 0.0f;
    bool first_frame_ = true;

    void render_titlebar();
    void render_header();
    void render_file_picker();
    void render_mode_selector();
    void render_custom_panel();
    void render_action_buttons();
    void render_progress();
    void render_log();
    void render_result();

    void add_log(int level, const std::string& msg);
    void start_protect();
    void clear_all();
    Config build_config();
    void open_file_dialog();

    void* glfw_win_ = nullptr;
};

}
