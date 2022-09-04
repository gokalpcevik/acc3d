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

	static constexpr ShaderCompilationEntryId SHADER_COMPILATION_ENTRY_INVALID_ID = 0;

	class ShaderLibrary
	{
	public:
		ShaderLibrary() = default;
		ShaderLibrary(const ShaderLibrary&) = delete;

		std::tuple<ShaderCompilationEntryId, ShaderCompilationEntry> CompileAndLoad(const ShaderCompilationParameters& params);

		const ShaderCompilationEntry& GetCompilationEntry(ShaderCompilationEntryId id);

		void RemoveCompilationEntry(ShaderCompilationEntryId id);

		static ShaderCompilationEntryId ShaderPathToId(const std::filesystem::path& path);

		bool IsLoaded(ShaderCompilationEntryId id);

	private:
		std::unordered_map<ShaderCompilationEntryId, ShaderCompilationEntry> s_ShaderIdMap{};
	};
} // Graphics
