#pragma once
#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"
namespace Tga2D
{
	class CImGuiInterface
	{
	public:
		void Init();
		void PreFrame();
		void Render();
	};


}
