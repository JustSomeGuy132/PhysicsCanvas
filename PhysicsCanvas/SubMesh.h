#pragma once
#include "pch.h"
#include "..\Common\DeviceResources.h"
#include "..\Common\DirectXHelper.h"
#include "..\Content\ShaderStructures.h"
#include "Mesh.h"

namespace PhysicsCanvas {
	class SubMesh {
	public:
		SubMesh(const std::shared_ptr<DX::DeviceResources>& deviceResources, 
				std::vector<VertexPositionColor> verts, 
				std::vector<unsigned short> indices);

		SubMesh(const SubMesh& sMesh);
		void Render(DirectX::XMMATRIX transposeMat);

	private:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;
		uint32 m_indexCount;
		
		Microsoft::WRL::ComPtr<ID3DBlob>			m_blob;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;
	};
}