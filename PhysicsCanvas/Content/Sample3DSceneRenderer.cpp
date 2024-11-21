#include "pch.h"
#include "Sample3DSceneRenderer.h"

#include "..\Common\DirectXHelper.h"
#include <DirectXMath.h>
#include <sstream>
#include "../ArrowMesh.h"
#include "../im-neo-sequencer-main/imgui_neo_sequencer.h"
#include <fstream>
#include <windows.h>
#include <coroutine>

using namespace PhysicsCanvas;

using namespace DirectX;
using namespace Windows::Foundation;

Sample3DSceneRenderer::Sample3DSceneRenderer(const std::shared_ptr<DX::DeviceResources>& deviceResources) :
	m_deviceResources(deviceResources),
	u_Time(0), latest_Time(0), is_stepping(false),
	is_graphing(false), data_obtained(false)
{
	library = std::unique_ptr<ProjectLib>(new ProjectLib(m_deviceResources));
	CreateDeviceDependentResources();
	CreateWindowSizeDependentResources();
}

// Initializes view parameters when the window size changes.
void Sample3DSceneRenderer::CreateWindowSizeDependentResources()
{
	//Initialise input handlers
	wnd = Windows::UI::Core::CoreWindow::GetForCurrentThread();
	controller = ref new MoveLookControls();
	controller->Initialize(wnd);

	if (!data_obtained) return library->CreateWindowSizeDependentResources();

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

	// This sample makes use of a right-handed coordinate system using row-major matrices.
	projectionMat = XMMatrixPerspectiveFovRH(
		fovAngleY,
		aspectRatio,
		0.01f,		//near z
		1000.0f		//far z
	);

	//Initialise camera controller
	controller->SetPosition(DirectX::XMFLOAT3(0.0f, 1.2f, -2.1f));
	controller->SetOrientation(-0.3f, 0.0f);

}

void Sample3DSceneRenderer::SaveToFile() {
	if (is_filing)
		return;
	is_filing = true;

	float current_time = u_Time;
	TimeJump(0);

	std::stringstream data;
	int i = 0;
	for (std::shared_ptr<PhysicsBody> body : pBodies) {
		if (i > 0)
			data << body->BodyData() << "\n";
		i++;
	}
	std::string d = data.str();
	if (currentFile == "NONE") {
		concurrency::create_task(library->getLocalFolder()->CreateFileAsync("Unnamed simulation.psim", Windows::Storage::CreationCollisionOption::GenerateUniqueName))
		.then([this, d](Windows::Storage::StorageFile^ newFile) {
			if (newFile) {
				std::wstring Wfilename(newFile->Name->Begin());
				std::string notice_text = "Your file has been saved under - " + std::string(Wfilename.begin(), Wfilename.end()) + 
					" - you may rename it from your file explorer";
				MessageBox(NULL, notice_text.c_str(), "Project file created", MB_ICONINFORMATION | MB_OK);
				std::wstring w_str(d.begin(), d.end());
				Platform::String^ text = ref new Platform::String(w_str.c_str());
				auto writeTask = concurrency::create_task(Windows::Storage::FileIO::WriteTextAsync(newFile, text));
				writeTask.then([&]() {
					is_filing = false;
					MessageBox(NULL, "Project successfully saved to file", "Project save successful", MB_ICONINFORMATION | MB_OK);
					});
			}
			else {
				MessageBox(NULL, "File could not be created", "File creation error", MB_ICONERROR | MB_OK);
			}
		});
	}
	else {
		concurrency::create_task(library->getLocalFolder()->GetFileAsync(currentFile))
		.then([this, d](Windows::Storage::StorageFile^ saveFile) {
			if (saveFile) {
				std::wstring w_str(d.begin(), d.end());
				Platform::String^ text = ref new Platform::String(w_str.c_str());
				auto writeTask = concurrency::create_task(Windows::Storage::FileIO::WriteTextAsync(saveFile, text));
				writeTask.then([&]() {
					is_filing = false;
					MessageBox(NULL, "Project successfully saved to file", "Project save successful", MB_ICONINFORMATION | MB_OK);
					});
			}
			else {
				MessageBox(NULL, "Data could not be saved to specified file, as the file could not be loaded.", "File loading error", MB_ICONWARNING | MB_OK);
				is_filing = false;
			}
		});
	}
	TimeJump(current_time);
}

