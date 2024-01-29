#include "pch.h"
#include "Sample3DSceneRenderer.h"

#include "..\Common\DirectXHelper.h"
#include <DirectXMath.h>
#include <sstream>

using namespace PhysicsCanvas;

using namespace DirectX;
using namespace Windows::Foundation;

// Loads vertex and pixel shaders from files and instantiates the cube geometry.
Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources),
	u_Time(0), latest_Time(0), is_stepping(false)
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

	if (controller->GetClick().x != -2) {
		RaycastFromClick(controller->GetClick().x, controller->GetClick().y);
	}
	//ensure camera controller can't go beneath the floor
	if (controller->get_Position().y <= 0.1f) {
		controller->SetPosition(
			XMFLOAT3(controller->get_Position().x, 0.1f, controller->get_Position().z)
		);
	}

	if (is_stepping) {
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
	if (u_Time >= latest_Time) {
		latest_Time = u_Time;
	}
	//u_Time += timer.GetElapsedSeconds();
	for each (std::shared_ptr<PhysicsBody> body in pBodies) {
		body->Step(u_Time);
	}
}

void Sample3DSceneRenderer::TimeManager() {
	ImGui::Begin("Time manager");
	if (ImGui::Button(is_stepping ? "Pause" : "Resume")) {
		is_stepping = !is_stepping;
	}
	ImGui::SameLine();
	ImGui::Text("Time ="); ImGui::SameLine();
	float timeBuf = u_Time;
	if (ImGui::InputFloat("s##Time", &timeBuf)) {

	}
	ImGui::SameLine();
	std::ostringstream timeText;
	timeText << "; Latest time = " << u_Time << "s";
	ImGui::Text(timeText.str().c_str());
	timeText.flush();
	ImGui::End();
}

