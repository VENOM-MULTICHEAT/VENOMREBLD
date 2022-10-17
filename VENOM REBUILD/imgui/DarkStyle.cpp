#include "../plugin.h"

#define VARNAME(v) (#v)

void ImGuiColorEdit(ImGuiCol_ tab, std::string text)
{
	static ImVec4 VecStockColors[43];
	static bool   bStockColors = false;

	if (!bStockColors)
	{
		for (int i = 0; i <= 42; i++)
			VecStockColors[i] = ImGui::GetStyle().Colors[i];
		bStockColors = true;
	}

	float col[4];

	col[0] = ImGui::GetStyle().Colors[tab].x;
	col[1] = ImGui::GetStyle().Colors[tab].y;
	col[2] = ImGui::GetStyle().Colors[tab].z;
	col[3] = ImGui::GetStyle().Colors[tab].w;

	if (col[0] == VecStockColors[tab].x &&
		col[1] == VecStockColors[tab].y &&
		col[2] == VecStockColors[tab].z &&
		col[3] == VecStockColors[tab].w)
	{
		text += " ( stock )";
	}

	ImGui::ColorEdit4(text.c_str(), col);
	ImGui::GetStyle().Colors[tab].x = col[0];
	ImGui::GetStyle().Colors[tab].y = col[1];
	ImGui::GetStyle().Colors[tab].z = col[2];
	ImGui::GetStyle().Colors[tab].w = col[3];
}

void ImGui_Dark_ApplyStyle()
{
	ImGuiStyle * style = &ImGui::GetStyle();

	style->WindowPadding = ImVec2(15, 15);
	style->WindowRounding = 5.0f;
	style->WindowBorderSize = 0.0f;
	style->FramePadding = ImVec2(5, 5);
	style->FrameRounding = 4.0f;
	style->ItemSpacing = ImVec2(12, 8);
	style->ItemInnerSpacing = ImVec2(8, 6);
	style->IndentSpacing = 25.0f;
	style->ScrollbarSize = 15.0f;
	style->ScrollbarRounding = 9.0f;
	style->GrabMinSize = 5.0f;
	style->GrabRounding = 3.0f;

	style->Colors[ImGuiCol_Text] = ImVec4(0.80f, 0.80f, 0.83f, 1.00f);
	style->Colors[ImGuiCol_TextDisabled] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_WindowBg] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_ChildWindowBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_PopupBg] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_Border] = ImVec4(0.80f, 0.80f, 0.83f, 0.88f);
	style->Colors[ImGuiCol_BorderShadow] = ImVec4(0.92f, 0.91f, 0.88f, 0.00f);
	style->Colors[ImGuiCol_FrameBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_FrameBgHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_FrameBgActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_TitleBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_TitleBgCollapsed] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_TitleBgActive] = ImVec4(0.07f, 0.07f, 0.09f, 1.00f);
	style->Colors[ImGuiCol_MenuBarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarBg] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_ScrollbarGrabHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ScrollbarGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	// style->Colors[ImGuiCol_ComboBg] = ImVec4(0.19f, 0.18f, 0.21f, 1.00f);
	style->Colors[ImGuiCol_CheckMark] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrab] = ImVec4(0.80f, 0.80f, 0.83f, 0.31f);
	style->Colors[ImGuiCol_SliderGrabActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Button] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_ButtonHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ButtonActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_Header] = ImVec4(0.10f, 0.09f, 0.12f, 1.00f);
	style->Colors[ImGuiCol_HeaderHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_HeaderActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	style->Colors[ImGuiCol_Column] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ColumnHovered] = ImVec4(0.24f, 0.23f, 0.29f, 1.00f);
	style->Colors[ImGuiCol_ColumnActive] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGrip] = ImVec4(0.00f, 0.00f, 0.00f, 0.00f);
	style->Colors[ImGuiCol_ResizeGripHovered] = ImVec4(0.56f, 0.56f, 0.58f, 1.00f);
	style->Colors[ImGuiCol_ResizeGripActive] = ImVec4(0.06f, 0.05f, 0.07f, 1.00f);
	// style->Colors[ImGuiCol_CloseButton] = ImVec4(0.40f, 0.39f, 0.38f, 0.16f);
	// style->Colors[ImGuiCol_CloseButtonHovered] = ImVec4(0.40f, 0.39f, 0.38f, 0.39f);
	// style->Colors[ImGuiCol_CloseButtonActive] = ImVec4(0.40f, 0.39f, 0.38f, 1.00f);
	style->Colors[ImGuiCol_PlotLines] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotLinesHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_PlotHistogram] = ImVec4(0.40f, 0.39f, 0.38f, 0.63f);
	style->Colors[ImGuiCol_PlotHistogramHovered] = ImVec4(0.25f, 1.00f, 0.00f, 1.00f);
	style->Colors[ImGuiCol_TextSelectedBg] = ImVec4(0.25f, 1.00f, 0.00f, 0.43f);
	style->Colors[ImGuiCol_ModalWindowDarkening] = ImVec4(1.00f, 0.98f, 0.95f, 0.73f);
}

