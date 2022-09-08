//
// Created by GOKALP on 9/4/2022.
//

#include "ShaderLibrary.h"

namespace acc3d::Graphics
{
    std::unordered_map<ShaderId, ShaderCompilationEntry> ShaderLibrary::s_ShaderIdMap{};


    std::tuple<ShaderId, ShaderCompilationEntry> ShaderLibrary::CompileAndLoad(const ShaderCompilationParameters& params)
    {
        ShaderId id = ShaderLibrary::ShaderPathToId(params.ShaderPath);
        if(IsLoaded(id))
        {
            return { id,s_ShaderIdMap[id] };
        }

        auto entry = ShaderCompiler::CompileShader(params);
        if(entry.CompilationSucceeded)
        {
            s_ShaderIdMap[id] = entry;
            return { id,entry };
        }
        return { SHADER_COMPILATION_ENTRY_INVALID_ID, ShaderCompilationEntry{nullptr,nullptr,ShaderType::Invalid} };
    }

    ShaderCompilationEntry const& ShaderLibrary::GetCompilationEntry(ShaderId id)
    {
        return s_ShaderIdMap[id];
    }

    void ShaderLibrary::RemoveCompilationEntry(ShaderId id)
    {
        s_ShaderIdMap.erase(id);
    }

    ShaderId ShaderLibrary::ShaderPathToId(const std::filesystem::path& path)
    {
        return std::filesystem::hash_value(path);
    }

    bool ShaderLibrary::IsLoaded(ShaderId id)
    {
        return s_ShaderIdMap.find(id) != s_ShaderIdMap.end();
    }
} // Graphics