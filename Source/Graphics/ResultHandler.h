#pragma once

#include <d3d12.h>
#include <exception>
#include "../Core/Log.h"

#define THROW_IFF(hr) ::acc3d::Graphics::ResultHandler::CheckResult((hr),__FILE__,__LINE__)

namespace acc3d::Graphics
{
    class ResultHandler
    {
    public:
        static void CheckResult(HRESULT hr, char const *file, unsigned int line);
    };
}

