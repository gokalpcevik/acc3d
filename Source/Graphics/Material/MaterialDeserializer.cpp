#include "MaterialDeserializer.h"

namespace acc3d::Graphics
{
	MaterialDeserializer::MaterialDeserializer(std::filesystem::path const& path)
	{
		acc3d_trace("Attempting to deserialize material file:\n		   {0}", path.string());

		YAML::Node root;

		try
		{
			root = YAML::LoadFile(path.string());
		}
		catch (YAML::ParserException const& e)
		{
			acc3d_error("Error deserializing material file: {0}", path.string(), e.what());
			return;
		}

		YAML::Node const materialNode = root["material"];

		if(!materialNode)
		{
			acc3d_error("No material node found at the root level. {0}", path.string());
			return;
		}

		YAML::Node const materialName = materialNode["material_name"];

        if(!materialName)
        {
            acc3d_error("No material_name is found in the material file:\n{0}",path.string());
            return;
        }


	}
}
