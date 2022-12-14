#include "ShaderCompiler.h"

namespace acc3d::Graphics
{
	ShaderCompilationParameters ShaderCompilationParameters::Param_CompileVS_StdIncNoFlagsMainEntry(
		LPCWSTR ShaderPath, ShaderId val)
	{
		return { ShaderPath, val , nullptr,D3D_COMPILE_STANDARD_FILE_INCLUDE,"main","vs_5_1",0,ShaderType::Vertex };
	}

	ShaderCompilationParameters ShaderCompilationParameters::Param_CompilePS_StdIncNoFlagsMainEntry(
		LPCWSTR ShaderPath, ShaderId val)
	{
		return { ShaderPath, val ,nullptr,D3D_COMPILE_STANDARD_FILE_INCLUDE,"main","ps_5_1",0,ShaderType::Pixel };
	}

	ShaderCompilationEntry ShaderCompiler::CompileShader(const ShaderCompilationParameters& param)
	{
		acc3d_trace("Attempting to compile shader:{0}", Util::WCharArrayToStdString(param.ShaderPath,std::wcslen(param.ShaderPath) + 1));
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

		acc3d_info("Shader compilation succeeded.");

		entry.ShaderType = param.ShaderType;
		entry.CompilationSucceeded = true;
		return entry;
	}
}
