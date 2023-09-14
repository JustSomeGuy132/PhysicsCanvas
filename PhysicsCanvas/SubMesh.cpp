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

void SubMesh::Render() {
	UINT stride = sizeof(VertexPositionColor);
	UINT offset = 0;
	m_deviceResources->GetD3DDeviceContext()->IASetVertexBuffers(0, 1, m_vertexBuffer.GetAddressOf(), &stride, &offset);

	m_deviceResources->GetD3DDeviceContext()->IASetIndexBuffer(
		m_indexBuffer.Get(),
		DXGI_FORMAT_R16_UINT, // Each index is one 16-bit unsigned integer (short).
		0
	);

	m_deviceResources->GetD3DDeviceContext()->DrawIndexed(m_indexCount, 0, 0);
}