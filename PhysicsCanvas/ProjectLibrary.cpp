#include "pch.h"
#include "ProjectLibrary.h"
#include <fstream>
#include <sstream>

using namespace PhysicsCanvas;

ProjectLib::ProjectLib(const std::shared_ptr<DX::DeviceResources>& deviceResources) : m_deviceResources(deviceResources), chosenPath("")
{
	input = ref new MoveLookControls();
	input->Initialize(Windows::UI::Core::CoreWindow::GetForCurrentThread());

	(void) std::ofstream("proj_dirs.txt", std::ios::app); //if the file does not already exist, this will create it

	std::fstream file; //the file storing all the project directories
	file.open("proj_dirs.txt", std::ios::in);

	std::ostringstream oss;
	if (file.is_open()) {
		char c;
		while (file) {
			c = file.get();
			oss << c;
		}
	}
	std::vector<std::string> lines;
	std::istringstream iss(oss.str());
	while (iss) {
		std::string l;
		std::getline(iss, l);
		lines.push_back(l);
	}


	file.close();
}

std::string ProjectLib::LoopAndReturnFileData() {
	while (chosenPath == "") {
		Render();
		m_deviceResources->Present();
	}
	std::fstream file;
	file.open(chosenPath, std::ios::out);
	std::ostringstream oss;
	if (file.is_open()) {
		char c;
		while (file) {
			c = file.get();
			oss << c;
		}
		return oss.str();
	}
	else {
		MessageBox(NULL, "File could not be retrieved and therefore could not be loaded", "File loading error", MB_ICONWARNING | MB_OK);
		return "";
	}
}

Concurrency::task<Windows::Storage::StorageFile^> ProjectLib::FilePicker() {
	// FILE PICKER, FOR SELECTING A SAVE FILE
	Windows::Storage::Pickers::FileOpenPicker^ filePicker = ref new Windows::Storage::Pickers::FileOpenPicker;

	// ARRAY OF FILE TYPES
	Platform::Array<Platform::String^>^ fileTypes = ref new Platform::Array<Platform::String^>(1);
	fileTypes->Data[0] = ".psim";

	filePicker->ViewMode = Windows::Storage::Pickers::PickerViewMode::Thumbnail;
	filePicker->SuggestedStartLocation = Windows::Storage::Pickers::PickerLocationId::Desktop;
	filePicker->FileTypeFilter->ReplaceAll(fileTypes);

	// THIS SHOULD HOPEFULLY LET US PICK A FILE
	auto fileChoose = filePicker->PickSingleFileAsync();
	return Concurrency::create_task(fileChoose);
}

void ProjectLib::Render() {
	ImGui::NewFrame();
	ImGui_ImplDX11_NewFrame();
	ImGui::SetNextWindowPos(ImVec2());
	ImGui::SetNextWindowSize(ImVec2(m_deviceResources->GetOutputSize().Width, m_deviceResources->GetOutputSize().Height));
	ImGui::Begin("##Project library");



	ImGui::End();
	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}