#pragma once

#include "pch.h"
#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"
#include "..\Common\DirectXHelper.h"
#include "MoveLookControls.h"
#include "PhysicsBody.h"
#include <list>
#include "..\ImGUI\imgui.h"
#include "..\ImGUI\imgui_impl_win32.h"
#include "..\ImGUI\imgui_impl_dx11.h"

namespace PhysicsCanvas
{
	// This sample renderer instantiates a basic rendering pipeline.
	class Sample3DSceneRenderer
	{
	public:
		Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		void CreateDeviceDependentResources();
		void CreateWindowSizeDependentResources();
		void ReleaseDeviceDependentResources();
		void Update(DX::StepTimer const& timer);
		void Render();
		void CreateNewMesh();
		
		float u_Time;
		void Step();
	private:
		void STransform(float radians);
	private:
		Windows::UI::Core::CoreWindow^ wnd;
		MoveLookControls^ controller;
		DirectX::XMMATRIX projectionMat;

		DirectX::XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

		bool isStepping;
	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// List of all meshes in the scene
		std::list<std::shared_ptr<PhysicsBody>> pBodies;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
	private:
		Microsoft::WRL::ComPtr<ID3D11Device> m_device;
		Microsoft::WRL::ComPtr<ID3D11DeviceContext> m_deviceContext;
		Microsoft::WRL::ComPtr<IDXGISwapChain> m_swapChain;

	};
}
