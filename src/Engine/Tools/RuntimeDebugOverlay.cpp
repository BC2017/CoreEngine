#include "Engine/Tools/RuntimeDebugOverlay.hpp"

#include "imgui.h"

namespace HFEngine::Tools
{
    void DrawRuntimeDebugOverlay(const RuntimeDebugOverlayData& data)
    {
        ImGui::SetNextWindowPos(ImVec2(16.0f, 16.0f), ImGuiCond_FirstUseEver);
        ImGui::SetNextWindowSize(ImVec2(360.0f, 180.0f), ImGuiCond_FirstUseEver);

        ImGui::Begin("Runtime Debug");
        ImGui::Text("Backend: %.*s", static_cast<int>(data.backendName.size()), data.backendName.data());
        ImGui::Text("Adapter: %.*s", static_cast<int>(data.adapterName.size()), data.adapterName.data());
        ImGui::Text("Validation: %s", data.validationEnabled ? "enabled" : "disabled");
        ImGui::Text("Frame: %u", data.framesRendered);
        ImGui::Text("Frame time: %.3f ms", data.frameTimeMilliseconds);
        ImGui::Separator();
        ImGui::TextUnformatted("Indexed mesh milestone");
        ImGui::End();
    }
}
