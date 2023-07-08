/*
 This file is part of Livecut
 Copyright 2004 by Remy Muller.
 Adapted for DISTRHO Plugin Framework (DPF) by eventual-recluse.
 
 Livecut can be redistributed and/or modified under the terms of the
 GNU General Public License, as published by the Free Software Foundation;
 either version 2 of the License, or (at your option) any later version.
 
 Livecut is distributed in the hope that it will be useful,
 but WITHOUT ANY WARRANTY; without even the implied warranty of
 MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 GNU General Public License for more details.
 
 You should have received a copy of the GNU General Public License
 along with Livecut; if not, visit www.gnu.org/licenses or write to the
 Free Software Foundation, Inc., 59 Temple Place, Suite 330, 
 Boston, MA 02111-1307 USA
 */

#include <cstdint>
#include "DistrhoUI.hpp"
#include "ResizeHandle.hpp"
#include "LiveCutControls.hpp"
#include "BrunoAceFont.hpp"
#include "BrunoAceSCFont.hpp"

START_NAMESPACE_DISTRHO

// --------------------------------------------------------------------------------------------------------------------

class LiveCutUI : public UI
{
    float ui_control[LVC_CONTROL_NR];
    
    // int and bool variables for Dear ImGui SliderInt and CheckBox widgets.
    int ui_maxphrase = static_cast<int>(LVC_DEFAULTS[LVC_MAXPHRS]);
    int ui_minphrase = static_cast<int>(LVC_DEFAULTS[LVC_MINPHRS]);
    int ui_maxrep = static_cast<int>(LVC_DEFAULTS[LVC_MAXREP]);
    int ui_minrep = static_cast<int>(LVC_DEFAULTS[LVC_MINREP]);
    int ui_seed = static_cast<int>(LVC_DEFAULTS[LVC_SEED]);
    bool ui_crusher_switch = static_cast<bool>(LVC_DEFAULTS[LVC_CRUSHER]);
    bool ui_comb_switch = static_cast<bool>(LVC_DEFAULTS[LVC_COMB]);
    
    ResizeHandle fResizeHandle;
    
    ImFont* brunoAceFont;
    ImFont* brunoAceSCFont;
    
    double scale_factor;
    int UI_COLUMN_WIDTH;
    
