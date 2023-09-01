#include "pch.h"
#include "Mesh.h"
#include "d3dcompiler.h"

using namespace PhysicsCanvas;
using namespace DirectX;

void Mesh::Create(const std::shared_ptr<DX::DeviceResources>& deviceResources) {
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
	static const VertexPositionColor cubeVertices[] =
	{
		{XMFLOAT3(-0.5f, -0.5f, -0.5f), XMFLOAT3(0.0f, 0.0f, 0.0f)},
		{XMFLOAT3(-0.5f, -0.5f,  0.5f), XMFLOAT3(0.0f, 0.0f, 1.0f)},
		{XMFLOAT3(-0.5f,  0.5f, -0.5f), XMFLOAT3(0.0f, 1.0f, 0.0f)},
		{XMFLOAT3(-0.5f,  0.5f,  0.5f), XMFLOAT3(0.0f, 1.0f, 1.0f)},
		{XMFLOAT3(0.5f, -0.5f, -0.5f), XMFLOAT3(1.0f, 0.0f, 0.0f)},
		{XMFLOAT3(0.5f, -0.5f,  0.5f), XMFLOAT3(1.0f, 0.0f, 1.0f)},
		{XMFLOAT3(0.5f,  0.5f, -0.5f), XMFLOAT3(1.0f, 1.0f, 0.0f)},
		{XMFLOAT3(0.5f,  0.5f,  0.5f), XMFLOAT3(0.7f, 1.0f, 2.0f)},
	};
	CD3D11_BUFFER_DESC vertexBufferDesc(sizeof(cubeVertices), D3D11_BIND_VERTEX_BUFFER);
	D3D11_SUBRESOURCE_DATA vbufData = {};
	vbufData.pSysMem = cubeVertices;
	vbufData.SysMemPitch = 0;
	vbufData.SysMemSlicePitch = 0;
	m_deviceResources->GetD3DDevice()->CreateBuffer(&vertexBufferDesc, &vbufData, &m_vertexBuffer);

	//index buffer
	static const unsigned short cubeIndices[] =
	{
		0,2,1, // -x
		1,2,3,

		4,5,6, // +x
		5,7,6,

		0,1,5, // -y
		0,5,4,

		2,6,7, // +y
		2,7,3,

		0,4,6, // -z
		0,6,2,

		1,3,7, // +z
		1,7,5,
	};
	m_indexCount = ARRAYSIZE(cubeIndices);
	CD3D11_BUFFER_DESC indexBufferDesc(sizeof(cubeIndices), D3D11_BIND_INDEX_BUFFER);
	D3D11_SUBRESOURCE_DATA indexBufferData = {};
	indexBufferData.pSysMem = cubeIndices;
	indexBufferData.SysMemPitch = 0;
	indexBufferData.SysMemSlicePitch = 0;
	m_deviceResources->GetD3DDevice()->CreateBuffer(&indexBufferDesc, &indexBufferData, &m_indexBuffer);
	m_deviceResources->GetD3DDeviceContext()->IASetIndexBuffer(m_indexBuffer.Get(), DXGI_FORMAT_R16_UINT, 0);
	m_loadingComplete = true;

}

void Mesh::Render(DirectX::XMMATRIX viewprojMat) {
	// Loading is asynchronous. Only draw geometry after it's loaded.
	if (!m_loadingComplete)
	{
		return;
	}

	auto context = m_deviceResources->GetD3DDeviceContext();

	// Prepare the constant buffer to send it to the graphics device.
	CBUFF cb;
	cb.transform = XMMatrixTranspose(worldMat * viewprojMat);
	D3D11_MAPPED_SUBRESOURCE mappedResource;
	context->Map(m_constantBuffer.Get(), 0, D3D11_MAP_WRITE_DISCARD, 0, &mappedResource);
	CopyMemory(mappedResource.pData, &cb, sizeof(CBUFF));
	context->Unmap(m_constantBuffer.Get(), 0);
	context->VSSetConstantBuffers(0, 1, m_constantBuffer.GetAddressOf());

	// Each vertex is one instance of the VertexPositionColor struct.
	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;
	context->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

	context->IASetIndexBuffer(
		m_indexBuffer.Get(),
		DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);

	context->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	context->IASetInputLayout(m_inputLayout.Get());

	// Attach our vertex shader.
	context->VSSetShader(m_vertexShader.Get(), nullptr, 0);

	// Send the constant buffer to the graphics device.
	context->VSSetConstantBuffers1(0, 1, m_constantBuffer.GetAddressOf(), nullptr, nullptr);

	// Attach our pixel shader.
	context->PSSetShader(m_pixelShader.Get(), nullptr, 0);

	// Draw the objects.
	context->DrawIndexed(m_indexCount, 0, 0);
}


void Mesh::SetWorldMat(DirectX::XMFLOAT3 position, DirectX::XMFLOAT3 rotation) {
	worldMat = DirectX::XMMatrixRotationX(rotation.x) *
		DirectX::XMMatrixRotationY(rotation.y) *
		DirectX::XMMatrixRotationZ(rotation.z) *
		DirectX::XMMatrixTranslation(position.x, position.y, position.z);
}