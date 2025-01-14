#pragma once
#include <memory>
#include <imgui.h>

#include "Colour.h"

class Hud
{
protected:
	void Text(const char* text) {
		ImGui::SameLine();
		ImGui::Text("%s", text);
	}

public:
	void Space() {
		ImGui::Spacing();
	}

	void Label(const char* labelText) {
		ImGui::Text("%s", labelText);
	}

	void Label(const char* labelText, Colour color) {
		ImGui::TextColored({color.r(),color.g(),color.b(),color.a()}, "%s", labelText);
	}

	void LabelText(const char* labelText, const char* text) {
		ImGui::Text("%s:", labelText);
		ImGui::SameLine();
		ImGui::Text("%s", text);
	}

	static bool InputInt(const char* labelText, int* value, int step = 1, int stepFast = 100) {
		ImGui::Text("%s:", labelText);
		ImGui::SameLine();
		return ImGui::InputInt("", value, step, stepFast);
	}

	static bool InputFloat(const char* labelText, float* value, float step = 0.1f, float stepFast = 1.0f, const char* format = "%.3f") {
		ImGui::Text("%s:", labelText);
		ImGui::SameLine();
		return ImGui::InputFloat("", value, step, stepFast, format);
	}

public:
	void Start();
	void End();

	void Render() {
		// This will close the ImGui window context
		ImGui::End();
		ImGui::Render();
	}
};