void ImGui_Dark_StyleEditor()
{
	if (ImGui::CollapsingHeader("Style Editor"))
	{
		if (ImGui::Button("Classic"))
			ImGui::StyleColorsClassic();
		if (ImGui::Button("Light"))
			ImGui::StyleColorsLight();
		if (ImGui::Button("Dark"))
			ImGui::StyleColorsDark();
		if (ImGui::Button("New Dark"))
			ImGui_Dark_ApplyStyle();

		ImGui::Separator();
		ImGuiStyle *style = &ImGui::GetStyle();

		ImGui::Checkbox("AntiAliasedFill", &style->AntiAliasedFill);
		ImGui::Checkbox("AntiAliasedLines", &style->AntiAliasedLines);
		ImGui::SliderFloat("Alpha", &style->Alpha, 0.15f, 100.0f);
		ImGui::SliderFloat("GrabRounding", &style->GrabRounding, 0.0f, 100.0f);
		ImGui::SliderFloat("GrabMinSize", &style->GrabMinSize, 0.0f, 100.0f);

		ImGui::Separator();
		ImGui::SliderFloat("ButtonTextAlign X", &style->ButtonTextAlign.x, 0.0f, 100.0f);
		ImGui::SliderFloat("ButtonTextAlign Y", &style->ButtonTextAlign.y, 0.0f, 100.0f);
		ImGui::Separator();
		ImGui::SliderFloat("DisplaySafeAreaPadding X", &style->DisplaySafeAreaPadding.x, 0.0f, 100.0f);
		ImGui::SliderFloat("DisplaySafeAreaPadding Y", &style->DisplaySafeAreaPadding.y, 0.0f, 100.0f);
		ImGui::Separator();
		ImGui::SliderFloat("ItemInnerSpacing X", &style->ItemInnerSpacing.x, 0.0f, 100.0f);
		ImGui::SliderFloat("ItemInnerSpacing Y", &style->ItemInnerSpacing.y, 0.0f, 100.0f);
		ImGui::Separator();
		ImGui::SliderFloat("ItemSpacing X", &style->ItemSpacing.x, 0.0f, 100.0f);
		ImGui::SliderFloat("ItemSpacing Y", &style->ItemSpacing.y, 0.0f, 100.0f);
		ImGui::Separator();
		ImGui::SliderFloat("TouchExtraPadding X", &style->TouchExtraPadding.x, 0.0f, 100.0f);
		ImGui::SliderFloat("TouchExtraPadding Y", &style->TouchExtraPadding.y, 0.0f, 100.0f);
		ImGui::Separator();

		ImGui::SliderFloat("ChildBorderSize", &style->ChildBorderSize, 0.0f, 100.0f);
		ImGui::SliderFloat("ChildRounding", &style->ChildRounding, 0.0f, 100.0f);
		ImGui::SliderFloat("ColumnsMinSpacing", &style->ColumnsMinSpacing, 0.0f, 100.0f);
		ImGui::SliderFloat("CurveTessellationTol", &style->CurveTessellationTol, 0.0f, 100.0f);
		ImGui::SliderFloat("IndentSpacing", &style->GrabMinSize, 0.0f, 100.0f);
		ImGui::SliderFloat("MouseCursorScale", &style->MouseCursorScale, 0.0f, 100.0f);
		ImGui::SliderFloat("PopupBorderSize", &style->PopupBorderSize, 0.0f, 100.0f);
		ImGui::SliderFloat("PopupRounding", &style->PopupRounding, 0.0f, 100.0f);
		ImGui::SliderFloat("ScrollbarRounding", &style->ScrollbarRounding, 0.0f, 100.0f);
		ImGui::SliderFloat("ScrollbarSize", &style->ScrollbarSize, 0.0f, 100.0f);
		
		if (ImGui::BeginMenu("Window"))
		{
			ImGui::SliderFloat("Padding X##WindowPaddingX", &style->WindowPadding.x, 0.0f, 100.0f);
			ImGui::SliderFloat("Padding Y##WindowPaddingY", &style->WindowPadding.y, 0.0f, 100.0f);
			ImGui::SliderFloat("TitleAlign X##WindowTitleAlignX", &style->WindowTitleAlign.x, 0.0f, 100.0f);
			ImGui::SliderFloat("TitleAlign Y##WindowTitleAlignY", &style->WindowTitleAlign.y, 0.0f, 100.0f);
			ImGui::SliderFloat("Round##WindowRound", &style->WindowRounding, 0.0f, 100.0f);
			ImGui::SliderFloat("Border##WindowBorder", &style->WindowBorderSize, 0.0f, 100.0f);
			ImGui::EndMenu();
		}

		if (ImGui::BeginMenu("Frame"))
		{
			ImGui::SliderFloat("Padding X##FramePaddingX", &style->FramePadding.x, 0.0f, 100.0f);
			ImGui::SliderFloat("Padding Y##FramePaddingY", &style->FramePadding.y, 0.0f, 100.0f);
			ImGui::SliderFloat("Round##FrameRound", &style->FrameRounding, 0.0f, 100.0f);
			ImGui::SliderFloat("Border##FrameBorder", &style->FrameBorderSize, 0.0f, 100.0f);
			ImGui::EndMenu();
		}

		ImGuiColorEdit(ImGuiCol_Border, "ImGuiCol_Border");
		ImGuiColorEdit(ImGuiCol_BorderShadow, "ImGuiCol_BorderShadow");
		ImGuiColorEdit(ImGuiCol_Button, "ImGuiCol_Button");
		ImGuiColorEdit(ImGuiCol_ButtonActive, "ImGuiCol_ButtonActive");
		ImGuiColorEdit(ImGuiCol_ButtonHovered, "ImGuiCol_ButtonHovered");
		ImGuiColorEdit(ImGuiCol_CheckMark, "ImGuiCol_CheckMark");
		ImGuiColorEdit(ImGuiCol_ChildBg, "ImGuiCol_ChildBg");
		ImGuiColorEdit(ImGuiCol_ChildWindowBg, "ImGuiCol_ChildWindowBg");
		ImGuiColorEdit(ImGuiCol_Column, "ImGuiCol_Column");
		ImGuiColorEdit(ImGuiCol_ColumnActive, "ImGuiCol_ColumnActive");
		ImGuiColorEdit(ImGuiCol_ColumnHovered, "ImGuiCol_ColumnHovered");
		ImGuiColorEdit(ImGuiCol_DragDropTarget, "ImGuiCol_DragDropTarget");
		ImGuiColorEdit(ImGuiCol_FrameBg, "ImGuiCol_FrameBg");
		ImGuiColorEdit(ImGuiCol_FrameBgActive, "ImGuiCol_FrameBgActive");
		ImGuiColorEdit(ImGuiCol_FrameBgHovered, "ImGuiCol_FrameBgHovered");
		ImGuiColorEdit(ImGuiCol_HeaderActive, "ImGuiCol_HeaderActive");
		ImGuiColorEdit(ImGuiCol_HeaderHovered, "ImGuiCol_HeaderHovered");
		ImGuiColorEdit(ImGuiCol_MenuBarBg, "ImGuiCol_MenuBarBg");
		ImGuiColorEdit(ImGuiCol_ModalWindowDarkening, "ImGuiCol_ModalWindowDarkening");
		ImGuiColorEdit(ImGuiCol_ModalWindowDimBg, "ImGuiCol_ModalWindowDimBg");
		ImGuiColorEdit(ImGuiCol_NavHighlight, "ImGuiCol_NavHighlight");
		ImGuiColorEdit(ImGuiCol_NavWindowingDimBg, "ImGuiCol_NavWindowingDimBg");
		ImGuiColorEdit(ImGuiCol_NavWindowingHighlight, "ImGuiCol_NavWindowingHighlight");
		ImGuiColorEdit(ImGuiCol_PlotHistogram, "ImGuiCol_PlotHistogram");
		ImGuiColorEdit(ImGuiCol_PlotHistogramHovered, "ImGuiCol_PlotHistogramHovered");
		ImGuiColorEdit(ImGuiCol_PlotLines, "ImGuiCol_PlotLines");
		ImGuiColorEdit(ImGuiCol_PlotLinesHovered, "ImGuiCol_PlotLinesHovered");
		ImGuiColorEdit(ImGuiCol_PopupBg, "ImGuiCol_PopupBg");
		ImGuiColorEdit(ImGuiCol_ResizeGrip, "ImGuiCol_ResizeGrip");
		ImGuiColorEdit(ImGuiCol_ResizeGripActive, "ImGuiCol_ResizeGripActive");
		ImGuiColorEdit(ImGuiCol_ResizeGripHovered, "ImGuiCol_ResizeGripHovered");
		ImGuiColorEdit(ImGuiCol_ScrollbarBg, "ImGuiCol_ScrollbarBg");
		ImGuiColorEdit(ImGuiCol_ScrollbarGrab, "ImGuiCol_ScrollbarGrab");
		ImGuiColorEdit(ImGuiCol_ScrollbarGrabActive, "ImGuiCol_ScrollbarGrabActive");
		ImGuiColorEdit(ImGuiCol_ScrollbarGrabHovered, "ImGuiCol_ScrollbarGrabHovered");
		ImGuiColorEdit(ImGuiCol_Separator, "ImGuiCol_Separator");
		ImGuiColorEdit(ImGuiCol_SeparatorActive, "ImGuiCol_SeparatorActive");
		ImGuiColorEdit(ImGuiCol_SeparatorHovered, "ImGuiCol_SeparatorHovered");
		ImGuiColorEdit(ImGuiCol_SliderGrab, "ImGuiCol_SliderGrab");
		ImGuiColorEdit(ImGuiCol_SliderGrabActive, "ImGuiCol_SliderGrabActive");
		ImGuiColorEdit(ImGuiCol_Text, "ImGuiCol_Text");
		ImGuiColorEdit(ImGuiCol_TextDisabled, "ImGuiCol_TextDisabled");
		ImGuiColorEdit(ImGuiCol_TextSelectedBg, "ImGuiCol_TextSelectedBg");
		ImGuiColorEdit(ImGuiCol_TitleBg, "ImGuiCol_TitleBg");
		ImGuiColorEdit(ImGuiCol_TitleBgActive, "ImGuiCol_TitleBgActive");
		ImGuiColorEdit(ImGuiCol_TitleBgCollapsed, "ImGuiCol_TitleBgCollapsed");
		ImGuiColorEdit(ImGuiCol_WindowBg, "ImGuiCol_WindowBg");
	}
}