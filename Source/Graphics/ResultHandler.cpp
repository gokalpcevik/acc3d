
#include "ResultHandler.h"

namespace acc3d::Graphics
{
    void ResultHandler::CheckResult(HRESULT hr, char const *file, unsigned int line)
    {
        if (SUCCEEDED(hr)) return;
        if (FAILED(hr))
        {
            LPTSTR errorText = NULL;
            FormatMessage(FORMAT_MESSAGE_FROM_SYSTEM |
                          FORMAT_MESSAGE_ALLOCATE_BUFFER |
                          FORMAT_MESSAGE_IGNORE_INSERTS,
                          nullptr,
                          hr,
                          MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT),
                          (LPTSTR) &errorText,
                          0,
                          NULL);
            if (errorText != NULL)
            {
                acc3d_error("{0} \n File: {1}:{2}", errorText, file, line);
                free(errorText);
                throw std::exception();
            }
        }
    }
} // Graphics