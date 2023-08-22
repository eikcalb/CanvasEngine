#include "Hud.h"

#if BUILD_DIRECTX
	#include "imgui_impl_dx11.h"
	#include "imGui_impl_win32.h"
#else if BUILD_OPENGL
	#include "imgui_impl_glfw.h"
	#include "imgui_impl_opengl3.h"
#endif

void Hud::Start() {
#if BUILD_DIRECTX
	ImGui_ImplDX11_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
#else if BUILD_OPENGL
	ImGui_ImplOpenGL3_NewFrame();
	ImGui_ImplGlfw_NewFrame();
	ImGui::NewFrame();
#endif
}

void Hud::End() {
#if BUILD_DIRECTX
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#else if BUILD_OPENGL
	ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
#endif
}