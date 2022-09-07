#pragma once
#include <filesystem>
#include <unordered_map>
#include <d3dcompiler.h>
#include <cstdint>
#include <tuple>
#include <optional>
#include "Type.h"
#include "ResultHandler.h"
#include "ShaderCompiler.h"
#include "../Core/Log.h"

namespace acc3d::Graphics
{

	/*
	 * There is chance that this value will clash with the hash value of a shader path but practically it is extremely low.
	 */
	static constexpr ShaderId SHADER_COMPILATION_ENTRY_INVALID_ID = 0;

	class ShaderLibrary
	{
	public:
		ShaderLibrary() = default;
		ShaderLibrary(const ShaderLibrary&) = delete;

		static std::tuple<ShaderId, ShaderCompilationEntry> CompileAndLoad(const ShaderCompilationParameters& params);

		static const ShaderCompilationEntry& GetCompilationEntry(ShaderId id);

		static void RemoveCompilationEntry(ShaderId id);

		static ShaderId ShaderPathToId(const std::filesystem::path& path);

		static bool IsLoaded(ShaderId id);

	private:
		static std::unordered_map<ShaderId, ShaderCompilationEntry> s_ShaderIdMap;
	};
} // Graphics
