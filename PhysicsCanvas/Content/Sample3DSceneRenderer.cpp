﻿#include "pch.h"
#include "Sample3DSceneRenderer.h"

#include "..\Common\DirectXHelper.h"
#include <sstream>

using namespace PhysicsCanvas;

using namespace DirectX;
using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_loadingComplete(false),
	m_degreesPerSecond(45),
	m_deviceResources(deviceResources),
	u_Time(0), isStepping(false)
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

	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = { outputSize.Width, outputSize.Height };

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
		1000.0f		//far z
	);

	//get access to window so as to pass into controller, initialise controller
	wnd = Windows::UI::Core::CoreWindow::GetForCurrentThread();
	controller = ref new MoveLookControls();
	controller->Initialize(wnd);
	controller->SetPosition(DirectX::XMFLOAT3(0.0f, 1.2f, -2.1f));
	controller->SetOrientation(-0.3f, 0.0f);

}

// Called once per frame
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer) {

	controller->Update(wnd);	//updates position and rotation of camera based on input
	//ensure camera controller can't go beneath the floor
	if (controller->get_Position().y <= 0.1f) {
		controller->SetPosition(
			XMFLOAT3(controller->get_Position().x, 0.2f, controller->get_Position().z)
		);
	}

	if (isStepping) {
		Step(timer);
	}
}

void Sample3DSceneRenderer::Step(DX::StepTimer const& timer) {
	
	int i = 0;
	for (std::shared_ptr<PhysicsBody> body1 : pBodies) {
		int j = 0;
		for (std::shared_ptr<PhysicsBody> body2 : pBodies) {
			//make sure not to check collisions on the same object
			if(i != j) {
				//if body1 is colliding with body2
				if (BoundingShape::IsColliding(body1->GetBounds(), body2->GetBounds())) {
					
					std::vector<XMFLOAT3> translations = BoundingShape::ResolveCollisions(body1->GetBounds(), body2->GetBounds());
					body1->ApplyTranslation(translations[0]);
					body2->ApplyTranslation(translations[1]);
					
					body1->RegisterCollision(body2, u_Time);
					//body2->RegisterCollision(body1, u_Time);
				}
				else {
					
				}
			}
			j++;
		}
		i++;
	}
	u_Time += 0.001f;
	//u_Time += timer.GetElapsedSeconds();
	for each (std::shared_ptr<PhysicsBody> body in pBodies) {
		body->Step(u_Time);
	}
}

void Sample3DSceneRenderer::STransform(float radians) {
	
}


// Renders a frame of all the models on screen
void Sample3DSceneRenderer::Render() {
	ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame();

	XMMATRIX viewMat = XMMatrixLookAtRH(
		XMLoadFloat3(&controller->get_Position()), XMLoadFloat3(&controller->get_LookPoint()), up);

	for each (std::shared_ptr<PhysicsBody> body in pBodies) {
		body->Render(viewMat * projectionMat);
	}

	ImGui::ShowDemoWindow();
	
	ImGui::Begin("Add Objects");
	ImGui::End();

	ImGui::Begin("Time manager");
	if (ImGui::Button(isStepping? "Pause" : "Resume")) {
		isStepping = !isStepping;
	}
	std::ostringstream timeText;
	timeText << "t = " << u_Time << "s";
	ImGui::Text(timeText.str().c_str());
	ImGui::End();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Sample3DSceneRenderer::CreateNewMesh(const UINT shape) {
	PhysicsBody body;
	body.Create(shape, m_deviceResources);
	body.GiveName("OBJ1");
	body.ApplyTranslation(DirectX::XMFLOAT3(0.0f, 1.0f, 0.0f));
	//body.AddForce(Force::Impulse, XMFLOAT3(0.0f, 1500.0f, 0.0f), 0.0f);
	pBodies.push_back(std::make_shared<PhysicsBody>(body));
}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	
	//create the floor of the world
	PhysicsBody floor;
	floor.Create(FLOOR, m_deviceResources);
	floor.GiveName("FLOOR");
	pBodies.push_back(std::make_shared<PhysicsBody>(floor));
	CreateNewMesh(CUBE);
	//CreateNewMesh(SPHERE);
}



void Sample3DSceneRenderer::ReleaseDeviceDependentResources() {
	for each (std::shared_ptr<PhysicsBody> body in pBodies) {
		body->ReleaseResources();
	}
	ImGui::DestroyContext();
	ImGui_ImplDX11_Shutdown();
}