void Sample3DSceneRenderer::ObjectManager() {
	ImGui::Begin("Object manager");

	static char nameBuf[128];
	for (int x = 0; x < selectedBody->GetName().length() + 1; x++) {
		nameBuf[x] = selectedBody->GetName()[x];
	}
	ImGui::Text("Name:"); ImGui::SameLine();
	if (ImGui::InputText(" ", nameBuf, IM_ARRAYSIZE(nameBuf)) && !is_stepping) {
		selectedBody->GiveName(nameBuf);
	}

	const char* shapeOptions[] = { "Cuboid", "Sphere" };
	int numOption = selectedBody->GetBounds()->GetType() == BoundingShape::Cuboid ? 0 : 1;
	ImGui::Text("Shape: "); ImGui::SameLine();
	ImGui::Combo("##ShapeSelect", &numOption, shapeOptions, 2);

	static float col[3] = { selectedBody->GetMesh().GetColour().x, selectedBody->GetMesh().GetColour().y, selectedBody->GetMesh().GetColour().z };
	ImGui::Text("Colour:");
	if (ImGui::ColorEdit3("##ColourEditor", col) && !is_stepping) {
		selectedBody->GetMesh().SetColour(XMFLOAT3(col[0], col[1], col[2]));
	}

	float massBuf = selectedBody->GetMass();
	ImGui::Text("Mass:"); ImGui::SameLine();
	if (ImGui::InputFloat("kg", &massBuf, 0, 0, "%e") && !is_stepping) {
		selectedBody->SetMass(massBuf);
	}


	switch (selectedBody->GetBounds()->GetType()) {
	case BoundingShape::Cuboid:
		ImGui::Text("Dimensions(x, y, z):");
		{
			float dimBuf[3] = { selectedBody->GetDimensions().x, selectedBody->GetDimensions().y, selectedBody->GetDimensions().z };
			if (ImGui::InputFloat3("m##DIMS", dimBuf) && !is_stepping) {
				selectedBody->ApplyScale(XMFLOAT3(dimBuf[0], dimBuf[1], dimBuf[2]));
			}
		}
		break;
	case BoundingShape::Sphere:
	{
		ImGui::Text("Radius:");
		float rBuf = selectedBody->GetDimensions().x;
		if (ImGui::InputFloat("m##Radius", &rBuf) && !is_stepping) {
			selectedBody->ApplyScale(XMFLOAT3(rBuf, rBuf, rBuf));
		}
	}
	break;
	}

	ImGui::Text("Position(x, y, z):");
	float posBuf[3] = { selectedBody->GetPosition().x, selectedBody->GetPosition().y, selectedBody->GetPosition().z };
	if (ImGui::InputFloat3("m##Pos", posBuf) && !is_stepping) {
		selectedBody->SetTransform(XMFLOAT3(posBuf[0], posBuf[1], posBuf[2]), selectedBody->GetRotation(), selectedBody->GetDimensions());
	}

	ImGui::Text("Velocity(x, y, z):");
	float velBuf[3] = { selectedBody->GetVelocity().x, selectedBody->GetVelocity().y, selectedBody->GetVelocity().x };
	if (ImGui::InputFloat3("m/s##VELOCITY", velBuf) && !is_stepping) {

	}

	ImGui::Text("Resultant force(x, y, z):"); ImGui::SameLine();
	XMFLOAT3 rForces = Force::ResultantF(selectedBody->ActiveForces(u_Time)).GetDirection();
	std::ostringstream rfor;
	rfor << rForces.x << "N, " << rForces.y << "N, " << rForces.z << "N";
	ImGui::Text(rfor.str().c_str());
	rfor.flush();
	ImGui::Text("  Magnitude: "); ImGui::SameLine();
	std::ostringstream rfor2;
	rfor2 << Force::ResultantF(selectedBody->ActiveForces(u_Time)).Magnitude() << "N";
	ImGui::Text(rfor2.str().c_str());
	rfor2.flush();

	if (ImGui::CollapsingHeader("Pre-determined object events")) {
		for (std::shared_ptr<PEvent> e : selectedBody->GetEvents()) {
			if (ImGui::TreeNode(e->GetId().c_str())) {
				//handle object weight first, this is non-negotiable and a special case
				if (e->GetId() == "Weight") {
					ImGui::Text(e->GetId().c_str());
					Force* eForce = dynamic_cast<Force*>(e.get()); //the weight will, no doubt, be of the Force type
					if (!eForce) {} //do nothing if the pointer is invalid, although it should not be	
					std::ostringstream oss;
					oss << "Direction(x, y, z):" << eForce->GetDirection().x << "N, " << eForce->GetDirection().y << "N, " << eForce->GetDirection().z << "N\n"
						<< "   Magnitude: " << eForce->Magnitude() << "N";
					ImGui::Text(oss.str().c_str());
					oss.flush();
				}
				else {
					ImGui::Text("Event name:"); ImGui::SameLine();
					char eNameBuf[128];
					for (int x = 0; x < e->GetId().length() + 1; x++) {
						eNameBuf[x] = e->GetId()[x];
					}
					if (ImGui::InputText("##EventName", eNameBuf, IM_ARRAYSIZE(eNameBuf)) && !is_stepping) {
						e->SetId(eNameBuf);
					}

					bool toggleBox = e->GetToggle();
					if (ImGui::Checkbox("Toggle", &toggleBox) && !is_stepping) {
						e->SetToggle(toggleBox);
					}

					const char* eventOptions[] = { "Force" };
					int eOptionNum = 0;
					ImGui::Combo("Event type", &eOptionNum, eventOptions, 1);

					ImGui::Text("Start time:"); ImGui::SameLine();
					float eventTimeNum = e->GetStart();
					if (ImGui::InputFloat("s##EStartT", &eventTimeNum) && !is_stepping) {
						e->SetStart(eventTimeNum);
					}
					//handle all other events
					switch (e->GetEventType()) {
					case PEvent::eventType::Force:
					{
						Force* eForce = dynamic_cast<Force*>(e.get());
						if (!eForce) { break; }
						const char* ForceOptions[] = { "Constant", "Impulse" };
						int forceTypeNum = eForce->GetForceType() == Force::Constant ? 0 : 1;
						if (ImGui::Combo("Force type", &forceTypeNum, ForceOptions, 2) && !is_stepping) {
							eForce->SetForceType(forceTypeNum == 0 ? Force::Constant : Force::Impulse);
						}
						if (eForce->GetForceType() == Force::Constant) {
							ImGui::Text("End time:"); ImGui::SameLine();
							float eventEndNum = e->GetEnd();
							if (ImGui::InputFloat("s##EEndT", &eventEndNum) && !is_stepping) {
								e->SetEnd(eventEndNum);
							}
						}
						ImGui::Text("Direction(x, y, z):");
						float eFBuf[3] = { eForce->GetDirection().x, eForce->GetDirection().y, eForce->GetDirection().z };
						float eFBuf2 = eForce->Magnitude();
						if (ImGui::InputFloat3("N##FORCEDIR", eFBuf) && !is_stepping) {
							eForce->SetDirection(XMFLOAT3(eFBuf[0], eFBuf[1], eFBuf[2]));
						}
						ImGui::Text("Magnitude:"); ImGui::SameLine();
						if (ImGui::InputFloat("N##FORCEMAG", &eFBuf2) && !is_stepping) {

						}
					}
					break;
					}
				}
				ImGui::TreePop();
			}
		}
		if (ImGui::Button("Add new event") && !is_stepping) {
			Force newEvent = Force(Force::Constant, XMFLOAT3());
			newEvent.SetId("New event");
			selectedBody->AddEvent(std::make_shared<Force>(newEvent));
		}
	}
	//handle events experienced in the specific instance in time
	if (ImGui::CollapsingHeader("Current object events")) {
		for (Force f : selectedBody->ActiveForces(u_Time)) {
			if (ImGui::TreeNode(f.GetId().c_str())) {
				ImGui::Text("Force name:"); ImGui::SameLine();
				ImGui::Text(f.GetId().c_str());

				std::ostringstream forceTxt;
				forceTxt << "Direction(x, y, z): " << f.GetDirection().x << "N, " << f.GetDirection().y << "N, " << f.GetDirection().z << "N\n"
					<< "   Magnitude: " << f.Magnitude() << "N";
				ImGui::Text(forceTxt.str().c_str());
				forceTxt.flush();

				ImGui::TreePop();
			}
		}
	}
	if (ImGui::CollapsingHeader("Entire forces array !!DEBUGGING!!")) {
		for (Force f : selectedBody->GetForces()) {
			ImGui::Text(f.GetId().c_str());
		}
	}
	ImGui::End();
}

