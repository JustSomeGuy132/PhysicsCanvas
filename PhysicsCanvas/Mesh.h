#pragma once

#include "pch.h"
#include "..\Common\DeviceResources.h"
#include "..\Common\DirectXHelper.h"
#include "..\Content\ShaderStructures.h"

namespace PhysicsCanvas {
	class Mesh {
	public:
		void Create(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void Render(DirectX::XMMATRIX viewprojMat);

		void ReleaseResources() {
			m_loadingComplete = false;
			m_vertexShader.Reset();
			m_inputLayout.Reset();
			m_pixelShader.Reset();
			m_constantBuffer.Reset();
			m_vertexBuffer.Reset();
			m_indexBuffer.Reset();
		}
		void SetWorldMat(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation);
	private:
		//VertexPositionColor vertices[];
		//static unsigned short indices[];
		uint32 m_indexCount;
		bool m_loadingComplete;
		DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();

	private:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		Microsoft::WRL::ComPtr<ID3DBlob>			m_blob;
		Microsoft::WRL::ComPtr<ID3D11InputLayout>	m_inputLayout;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_indexBuffer;
		Microsoft::WRL::ComPtr<ID3D11VertexShader>	m_vertexShader;
		Microsoft::WRL::ComPtr<ID3D11PixelShader>	m_pixelShader;
		Microsoft::WRL::ComPtr<ID3D11Buffer>		m_constantBuffer;
	};
}