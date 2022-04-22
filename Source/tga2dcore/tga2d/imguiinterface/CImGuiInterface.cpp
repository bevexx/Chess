#include "stdafx.h"
#include "CImGuiInterface.h"
#include <string>
#include <tga2d/engine.h>
#include <tga2d/d3d/direct_3d.h>

//#pragma comment(lib, "..\\Libs\\imgui.lib")
//#pragma comment(lib, "..\\Libs\\imgui_canvas.lib")

using namespace Tga2D;
#ifndef _RETAIL
static ImFont* ImGui_LoadFont(ImFontAtlas& atlas, const char* name, float size, const ImVec2& displayOffset = ImVec2(0, 0))
{
	char* windir = nullptr;
	if (_dupenv_s(&windir, nullptr, "WINDIR") || windir == nullptr)
		return nullptr;

	static const ImWchar ranges[] =
	{
		0x0020, 0x00FF, // Basic Latin + Latin Supplement
		0x0104, 0x017C, // Polish characters and more
		0,
	};

	ImFontConfig config;
	config.OversampleH = 4;
	config.OversampleV = 4;
	config.PixelSnapH = false;

	auto path = std::string(windir) + "\\Fonts\\" + name;
	auto font = atlas.AddFontFromFileTTF(path.c_str(), size, &config, ranges);
	if (font)
		font->DisplayOffset = displayOffset;

	free(windir);

	return font;
}
ImFontAtlas fontAtlas;
#endif

void CImGuiInterface::Init()
{
#ifndef _RETAIL
	ImGui_LoadFont(fontAtlas, "segoeui.ttf", 18.0f);//16.0f * 96.0f / 72.0f);
	fontAtlas.Build();

	ImGui::CreateContext(&fontAtlas);
	ImGuiIO& io = ImGui::GetIO();
	io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
	//io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;  // Enable Keyboard Controls
	io.IniFilename = nullptr;
	io.LogFilename = nullptr;


	// Setup ImGui binding
	ImGui_ImplWin32_Init(*Tga2D::CEngine::GetInstance()->GetHWND());
	ImGui_ImplDX11_Init(Tga2D::CEngine::GetInstance()->GetDirect3D().GetDevice(), Tga2D::CEngine::GetInstance()->GetDirect3D().GetContext());
	ImGui::StyleColorsDark();
#endif
}

void CImGuiInterface::PreFrame()
{
#ifndef _RETAIL
	ImGui_ImplWin32_NewFrame();
	ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame();
#endif
}

void CImGuiInterface::Render()
{
#ifndef _RETAIL
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
#endif
}
