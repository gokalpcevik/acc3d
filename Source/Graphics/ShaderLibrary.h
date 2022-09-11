#pragma once
#include <sparsehash/dense_hash_map>
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
	class ShaderLibrary
	{
	public:
		ShaderLibrary() = default;
		ShaderLibrary(const ShaderLibrary&) = delete;

		static void Init();

		static std::tuple<ShaderId, ShaderCompilationEntry> CompileAndLoad(const ShaderCompilationParameters& params);

		static const ShaderCompilationEntry& GetCompilationEntry(ShaderId id);

		static void RemoveCompilationEntry(ShaderId id);

		static bool IsLoaded(ShaderId id);

	private:
		static google::dense_hash_map<ShaderId, ShaderCompilationEntry> s_ShaderIdMap;
	};
} // Graphics