void Sample3DSceneRenderer::LoadFromFile(std::string d) { //d represents data input
	pBodies.clear();
	CreateDeviceDependentResources();
	std::istringstream dss(d);
	//read file contents into a variable
	std::vector<std::string> data;
	while (dss) {
		std::string line;
		std::getline(dss, line);
		data.push_back(line);
	}
	//process contents to load simulation
	std::vector<PhysicsBody> bodies;
	PhysicsBody b;
	std::vector<std::shared_ptr<PEvent>> events;
	std::shared_ptr<PEvent> e;
	DirectX::XMFLOAT3 float3Buffer;
	Force* eForce = dynamic_cast<Force*>(e.get());
	for (std::string l : data) {
		//split the string into a list
		std::istringstream d(l);
		std::vector<std::string> words;
		std::string word;
		while (std::getline(d, word, ' ')) {
			words.push_back(word);
		}
		if (words.size() > 0) {
			//to process the line, go through its words one by one
			if (words[0] == "OBJECT") {
				if (words[1] == "KINEMATIC") {/*No need to make changes if this is a kinematic body since its the default*/ }
			}
			else if (words[0] == "NAME") {
				std::string name = "";
				for (int i = 1; i < words.size(); i++) {
					name += words[i] + " ";
				}
				name = name.substr(0, name.size() - 1); //get rid of extra space
				b.GiveName(name);
			}
			else if (words[0] == "COL") {
				float3Buffer = { std::stof(words[1].c_str()), std::stof(words[2].c_str()), std::stof(words[3].c_str()) };
			}
			else if (words[0] == "SHAPE") {
				b.Create(words[1] == "Cuboid" ? CUBE : SPHERE, m_deviceResources);
			}
			else if (words[0] == "DIMS") {
				switch (b.GetBounds()->GetType()) {
				case BoundingShape::Cuboid:
					b.ApplyScale(XMFLOAT3(std::stof(words[1].c_str()), std::stof(words[2].c_str()), std::stof(words[3].c_str())));
					break;
				case BoundingShape::Sphere:
					b.ApplyScale(XMFLOAT3(std::stof(words[1].c_str()), std::stof(words[1].c_str()), std::stof(words[1].c_str())));
					break;
				}
			}
			else if (words[0] == "POS") {
				b.ApplyTranslation(XMFLOAT3(std::stof(words[1].c_str()), std::stof(words[2].c_str()), std::stof(words[3].c_str())));
			}
			else if (words[0] == "ROT") {
				b.ApplyRotation(XMFLOAT3(std::stof(words[1].c_str()), std::stof(words[2].c_str()), std::stof(words[3].c_str())));
			}
			else if (words[0] == "VEL") {
				b.SetVelocity(XMFLOAT3(std::stof(words[1].c_str()), std::stof(words[2].c_str()), std::stof(words[3].c_str())));
			}
			else if (words[0] == "AVEL") {
				b.SetAngVelocity(XMFLOAT3(std::stof(words[1].c_str()), std::stof(words[2].c_str()), std::stof(words[3].c_str())));
			}
			else if (words[0] == "MASS") {
				b.SetMass(std::stof(words[1].c_str()));
			}
			//handle adding events from the file data
			else if (words[0] == "EVENT") {
				if (words[1] == "FORCE") {
					e = std::make_shared<Force>(Force::Constant, XMFLOAT3());
					eForce = dynamic_cast<Force*>(e.get());
				}
			}
			else if (words[0] == "ID") {
				std::string id = "";
				for (int i = 1; i < words.size(); i++) {
					id += words[i] + " ";
				}
				id = id.substr(0, id.size() - 1); //get rid of extra space
				e->SetId(id);
			}
			else if (words[0] == "START") {
				e->SetStart(std::stof(words[1].c_str()));
			}
			else if (words[0] == "FTYPE") {
				if (eForce) {
					if (words[1] == "Constant")
						eForce->SetForceType(Force::Constant);
					else if (words[1] == "Impulse")
						eForce->SetForceType(Force::Impulse);
				}
			}
			else if (words[0] == "END") {
				e->SetEnd(std::stof(words[1].c_str()));
			}
			else if (words[0] == "DIR") {
				if (eForce)
					eForce->SetDirection(XMFLOAT3(std::stof(words[1].c_str()), std::stof(words[2].c_str()), std::stof(words[3].c_str())));
			}
			else if (words[0] == "FROM") {
				if (eForce)
					eForce->SetFrom(XMFLOAT3(std::stof(words[1].c_str()), std::stof(words[2].c_str()), std::stof(words[3].c_str())));
			}
			else if (words[0] == "ENDEVENT") {
				events.push_back(e);
				e = std::make_shared<PEvent>();
			}
			else if (words[0] == "ENDOBJECT") {
				for (std::shared_ptr<PEvent> ev : events)
					b.AddEvent(ev);
				bodies.push_back(b);
				b = PhysicsBody();
			}
		}
	}
	for (PhysicsBody bo : bodies) {
		pBodies.push_back(std::make_shared<PhysicsBody>(bo));
	}
}

