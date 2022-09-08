#pragma once
#include <d3dcompiler.h>
#include <wrl.h>
#include <filesystem>
#include "ResultHandler.h"
#include "../Core/Log.h"
#include "../Util/StringUtil.h"

namespace acc3d::Graphics
{
	enum class ShaderType
	{
		Vertex = 0,
		Pixel = 1,
		Invalid
	};

	struct ShaderCompilationEntry
	{
		ShaderCompilationEntry() = default;

		Microsoft::WRL::ComPtr<ID3DBlob> Blob{nullptr};
		Microsoft::WRL::ComPtr<ID3DBlob> ErrorBlob{nullptr};
		acc3d::Graphics::ShaderType ShaderType{ShaderType::Invalid};
		bool CompilationSucceeded = false;
	};

	struct ShaderCompilationParameters
	{
		LPCWSTR ShaderPath{};
		D3D_SHADER_MACRO const* Defines{nullptr};
		ID3DInclude* Include{ D3D_COMPILE_STANDARD_FILE_INCLUDE };
		LPCSTR EntryPoint = "main";
		LPCSTR Target{};
		UINT Flags{};
		acc3d::Graphics::ShaderType ShaderType{ ShaderType::Invalid };

		static ShaderCompilationParameters Param_CompileVS_StdIncNoFlagsMainEntry(LPCWSTR ShaderPath);
		static ShaderCompilationParameters Param_CompilePS_StdIncNoFlagsMainEntry(LPCWSTR ShaderPath);
	};

	class ShaderCompiler
	{
	public:
		ShaderCompiler() = default;

		static ShaderCompilationEntry CompileShader(const ShaderCompilationParameters& param);
	};

}