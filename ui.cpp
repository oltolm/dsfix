#include "ui.h"
#include "Settings.h"
#include "RenderstateManager.h"
#include "WindowManager.h"
#include "imgui/imgui.h"
#include "imgui/backends/imgui_impl_dx9.h"
#include "imgui/backends/imgui_impl_win32.h"
#include <cstring>
#include <sstream>
#include <array>
#include <ctime>

#include "myfont.cpp"

static void HelpMarker(const char* desc);

extern bool g_paused;

std::string modeToString(const D3DDISPLAYMODE& mode);

static WNDPROC oWndProc = nullptr;

extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam,
                                                             LPARAM lParam);

bool g_open = false;

LRESULT CALLBACK hkWindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam) {
  if (ImGui_ImplWin32_WndProcHandler(hwnd, uMsg, wParam, lParam) > 0)
    return 1L;
  auto& io = ImGui::GetIO();
  g_paused = g_open && Settings::get().getPauseGame();
  if ((io.WantCaptureMouse && uMsg >= WM_MOUSEFIRST && uMsg <= WM_MOUSELAST) ||
      (io.WantCaptureKeyboard && uMsg >= WM_KEYFIRST && uMsg <= WM_KEYLAST)) {
    return 1L;
  }
  return ::CallWindowProcW(oWndProc, hwnd, uMsg, wParam, lParam);
}

static time_t s_start;

Ui::Ui(IDirect3DDevice9* pDevice, RSManager* pRSManager)
    : m_pDevice(pDevice), m_pRSManager(pRSManager){};

void Ui::onEndScene() {
  static bool s_init = false;

  if (!s_init) {
    D3DDEVICE_CREATION_PARAMETERS params;
    m_pDevice->GetCreationParameters(&params);

    oWndProc = (WNDPROC)::SetWindowLongPtr(params.hFocusWindow, GWLP_WNDPROC, (LONG)hkWindowProc);

    ImGui::CreateContext();
    ImGui_ImplWin32_Init(params.hFocusWindow);
    ImGui_ImplDX9_Init(m_pDevice.Get());
    ImGui::GetIO().Fonts->AddFontFromMemoryCompressedTTF(MyFont_compressed_data,
                                                         MyFont_compressed_size, 24);

    s_start = std::time(nullptr);

    s_init = true;
  }

  if (GetAsyncKeyState(VK_F1) & 0x01)
    g_open = !g_open;

  ImGui_ImplDX9_NewFrame();
  ImGui_ImplWin32_NewFrame();
  ImGui::NewFrame();
  auto& io = ImGui::GetIO();
  io.MouseDrawCursor = g_open;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;
  io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;

  static ImGuiWindowFlags s_flags = ImGuiWindowFlags_NoTitleBar | ImGuiWindowFlags_NoResize;

  double elapsed = std::difftime(std::time(nullptr), s_start);
  if (elapsed < 10) {
    if (ImGui::Begin("Popup", nullptr, s_flags)) {
      ImGui::Text("Press F1 to open the DSFix options. Closing in %.0fs.", 10 - elapsed);
    }
    ImGui::End();
  }

  if (g_open) {
    showWindow(&g_open);
    // ImGui::ShowDemoWindow(&open);
  }

  ImGui::EndFrame();
  ImGui::Render();
  ImGui_ImplDX9_RenderDrawData(ImGui::GetDrawData());
}

struct LanguageInfo {
  std::string label;
  std::string id;
};

