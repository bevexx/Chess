#include "stdafx.h"
#include <tga2d/Engine.h>
#include "GameWorld.h"
#include "Game.h"
#include "settings.h"
#include "Client.h"
#include "Server.h"

#include <tga2d/error/error_manager.h>

using namespace std::placeholders;


#ifdef _DEBUG
#pragma comment(lib,"TGA2D_Debug.lib")
std::wstring BUILD_NAME = L"Debug";
#endif // DEBUG
#ifdef _RELEASE
#pragma comment(lib,"TGA2D_Release.lib")
std::wstring BUILD_NAME = L"Release";
#endif // DEBUG
#ifdef _RETAIL
#pragma comment(lib,"TGA2D_Retail.lib")
std::wstring BUILD_NAME = L"Retail";
#endif // DEBUG

CGame::CGame()
{
}


CGame::~CGame()
{
	delete myGameWorld;
	myGameWorld = nullptr;
}

LRESULT CGame::WinProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	lParam;
	wParam;
	hWnd;
	switch (message)
	{
		// this message is read when the window is closed
	case WM_LBUTTONDOWN:
	{
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(*Tga2D::CEngine::GetInstance()->GetHWND(), &p);

		float x = static_cast<float>(p.x) / static_cast<float>(Tga2D::CEngine::GetInstance()->GetWindowSize().x);
		float y = static_cast<float>(p.y) / static_cast<float>(Tga2D::CEngine::GetInstance()->GetWindowSize().y);
		myGameWorld->OnClick({ x, y });
		break;
	}
	case WM_LBUTTONUP:
	{
		POINT p;
		GetCursorPos(&p);
		ScreenToClient(*Tga2D::CEngine::GetInstance()->GetHWND(), &p);

		float x = static_cast<float>(p.x) / static_cast<float>(Tga2D::CEngine::GetInstance()->GetWindowSize().x);
		float y = static_cast<float>(p.y) / static_cast<float>(Tga2D::CEngine::GetInstance()->GetWindowSize().y);
		myGameWorld->OnRelease({ x, y });
		break;
	}
	
	case WM_DESTROY:
	{
		// close the application entirely
		PostQuitMessage(0);
		return 0;
	}
	}

	return 0;
}



bool CGame::Init(const std::wstring& aVersion, const bool aHasAuthority, HWND /*aHWND*/)
{
    Tga2D::SEngineCreateParameters createParameters;
    
    createParameters.myInitFunctionToCall = std::bind( &CGame::InitCallBack, this );
	createParameters.myWinProcCallback = std::bind(&CGame::WinProc, this, _1, _2, _3, _4);
    createParameters.myUpdateFunctionToCall = std::bind( &CGame::UpdateCallBack, this );
    createParameters.myApplicationName = L"TGA 2D " + BUILD_NAME + L"[" + aVersion + L"] ";
	createParameters.myWindowWidth = SCREEN_WIDTH;
	createParameters.myWindowHeight = SCREEN_HEIGHT;
	createParameters.myRenderWidth = SCREEN_WIDTH;
	createParameters.myRenderHeight = SCREEN_HEIGHT;
	createParameters.myTargetWidth = SCREEN_WIDTH;
	createParameters.myTargetHeight = SCREEN_HEIGHT;
	createParameters.myActivateDebugSystems = Tga2D::eDebugFeature_Fps |
		Tga2D::eDebugFeature_Mem |
		Tga2D::eDebugFeature_Drawcalls |
		Tga2D::eDebugFeature_Cpu |
		Tga2D::eDebugFeature_Filewatcher |
		Tga2D::eDebugFeature_OptimiceWarnings;

	myGameWorld = new CGameWorld();
	if (myClient && !myGameWorld->myPlayingLocally)
	{
		myGameWorld->myClient = myClient;
		myClient->myGameWorld = myGameWorld;
		myClient->Init();
	}

	if (!Tga2D::CEngine::Start(createParameters))
    {
        ERROR_PRINT( "Fatal error! Engine could not start!" );
		system("pause");
		return false;
    }

	// End of program

	return true;
}

void CGame::InitCallBack()
{
    myGameWorld->Init(myClient);
}

void CGame::UpdateCallBack()
{
	myGameWorld->Update(Tga2D::CEngine::GetInstance()->GetDeltaTime());
}
