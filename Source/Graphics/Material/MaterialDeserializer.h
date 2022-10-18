#pragma once
#include <filesystem>
#include "acc3d_yaml.h"
#include "../../Core/Log.h"

namespace acc3d::Graphics
{
	class MaterialDeserializer
	{
	public:
		explicit MaterialDeserializer(std::filesystem::path const& path);


	};

}