void Ui::showWindow(bool* pOpen) {
  if (!ImGui::Begin("dsfix", pOpen)) {
    ImGui::End();
    return;
  }

  if (ImGui::Button("Save"))
    Settings::get().save();

  if (ImGui::BeginTabBar("settings")) {
    if (ImGui::BeginTabItem("Graphics Options")) {
      ImGui::BeginChild("Scrolling");

      ImGui::Text("Display Resolution");
      ImGui::Indent();

      {
        int adapter = Settings::get().getD3DAdapterOverride();
        if (adapter < 0)
          adapter = D3DADAPTER_DEFAULT;
        D3DDISPLAYMODE currentMode = {};
        WRL::ComPtr<IDirect3D9> pD3D9;
        m_pDevice->GetDirect3D(&pD3D9);
        if (FAILED(pD3D9->GetAdapterDisplayMode(adapter, &currentMode))) {
          ImGui::End();
          return;
        }

        UINT modeCount = pD3D9->GetAdapterModeCount(adapter, currentMode.Format);

        static ImGuiComboFlags flags = 0;
        auto width = Settings::get().getRenderWidth();
        auto height = Settings::get().getRenderHeight();
        std::string preview_value = modeToString({width, height, 0, D3DFMT_UNKNOWN});

        if (ImGui::BeginCombo("Display Resolution", preview_value.c_str(), flags)) {
          for (UINT i = 0; i < modeCount; ++i) {
            D3DDISPLAYMODE mode = {};
            if (SUCCEEDED(pD3D9->EnumAdapterModes(adapter, currentMode.Format, i, &mode))) {
              if (currentMode.RefreshRate != mode.RefreshRate)
                continue;
              std::string label = modeToString(mode);
              bool is_selected = mode.Height == height && mode.Width == width;
              if (ImGui::Selectable(label.c_str(), &is_selected)) {
                Settings::get().setRenderHeight(mode.Height);
                Settings::get().setRenderWidth(mode.Width);
              }
              if (is_selected)
                ImGui::SetItemDefaultFocus();
            }
          }
          ImGui::EndCombo();
        }
        ImGui::SameLine();
        HelpMarker(
            "Internal rendering resolution of the game. Higher values will decrease performance.");
      }

      ImGui::Unindent();
      ImGui::Separator();

      ImGui::Text("Anti Aliasing");
      ImGui::Indent();

      {
        int aAQuality = Settings::get().getAAQuality();
        std::array<const char*, 5> items = {"off (best performance, worst IQ)", "low", "medium",
                                            "high", "ultra (worst performance, best IQ)"};
        if (ImGui::Combo("AA Quality", &aAQuality, items.data(), items.size())) {
          Settings::get().setAAQuality(aAQuality);
          m_pRSManager->setupAA();
        }
      }

      {
        static ImGuiComboFlags flags = 0;
        std::string item_current = Settings::get().getAAType();
        const std::array<std::string, 2> items = {"FXAA", "SMAA"};
        if (ImGui::BeginCombo("AA Type", item_current.c_str(), flags)) {
          for (const auto& item : items) {
            bool selected = item == item_current;
            if (ImGui::Selectable(item.c_str(), &selected)) {
              Settings::get().setAAType(item);
              m_pRSManager->setupAA();
            }
            if (selected)
              ImGui::SetItemDefaultFocus();
          }
          ImGui::EndCombo();
        }
      }

      ImGui::Unindent();
      ImGui::Separator();

      ImGui::Text("Screen Space Ambient Occlusion");
      ImGui::Indent();

      {
        int ssaoStrength = Settings::get().getSsaoStrength();
        std::array<const char*, 4> items = {"off", "low", "medium", "high"};
        if (ImGui::Combo("SSAO Strength", &ssaoStrength, items.data(), items.size())) {
          Settings::get().setSsaoStrength(ssaoStrength);
          m_pRSManager->setupSSAO();
        }

        ImGui::SameLine();
        HelpMarker("All 3 settings have the same performance impact!");
      }

      {
        std::array<std::string, 3> items = {"HBAO", "VSSAO", "VSSAO2"};
        auto ssaoType = Settings::get().getSsaoType();

        if (ImGui::BeginCombo("SSAO Type", ssaoType.c_str())) {
          for (const auto& item : items) {
            bool selected = item == ssaoType;
            if (ImGui::Selectable(item.c_str(), &selected)) {
              Settings::get().setSsaoType(item);
              m_pRSManager->setupSSAO();
            }
            if (selected)
              ImGui::SetItemDefaultFocus();
          }
          ImGui::EndCombo();
        }

        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
          ImGui::BeginTooltip();
          ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
          ImGui::TextWrapped("Determine the type of AO used");
          ImGui::Bullet();
          ImGui::Text(R"("HBAO" = Horizon-Based Ambient Occlusion)");
          ImGui::Bullet();
          ImGui::TextWrapped(R"("VSSAO" = Volumetric SSAO (default, only option pre-1.9))");
          ImGui::Bullet();
          ImGui::TextWrapped(
              R"("VSSAO2" = Volumetric SSAO with more samples (tweaked by Asmodean))");
          ImGui::TextWrapped(
              "VSSAO2 is generally more accurate, but also requires more performance.");
          ImGui::PopTextWrapPos();
          ImGui::EndTooltip();
        }
      }

      ImGui::Unindent();
      ImGui::Separator();

      ImGui::Text("Gaussian Depth of Field");
      ImGui::Indent();

      {
        int dofOverrideResolution = Settings::get().getDOFOverrideResolution();
        if (ImGui::SliderInt("DoF resolution override", &dofOverrideResolution, 0, 2160)) {
          Settings::get().setDOFOverrideResolution(dofOverrideResolution);
          m_pRSManager->setupDoF();
        }

        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
          ImGui::BeginTooltip();
          ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
          ImGui::TextWrapped("Depth of Field resolution override, possible values:");
          ImGui::BulletText("0 = no change from default (DoF pyramid starts at 512x360)");
          ImGui::BulletText("540 = DoF pyramid starts at 960x540");
          ImGui::BulletText("810 = DoF pyramid starts at 1440x810");
          ImGui::BulletText("1080 = DoF pyramid starts at 1920x1080");
          ImGui::BulletText("2160 = DoF pyramid starts at 3840x2160");
          ImGui::TextWrapped("higher values will decrease performance.");
          ImGui::PopTextWrapPos();
          ImGui::EndTooltip();
        }

        if ((unsigned int)dofOverrideResolution == Settings::get().getRenderHeight())
          ImGui::TextColored(
              ImVec4(1, 1, 0, 1),
              "Do NOT set this to the same value as your vertical rendering resolution!");
      }

      {
        int dofBlurAmount = Settings::get().getDOFBlurAmount();
        if (ImGui::SliderInt("DoF additional blur", &dofBlurAmount, 0, 4)) {
          Settings::get().setDOFBlurAmount(dofBlurAmount);
          m_pRSManager->setupDoF();
        }

        ImGui::SameLine();
        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
          ImGui::BeginTooltip();
          ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
          ImGui::TextWrapped(
              "Depth of field additional blur allows you to use high DoF resolutions "
              "and still get the originally intended effect. Suggested values:");
          ImGui::BulletText("0 (off) at default DoF resolution");
          ImGui::BulletText("0 (off) or 1 at 540 DoF resolution");
          ImGui::BulletText("1 or 2 above that");
          ImGui::BulletText("3 or 4 at 2160 DoF resolution (if you're running a 680+)");
          ImGui::PopTextWrapPos();
          ImGui::EndTooltip();
        }
      }

      ImGui::Unindent();
      ImGui::Separator();

      ImGui::Text("Framerate");
      ImGui::Indent();

      {
        bool unlockFPS = Settings::get().getUnlockFPS();
        if (ImGui::Checkbox("Enable variable framerate", &unlockFPS))
          Settings::get().setUnlockFPS(unlockFPS);

        ImGui::SameLine();

        ImGui::TextDisabled("(?)");
        if (ImGui::IsItemHovered()) {
          ImGui::BeginTooltip();
          ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
          ImGui::TextWrapped("NOTE:");
          ImGui::Bullet();
          ImGui::TextWrapped("There may be unintended side-effects in terms of gameplay.");
          ImGui::Bullet();
          ImGui::TextWrapped(
              "You need a very powerful system (especially CPU) in order to maintain 60 FPS.");
          ImGui::Bullet();
          ImGui::TextWrapped(
              "In some instances, collision detection may fail. Avoid sliding down ladders.");
          ImGui::Bullet();
          ImGui::TextWrapped("Requires restart.");
          ImGui::PopTextWrapPos();
          ImGui::EndTooltip();
        }
      }

      {
        int FPSlimit = Settings::get().getFPSLimit();
        if (ImGui::SliderInt("FPS limit", &FPSlimit, 30, 60, "%d FPS"))
          Settings::get().setFPSLimit(FPSlimit);

        ImGui::SameLine();
        HelpMarker("FPS limit, only used with unlocked framerate.");
      }

      ImGui::Unindent();
      ImGui::Separator();

      ImGui::Text("Anisotropic Filtering");
      ImGui::Indent();

      {
        bool filteringOverride = Settings::get().getFilteringOverride() == 2;
        if (ImGui::Checkbox("Texture filtering override", &filteringOverride))
          Settings::get().setFilteringOverride(filteringOverride ? 2 : 0);
      }

      ImGui::Unindent();
      ImGui::EndChild();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Window & Mouse Cursor Options")) {
      ImGui::BeginChild("Scrolling");

      ImGui::Indent();

      bool borderlessFullscreen = Settings::get().getBorderlessFullscreen();
      if (ImGui::Checkbox("Borderless Fullscreen", &borderlessFullscreen)) {
        Settings::get().setBorderlessFullscreen(borderlessFullscreen);
        WindowManager::get().toggleBorderlessFullscreen();
      }
      ImGui::SameLine();
      HelpMarker("Make sure to select windowed mode in the game settings for this to work!");

      bool disableCursor = Settings::get().getDisableCursor();
      if (ImGui::Checkbox("Disable cursor at startup", &disableCursor)) {
        Settings::get().setDisableCursor(disableCursor);
        WindowManager::get().toggleCursorCapture();
      }

      bool captureCursor = Settings::get().getCaptureCursor();
      if (ImGui::Checkbox("Capture cursor", &captureCursor)) {
        Settings::get().setCaptureCursor(captureCursor);
        WindowManager::get().toggleCursorCapture();
      }
      ImGui::SameLine();
      HelpMarker("This also works if the cursor is not visible.");

      ImGui::Unindent();

      ImGui::EndChild();
      ImGui::EndTabItem();
    }

    if (ImGui::BeginTabItem("Other Options")) {
      ImGui::BeginChild("Scrolling");

      ImGui::Indent();

      bool skipIntro = Settings::get().getSkipIntro();
      if (ImGui::Checkbox("Skip the intro logos", &skipIntro))
        Settings::get().setSkipIntro(skipIntro);

      bool pauseGame = Settings::get().getPauseGame();
      if (ImGui::Checkbox("Pause game when DSFix dialog is open", &pauseGame))
        Settings::get().setPauseGame(pauseGame);

      // en-GB = English, fr = French, it = Italian, de = German, es = Spanish
      // ko = Korean, zh-tw = Chinese, pl = Polish, ru = Russian

      static std::array<LanguageInfo, 10> languages = {{{"None", "none"},
                                                        {"English", "en-GB"},
                                                        {"French", "fr"},
                                                        {"Italian", "it"},
                                                        {"German", "de"},
                                                        {"Spanish", "es"},
                                                        {"Korean", "ko"},
                                                        {"Chinese", "zh-tw"},
                                                        {"Polish", "pl"},
                                                        {"Russian", "ru"}}};
      const auto& overrideLanguage = Settings::get().getOverrideLanguage();
      auto it = std::find_if(
          languages.begin(), languages.end(),
          [&overrideLanguage](const LanguageInfo& lang) { return lang.id == overrideLanguage; });
      std::string preview_value = it != languages.end() ? it->label : "None";

      if (ImGui::BeginCombo("override the in-game language", preview_value.c_str())) {
        for (const auto& lang : languages) {
          bool selected = lang.id == overrideLanguage;
          if (ImGui::Selectable(lang.label.c_str(), &selected)) {
            Settings::get().setOverrideLanguage(lang.id);
          }
          if (selected)
            ImGui::SetItemDefaultFocus();
        }
        ImGui::EndCombo();
      }
      ImGui::SameLine();
      HelpMarker("Requires restart.");

      ImGui::Unindent();

      ImGui::EndChild();
      ImGui::EndTabItem();
    }

    ImGui::EndTabBar();
  }

  ImGui::End();
}

std::string modeToString(const D3DDISPLAYMODE& mode) {
  std::stringstream ss;
  ss << mode.Width << "x" << mode.Height;
  return ss.str();
}

void Ui::onReset() {
  ImGui_ImplDX9_InvalidateDeviceObjects();
  ImGui_ImplDX9_CreateDeviceObjects();
}

void Ui::setRSManager(RSManager* p_RSManager) { m_pRSManager = p_RSManager; }

// Helper to display a little (?) mark which shows a tooltip when hovered.
// In your own code you may want to display an actual icon if you are using a merged icon fonts (see
// docs/FONTS.md)
static void HelpMarker(const char* desc) {
  ImGui::TextDisabled("(?)");
  if (ImGui::IsItemHovered()) {
    ImGui::BeginTooltip();
    ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
    ImGui::TextUnformatted(desc);
    ImGui::PopTextWrapPos();
    ImGui::EndTooltip();
  }
}
