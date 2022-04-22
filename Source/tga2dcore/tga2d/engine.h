/*
Engine
This class is the interface between game logic and rendering. 
It takes care of creating everything necessary for window handling and rendering
*/

#pragma once
//#define _X86_
#include <WinSock2.h>
#include <windows.h>

#include <functional>
#include "math/Color.h"
#include "math/vector2.h"
#include "render/render_common.h"
#include <chrono>
#include "StepTimer.h"
#include "engine_defines.h"

namespace Tga2D
{
    class CWindowsWindow;
    class CDirectEngine;
    class CRenderer;
    class CTextureManager;
    class CDebugDrawer;
    class CTextService;
    class CLightManager;
    class CErrorManager;
    class CFileWatcher;
}


#define TGA_DEFAULT_CRYSTAL_BLUE { 3.0f / 255.0f, 153.0f / 255.0f, 176.0f / 255.0f, 1.0f }
namespace Tga2D
{
    using callback_function        = std::function<void()>;
    using callback_function_update = std::function<void()>;
	using callback_function_wndProc = std::function<LRESULT(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)>;
    using callback_function_log    = std::function<void( std::string )>;
    using callback_function_error  = std::function<void( std::string )>;

    enum eDebugFeatures
    {
        eDebugFeature_Fps              = 1 << 0,
        eDebugFeature_Mem              = 1 << 1,
        eDebugFeature_Drawcalls        = 1 << 2,
        eDebugFeature_Cpu              = 1 << 3,
        eDebugFeature_FpsGraph         = 1 << 4,
        eDebugFeature_Filewatcher      = 1 << 5,
        eDebugFeature_OptimiceWarnings = 1 << 6,
        eDebugFeature_All              = 1 << 7,
    };

    enum EWindowSetting
    {
        EWindowSetting_Overlapped,
        EWindowSetting_Borderless,
    };

	enum EMultiSamplingQuality
	{
		EMultiSamplingQuality_Off,	
		EMultiSamplingQuality_Low = 1,
		EMultiSamplingQuality_Medium = 2,
		EMultiSamplingQuality_High = 3,
	};

    struct SEngineCreateParameters
    {
        SEngineCreateParameters()
        { 
            myHwnd							= nullptr; 
            myHInstance						= nullptr;  
            myWindowWidth					= 1280;
            myWindowHeight					= 20; 
            myEnableVSync					= false; 
            myRenderWidth					= myWindowWidth; 
            myRenderHeight					= myWindowHeight; 
            myErrorFunction					= nullptr; 
			myLogFunction					= nullptr;
			myWinProcCallback				= nullptr;
            myStartInFullScreen				= false;
            myWindowSetting					= EWindowSetting_Overlapped;
            myTargetHeight					= myWindowHeight;
            myTargetWidth					= myWindowWidth;
            myUseLetterboxAndPillarbox		= true;
            myActivateDebugSystems			= eDebugFeature_Fps | eDebugFeature_Mem;
			myPreferedMultiSamplingQuality	= EMultiSamplingQuality_Off;
			myClearColor					= TGA_DEFAULT_CRYSTAL_BLUE;
        }

        callback_function myInitFunctionToCall;
        callback_function_update myUpdateFunctionToCall;
		callback_function_wndProc myWinProcCallback;
        callback_function_log myLogFunction;
        callback_function_error myErrorFunction;

		/* How big should the window be? */
        unsigned short myWindowWidth;
        unsigned short myWindowHeight;

		/* What resolution should we render everything in?*/
        unsigned short myRenderWidth;
        unsigned short myRenderHeight;

		/* In what resolution are the graphic artist working?*/
        unsigned short myTargetHeight;
        unsigned short myTargetWidth;

        /* Will show the FPS and memory text*/
        int myActivateDebugSystems;
        CColor myClearColor;
        HWND *myHwnd;
        HINSTANCE myHInstance;
        std::wstring myApplicationName;
        bool myEnableVSync;
        bool myStartInFullScreen;
        EWindowSetting myWindowSetting;
        bool myUseLetterboxAndPillarbox;

		EMultiSamplingQuality myPreferedMultiSamplingQuality;
    };


