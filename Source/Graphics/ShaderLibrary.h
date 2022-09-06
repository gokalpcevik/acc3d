#pragma once
#include <filesystem>
#include <unordered_map>
#include <d3dcompiler.h>
#include <cstdint>
#include <tuple>
#include <optional>
#include "ResultHandler.h"
#include "ShaderCompiler.h"
#include "../Core/Log.h"

namespace acc3d::Graphics
{
	using ShaderCompilationEntryId = size_t;

	/*
	 * There is chance that this value will clash with the hash value of a shader path but practically it is extremely low.
	 */
	static constexpr ShaderCompilationEntryId SHADER_COMPILATION_ENTRY_INVALID_ID = 0;

	class ShaderLibrary
	{
	public:
		ShaderLibrary() = default;
		ShaderLibrary(const ShaderLibrary&) = delete;

		static std::tuple<ShaderCompilationEntryId, ShaderCompilationEntry> CompileAndLoad(const ShaderCompilationParameters& params);

		static const ShaderCompilationEntry& GetCompilationEntry(ShaderCompilationEntryId id);

		static void RemoveCompilationEntry(ShaderCompilationEntryId id);

		static ShaderCompilationEntryId ShaderPathToId(const std::filesystem::path& path);

		static bool IsLoaded(ShaderCompilationEntryId id);

	private:
		static std::unordered_map<ShaderCompilationEntryId, ShaderCompilationEntry> s_ShaderIdMap;
	};
} // Graphics
