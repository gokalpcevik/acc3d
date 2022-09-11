//
// Created by GOKALP on 9/4/2022.
//

#include "ShaderLibrary.h"

namespace acc3d::Graphics
{
    google::dense_hash_map<ShaderId, ShaderCompilationEntry> ShaderLibrary::s_ShaderIdMap{};

    void ShaderLibrary::Init()
    {
        ShaderLibrary::s_ShaderIdMap.set_empty_key(SHADER_ID_EMPTY_KEY_VALUE);
        ShaderLibrary::s_ShaderIdMap.set_deleted_key(SHADER_ID_DELETED_KEY_VALUE);
    }

    std::tuple<ShaderId, ShaderCompilationEntry> ShaderLibrary::CompileAndLoad(const ShaderCompilationParameters& params)
    {
        ShaderId id = params.AssignedShaderId;
        assert(
            id != SHADER_ID_DELETED_KEY_VALUE && id != SHADER_ID_EMPTY_KEY_VALUE &&
            "Last ShaderId clashes with the deleted or empty shader id values.");

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
        return { SHADER_ID_EMPTY_KEY_VALUE, ShaderCompilationEntry{nullptr,nullptr,ShaderType::Invalid} };
    }

    ShaderCompilationEntry const& ShaderLibrary::GetCompilationEntry(ShaderId id)
    {
        return s_ShaderIdMap[id];
    }

    void ShaderLibrary::RemoveCompilationEntry(ShaderId id)
    {
        s_ShaderIdMap.erase(id);
    }

    bool ShaderLibrary::IsLoaded(ShaderId id)
    {
        return s_ShaderIdMap.find(id) != s_ShaderIdMap.end();
    }
} // Graphics