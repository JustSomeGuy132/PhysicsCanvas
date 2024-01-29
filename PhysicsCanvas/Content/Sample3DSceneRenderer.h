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
		void CreateNewMesh(const UINT shape);
		void RaycastFromClick(float x, float y);
		void ObjectManager();
		void TimeManager();
	
		float u_Time;
		float latest_Time;
		void Step(DX::StepTimer const& timer);
	
	private:
		Windows::UI::Core::CoreWindow^ wnd;
		MoveLookControls^ controller;
		DirectX::XMMATRIX projectionMat;

		DirectX::XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };

		bool is_stepping;
	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// List of all meshes in the scene
		std::list<std::shared_ptr<PhysicsBody>> pBodies;
		std::shared_ptr<PhysicsBody> selectedBody = nullptr;

		bool already_casting = false;

	};
}