// Renders a frame of all the models (and all UI widgets) on screen
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
	if (ImGui::TreeNode("Kinematic body")) {
		if (ImGui::Button("Cube")) {
			CreateNewMesh(CUBE);
		}
		if (ImGui::Button("Sphere")) {
			CreateNewMesh(SPHERE);
		}
		ImGui::TreePop();
	}
	if (ImGui::TreeNode("Quantum body")) {
		ImGui::TreePop();
	}
	ImGui::End();

	TimeManager();

	if (selectedBody != nullptr) {
		ObjectManager();
	}

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void Sample3DSceneRenderer::CreateNewMesh(const UINT shape) {
	PhysicsBody nbody;
	nbody.Create(shape, m_deviceResources);
	nbody.GiveName("OBJ1");
	//cast ray from centre of view to find appropriate position to place it
	Size display = m_deviceResources->GetOutputSize();
	XMMATRIX viewMat = XMMatrixLookAtRH(
		XMLoadFloat3(&controller->get_Position()), XMLoadFloat3(&controller->get_LookPoint()), up);
	XMVECTOR rayOriginScreen, rayDirScreen;
	XMVECTOR rayOrigin, rayDir;
	rayOriginScreen = XMVectorSet(display.Width / 2.0f, display.Height / 2.0f, 0.1f, 1.0f);
	rayDirScreen = XMVectorSet(display.Width / 2.0f, display.Height / 2.0f, 1.0f, 1.0f);
	
	XMFLOAT3 newpos;
	for (std::shared_ptr<PhysicsBody> body : pBodies) {
		rayOrigin = XMVector3Unproject(rayOriginScreen, 0, 0, display.Width, display.Height, 0, 1, projectionMat, viewMat, body->GetMesh().GetWorldMat());
		rayDir = XMVector3Unproject(rayDirScreen, 0, 0, display.Width, display.Height, 0, 1, projectionMat, viewMat, body->GetMesh().GetWorldMat());
		rayDir = XMVector3Normalize(rayDir - rayOrigin);
		XMFLOAT3 direction;
		XMStoreFloat3(&direction, rayDir);
		XMFLOAT3 point;
		for (float proportion = 0.05f; proportion < 30.0f; proportion += 0.05f) {
			point = { controller->get_Position().x + proportion * direction.x,
					controller->get_Position().y + proportion * direction.y,
					controller->get_Position().z + proportion * direction.z };
			newpos = point;
			if (BoundingShape::PointCollidingWithObject(point, body->GetBounds())) {
				nbody.ApplyTranslation(newpos);
				std::vector<XMFLOAT3> translations = BoundingShape::ResolveCollisions(nbody.GetBounds(), body->GetBounds());
				newpos = { newpos.x + translations[0].x - translations[1].x,
						newpos.y + translations[0].y - translations[1].y,
						newpos.z + translations[0].z - translations[1].z };
				nbody.SetTransform(newpos, nbody.GetRotation(), nbody.GetDimensions());
				
				std::shared_ptr<PhysicsBody> nbodyPointer = std::make_shared<PhysicsBody>(nbody);
				selectedBody = nbodyPointer;
				pBodies.push_back(nbodyPointer);
				return;
			}
		}	
	}
	nbody.ApplyTranslation(newpos);
	for (std::shared_ptr<PhysicsBody> body : pBodies) {
		if(BoundingShape::IsColliding(nbody.GetBounds(), body->GetBounds())) {
			std::vector<XMFLOAT3> translations = BoundingShape::ResolveCollisions(nbody.GetBounds(), body->GetBounds());
			newpos = { newpos.x + translations[0].x - translations[1].x,
					newpos.y + translations[0].y - translations[1].y,
					newpos.z + translations[0].z - translations[1].z };
			nbody.SetTransform(newpos, nbody.GetRotation(), nbody.GetDimensions());
		}
	}
	std::shared_ptr<PhysicsBody> nbodyPointer = std::make_shared<PhysicsBody>(nbody);
	selectedBody = nbodyPointer;
	pBodies.push_back(nbodyPointer);
}

