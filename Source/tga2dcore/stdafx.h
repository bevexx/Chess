#pragma once
#pragma message("-------Never give up your dreams!-------------")
#include "targetver.h"
#include "tga2d/error/error_manager.h"

#if !defined(WIN32_LEAN_AND_MEAN)
#define WIN32_LEAN_AND_MEAN
#endif

#include "tga2d/math/matrix44.h"
#include "tga2d/math/vector2.h"
#include "tga2d/math/vector4.h"
#include "tga2d/common_converters.h"
#include "tga2d/sprite/sprite.h"
#include "tga2d/d3d/direct_3d.h"
#include "tga2d/shaders/shader.h"
#include "tga2d/arrays/simple_pointer_array.h"

#include <d3d11_1.h>

#include <algorithm>
#include <array>
#include <exception>
#include <malloc.h>
#include <map>
#include <memory>
#include <set>
#include <string>
#include <vector>

#include "imgui.h"
#include "imgui_impl_dx11.h"
#include "imgui_impl_win32.h"

#define MIN(a,b) (((a)<(b))?(a):(b))
#define MAX(a,b) (((a)>(b))?(a):(b))

// Will fix VS 2015 bugs (Microsoft is working on it atm)
#pragma warning( disable : 4458 )

// VS 2010's stdint.h conflicts with intsafe.h
#pragma warning(push)
#pragma warning(disable : 4005)
#include <stdint.h>
#include <intsafe.h>
#pragma warning(pop)

#define STATIC_SIZEOF( X ) { char STATIC_SIZEOF_TEMP[(X)]; STATIC_SIZEOF_TEMP = 1; }

#define SAFE_RELEASE(aPointer) if(aPointer){ aPointer->Release();}
#define SAFE_DELETE(aPointer) delete aPointer; aPointer = nullptr;
#define SAFE_ARRAYDELETE( x )  \
    if( NULL != x )        \
            {                      \
        delete [] x;       \
        x = NULL;          \
            }


struct EulerAngles
{
	float roll, pitch, yaw;
};