// Called once per frame
void Sample3DSceneRenderer::Update(DX::StepTimer const& timer) {
	if (!data_obtained) {
		if (library->ChosenFile() != "")
			return OnDataObtained();
		return;
	}

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
		Step();
	}
}

void Sample3DSceneRenderer::OnDataObtained() {
	data_obtained = true;
	CreateWindowSizeDependentResources();
	CreateDeviceDependentResources();
	currentFile = library->ChosenFile();
	if (currentFile != "NONE") {
		if (library->ChosenPreset() != "") {
			//load preset data
			concurrency::create_task(library->getLocalFolder()->GetFileAsync(library->ChosenPreset()))
			.then([&](Windows::Storage::StorageFile^ preset_file) {
				concurrency::create_task(Windows::Storage::FileIO::ReadTextAsync(preset_file))
				.then([&](Platform::String^ data) {
					std::wstring w_data(data->Begin());
					std::string s_data(w_data.begin(), w_data.end());
					LoadFromFile(s_data);
					SaveToFile();
				});
			});
		}
		else {
			//load file data
			concurrency::create_task(library->getLocalFolder()->GetFileAsync(currentFile))
			.then([&](Windows::Storage::StorageFile^ current_file) {
				concurrency::create_task(Windows::Storage::FileIO::ReadTextAsync(current_file))
				.then([&](Platform::String^ data) {
					std::wstring w_data(data->Begin());
					std::string s_data(w_data.begin(), w_data.end());
					LoadFromFile(s_data);
				});
			});
		}
	}
}

void Sample3DSceneRenderer::Step() {
	if (is_step) return;
	is_step = true;

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
					body2->RegisterCollision(body1, u_Time);
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
	for each (std::shared_ptr<PhysicsBody> body in pBodies) {
		body->Step(u_Time);
	}
	is_step = false;
}

void Sample3DSceneRenderer::TimeManager() {
	ImGui::Begin("Time manager");

	if (ImGui::Button(is_stepping ? "Pause" : "Resume")) {
		is_stepping = !is_stepping;
	}
	ImGui::SameLine();
	ImGui::Text("Time ="); ImGui::SameLine();
	float timeBuf = u_Time;
	if (ImGui::InputFloat("s##Time", &timeBuf) && timeBuf >= 0) {
		TimeJump(timeBuf);
	}
	ImGui::SameLine();
	std::ostringstream timeText;
	timeText << "; Latest time = " << latest_Time << "s";
	ImGui::Text(timeText.str().c_str());
	timeText.flush();
	ImGui::SameLine();
	if (ImGui::Button("Toggle grapher"))
		is_graphing = !is_graphing;

	int32_t currentFrame = u_Time * 1000;
	int32_t startFrame = 0;
	int32_t endFrame = latest_Time >= 1.0f? latest_Time * 1000 : 1000;
	if (ImGui::BeginNeoSequencer("Sequencer", &currentFrame, &startFrame, &endFrame)) {
		if (!is_stepping && pBodies.size() > 1) {
			TimeJump(currentFrame / 1000.0f);
		}
		int i = 0;
		for(std::shared_ptr<PhysicsBody> b : pBodies) {
			if (i > 0) {
				std::vector<ImGui::FrameIndexType> keyframes;
				for (std::shared_ptr<PEvent> e : b->GetEvents()) {
					keyframes.push_back(e->GetStart() * 1000);
				}
				for (std::tuple<float, std::string> stamp : b->GetTimestamps()) {
					keyframes.push_back(std::get<0>(stamp) * 1000);
				}
				if (ImGui::BeginNeoTimeline(b->GetName().c_str(), keyframes)) {
					ImGui::EndNeoTimeLine();
				}
			} i++;
		}
		ImGui::EndNeoSequencer();
	}

	ImGui::End();
}

