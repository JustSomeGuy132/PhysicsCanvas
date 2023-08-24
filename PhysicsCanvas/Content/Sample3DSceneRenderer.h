#pragma once

#include "pch.h"
#include "..\Common\DeviceResources.h"
#include "ShaderStructures.h"
#include "..\Common\StepTimer.h"
#include "..\Common\DirectXHelper.h"
#include "MoveLookControls.h"
#include "Mesh.h"
#include <list>
#include "d3dcompiler.h"

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
		void StartTracking();
		void TrackingUpdate(float positionX);
		void StopTracking();
		bool IsTracking() { return m_tracking; }
		void CreateNewMesh();

	private:
		void STransform(float radians);
	private:
		Windows::UI::Core::CoreWindow^ wnd;
		MoveLookControls^ controller;
		DirectX::XMMATRIX projectionMat;

		DirectX::XMVECTORF32 up = { 0.0f, 1.0f, 0.0f, 0.0f };
	private:
		// Cached pointer to device resources.
		std::shared_ptr<DX::DeviceResources> m_deviceResources;

		// List of all meshes in the scene
		std::list<std::shared_ptr<Mesh>> meshes;

		// Variables used with the rendering loop.
		bool	m_loadingComplete;
		float	m_degreesPerSecond;
		bool	m_tracking;
		
	};
}

