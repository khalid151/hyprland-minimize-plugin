#include <algorithm>
#include <hyprland/src/SharedDefs.hpp>
#include <hyprland/src/plugins/PluginAPI.hpp>
#define WLR_USE_UNSTABLE

#include <hyprland/src/Compositor.hpp>
#include <hyprland/src/managers/KeybindManager.hpp>

#include <hyprutils/string/VarList.hpp>

#include "globals.hpp"

void minimize(PHLWINDOW pWindow) {
    auto workspace = pWindow->m_workspace;
    if (workspace->m_name == "special:minimized")
        return;
    pWindow->m_tags.applyTag(std::format("workspace:{}", (int)workspace->m_id));
    g_pKeybindManager->m_dispatchers["movetoworkspacesilent"](std::format("special:minimized,address:0x{:x}", pWindow));
}

void unminimize(PHLWINDOW pWindow) {
    if (pWindow->m_workspace->m_name != "special:minimized")
        return;
    auto tags = pWindow->m_tags.getTags();
    auto it   = std::find_if(tags.begin(), tags.end(), [](const std::string& tag) { return tag.starts_with("workspace:"); });
    if (it != tags.end() && it->substr(0, 10) == "workspace:") {
        pWindow->m_tags.applyTag(*it); // removing the tag
        g_pKeybindManager->m_dispatchers["movetoworkspace"](std::format("{},address:0x{:x}", it->substr(10), pWindow));
    }
}

static SDispatchResult dispatchMinimize(std::string args) {
    CVarList vars(args, 0, ',');

    if (!vars[0].empty()) {
        const auto PWINDOW = g_pCompositor->getWindowByRegex(vars[0]);
        if (PWINDOW)
            minimize(PWINDOW);
        else
            return SDispatchResult{.success = false, .error = "No focused window."};
    } else if (auto window = g_pCompositor->m_lastWindow.lock())
        minimize(window);

    return SDispatchResult{};
}

static SDispatchResult dispatchUnminimize(std::string args) {
    CVarList vars(args, 0, ',');

    if (vars[0].empty())
        return SDispatchResult{.success = false, .error = "No window was provided"};

    const auto PWINDOW = g_pCompositor->getWindowByRegex(vars[0]);
    if (!PWINDOW) {
        return SDispatchResult{.success = false, .error = std::format("Invalid window: {}", vars[0])};
    }
    unminimize(PWINDOW);
    return SDispatchResult{};
}

APICALL EXPORT std::string PLUGIN_API_VERSION() {
    return HYPRLAND_API_VERSION;
}

APICALL EXPORT PLUGIN_DESCRIPTION_INFO PLUGIN_INIT(HANDLE handle) {
    PHANDLE = handle;

    const std::string HASH = __hyprland_api_get_hash();
    const std::string CLIENT_HASH = __hyprland_api_get_client_hash();

    if (HASH != CLIENT_HASH) {
        HyprlandAPI::addNotification(PHANDLE,
                                     "[minimize-dispatcher] Failure in initialization: Version mismatch "
                                     "(headers ver is not equal to running hyprland ver)",
                                     CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
        throw std::runtime_error("[minimize-dispatcher] Version mismatch");
    }

    bool sucess = true;
    sucess      = sucess && HyprlandAPI::addDispatcherV2(PHANDLE, "plugin:minimize:minimize", ::dispatchMinimize);
    sucess      = sucess && HyprlandAPI::addDispatcherV2(PHANDLE, "plugin:minimize:unminimize", ::dispatchUnminimize);

    if (!sucess) {
        HyprlandAPI::addNotification(PHANDLE,
                                     "[minimize-dispatcher] Failure in initialization: Failed to find "
                                     "required hook fns",
                                     CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
        throw std::runtime_error("[minimize-dispatcher] Hooks fn init failed");
    }

    if (sucess)
        HyprlandAPI::addNotification(PHANDLE, "[minimize-dispatcher] Initialized successfully!", CHyprColor{0.2, 1.0, 0.2, 1.0}, 5000);
    else {
        HyprlandAPI::addNotification(PHANDLE, "[minimize-dispatcher] Failure in initialization (hook failed)!", CHyprColor{1.0, 0.2, 0.2, 1.0}, 5000);
        throw std::runtime_error("[minimize-dispatcher] Hooks failed");
    }

    return {"minimize-dispatcher", "A hyprland plugin to add minimize and unminimize dispatchers in addition to having CSD minimize button work.", "khalid151", "0.1"};
}

APICALL EXPORT void PLUGIN_EXIT() {
    ;
}