    class CEngine
    {
        friend class CTextureManager;
        friend class CRenderer;
        friend class CShader;
        friend class CErrorManager;
        friend class CDirectEngine;
		friend class CWindowsWindow;
		friend class CErrorManager;
		friend class CCustomShapeDrawer;
		friend class CLineDrawer;
		friend class CDebugDrawer;
		friend class CCustomShape;
		friend class CLinePrimitive;
		friend class CCustomShader;
		friend class CTexturedQuad;
		friend class CTexture;
		friend class CText;
		friend class CTextService;
		friend class CVideo;
		friend class CLineMultiPrimitive;
		friend class CTexturedQuadBatch;
		friend class CLight;
		friend class CModelShape;
    public:
        CEngine &operator =( const CEngine &anOther ) = delete;
		static bool Start(const SEngineCreateParameters& aCreateParameters);
		static void Shutdown();
        static CEngine* GetInstance() {return myInstance;}
               
        CTextureManager& GetTextureManager() const {return *myTextureManager;}
        CDebugDrawer& GetDebugDrawer() const {return *myDebugDrawer;}

		CErrorManager& GetErrorManager() const { return *myErrorManager; }

		VECTOR2UI GetWindowSize() const {return myWindowSize;}
		VECTOR2UI GetRenderSize() const { return myRenderSize; }
		VECTOR2UI GetTargetSize() const { return myTargetSize; }
        float GetWindowRatio() const;
        float GetWindowRatioInversed() const;
        bool GetIsTopWindow();

		float GetDeltaTime() const { return myDeltaTime; }
        HWND* GetHWND() const;
        HINSTANCE GetHInstance() const;
      
		bool IsDebugFeatureOn(eDebugFeatures aFeature) const;

		void SetTargetSize(const VECTOR2UI& aResolution);
		void SetResolution(const VECTOR2UI &aResolution, bool aAlsoSetWindowSize = true);
        void SetFullScreen(bool aFullScreen);
		
		/* Set to null to render to screen */
		void SetRenderTarget(class CSprite* aSpriteTarget);

		void SetClearColor(const CColor& aClearColor); 
		// If you want to manually tell the engine to render instead of the callback function with the (myUpdateFunctionToCall)
		bool BeginFrame(const CColor &aClearColor = TGA_DEFAULT_CRYSTAL_BLUE);
		void EndFrame(void);

		void SetAmbientLightValue(float aAmbientLight);
		CDirectEngine& GetDirect3D() const { return *myDirect3D; }
    private:
		static void DestroyInstance();

		CFileWatcher* GetFileWatcher() { return myFileWatcher; }
		
		CRenderer& GetRenderer() const { return *myRenderer; }
		CTextService& GetTextService() const { return *myTextService; }
		CWindowsWindow& GetWindow() const { return *myWindow; }
		CLightManager& GetLightManager() const { return *myLightManager; }
        CEngine(const SEngineCreateParameters& aCreateParameters);
        ~CEngine();
		
		bool InternalStart();

        void StartStep();
        void DoStep();
		void IdleProcess();
        void CalculateRatios();
		void SetViewPort(float aTopLeftX, float aTopLeftY, float aWidth, float aHeight, float aMinDepth = 0.0f, float aMaxDepth = 1.0f, bool aSetEngineResolution = false);
		void SetWantToUpdateSize() { myWantToUpdateSize = true; }

		void UpdateWindowSizeChanges(bool aIgnoreAutoUpdate = false);

		VECTOR2F GetWindowRatioVec() const;
		VECTOR2F GetWindowRatioInversedVec() const;

        static CEngine* myInstance;

        callback_function_update myUpdateFunctionToCall;
        callback_function myInitFunctionToCall;
        CWindowsWindow* myWindow;
        CDirectEngine* myDirect3D;
        CRenderer* myRenderer;
        CTextureManager* myTextureManager;
        CDebugDrawer* myDebugDrawer;
        CTextService* myTextService;
		VECTOR2UI myWindowSize;
		VECTOR2UI myRenderSize;
		VECTOR2UI myTargetSize;
		VECTOR2UI myNativeRenderSize;
        HWND *myHwnd;
        HINSTANCE myHInstance;
        CColor myClearColor;
        SEngineCreateParameters myCreateParameters;
        CLightManager* myLightManager;
        CErrorManager* myErrorManager;
        CFileWatcher* myFileWatcher;
		bool myWantToUpdateSize;

        bool myRunEngine;
        float myWindowRatio;
        float myWindowRatioInversed;
		VECTOR2F myWindowRatioVec;
		VECTOR2F myWindowRatioInversedVec;

		std::chrono::steady_clock::time_point myStartOfTime;
        float myTotalTime;
        float myDeltaTime;

		DX::StepTimer myTimer;

		bool myShouldExit; // Only used when using beginframe and endframe
		class CImGuiInterface* myImguiInterFace = nullptr;
    };
}