    // ----------------------------------------------------------------------------------------------------------------

public:
   /**
      UI class constructor.
      The UI should be initialized to a default state that matches the plugin side.
    */
    LiveCutUI()
        : UI(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT, true),
          fResizeHandle(this)
    {
        setGeometryConstraints(DISTRHO_UI_DEFAULT_WIDTH, DISTRHO_UI_DEFAULT_HEIGHT, true);
        
        // hide handle if UI is resizable
        if (isResizable())
            fResizeHandle.hide();
            
        std::memset(ui_control, 0, sizeof(float)*LVC_CONTROL_NR);
        
        // populate ui_control with defaults
        for (int32_t i = 0; i < LVC_CONTROL_NR; i++)
        {
            ui_control[i] = LVC_DEFAULTS[i];
        }
        
        // account for scaling
        scale_factor = getScaleFactor();
        if (scale_factor == 0) {scale_factor = 1.0;}
        
        UI_COLUMN_WIDTH = 312 * scale_factor;
        
        // Setup fonts
        ImGuiIO& io = ImGui::GetIO();
        io.Fonts->Clear();
        
        brunoAceFont = AddBrunoAceFont(scale_factor);
        brunoAceSCFont = AddBrunoAceSCFont(scale_factor);
        
        // Set style and colours
        ImGuiStyle& uistyle = ImGui::GetStyle();
        
        uistyle.GrabRounding = 3.6f;
        
        ImVec4 light_orange (0.965f, 0.714f, 0.282f, 0.8f);
        ImVec4 medium_orange (1.0f, 0.63f, 0.0f, 0.9f);
        ImVec4 pale_orange (0.965f, 0.851f, 0.655f, 0.96f);
        ImVec4 pale (0.98f, 0.98f, 0.98f, 1.00f);
        ImVec4 black (0.0f, 0.0f, 0.0f, 1.00f);
        ImVec4 pale_trans (1.0f, 1.0f, 1.0f, 0.7f);
        ImVec4 transparent (0.0f, 0.0f, 0.0f, 0.00f);
        ImVec4 grey (0.50f, 0.50f, 0.50f, 1.0f);
        
        ImVec4 light_peach (0.95f, 0.33f, 0.14f, 0.36f);
        ImVec4 peach (0.95f, 0.33f, 0.14f, 0.47f);
        ImVec4 deep_peach (0.97f, 0.31f, 0.13f, 0.81f);
        ImVec4 light_orange_opaque (1.00f, 0.79f, 0.18f, 1.0f);

        uistyle.Colors[ImGuiCol_Text] = black;
        uistyle.Colors[ImGuiCol_TextDisabled] = black;
        uistyle.Colors[ImGuiCol_WindowBg] = pale;
        uistyle.Colors[ImGuiCol_ChildBg] = transparent;
        uistyle.Colors[ImGuiCol_PopupBg] = pale;
        uistyle.Colors[ImGuiCol_Border] = grey;
        uistyle.Colors[ImGuiCol_BorderShadow] = transparent;
        uistyle.Colors[ImGuiCol_FrameBg] = light_orange;
        uistyle.Colors[ImGuiCol_FrameBgHovered] = medium_orange;
        uistyle.Colors[ImGuiCol_FrameBgActive] = pale_orange;
        uistyle.Colors[ImGuiCol_TitleBg] = light_orange_opaque;
        uistyle.Colors[ImGuiCol_TitleBgActive] = light_orange_opaque;
        uistyle.Colors[ImGuiCol_TitleBgCollapsed] = light_orange_opaque;
        uistyle.Colors[ImGuiCol_MenuBarBg] = light_orange;
        uistyle.Colors[ImGuiCol_ScrollbarBg] = light_orange;
        uistyle.Colors[ImGuiCol_ScrollbarGrab] = pale_trans;
        uistyle.Colors[ImGuiCol_ScrollbarGrabHovered] = peach;
        uistyle.Colors[ImGuiCol_ScrollbarGrabActive] = deep_peach;
        uistyle.Colors[ImGuiCol_CheckMark] = pale;
        uistyle.Colors[ImGuiCol_SliderGrab] = pale_trans;
        uistyle.Colors[ImGuiCol_SliderGrabActive] = pale;
        uistyle.Colors[ImGuiCol_Button] = light_orange;
        uistyle.Colors[ImGuiCol_ButtonHovered] = medium_orange;
        uistyle.Colors[ImGuiCol_ButtonActive] = pale_orange;
        uistyle.Colors[ImGuiCol_Header] = light_orange;
        uistyle.Colors[ImGuiCol_HeaderHovered] = medium_orange;
        uistyle.Colors[ImGuiCol_HeaderActive] = pale_orange;
        uistyle.Colors[ImGuiCol_Separator] = pale;
        uistyle.Colors[ImGuiCol_SeparatorHovered] = pale;
        uistyle.Colors[ImGuiCol_SeparatorActive] = pale;
        uistyle.Colors[ImGuiCol_ResizeGrip] = light_orange;
        uistyle.Colors[ImGuiCol_ResizeGripHovered] = medium_orange;
        uistyle.Colors[ImGuiCol_ResizeGripActive] = pale_orange;
        uistyle.Colors[ImGuiCol_Tab] = black;
        uistyle.Colors[ImGuiCol_TabHovered] = black;
        uistyle.Colors[ImGuiCol_TabActive] = black;
        uistyle.Colors[ImGuiCol_TabUnfocused] = black;
        uistyle.Colors[ImGuiCol_TabUnfocusedActive] = black;
        uistyle.Colors[ImGuiCol_PlotLines] = black;
        uistyle.Colors[ImGuiCol_PlotLinesHovered] = black;
        uistyle.Colors[ImGuiCol_PlotHistogram] = black;
        uistyle.Colors[ImGuiCol_PlotHistogramHovered] = black;
        uistyle.Colors[ImGuiCol_TableHeaderBg] = black;
        uistyle.Colors[ImGuiCol_TableBorderStrong] = black;
        uistyle.Colors[ImGuiCol_TableBorderLight] = black;
        uistyle.Colors[ImGuiCol_TableRowBg] = black;
        uistyle.Colors[ImGuiCol_TableRowBgAlt] = black;
        uistyle.Colors[ImGuiCol_TextSelectedBg] = medium_orange;
        uistyle.Colors[ImGuiCol_DragDropTarget] = black;
        uistyle.Colors[ImGuiCol_NavHighlight] = black;
        uistyle.Colors[ImGuiCol_NavWindowingHighlight] = black;
        uistyle.Colors[ImGuiCol_NavWindowingDimBg] = black;
        uistyle.Colors[ImGuiCol_ModalWindowDimBg] =  black;
    }

protected:
    // ----------------------------------------------------------------------------------------------------------------
    // DSP/Plugin Callbacks

   /**
      A parameter has changed on the plugin side.@n
      This is called by the host to inform the UI about parameter changes.
    */
    void parameterChanged(uint32_t index, float value) override
    {
        ui_control[index] = value;
        // update ui variables for SliderInt and CheckBox widgets
        switch (index)
        {
        case LVC_MINPHRS:
            ui_minphrase = static_cast<int>(ui_control[LVC_MINPHRS]);
            break;
        case LVC_MAXPHRS:
            ui_maxphrase = static_cast<int>(ui_control[LVC_MAXPHRS]);
            break;
        case LVC_MAXREP:
            ui_maxrep = static_cast<int>(ui_control[LVC_MAXREP]);
            break;
        case LVC_MINREP:
            ui_minrep = static_cast<int>(ui_control[LVC_MINREP]);
            break;
        case LVC_SEED:
            ui_seed = static_cast<int>(ui_control[LVC_SEED]);
            break;
        case LVC_CRUSHER:
            ui_crusher_switch = static_cast<bool>(ui_control[LVC_CRUSHER]);
            break;
        case LVC_COMB:
            ui_comb_switch = static_cast<bool>(ui_control[LVC_COMB]);
            break;
        default:
            break;
        }
        
        repaint();
    }