void Sample3DSceneRenderer::RaycastFromClick(float x, float y) {
	if (already_casting)
		return;
	already_casting = true;

	Size display = m_deviceResources->GetOutputSize();

	XMMATRIX viewMat = XMMatrixLookAtRH(
		XMLoadFloat3(&controller->get_Position()), XMLoadFloat3(&controller->get_LookPoint()), up);
	
	XMVECTOR rayOriginScreen, rayDirScreen;
	XMVECTOR rayOrigin, rayDir;
	rayOriginScreen = XMVectorSet(x, y, 0.1f, 1.0f);
	rayDirScreen = XMVectorSet(x, y, 1.0f, 1.0f);

	int i = 0;
	for (std::shared_ptr<PhysicsBody> body : pBodies) {
		if(i > 0) {
			rayOrigin = XMVector3Unproject(rayOriginScreen, 0, 0, display.Width, display.Height, 0, 1, projectionMat, viewMat, body->GetMesh().GetWorldMat());
			rayDir = XMVector3Unproject(rayDirScreen, 0, 0, display.Width, display.Height, 0, 1, projectionMat, viewMat, body->GetMesh().GetWorldMat());
			rayDir = XMVector3Normalize(rayDir - rayOrigin);

			XMFLOAT3 direction;
			XMStoreFloat3(&direction, rayDir);

			XMFLOAT3 point;
			for (float proportion = 0.05f; proportion < 100.0f; proportion += 0.05f) {
				point = { controller->get_Position().x + proportion * direction.x,
						controller->get_Position().y + proportion * direction.y,
						controller->get_Position().z + proportion * direction.z };

				if (BoundingShape::PointCollidingWithObject(point, body->GetBounds())) {
					selectedBody = body;
					already_casting = false;
					return;
				}
			}
		}
		i++;
	}
	selectedBody = nullptr;
	already_casting = false;
}

void Sample3DSceneRenderer::CreateDeviceDependentResources()
{
	//create the floor of the world
	PhysicsBody floor;
	floor.Create(FLOOR, m_deviceResources);
	floor.GiveName("FLOOR");
	pBodies.push_front(std::make_shared<PhysicsBody>(floor));
	//CreateNewMesh(CUBE);
}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources() {
	for each (std::shared_ptr<PhysicsBody> body in pBodies) {
		body->ReleaseResources();
	}
	ImGui::DestroyContext();
	ImGui_ImplDX11_Shutdown();
}