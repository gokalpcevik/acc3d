#pragma once

#include <d3d12.h>
#include <exception>
#include "../Core/Log.h"

#if defined(_DEBUG) || defined(DEBUG)
#define THROW_IFF(hr) ::acc3d::Graphics::ResultHandler::CheckResult((hr),__FILE__,__LINE__)
#else
#define THROW_IFF(hr) (hr)
#endif

namespace acc3d::Graphics
{
    class ResultHandler
    {
    public:
        static void CheckResult(HRESULT hr, char const *file, unsigned int line);
    };
}

