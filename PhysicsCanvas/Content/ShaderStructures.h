#pragma once

namespace PhysicsCanvas
{
	
	// Used to send per-vertex data to the vertex shader.
	struct VertexPositionColor
	{
		DirectX::XMFLOAT3 pos;
		DirectX::XMFLOAT3 color;
	};

	// Constant buffer based upon mesh transform
	struct CBUFF {
		DirectX::XMMATRIX transform;
	};
}