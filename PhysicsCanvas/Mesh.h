#pragma once

#include "pch.h"
#include "..\Common\DeviceResources.h"
#include "..\Common\DirectXHelper.h"
#include "..\Content\ShaderStructures.h"
#include <assimp\Importer.hpp>
#include <assimp\postprocess.h>
#include <assimp\scene.h>
#include <fstream>

#define FLOOR 1
#define CUBE 2
#define SPHERE 3

namespace PhysicsCanvas {
	class Mesh {
	public:
		void Create(const UINT _shape, const std::shared_ptr<DX::DeviceResources>& deviceResources, DirectX::XMFLOAT3 colour = DirectX::XMFLOAT3(0.1f, 0.6f, 0.1f));
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
		void SetWorldMat(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation, DirectX::XMFLOAT3 scale);
		DirectX::XMMATRIX GetWorldMat() { return worldMat; }

		void Scale(DirectX::XMFLOAT3 scale) {
			worldMat = DirectX::XMMatrixScaling(scale.x, scale.y, scale.z) * worldMat;
		}

		DirectX::XMFLOAT3 GetColour() { return _colour; }

		void SetVertexBuffer(DirectX::XMFLOAT3 colour);

		void SetColour(DirectX::XMFLOAT3 col);

		
	private:
		uint32 m_indexCount;
		bool m_loadingComplete;
		DirectX::XMMATRIX worldMat = DirectX::XMMatrixIdentity();

		UINT shape;
		DirectX::XMFLOAT3 _colour;

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