void Sample3DSceneRenderer::TimeJump(float time) {
	if (time <= latest_Time) {
		u_Time = time;
		for (std::shared_ptr<PhysicsBody> body : pBodies) {
			body->TimeJump(u_Time);
		}
	}
	else {
		u_Time = latest_Time;
		for (std::shared_ptr<PhysicsBody> b : pBodies)
			b->TimeJump(u_Time);
		while (u_Time < time) {
			Step();
		}
	}
}

void Sample3DSceneRenderer::TimeWipe() {
	u_Time = latest_Time = 0;
	for (std::shared_ptr<PhysicsBody> b : pBodies) {
		b->GetTimeKeeper().Wipe({0, b->GetPosition(), b->GetRotation(), XMFLOAT3(), XMFLOAT3()});
		b->GetForces().clear();
		b->GetTimestamps().clear();
	}
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

	ImGui::TextDisabled("(!)Before editing these properties...(!)");
	if (ImGui::BeginItemTooltip()) {
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted("Editing the below properties will erase the simulation!\n"
								"The time values will be reset to 0 with the current state being set to the initial state!\n"
								"Please ensure your timeline is set to 0s before making edits here!");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
	
	switch (selectedBody->GetType()) {
	case PhysicsBody::Kinematic:
		KinematicManager();
		break;
	}

	ImGui::End();
}

void Sample3DSceneRenderer::KinematicManager() {
	ImGui::Text("Position(x, y, z):");
	float posBuf[3] = { selectedBody->GetPosition().x, selectedBody->GetPosition().y, selectedBody->GetPosition().z };
	if (ImGui::InputFloat3("m##Pos", posBuf) && !is_stepping) {
		selectedBody->SetTransform(XMFLOAT3(posBuf[0], posBuf[1], posBuf[2]), selectedBody->GetRotation(), selectedBody->GetDimensions());
		TimeWipe();
	}

	ImGui::Text("Rotation(roll, pitch, yaw):");
	float rotBuf[3] = { selectedBody->GetRotation().x, selectedBody->GetRotation().z, selectedBody->GetRotation().y };
	if (ImGui::DragFloat3("rad##Rot", rotBuf, 0.001f) && !is_stepping) {
		selectedBody->SetTransform(selectedBody->GetPosition(), XMFLOAT3(rotBuf[0], rotBuf[2], rotBuf[1]), selectedBody->GetDimensions());
		TimeWipe();
	}

	ImGui::Text("Velocity(x, y, z):");
	float velBuf[3] = { selectedBody->GetVelocity().x, selectedBody->GetVelocity().y, selectedBody->GetVelocity().z };
	if (ImGui::InputFloat3("m/s##Vel", velBuf) && !is_stepping) {
		selectedBody->SetVelocity(XMFLOAT3(velBuf[0], velBuf[1], velBuf[2]));
		TimeWipe();
	}

	ImGui::Text("Angular velocity(roll, pitch, yaw):");
	float angvelBuf[3] = { selectedBody->GetAngularVelocity().x, selectedBody->GetAngularVelocity().y, selectedBody->GetAngularVelocity().z };
	if (ImGui::InputFloat3("rad/s##AVel", angvelBuf) && !is_stepping) {
		selectedBody->SetAngVelocity(XMFLOAT3(angvelBuf[0], angvelBuf[1], angvelBuf[2]));
		TimeWipe();
	}

	
	float massBuf = selectedBody->GetMass();
	ImGui::Text("Mass:"); ImGui::SameLine();
	if (ImGui::InputFloat("kg", &massBuf, 0, 0, "%e") && !is_stepping) {
		selectedBody->SetMass(massBuf);
		TimeWipe();
	}

	switch (selectedBody->GetBounds()->GetType()) {
	case BoundingShape::Cuboid:
		ImGui::Text("Dimensions(x, y, z):");
		{
			float dimBuf[3] = { selectedBody->GetDimensions().x, selectedBody->GetDimensions().y, selectedBody->GetDimensions().z };
			if (ImGui::InputFloat3("m##DIMS", dimBuf) && !is_stepping) {
				selectedBody->ApplyScale(XMFLOAT3(dimBuf[0], dimBuf[1], dimBuf[2]));
				TimeWipe();
			}
		}
		break;
	case BoundingShape::Sphere:
		ImGui::Text("Radius:");
		{
			float rBuf = selectedBody->GetDimensions().x;
			if (ImGui::InputFloat("m##Radius", &rBuf) && !is_stepping) {
				selectedBody->ApplyScale(XMFLOAT3(rBuf, rBuf, rBuf));
				TimeWipe();
			}
		}
		break;
	}
	ImGui::Text("Resultant force(x, y, z):");
	XMFLOAT3 rForces = Force::ResultantF(selectedBody->ActiveForces(u_Time)).GetDirection();
	std::ostringstream rfor;
	rfor << rForces.x << "N, " << rForces.y << "N, " << rForces.z << "N\n"
		<< "  Magnitude: " << PhysMaths::Magnitude(rForces) << "N";
	ImGui::Text(rfor.str().c_str());
	rfor.flush();

	ImGui::Text("Torque(roll, pitch, yaw):");
	XMFLOAT3 torq = selectedBody->Torque(u_Time);
	std::ostringstream torText;
	torText << torq.x << "Nm, " << torq.z << "Nm, " << torq.y << "Nm\n"
		<< "  Magnitude: " << PhysMaths::Magnitude(torq) << "Nm";
	ImGui::Text(torText.str().c_str());
	torText.flush();

	if (ImGui::CollapsingHeader("Pre-determined object events")) {
		for (std::shared_ptr<PEvent> e : selectedBody->GetEvents()) {
			if (ImGui::TreeNode(e->GetId().c_str())) {
				//handle object weight first, this is non-negotiable and a special case
				if (e->GetId() == "Weight") {
					ImGui::Text(e->GetId().c_str());
					Force* eForce = dynamic_cast<Force*>(e.get()); //the weight will, no doubt, be of the Force type	
					std::ostringstream oss;
					oss << "Direction(x, y, z):" << eForce->GetDirection().x << "N, " << eForce->GetDirection().y << "N, " << eForce->GetDirection().z << "N\n"
						<< "   Magnitude: " << eForce->Magnitude() << "N";
					ImGui::Text(oss.str().c_str());
					oss.flush();
					std::ostringstream oss2;
					oss2 << "Acting from(x,y,z): " << eForce->GetFrom().x << "m, " << eForce->GetFrom().y << "m, " << eForce->GetFrom().z << "m";
					ImGui::Text(oss2.str().c_str());
					oss2.flush();
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
						TimeWipe();
					}

					const char* eventOptions[] = { "Force" };
					int eOptionNum = 0;
					ImGui::Combo("Event type", &eOptionNum, eventOptions, 1);

					ImGui::Text("Start time:"); ImGui::SameLine();
					float eventTimeNum = e->GetStart();
					if (ImGui::InputFloat("s##EStartT", &eventTimeNum) && !is_stepping) {
						e->SetStart(eventTimeNum);
						TimeWipe();
					}
					//handle all other events
					switch (e->GetEventType()) {
					case PEvent::eventType::Force:
						Force* eForce = dynamic_cast<Force*>(e.get());
						if (!eForce) { break; }
						const char* ForceOptions[] = { "Constant", "Impulse" };
						int forceTypeNum = eForce->GetForceType() == Force::Constant ? 0 : 1;
						if (ImGui::Combo("Force type", &forceTypeNum, ForceOptions, 2) && !is_stepping) {
							eForce->SetForceType(forceTypeNum == 0 ? Force::Constant : Force::Impulse);
							TimeWipe();
						}
						if (eForce->GetForceType() == Force::Constant) {
							ImGui::Text("End time:"); ImGui::SameLine();
							float eventEndNum = e->GetEnd();
							if (ImGui::InputFloat("s##EEndT", &eventEndNum) && !is_stepping) {
								e->SetEnd(eventEndNum);
								TimeWipe();
							}
						}
						ImGui::Text("Direction(x, y, z):");
						float eFBuf[3] = { eForce->GetDirection().x, eForce->GetDirection().y, eForce->GetDirection().z };
						float eFBuf2 = eForce->Magnitude();
						if (ImGui::InputFloat3("N##FORCEDIR", eFBuf) && !is_stepping) {
							eForce->SetDirection(XMFLOAT3(eFBuf[0], eFBuf[1], eFBuf[2]));
							TimeWipe();
						}
						ImGui::Text("Magnitude:"); ImGui::SameLine();
						if (ImGui::InputFloat("N##FORCEMAG", &eFBuf2) && !is_stepping) {
							eForce->SetDirection(PhysMaths::VecTimesByConstant(eForce->GetDirection(), eFBuf2 / eForce->Magnitude()));
							TimeWipe();
						}
						ImGui::Text("Acting from(x, y, z):");
						static float eFfromBuf0 = eForce->GetFrom().x, eFfromBuf1 = eForce->GetFrom().y, eFfromBuf2 = eForce->GetFrom().z;

						if (ImGui::DragFloat("m##ActingFromX", &eFfromBuf0, 0.001f,
							selectedBody->GetBounds()->GetMinPoint().x, selectedBody->GetBounds()->GetMaxPoint().x) && !is_stepping
							&& BoundingShape::PointCollidingWithObject(XMFLOAT3(eFfromBuf0, eFfromBuf1, eFfromBuf2), selectedBody->GetBounds())
							) {
							eForce->SetFrom(XMFLOAT3(eFfromBuf0, eFfromBuf1, eFfromBuf2));
							TimeWipe();
						}
						if (ImGui::DragFloat("m##ActingFromY", &eFfromBuf1, 0.001f,
							selectedBody->GetBounds()->GetMinPoint().y, selectedBody->GetBounds()->GetMaxPoint().y) && !is_stepping
							&& BoundingShape::PointCollidingWithObject(XMFLOAT3(eFfromBuf0, eFfromBuf1, eFfromBuf2), selectedBody->GetBounds())
							) {
							eForce->SetFrom(XMFLOAT3(eFfromBuf0, eFfromBuf1, eFfromBuf2));
							TimeWipe();
						}
						if (ImGui::DragFloat("m##ActingFromZ", &eFfromBuf2, 0.001f,
							selectedBody->GetBounds()->GetMinPoint().z, selectedBody->GetBounds()->GetMaxPoint().z) && !is_stepping
							&& BoundingShape::PointCollidingWithObject(XMFLOAT3(eFfromBuf0, eFfromBuf1, eFfromBuf2), selectedBody->GetBounds())
							) {
							eForce->SetFrom(XMFLOAT3(eFfromBuf0, eFfromBuf1, eFfromBuf2));
							TimeWipe();
						}

						break;
					}
				}
				ImGui::TreePop();
			}
		}
		if (ImGui::Button("Add new event") && !is_stepping) {
			Force newEvent = Force(Force::Constant, XMFLOAT3());
			newEvent.SetFrom(selectedBody->GetPosition());
			newEvent.SetId("New event");
			selectedBody->AddEvent(std::make_shared<Force>(newEvent));
			TimeWipe();
		}
	}
	//handle events experienced in the specific instance in time
	if (ImGui::CollapsingHeader("Current object events")) {
		for (Force f : selectedBody->ActiveForces(u_Time)) {
			if (ImGui::TreeNode(f.GetId().c_str())) {
				ImGui::Text("Force name:"); ImGui::SameLine();
				ImGui::Text(f.GetId().c_str());
				XMFLOAT3 torq = PhysMaths::Float3Cross(
					XMFLOAT3(selectedBody->GetPosition().x - f.GetFrom().x,
						selectedBody->GetPosition().y - f.GetFrom().y, selectedBody->GetPosition().z - f.GetFrom().z), f.GetDirection());

				std::ostringstream forceTxt;
				forceTxt << "Direction(x, y, z): " << f.GetDirection().x << "N, " << f.GetDirection().y << "N, " << f.GetDirection().z << "N\n"
					<< "   Magnitude: " << f.Magnitude() << "N\n"
					<< "Acting from(x,y,z): " << f.GetFrom().x << "m, " << f.GetFrom().y << "m, " << f.GetFrom().z << "m\n"
					<< "Resulting torque(roll, pitch, yaw): \n" << torq.x << "Nm, " << torq.z << "Nm, " << torq.y << "Nm";
				ImGui::Text(forceTxt.str().c_str());
				forceTxt.flush();

				ImGui::TreePop();
			}
		}
	}

}

