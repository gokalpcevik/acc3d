#include "ShaderCompiler.h"

namespace acc3d::Graphics
{
	ShaderCompilationEntry ShaderCompiler::CompileShader(const ShaderCompilationParameters& param)
	{
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

		D3D_CALL(result);

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
