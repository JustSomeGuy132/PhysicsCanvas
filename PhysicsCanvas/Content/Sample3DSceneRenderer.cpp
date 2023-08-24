#include "pch.h"
#include "Sample3DSceneRenderer.h"

#include "..\Common\DirectXHelper.h"

using namespace PhysicsCanvas;

using namespace DirectX;
using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_tracking(false),
	m_deviceResources(deviceResources)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	Size outputSize = m_deviceResources->GetOutputSize();
	float aspectRatio = outputSize.Width / outputSize.Height;
	float fovAngleY = 70.0f * XM_PI / 180.0f;

	// This is a simple example of change that can be made when the app is in
	// portrait or snapped view.
	if (aspectRatio < 1.0f)
	{
		fovAngleY *= 2.0f;
	}

	// Note that the OrientationTransform3D matrix is post-multiplied here
	// in order to correctly orient the scene to match the display orientation.
	// This post-multiplication step is required for any draw calls that are
	// made to the swap chain render target. For draw calls to other targets,
	// this transform should not be applied.

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	projectionMat = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,		//near z
		100.0f		//far z
		);

	//get access to window so as to pass into controller, initialise controller
	wnd = Windows::UI::Core::CoreWindow::GetForCurrentThread();
	controller = ref new MoveLookControls();
	controller->Initialize(wnd);
	controller->SetPosition(DirectX::XMFLOAT3(0.0f, 1.0f, -2.1f));
	
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer) {
	if (!m_tracking)
	{
		controller->Update(wnd);	//updates position and rotation of camera based on input
		
		// Convert degrees to radians, then convert seconds to rotation angle
		float radiansPerSecond = XMConvertToRadians(m_degreesPerSecond);
		double totalRotation = timer.GetTotalSeconds() * radiansPerSecond;
		float radians = static_cast<float>(fmod(totalRotation, XM_2PI));

		STransform(radians);
	}
}

// Rotate the 3D cube model a set amount of radians.
void Sample3DSceneRenderer::STransform(float radians) {
	int i = 0;
	for each(std::shared_ptr<Mesh> m in meshes) {
		if (i == 0) {
			m->UpdateWorldMat(XMFLOAT3(1.0f, 0.0f, -0.2f), XMFLOAT3(0, radians, 0));
		}
		else {
			m->UpdateWorldMat(XMFLOAT3(-0.7f, 0.0f, 0.5f), XMFLOAT3());
		}
		i++;
	}
}

void Sample3DSceneRenderer::StartTracking()
{
	m_tracking = true;
}

// When tracking, the 3D cube can be rotated around its Y axis by tracking pointer position relative to the output screen width.
void Sample3DSceneRenderer::TrackingUpdate(float positionX)
{
	if (m_tracking)
	{
		float radians = XM_2PI * 2.0f * positionX / m_deviceResources->GetOutputSize().Width;
		STransform(radians);
	}
}

void Sample3DSceneRenderer::StopTracking()
{
	m_tracking = false;
}

// Renders a frame using the vertex and pixel shaders.
void Sample3DSceneRenderer::Render() {
	
	XMMATRIX viewMat = XMMatrixLookAtRH(
		XMLoadFloat3(&controller->get_Position()), XMLoadFloat3(&controller->get_LookPoint()), up);
	
	for each (std::shared_ptr<Mesh> m in meshes) {
		m->Render(viewMat * projectionMat);
	}
	
}

void Sample3DSceneRenderer::CreateNewMesh() {

}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	Mesh cube;
	cube.Create(m_deviceResources);
	meshes.push_front(std::make_shared<Mesh>(cube));
	Mesh cube2;
	cube2.Create(m_deviceResources);
	meshes.push_front(std::make_shared<Mesh>(cube2));
	
}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources() {
	for each (std::shared_ptr<Mesh> m in meshes) {
		m->ReleaseResources();
	}
}