    // ----------------------------------------------------------------------------------------------------------------
    // Widget Callbacks

   /**
      ImGui specific onDisplay function.
    */
    void onImGuiDisplay() override
    {
        const float width = getWidth();
        const float height = getHeight();
        const float margin = 10.0f * getScaleFactor();

        ImGui::SetNextWindowPos(ImVec2(margin, margin));
        ImGui::SetNextWindowSize(ImVec2(width - 2 * margin, height - 2 * margin));

        if (ImGui::Begin("LiveCut", nullptr, ImGuiWindowFlags_NoResize|ImGuiWindowFlags_NoCollapse|ImGuiWindowFlags_NoTitleBar))
        {
            ImGui::BeginChild("title pane", ImVec2(0, ImGui::GetFontSize() * 3));
            
            ImGui::PushFont(brunoAceSCFont);
            //ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(40,40,40,255));
            
            ImGui::LabelText("##title_label", "LiveCut");
            
            //ImGui::PopStyleColor();
            ImGui::PopFont();
            
            ImGui::EndChild();
            
            ImGui::BeginChild("control pane", ImVec2(0, 0)); // control pane holds three columns: left, middle and right
            
            ImGui::BeginChild("left pane", ImVec2(UI_COLUMN_WIDTH, 0));
            
            ImGui::BeginChild("cutproc pane", ImVec2(UI_COLUMN_WIDTH, ImGui::GetFontSize() * 4.0f), true);
            
            //ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(40,40,40,255));

            ImGui::LabelText("##cutproc_label", "CUTPROC");
            
            //ImGui::PopStyleColor();
            
            //LVC_CUTPROC
            const char* strategies[3] = { "CutProc11", "WarpCut", "SQPusher"};
            const char* current_strategy = strategies[static_cast<int32_t>(ui_control[LVC_CUTPROC])];

            ImGuiStyle& strat_style = ImGui::GetStyle();
            float strat_w = ImGui::CalcItemWidth();
            float strat_spacing = strat_style.ItemInnerSpacing.x;
            float strat_button_sz = ImGui::GetFrameHeight();
            ImGui::PushItemWidth(strat_w - strat_spacing * 2.0f - strat_button_sz * 2.0f);
            if (ImGui::BeginCombo("##strat_combo", current_strategy, ImGuiComboFlags_NoArrowButton))
            {
                if (ImGui::IsItemActivated())
                        editParameter(LVC_CUTPROC, true);
                        
                for (int n = 0; n < IM_ARRAYSIZE(strategies); n++)
                {
                    bool is_selected = (current_strategy == strategies[n]);
                    if (ImGui::Selectable(strategies[n], is_selected))
                    {
                        current_strategy = strategies[n];
                        ui_control[LVC_CUTPROC] = static_cast<float>(n);
                        setParameterValue(LVC_CUTPROC, ui_control[LVC_CUTPROC]);
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            
            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_CUTPROC, false);
            }
            
            ImGui::PopItemWidth();
            ImGui::SameLine(0, strat_spacing);
            
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,255,255,220)); // white arrows
            
            if (ImGui::ArrowButton("##strat_l", ImGuiDir_Left))
            {
                if (ImGui::IsItemActivated())
                        editParameter(LVC_CUTPROC, true);  
                                      
                int32_t current_strat_index = static_cast<int32_t>(ui_control[LVC_CUTPROC]);
                if (current_strat_index > 0)
                {
                    current_strat_index -= 1;
                    current_strategy = strategies[current_strat_index];
                    ui_control[LVC_CUTPROC] = static_cast<float>(current_strat_index);
                    setParameterValue(LVC_CUTPROC, ui_control[LVC_CUTPROC]);
                }
            }
            
            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_CUTPROC, false);
            }
            
            
            ImGui::SameLine(0, strat_spacing);
            if (ImGui::ArrowButton("##strat_r", ImGuiDir_Right))
            {
                if (ImGui::IsItemActivated())
                        editParameter(LVC_CUTPROC, true);
                        
                int32_t current_strat_index = static_cast<int32_t>(ui_control[LVC_CUTPROC]);
                if (current_strat_index < 2)
                {
                    current_strat_index += 1;
                    current_strategy = strategies[current_strat_index];
                    ui_control[LVC_CUTPROC] = static_cast<float>(current_strat_index);
                    setParameterValue(LVC_CUTPROC, ui_control[LVC_CUTPROC]);
                }
            }
            
            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_CUTPROC, false);
            }
            
            ImGui::PopStyleColor(); // undo white text color for arrows
            
            ImGui::SameLine(0, strat_style.ItemInnerSpacing.x);
            ImGui::Text("CutProc");
            
            ImGui::EndChild(); // cutproc pane
            
            ImGui::BeginChild("global pane", ImVec2(UI_COLUMN_WIDTH, ImGui::GetFontSize() * 7.4f), true);
            
            //ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(40,40,40,255));
            
            ImGui::LabelText("##global_label", "GLOBAL");
            
            //ImGui::PopStyleColor();
            
            // LVC_MINPHRS
            if (ImGui::SliderInt("Min Phrs", &ui_minphrase, static_cast<int>(controlLimits[LVC_MINPHRS].first), static_cast<int>(controlLimits[LVC_MINPHRS].second)))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_MINPHRS, true);
                
                ui_control[LVC_MINPHRS] = static_cast<float>(ui_minphrase);
                setParameterValue(LVC_MINPHRS, ui_control[LVC_MINPHRS]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_MINPHRS, false);
            }
            
            // LVC_MAXPHRS
            if (ImGui::SliderInt("Max Phrs", &ui_maxphrase, static_cast<int>(controlLimits[LVC_MAXPHRS].first), static_cast<int>(controlLimits[LVC_MAXPHRS].second)))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_MAXPHRS, true);
                
                ui_control[LVC_MAXPHRS] = static_cast<float>(ui_maxphrase);
                setParameterValue(LVC_MAXPHRS, ui_control[LVC_MAXPHRS]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_MAXPHRS, false);
            }
            
            
            
            // LVC SUBDIV
            const char* subdivisions[7] = { "6", "8", "12", "16", "18", "24", "32"};
            const char* current_sdiv = subdivisions[static_cast<int32_t>(ui_control[LVC_SUBDIV])];

            ImGuiStyle& sdiv_style = ImGui::GetStyle();
            float sdiv_w = ImGui::CalcItemWidth();
            float sdiv_spacing = sdiv_style.ItemInnerSpacing.x;
            float sdiv_button_sz = ImGui::GetFrameHeight();
            ImGui::PushItemWidth(sdiv_w - sdiv_spacing * 2.0f - sdiv_button_sz * 2.0f);
            if (ImGui::BeginCombo("##subDiv", current_sdiv, ImGuiComboFlags_NoArrowButton))
            {
                if (ImGui::IsItemActivated())
                        editParameter(LVC_SUBDIV, true);
                        
                for (int n = 0; n < IM_ARRAYSIZE(subdivisions); n++)
                {
                    bool is_selected = (current_sdiv == subdivisions[n]);
                    if (ImGui::Selectable(subdivisions[n], is_selected))
                    {
                        current_sdiv = subdivisions[n];
                        ui_control[LVC_SUBDIV] = static_cast<float>(n);
                        setParameterValue(LVC_SUBDIV, ui_control[LVC_SUBDIV]);
                        
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            
            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_SUBDIV, false);
            }
            
            ImGui::PopItemWidth();
            
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,255,255,220)); // white arrows
            
            ImGui::SameLine(0, sdiv_spacing);
            if (ImGui::ArrowButton("##subdiv_l", ImGuiDir_Left))
            {
                if (ImGui::IsItemActivated())
                        editParameter(LVC_SUBDIV, true);
                int32_t current_subdiv_index = static_cast<int32_t>(ui_control[LVC_SUBDIV]);
                if (current_subdiv_index > 0)
                {
                    current_subdiv_index -= 1;
                    current_sdiv = subdivisions[current_subdiv_index];
                    ui_control[LVC_SUBDIV] = static_cast<float>(current_subdiv_index);
                    setParameterValue(LVC_SUBDIV, ui_control[LVC_SUBDIV]);
                }
            }
            
            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_SUBDIV, false);
            }
            
            ImGui::SameLine(0, sdiv_spacing);
            if (ImGui::ArrowButton("##subdiv_r", ImGuiDir_Right))
            {
                if (ImGui::IsItemActivated())
                        editParameter(LVC_SUBDIV, true);
                int32_t current_subdiv_index = static_cast<int32_t>(ui_control[LVC_SUBDIV]);
                if (current_subdiv_index < 6)
                {
                    current_subdiv_index += 1;
                    current_sdiv = subdivisions[current_subdiv_index];
                    ui_control[LVC_SUBDIV] = static_cast<float>(current_subdiv_index);
                    setParameterValue(LVC_SUBDIV, ui_control[LVC_SUBDIV]);
                }
            }
            
            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_SUBDIV, false);
            }
            
            ImGui::PopStyleColor(); // undo white text color for arrows
            
            ImGui::SameLine(0, sdiv_style.ItemInnerSpacing.x);
            
            ImGui::Text("SubDiv");
            
            ImGui::EndChild(); // global pane
            
            ImGui::BeginChild("env pane", ImVec2(UI_COLUMN_WIDTH, ImGui::GetFontSize() * 7.4f), true);
            
            //ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(40,40,40,255));
            
            ImGui::LabelText("##env_label", "ENV");
            
            //ImGui::PopStyleColor();
            
            // LVC_DUTY
            if (ImGui::SliderFloat("Duty", &ui_control[LVC_DUTY], controlLimits[LVC_DUTY].first, controlLimits[LVC_DUTY].second, "%.2f", ImGuiSliderFlags_NoInput))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_DUTY, true);

                setParameterValue(LVC_DUTY, ui_control[LVC_DUTY]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_DUTY, false);
            }
            
            // LVC_FILLDUTY
            if (ImGui::SliderFloat("Fill Duty", &ui_control[LVC_FILLDUTY], controlLimits[LVC_FILLDUTY].first, controlLimits[LVC_FILLDUTY].second, "%.2f", ImGuiSliderFlags_NoInput))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_FILLDUTY, true);

                setParameterValue(LVC_FILLDUTY, ui_control[LVC_FILLDUTY]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_FILLDUTY, false);
            }
            
            // LVC_FADE
            if (ImGui::SliderFloat("Fade", &ui_control[LVC_FADE], controlLimits[LVC_FADE].first, controlLimits[LVC_FADE].second, "%.2f ms", ImGuiSliderFlags_Logarithmic|ImGuiSliderFlags_NoInput))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_FADE, true);

                setParameterValue(LVC_FADE, ui_control[LVC_FADE]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_FADE, false);
            }
            
            ImGui::EndChild(); // env pane
            
            
            ImGui::BeginChild("amp pane", ImVec2(UI_COLUMN_WIDTH, ImGui::GetFontSize() * 9.0f), true);
            
            //ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(40,40,40,255));
            
            ImGui::LabelText("##amp_label", "AMP");
            
            //ImGui::PopStyleColor();
            
            // LVC_MINAMP
            if (ImGui::SliderFloat("Min Amp", &ui_control[LVC_MINAMP], controlLimits[LVC_MINAMP].first, controlLimits[LVC_MINAMP].second, "%.2f", ImGuiSliderFlags_NoInput))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_MINAMP, true);

                setParameterValue(LVC_MINAMP, ui_control[LVC_MINAMP]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_MINAMP, false);
            }
            
            // LVC_MAXAMP
            if (ImGui::SliderFloat("Max Amp", &ui_control[LVC_MAXAMP], controlLimits[LVC_MAXAMP].first, controlLimits[LVC_MAXAMP].second, "%.2f", ImGuiSliderFlags_NoInput))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_MAXAMP, true);

                setParameterValue(LVC_MAXAMP, ui_control[LVC_MAXAMP]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_MAXAMP, false);
            }
            
            // LVC_MINPAN
            if (ImGui::SliderFloat("Min Pan", &ui_control[LVC_MINPAN], controlLimits[LVC_MINPAN].first, controlLimits[LVC_MINPAN].second, "%.2f", ImGuiSliderFlags_NoInput))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_MINPAN, true);

                setParameterValue(LVC_MINPAN, ui_control[LVC_MINPAN]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_MINPAN, false);
            }
            
            // LVC_MAXPAN
            if (ImGui::SliderFloat("Max Pan", &ui_control[LVC_MAXPAN], controlLimits[LVC_MAXPAN].first, controlLimits[LVC_MAXPAN].second, "%.2f", ImGuiSliderFlags_NoInput))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_MAXPAN, true);

                setParameterValue(LVC_MAXPAN, ui_control[LVC_MAXPAN]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_MAXPAN, false);
            }
            
            
            ImGui::EndChild(); // amp pane
            
            ImGui::EndChild(); // left pane
            
            ImGui::SameLine(); 
            
            ImGui::BeginChild("middle pane", ImVec2(UI_COLUMN_WIDTH, 0));
            
            ImGui::BeginChild("pitch pane", ImVec2(UI_COLUMN_WIDTH, ImGui::GetFontSize() * 6.0f), true);
            
            //ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(40,40,40,255));
            
            ImGui::LabelText("##pitch_label", "PITCH");
            
            //ImGui::PopStyleColor();
            
            // LVC_MINPITCH
            if (ImGui::SliderFloat("Min Pitch", &ui_control[LVC_MINPITCH], controlLimits[LVC_MINPITCH].first, controlLimits[LVC_MINPITCH].second, "%.2f cents", ImGuiSliderFlags_NoInput))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_MINPITCH, true);

                setParameterValue(LVC_MINPITCH, ui_control[LVC_MINPITCH]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_MINPITCH, false);
            }
            
            // LVC_MAXPITCH
            if (ImGui::SliderFloat("Max Pitch", &ui_control[LVC_MAXPITCH], controlLimits[LVC_MAXPITCH].first, controlLimits[LVC_MAXPITCH].second, "%.2f cents", ImGuiSliderFlags_NoInput))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_MAXPITCH, true);

                setParameterValue(LVC_MAXPITCH, ui_control[LVC_MAXPITCH]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_MAXPITCH, false);
            }
            
            ImGui::EndChild(); // pitch pane
            
            ImGui::BeginChild("crusher pane", ImVec2(UI_COLUMN_WIDTH, ImGui::GetFontSize() * 11.0f), true);
            
            //ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(40,40,40,255));
            
            ImGui::LabelText("##crusher_label", "BITCRUSHER");
            
            //ImGui::PopStyleColor();
            
            // LVC_CRUSHER
            if (ImGui::Checkbox("On/Off", &ui_crusher_switch))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_CRUSHER, true);
                    
                ui_control[LVC_CRUSHER] = static_cast<float>(ui_crusher_switch);
                setParameterValue(LVC_CRUSHER, ui_control[LVC_CRUSHER]);

            }
            
            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_CRUSHER, false);
            }
            
            // LVC_MINBITS
            if (ImGui::SliderFloat("Min Bits", &ui_control[LVC_MINBITS], controlLimits[LVC_MINBITS].first, controlLimits[LVC_MINBITS].second, "%.2f", ImGuiSliderFlags_NoInput))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_MINBITS, true);

                setParameterValue(LVC_MINBITS, ui_control[LVC_MINBITS]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_MINBITS, false);
            }
            
            // LVC_MAXBITS
            if (ImGui::SliderFloat("Max Bits", &ui_control[LVC_MAXBITS], controlLimits[LVC_MAXBITS].first, controlLimits[LVC_MAXBITS].second, "%.2f", ImGuiSliderFlags_NoInput))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_MAXBITS, true);

                setParameterValue(LVC_MAXBITS, ui_control[LVC_MAXBITS]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_MAXBITS, false);
            }
            
            // LVC_MINFREQ
            if (ImGui::SliderFloat("Min Freq", &ui_control[LVC_MINFREQ], controlLimits[LVC_MINFREQ].first, controlLimits[LVC_MINFREQ].second, "%.2f Hz", ImGuiSliderFlags_Logarithmic|ImGuiSliderFlags_NoInput))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_MINFREQ, true);

                setParameterValue(LVC_MINFREQ, ui_control[LVC_MINFREQ]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_MINFREQ, false);
            }
            
            // LVC_MAXFREQ
            if (ImGui::SliderFloat("Max Freq", &ui_control[LVC_MAXFREQ], controlLimits[LVC_MAXFREQ].first, controlLimits[LVC_MAXFREQ].second, "%.2f Hz", ImGuiSliderFlags_Logarithmic|ImGuiSliderFlags_NoInput))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_MAXFREQ, true);

                setParameterValue(LVC_MAXFREQ, ui_control[LVC_MAXFREQ]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_MAXFREQ, false);
            }
            
            ImGui::EndChild(); // crusher pane
            
            ImGui::BeginChild("comb pane", ImVec2(UI_COLUMN_WIDTH, ImGui::GetFontSize() * 11.0f), true);
            
            //ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(40,40,40,255));
            
            ImGui::LabelText("##comb_label", "COMB");
            
            //ImGui::PopStyleColor();
            
            // LVC_COMB
            if (ImGui::Checkbox("On/Off", &ui_comb_switch))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_COMB, true);
                    
                ui_control[LVC_COMB] = static_cast<float>(ui_comb_switch);
                setParameterValue(LVC_COMB, ui_control[LVC_COMB]);

            }
            
            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_COMB, false);
            }
            
            
            // LVC_TYPE
            const char* comb_types[2] = { "FeedForward", "FeedBack"};
            const char* current_comb_type = comb_types[static_cast<int32_t>(ui_control[LVC_TYPE])];

            ImGuiStyle& comb_style = ImGui::GetStyle();
            float comb_w = ImGui::CalcItemWidth();
            float comb_spacing = comb_style.ItemInnerSpacing.x;
            float comb_button_sz = ImGui::GetFrameHeight();
            ImGui::PushItemWidth(comb_w - comb_spacing * 2.0f - comb_button_sz * 2.0f);
            if (ImGui::BeginCombo("##comb_combo", current_comb_type, ImGuiComboFlags_NoArrowButton))
            {
                if (ImGui::IsItemActivated())
                        editParameter(LVC_TYPE, true);
                        
                for (int n = 0; n < IM_ARRAYSIZE(comb_types); n++)
                {
                    bool is_selected = (current_comb_type == comb_types[n]);
                    if (ImGui::Selectable(comb_types[n], is_selected))
                    {
                        current_comb_type = comb_types[n];
                        ui_control[LVC_TYPE] = static_cast<float>(n);
                        setParameterValue(LVC_TYPE, ui_control[LVC_TYPE]);
                    }
                    if (is_selected)
                        ImGui::SetItemDefaultFocus();
                }
                ImGui::EndCombo();
            }
            
            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_TYPE, false);
            }
            
            ImGui::PopItemWidth();
            
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,255,255,220)); // white arrows
            
            ImGui::SameLine(0, comb_spacing);
            if (ImGui::ArrowButton("##comb_l", ImGuiDir_Left))
            {
                if (ImGui::IsItemActivated())
                        editParameter(LVC_TYPE, true);  
                                      
                int32_t current_comb_index = static_cast<int32_t>(ui_control[LVC_TYPE]);
                if (current_comb_index > 0)
                {
                    current_comb_index -= 1;
                    current_comb_type = comb_types[current_comb_index];
                    ui_control[LVC_TYPE] = static_cast<float>(current_comb_index);
                    setParameterValue(LVC_TYPE, ui_control[LVC_TYPE]);
                }
            }
            
            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_TYPE, false);
            }
            
            ImGui::SameLine(0, comb_spacing);
            if (ImGui::ArrowButton("##comb_r", ImGuiDir_Right))
            {
                if (ImGui::IsItemActivated())
                        editParameter(LVC_TYPE, true);
                        
                int32_t current_comb_index = static_cast<int32_t>(ui_control[LVC_TYPE]);
                if (current_comb_index < 1)
                {
                    current_comb_index += 1;
                    current_comb_type = comb_types[current_comb_index];
                    ui_control[LVC_TYPE] = static_cast<float>(current_comb_index);
                    setParameterValue(LVC_TYPE, ui_control[LVC_TYPE]);
                }
            }
            
            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_TYPE, false);
            }
            
            ImGui::PopStyleColor(); // undo white text for arrows
            
            ImGui::SameLine(0, comb_style.ItemInnerSpacing.x);
            ImGui::Text("Comb Type");
            
            
            // LVC_FEEDBACK
            if (ImGui::SliderFloat("Feedback", &ui_control[LVC_FEEDBACK], controlLimits[LVC_FEEDBACK].first, controlLimits[LVC_FEEDBACK].second, "%.2f", ImGuiSliderFlags_NoInput))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_FEEDBACK, true);

                setParameterValue(LVC_FEEDBACK, ui_control[LVC_FEEDBACK]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_FEEDBACK, false);
            }
            
            // LVC_MINDELAY
            if (ImGui::SliderFloat("Min Delay", &ui_control[LVC_MINDELAY], controlLimits[LVC_MINDELAY].first, controlLimits[LVC_MINDELAY].second, "%.2f ms", ImGuiSliderFlags_Logarithmic))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_MINDELAY, true);

                setParameterValue(LVC_MINDELAY, ui_control[LVC_MINDELAY]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_MINDELAY, false);
            }
            
            // LVC_MAXDELAY
            if (ImGui::SliderFloat("Max Delay", &ui_control[LVC_MAXDELAY], controlLimits[LVC_MAXDELAY].first, controlLimits[LVC_MAXDELAY].second, "%.2f ms", ImGuiSliderFlags_Logarithmic))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_MAXDELAY, true);

                setParameterValue(LVC_MAXDELAY, ui_control[LVC_MAXDELAY]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_MAXDELAY, false);
            }
            
            ImGui::EndChild(); // comb pane
            
            
            ImGui::EndChild(); // middle pane
            
            ImGui::SameLine(); // for right pane to be right of the middle pane
            
            ImGui::BeginChild("right pane", ImVec2(UI_COLUMN_WIDTH, 0));
            
            ImGui::BeginChild("cutproc11 pane", ImVec2(UI_COLUMN_WIDTH, ImGui::GetFontSize() * 9.4f), true);
            
            //ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(40,40,40,255));
            
            ImGui::LabelText("##cutproc11_label", "CUTPROC11");
            
            //ImGui::PopStyleColor();
            
            // LVC_MAXREP
            if (ImGui::SliderInt("Max Rep", &ui_maxrep, static_cast<int>(controlLimits[LVC_MAXREP].first), static_cast<int>(controlLimits[LVC_MAXREP].second)))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_MAXREP, true);
                
                ui_control[LVC_MAXREP] = static_cast<float>(ui_maxrep);
                setParameterValue(LVC_MAXREP, ui_control[LVC_MAXREP]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_MAXREP, false);
            }
            
            // LVC_MINREP
            if (ImGui::SliderInt("Min Rep", &ui_minrep, static_cast<int>(controlLimits[LVC_MINREP].first), static_cast<int>(controlLimits[LVC_MINREP].second)))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_MINREP, true);
                
                ui_control[LVC_MINREP] = static_cast<float>(ui_minrep);
                setParameterValue(LVC_MINREP, ui_control[LVC_MINREP]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_MINREP, false);
            }
            
            // LVC_STUTTER
            if (ImGui::SliderFloat("Stutter", &ui_control[LVC_STUTTER], controlLimits[LVC_STUTTER].first, controlLimits[LVC_STUTTER].second, "%.2f", ImGuiSliderFlags_NoInput))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_STUTTER, true);

                setParameterValue(LVC_STUTTER, ui_control[LVC_STUTTER]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_STUTTER, false);
            }
            
            // LVC_AREA
            if (ImGui::SliderFloat("Area", &ui_control[LVC_AREA], controlLimits[LVC_AREA].first, controlLimits[LVC_AREA].second, "%.2f", ImGuiSliderFlags_NoInput))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_AREA, true);

                setParameterValue(LVC_AREA, ui_control[LVC_AREA]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_AREA, false);
            }
            
            ImGui::EndChild(); // cutproc11 pane
            
            ImGui::BeginChild("warpcut pane", ImVec2(UI_COLUMN_WIDTH, ImGui::GetFontSize() * 9.4f), true);
            
            //ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(40,40,40,255));
            
            ImGui::LabelText("##warpcut_label", "WARPCUT");
            
            //ImGui::PopStyleColor();
            
            // LVC_STRAIGHT
            if (ImGui::SliderFloat("Straight", &ui_control[LVC_STRAIGHT], controlLimits[LVC_STRAIGHT].first, controlLimits[LVC_STRAIGHT].second, "%.2f", ImGuiSliderFlags_NoInput))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_STRAIGHT, true);

                setParameterValue(LVC_STRAIGHT, ui_control[LVC_STRAIGHT]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_STRAIGHT, false);
            }
            
            // LVC_REGULAR
            if (ImGui::SliderFloat("Regular", &ui_control[LVC_REGULAR], controlLimits[LVC_REGULAR].first, controlLimits[LVC_REGULAR].second, "%.2f", ImGuiSliderFlags_NoInput))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_REGULAR, true);

                setParameterValue(LVC_REGULAR, ui_control[LVC_REGULAR]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_REGULAR, false);
            }
            
            // LVC_RITARD
            if (ImGui::SliderFloat("Ritard", &ui_control[LVC_RITARD], controlLimits[LVC_RITARD].first, controlLimits[LVC_RITARD].second, "%.2f", ImGuiSliderFlags_NoInput))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_RITARD, true);

                setParameterValue(LVC_RITARD, ui_control[LVC_RITARD]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_RITARD, false);
            }
            
            // LVC_SPEED
            if (ImGui::SliderFloat("Speed", &ui_control[LVC_SPEED], controlLimits[LVC_SPEED].first, controlLimits[LVC_SPEED].second, "%.2f", ImGuiSliderFlags_NoInput))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_SPEED, true);

                setParameterValue(LVC_SPEED, ui_control[LVC_SPEED]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_SPEED, false);
            }
            
            ImGui::EndChild(); // warpcut pane
            
            ImGui::BeginChild("sqpusher pane", ImVec2(UI_COLUMN_WIDTH, ImGui::GetFontSize() * 4.5f), true);
            
            //ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(40,40,40,255));
            
            ImGui::LabelText("##sqpusher_label", "SQPUSHER");
            
            //ImGui::PopStyleColor();
            
            // LVC_ACTIVITY
            if (ImGui::SliderFloat("Activity", &ui_control[LVC_ACTIVITY], controlLimits[LVC_ACTIVITY].first, controlLimits[LVC_ACTIVITY].second), "%.2f", ImGuiSliderFlags_NoInput)
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_ACTIVITY, true);

                setParameterValue(LVC_ACTIVITY, ui_control[LVC_ACTIVITY]);
            }

            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_ACTIVITY, false);
            }
            
            ImGui::EndChild(); // sqpusher pane
            
            ImGui::BeginChild("seed pane", ImVec2(UI_COLUMN_WIDTH, ImGui::GetFontSize() * 4.5f), true);
            
            //ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(40,40,40,255));
            
            ImGui::LabelText("##seed_label", "SEED");
            
            //ImGui::PopStyleColor();
            
            // LVC_SEED
            if (ImGui::SliderInt("Seed", &ui_seed, static_cast<int>(controlLimits[LVC_SEED].first), static_cast<int>(controlLimits[LVC_SEED].second)))
            {
                if (ImGui::IsItemActivated())
                    editParameter(LVC_SEED, true);
                
                ui_control[LVC_SEED] = static_cast<float>(ui_seed);
                setParameterValue(LVC_SEED, ui_control[LVC_SEED]);
            }
            
            if (ImGui::IsItemDeactivated())
            {
                editParameter(LVC_SEED, false);
            }
            
            ImGui::EndChild(); // seed pane
            
            ImGui::EndChild(); // right pane
            
            ImGui::EndChild(); // control pane
        }
        ImGui::End();
    }

    DISTRHO_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(LiveCutUI)
};

// --------------------------------------------------------------------------------------------------------------------

UI* createUI()
{
    return new LiveCutUI();
}

// --------------------------------------------------------------------------------------------------------------------

END_NAMESPACE_DISTRHO
