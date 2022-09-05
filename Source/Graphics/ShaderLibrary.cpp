//
// Created by GOKALP on 9/4/2022.
//

#include "ShaderLibrary.h"

namespace acc3d::Graphics
{
    std::unordered_map<ShaderCompilationEntryId, ShaderCompilationEntry> ShaderLibrary::s_ShaderIdMap{};


    std::tuple<ShaderCompilationEntryId, ShaderCompilationEntry> ShaderLibrary::CompileAndLoad(const ShaderCompilationParameters& params)
    {
        ShaderCompilationEntryId id = ShaderLibrary::ShaderPathToId(params.ShaderPath);
        if(IsLoaded(id))
        {
            return { id,GetCompilationEntry(id) };
        }

        auto&& entry = ShaderCompiler::CompileShader(params);
        if(entry.CompilationSucceeded)
        {
            s_ShaderIdMap[id] = entry;
            return { id,entry };
        }
        return { SHADER_COMPILATION_ENTRY_INVALID_ID, ShaderCompilationEntry{nullptr,nullptr,ShaderType::Invalid} };
    }

    ShaderCompilationEntry const& ShaderLibrary::GetCompilationEntry(ShaderCompilationEntryId id)
    {
        return s_ShaderIdMap[id];
    }

    void ShaderLibrary::RemoveCompilationEntry(ShaderCompilationEntryId id)
    {
        s_ShaderIdMap.erase(id);
    }

    ShaderCompilationEntryId ShaderLibrary::ShaderPathToId(const std::filesystem::path& path)
    {
        return std::filesystem::hash_value(path);
    }

    bool ShaderLibrary::IsLoaded(ShaderCompilationEntryId id)
    {
        return s_ShaderIdMap.find(id) != s_ShaderIdMap.end();
    }
} // Graphics