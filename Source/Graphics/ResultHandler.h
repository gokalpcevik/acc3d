#pragma once

#include <d3d12.h>
#include "../Core/Log.h"

#define D3D_CALL(hr) ::acc3d::Graphics::ResultHandler::CheckResult((hr),__FILE__,__LINE__)

namespace acc3d::Graphics
{
    class ResultHandler
    {
    public:
        static void CheckResult(HRESULT hr, char const *file, unsigned int line);
    };
}