void Sample3DSceneRenderer::GraphPlotter() {
	ImGui::Begin("Graph plotter");

	std::vector<float> timeVals;
	timeVals.reserve(static_cast<size_t>((latest_Time / 0.001f) + 1));
	for (float t = 0; t <= latest_Time; t += 0.001f)
		timeVals.push_back(t);

	std::vector<std::tuple<std::string, std::vector<float>>> yAxes;
	int bI = 0;
	for (std::shared_ptr<PhysicsBody> b : pBodies) {
		if (bI > 0 && ImGui::TreeNode(b->GetName().c_str())) {
			static bool displacement = false;
			static bool speed = false;
			static bool momentum = false;
			static bool k_energy = false;
			static bool gp_energy = false;

			ImGui::Checkbox(("Plot displacement##" + b->GetName()).c_str(), &displacement);
			ImGui::Checkbox(("Plot speed##" + b->GetName()).c_str(), &speed);
			ImGui::Checkbox(("Plot momentum##" + b->GetName()).c_str(), &momentum);
			ImGui::Checkbox(("Plot kinetic energy##" + b->GetName()).c_str(), &k_energy);
			ImGui::Checkbox(("Plot relative gravitational potential energy##" + b->GetName()).c_str(), &gp_energy);

			if (displacement) {
				std::vector<float> dispY;
				dispY.reserve(timeVals.size());
				for (float t : timeVals) {
					dispY.push_back(PhysMaths::Distance(b->GetTimeKeeper().Retrieve(0).position, b->GetTimeKeeper().Retrieve(t).position));
				}
				yAxes.push_back(std::make_tuple(("Displacement of " + b->GetName() + "(m)"), std::move(dispY)));
			}
			if (speed) {
				std::vector<float> speedY;
				speedY.reserve(timeVals.size());
				for (float t : timeVals) {
					speedY.push_back(PhysMaths::Magnitude(b->GetTimeKeeper().Retrieve(t).velocity));
				}
				yAxes.push_back(std::make_tuple(("Speed of " + b->GetName() + "(m/s)"), std::move(speedY)));
			}
			if (momentum) {
				std::vector<float> momY;
				momY.reserve(timeVals.size());
				for (float t : timeVals) {
					momY.push_back(PhysMaths::Magnitude(b->GetTimeKeeper().Retrieve(t).velocity) * b->GetMass());
				}
				yAxes.push_back(std::make_tuple(("Momentum of " + b->GetName() + "(kg m/s)"), std::move(momY)));
			}
			if (k_energy) {
				std::vector<float> kinY;
				kinY.reserve(timeVals.size());
				for (float t : timeVals) {
					kinY.push_back((0.5f * b->GetMass() * pow(PhysMaths::Magnitude(b->GetTimeKeeper().Retrieve(t).velocity), 2))
									+ (0.5f * b->GetMass() * pow(PhysMaths::Magnitude(b->GetTimeKeeper().Retrieve(t).ang_velocity), 2)));
				}
				yAxes.push_back(std::make_tuple(("Kinetic energy of " + b->GetName() + "(J)"), std::move(kinY)));
			}
			if (gp_energy) {
				std::vector<float> gravY;
				gravY.reserve(timeVals.size());
				for (float t : timeVals) {
					gravY.push_back(b->GetMass() * 9.81f *  b->GetTimeKeeper().Retrieve(t).position.y);
				}
				yAxes.push_back(std::make_tuple(("Relative GPE of " + b->GetName() + "(J)"), std::move(gravY)));
			}
			ImGui::TreePop();
		}
		bI++;
	}

	if (ImPlot::BeginPlot("Graph 1")) {
		ImPlot::SetupAxes("Time(s)", "Quantity(units)");
		ImPlot::SetNextMarkerStyle(ImPlotMarker_None);
		for (const auto& yVals : yAxes) {
			ImPlot::PlotLine(std::get<0>(yVals).c_str(), timeVals.data(), std::get<1>(yVals).data(), timeVals.size());
		}
		ImPlot::EndPlot();
	}

	ImGui::End();
}

