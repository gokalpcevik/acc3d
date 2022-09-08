#pragma once
#include <DirectXMath.h>

namespace acc3d::Graphics
{
	class Renderer;
}

namespace acc3d::ECS
{
	struct CameraComponent
	{
		CameraComponent() = default;
		CameraComponent(bool primary) : IsPrimaryCamera(primary) {}

		[[nodiscard]] DirectX::XMMATRIX GetProjectionMatrix(float aspectRatio) const
		{
			return DirectX::XMMatrixPerspectiveFovLH(FOVHalfAngle, aspectRatio,NearZ,FarZ);
		}

		bool IsPrimaryCamera = false;
		float FOVHalfAngle = 45.0f;
		float NearZ = 0.1f;
		float FarZ = 1000.0f;
		DirectX::XMMATRIX ViewMatrix = DirectX::XMMatrixIdentity();
	};


}