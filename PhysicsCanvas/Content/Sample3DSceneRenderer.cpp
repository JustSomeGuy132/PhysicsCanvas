#include "pch.h"
#include "Sample3DSceneRenderer.h"

#include "..\Common\DirectXHelper.h"
#include "Wrappers.h"

using namespace PhysicsCanvas;

using namespace DirectX;
using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_deviceResources(deviceResources),
	u_Time(0), isStepping(true)
{
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	/*D3D11DeviceWrapper devWrap(m_deviceResources->GetD3DDevice());
	D3D11DeviceContextWrapper contextWrap(m_deviceResources->GetD3DDeviceContext());
	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGui_ImplDX11_Init(
		&devWrap,
		&contextWrap
	);
	ImGuiIO& io = ImGui::GetIO();*/

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
	controller->SetPosition(DirectX::XMFLOAT3(0.0f, 1.2f, -2.1f));
	controller->SetOrientation(-0.3f, 0.0f);
}

// Called once per frame, rotates the cube and calculates the model and view matrices.
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer) {

	controller->Update(wnd);	//updates position and rotation of camera based on input

	if (isStepping) {
		Step();
	}
}

void Sample3DSceneRenderer::Step() {
	u_Time += 0.001f;
	for each (std::shared_ptr<PhysicsBody> body in pBodies) {
		body->Step(u_Time);
	}
}

void Sample3DSceneRenderer::STransform(float radians) {
	int i = 0;
	for each (std::shared_ptr<PhysicsBody> body in pBodies) {
		if (i == 0) {
			body->SetTransform(XMFLOAT3(1.0f, 0.0f, -0.2f), XMFLOAT3(0, radians, 0));
		}
		else {
			body->SetTransform(XMFLOAT3(-0.7f, 0.0f, -0.2f), XMFLOAT3(0, -radians, 0));
		}
		i++;
	}
}


// Renders a frame of all the models on screen [UI COMING SOON]
void Sample3DSceneRenderer::Render() {
	/*ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame();*/

	XMMATRIX viewMat = XMMatrixLookAtRH(
		XMLoadFloat3(&controller->get_Position()), XMLoadFloat3(&controller->get_LookPoint()), up);

	for each (std::shared_ptr<PhysicsBody> body in pBodies) {
		body->Render(viewMat * projectionMat);
	}

	/*ImGui::ShowDemoWindow();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());*/
}

void Sample3DSceneRenderer::CreateNewMesh() {
	PhysicsBody body;
	body.Create(m_deviceResources);
	body.AddForce(Force::ForceType::Impulse, XMFLOAT3(0.0f, 1500.0f, 0.0f));
	pBodies.push_front(std::make_shared<PhysicsBody>(body));
}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	CreateNewMesh();
}



void Sample3DSceneRenderer::ReleaseDeviceDependentResources() {
	for each (std::shared_ptr<PhysicsBody> body in pBodies) {
		body->ReleaseResources();
	}
	
}
