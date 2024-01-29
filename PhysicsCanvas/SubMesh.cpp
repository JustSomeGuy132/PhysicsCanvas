#include "pch.h"
#include "Mesh.h"
#include "d3dcompiler.h"
#include "SubMesh.h"

using namespace PhysicsCanvas;
using namespace DirectX;

SubMesh::SubMesh(const std::shared_ptr<DX::DeviceResources>& deviceResources, 
				std::vector<VertexPositionColor> verts, 
				std::vector<unsigned short> indices) {

	m_deviceResources = deviceResources;

	//create pixel shader
	D3DReadFileToBlob(L"SamplePixelShader.cso", &m_blob);
	m_deviceResources->GetD3DDevice()->CreatePixelShader(
		m_blob->GetBufferPointer(), m_blob->GetBufferSize(), nullptr, &m_pixelShader);
	m_deviceResources->GetD3DDeviceContext()->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	//create vertex shader
	D3DReadFileToBlob(L"MyVertexShader.cso", &m_blob);
	m_deviceResources->GetD3DDevice()->CreateVertexShader(
		m_blob->GetBufferPointer(), m_blob->GetBufferSize(), nullptr, &m_vertexShader);
	m_deviceResources->GetD3DDeviceContext()->VSSetShader(m_vertexShader.Get(), nullptr, 0);

	//create input layout
	static const D3D11_INPUT_ELEMENT_DESC vertexDesc[] =
	{
		{ "COLOR", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 12, D3D11_INPUT_PER_VERTEX_DATA, 0 },
		{ "POSITION", 0, DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA, 0 },
	};
	m_deviceResources->GetD3DDevice()->CreateInputLayout(
		vertexDesc, ARRAYSIZE(vertexDesc), m_blob->GetBufferPointer(), m_blob->GetBufferSize(), &m_inputLayout);

	//create constant buffer
	CBUFF cb = {
		XMMatrixIdentity()
	};
	D3D11_BUFFER_DESC constantBufferDesc = {};
	constantBufferDesc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	constantBufferDesc.Usage = D3D11_USAGE_DYNAMIC;
	constantBufferDesc.CPUAccessFlags = D3D11_CPU_ACCESS_WRITE;
	constantBufferDesc.MiscFlags = 0;
	constantBufferDesc.ByteWidth = sizeof(cb);
	constantBufferDesc.StructureByteStride = 0;
	D3D11_SUBRESOURCE_DATA csd = {};
	csd.pSysMem = &cb;
	m_deviceResources->GetD3DDevice()->CreateBuffer(&constantBufferDesc, &csd, &m_constantBuffer);
	
	//vertex buffer
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(verts), D3D11_BIND_VERTEX_BUFFER);
	D3D11_SUBRESOURCE_DATA vbufData = {};
	vbufData.pSysMem = verts.data();
	vbufData.SysMemPitch = 0;
	vbufData.SysMemSlicePitch = 0;
	m_deviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vbufData, &m_vertexBuffer);

	//index buffer
	m_indexCount = _ARRAYSIZE(indices);
	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(indices), D3D11_BIND_INDEX_BUFFER);
	D3D11_SUBRESOURCE_DATA indexBufferData = {};
	indexBufferData.pSysMem = indices.data();
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	m_deviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_indexBuffer);
	m_deviceResources->GetD3DDeviceContext()->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
}

SubMesh::SubMesh(const SubMesh& sMesh) {
	m_deviceResources = sMesh.m_deviceResources;
	m_indexBuffer = sMesh.m_indexBuffer;
	m_vertexBuffer = sMesh.m_vertexBuffer;
}

void SubMesh::Render(DirectX::XMMATRIX transposeMat) {
	CBUFF cb;
	cb.transform = XMMatrixTranspose(transposeMat);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	m_deviceResources->GetD3DDeviceContext()->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &cb, sizeof(CBUFF));
	m_deviceResources->GetD3DDeviceContext()->Unmap(m_constantBuffer.Get(), 0);
	m_deviceResources->GetD3DDeviceContext()->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;
	m_deviceResources->GetD3DDeviceContext()->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

	m_deviceResources->GetD3DDeviceContext()->IASetIndexBuffer(
		m_indexBuffer.Get(),
		DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);

	m_deviceResources->GetD3DDeviceContext()->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	m_deviceResources->GetD3DDeviceContext()->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	m_deviceResources->GetD3DDeviceContext()->VSSetShader(m_vertexShader.Get(), nullptr, 0);

	// Send the constant buffer to the graphics device.
	m_deviceResources->GetD3DDeviceContext()->VSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);

	// Attach our pixel shader.
	m_deviceResources->GetD3DDeviceContext()->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	m_deviceResources->GetD3DDeviceContext()->DrawIndexed(m_indexCount, 0, 0);
}