// Renders a frame of all the models (and all UI widgets) on screen
void Sample3DSceneRenderer::Render() {
	if (!data_obtained) return library->Render();

	ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame();

	if (ImGui::BeginMainMenuBar()) {
		if (ImGui::BeginMenu("PhysicsCanvas")) {
			if (ImGui::MenuItem("Return to library")) {
				data_obtained = false;
				library->Refresh();
			}
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("File")) {
			std::wstring w_current(currentFile->Begin());
			ImGui::Text("Current project:");
			ImGui::Text(std::string(w_current.begin(), w_current.end()).c_str());
			if (ImGui::MenuItem("Save to file"))
				SaveToFile();
			ImGui::EndMenu();
		}
		if (ImGui::BeginMenu("Edit")) {
			if (ImGui::BeginMenu("Add kinematic body")) {
				if (ImGui::MenuItem("Cube"))
					CreateNewMesh(CUBE);
				if (ImGui::MenuItem("Sphere"))
					CreateNewMesh(SPHERE);
				ImGui::EndMenu();
			}
			if (ImGui::BeginMenu("Add quantum body")) {
				ImGui::Text("Coming soon!");
				ImGui::EndMenu();
			}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}

	XMMATRIX viewMat = XMMatrixLookAtRH(
		XMLoadFloat3(&controller->get_Position()), XMLoadFloat3(&controller->get_LookPoint()), up);

	for (std::shared_ptr<PhysicsBody> body : pBodies) {
		body->Render(viewMat * projectionMat);
	}
	
	ImGui::SetNextWindowPos(ImVec2(12, 60));
	ImGui::SetNextWindowSize(ImVec2(170, 50 * (pBodies.size() + 1) > 120? 120 : 50 * (pBodies.size() + 1)));
	ImGui::Begin("All objects");
	int i = 0;
	for (std::shared_ptr<PhysicsBody> b : pBodies) {
		if (i > 0) {
			if (ImGui::Button(b->GetName() != "" ? b->GetName().c_str() : "##Empty label")) {
				selectedBody = b;
			}
		}
		i++;
	}
	if (i == 1)
		ImGui::TextWrapped("Go to Edit to add a new object");
	ImGui::End();

	ImGui::SetNextWindowPos(ImVec2(12, m_deviceResources->GetOutputSize().Height * 0.68f));
	ImGui::SetNextWindowSize(ImVec2(m_deviceResources->GetOutputSize().Width * 0.95f, m_deviceResources->GetOutputSize().Height * 0.275f));
	TimeManager();

	if (selectedBody != nullptr) {
		ObjectManager();

		for (Force& f : selectedBody->ActiveForces(u_Time)) {
			ArrowMesh arr;
			arr.Create(m_deviceResources, f.GetColour());
			XMFLOAT3 rot(0,0,0);
			rot.x = atanf(f.GetDirection().y / PhysMaths::Magnitude(XMFLOAT3(f.GetDirection().x, 0, f.GetDirection().z)));
			rot.y = f.GetDirection().x == 0 && f.GetDirection().z == 0? 0
				: acosf(PhysMaths::Float3Dot(XMFLOAT3(f.GetDirection().x, 0, f.GetDirection().z), XMFLOAT3(0,0,1))
					/ PhysMaths::Magnitude(XMFLOAT3(f.GetDirection().x, 0, f.GetDirection().z)));
			arr.SetWorldMat(f.GetFrom(), rot, 0.01f * f.Magnitude());
			arr.Render(viewMat * projectionMat);
			arr.ReleaseResources();
		}
	}

	if (is_graphing) {
		GraphPlotter();
	}
	
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
	
}

void Sample3DSceneRenderer::CreateNewMesh(const UINT shape) {
	PhysicsBody nbody;
	nbody.Create(shape, m_deviceResources);
	std::string name;
	switch (shape) {
	case CUBE:
		name = "CUBE";
		break;
	case SPHERE:
		name = "SPHERE";
		break;
	}
	bool nameExists = false;
	for (std::shared_ptr<PhysicsBody> b : pBodies) {
		if (name == b->GetName()) {
			nameExists = true;
			break;
		}
	}
	if (!nameExists)
		nbody.GiveName(name);
	else {
		name = name + std::to_string(rand() % 100);
		nbody.GiveName(name);
	}
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
				TimeWipe();
				return;
			}
		}	
	}
	nbody.ApplyTranslation(newpos);
	for (std::shared_ptr<PhysicsBody> body : pBodies) {
		if(BoundingShape::IsColliding(nbody.GetBounds(), body->GetBounds())) {
			std::vector<XMFLOAT3> translations = BoundingShape::ResolveCollisions(nbody.GetBounds(), body->GetBounds());
			newpos = { newpos.x + translations[0].x - translations[1].x,
					abs(newpos.y + translations[0].y - translations[1].y),
					newpos.z + translations[0].z - translations[1].z };
			nbody.SetTransform(newpos, nbody.GetRotation(), nbody.GetDimensions());
		}
	}
	std::shared_ptr<PhysicsBody> nbodyPointer = std::make_shared<PhysicsBody>(nbody);
	selectedBody = nbodyPointer;
	pBodies.push_back(nbodyPointer);
	TimeWipe();
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
	if (!data_obtained) return library->CreateDeviceDependentResources();

	//create the floor of the world
	PhysicsBody floor;
	floor.Create(FLOOR, m_deviceResources);
	floor.GiveName("FLOOR");
	pBodies.push_front(std::make_shared<PhysicsBody>(floor));
}

void Sample3DSceneRenderer::ReleaseDeviceDependentResources() {
	for each (std::shared_ptr<PhysicsBody> body in pBodies) {
		body->ReleaseResources();
	}
	ImPlot::DestroyContext();
	ImGui::DestroyContext();
	ImGui_ImplDX11_Shutdown();
}