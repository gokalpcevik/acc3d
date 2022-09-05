#include "ShaderCompiler.h"

namespace acc3d::Graphics
{
	ShaderCompilationParameters ShaderCompilationParameters::Param_CompileVS_StdIncNoFlagsMainEntry(LPCWSTR ShaderPath)
	{
		return { ShaderPath,nullptr,D3D_COMPILE_STANDARD_FILE_INCLUDE,"main","vs_5_1",0,ShaderType::Vertex };
	}

	ShaderCompilationParameters ShaderCompilationParameters::Param_CompilePS_StdIncNoFlagsMainEntry(LPCWSTR ShaderPath)
	{
		return { ShaderPath,nullptr,D3D_COMPILE_STANDARD_FILE_INCLUDE,"main","ps_5_1",0,ShaderType::Pixel };
	}

	ShaderCompilationEntry ShaderCompiler::CompileShader(const ShaderCompilationParameters& param)
	{
		assert(std::filesystem::exists(param.ShaderPath) && "Shader file doesn't exist!");
		ShaderCompilationEntry entry;

		HRESULT result = D3DCompileFromFile(
			param.ShaderPath, 
			param.Defines, 
			param.Include, 
			param.EntryPoint, 
			param.Target, 
			param.Flags,
			0, 
			&entry.Blob, 
			&entry.ErrorBlob);

		if(FAILED(result) && entry.ErrorBlob)
		{
			acc3d_error(static_cast<char const*>(entry.ErrorBlob->GetBufferPointer()));
			return { nullptr,nullptr };
		}

		entry.ShaderType = param.ShaderType;
		entry.CompilationSucceeded = true;
		return entry;
	}
}
