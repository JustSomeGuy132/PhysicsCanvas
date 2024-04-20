#include "pch.h"
#include "ProjectLibrary.h"
#include <fstream>
#include <sstream>
#include <filesystem>
#include <windows.foundation.h>

using namespace PhysicsCanvas;
using namespace concurrency;

ProjectLib::ProjectLib(const std::shared_ptr<DX::DeviceResources>& deviceResources) : m_deviceResources(deviceResources), chosenPath("")
{
	CreateWindowSizeDependentResources();
	CreateDeviceDependentResources();
}

void ProjectLib::CreateWindowSizeDependentResources() {
	ImGuiIO& io = ImGui::GetIO();
	io.DisplaySize = { m_deviceResources->GetOutputSize().Width, m_deviceResources->GetOutputSize().Height };
}
void ProjectLib::CreateDeviceDependentResources() {
	// Open or create the file for reading and writing (or open it if it already exists)
	Windows::Storage::StorageFolder^ localFolder = Windows::Storage::ApplicationData::Current->LocalFolder;
	create_task(localFolder->GetFilesAsync())
	.then([&](Windows::Foundation::Collections::IVectorView<Windows::Storage::StorageFile^>^ files) {
		for (int i = 0; i < files->Size; i++) {
			std::wstring Wtitle(files->GetAt(i)->Name->Begin());
			std::string title(Wtitle.begin(), Wtitle.end());
			bool already_exists = false;
			for (std::string s : existingProjectPaths)
				if (s == title)
					already_exists = true;
			if (!already_exists)
				existingProjectPaths.push_back(title);
		}
	});
}

Platform::String^ ProjectLib::ChosenFile() {
	std::wstring wchosen(chosenPath.begin(), chosenPath.end());
	return ref new Platform::String(wchosen.c_str());
}
Platform::String^ ProjectLib::ChosenPreset() {
	std::wstring wpreset(presetPath.begin(), presetPath.end());
	return ref new Platform::String(wpreset.c_str());
}

void ProjectLib::Refresh() {
	chosenPath = "";
	presetPath = "";
	existingProjectPaths.clear();
	newproj_window = preset_window = false;
	CreateWindowSizeDependentResources();
	CreateDeviceDependentResources();
}

void ProjectLib::Render() {
	ImGui_ImplDX11_NewFrame();
	ImGui::NewFrame();
	//Initialise this window to take up the entire window display
	ImGui::SetNextWindowPos(ImVec2());
	ImGui::SetNextWindowSize(ImVec2(m_deviceResources->GetOutputSize().Width, m_deviceResources->GetOutputSize().Height));
	ImGui::Begin("Project library", NULL, ImGuiWindowFlags_NoCollapse);
	//Buttons on the top navigation bar
	if (ImGui::Button("+")) 
		newproj_window = true;	ImGui::SameLine();
	ImGui::Button("Catalogue"); ImGui::SameLine();
	ImGui::Button("Info");		ImGui::SameLine();
	ImGui::Button("Settings");
	//Buttons to go to existing projects registered
	int count = 0;
	if (ImGui::Button("New simulation\nproject", ImVec2(120, 120)))
		newproj_window = true;

	ImGui::SameLine(); count++;
	for (std::string proj : existingProjectPaths) {
		if (ImGui::Button(proj.c_str(), ImVec2(120, 120)))
			chosenPath = proj;
		//Ensure that there are up to 5 buttons per row
		count++;
		if (count < ImGui::GetWindowSize().x / 120) {
			ImGui::SameLine();
		}
		else count = 0;
	}

	ImGui::End();
	
	if (newproj_window)
		NewFileWindow();

	ImGui::Render();
	ImGui_ImplDX11_RenderDrawData(ImGui::GetDrawData());
}

void ProjectLib::NewFileWindow() {
	ImGui::Begin("Creating new project", NULL, ImGuiWindowFlags_NoCollapse);
	//Make sure we're not forcing focus on both windows as this will
	//hinder them from doing anything on either!
	if (!preset_window) ImGui::SetWindowFocus();
	//If they don't want to create a project file
	if (ImGui::Button("Continue without creating a file"))
		chosenPath = "NONE";
	//Provide options for creating a project file
	static int e = 0;
	ImGui::RadioButton("Create empty", &e, 0); ImGui::SameLine();
	if (ImGui::RadioButton("Create from preset", &e, 1))
		preset_window = true;
	ImGui::SameLine();
	ImGui::TextDisabled("(?)");
	if (ImGui::BeginItemTooltip()) {
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 35.0f);
		ImGui::TextUnformatted("A 'preset' is referring to a prerequisite - a template, if you will - "
								"on which this new project will be based upon. It will be initialised with"
								"the data of the specified file but will be under a new project file.");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
	static char nameBuffer[128];
	std::filesystem::path currentPath = std::wstring(Windows::Storage::ApplicationData::Current->LocalFolder->Path->Begin());

	static std::filesystem::path directoryPath = currentPath / (std::string(nameBuffer) + ".psim");
	static std::string directoryTxt = directoryPath.string();

	ImGui::Text("Enter a title:"); ImGui::SameLine();
	if (ImGui::InputText("##Project_Name", nameBuffer, IM_ARRAYSIZE(nameBuffer)))
		directoryPath.replace_filename(std::string(nameBuffer) + ".psim");
	directoryTxt = directoryPath.string();
	ImGui::Text("Its path will be:");
	ImGui::TextUnformatted(directoryTxt.c_str());

	float width = ImGui::GetWindowSize().x;
	//Cancel button (closes window)
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
	if (ImGui::Button("Cancel")) newproj_window = false;
	ImGui::PopStyleColor(3);
	ImGui::SameLine(0, width * 0.85f);
	//OK button (proceeds to main program)
	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(2.0f / 7.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(2.0f / 7.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(2.0f / 7.0f, 0.8f, 0.8f));
	if (ImGui::Button("OK")) {
		if (e == 1 && presetPath == "")
			MessageBox(NULL, "Cannot proceed: preset data not selected.", "Cannot create simulation", MB_ICONERROR | MB_OK);
		else if (std::string(nameBuffer).length() == 0 || directoryTxt.size() == 0)
			MessageBox(NULL, "Cannot proceed: must enter a title.", "Cannot create simulation", MB_ICONERROR | MB_OK);
		else {
			Windows::Storage::StorageFolder^ localFolder = Windows::Storage::ApplicationData::Current->LocalFolder;
			create_task(localFolder->CreateFileAsync(ref new Platform::String(directoryPath.filename().wstring().c_str()), 
																					Windows::Storage::CreationCollisionOption::GenerateUniqueName)
			).then([&](Windows::Storage::StorageFile^ file) {
				chosenPath = directoryPath.filename().string();
				if (e == 0)
					presetPath = "";
			});
		}
	}
	ImGui::PopStyleColor(3);

	ImGui::End();

	if (preset_window)
		PresetSelectorWindow();
}

void ProjectLib::PresetSelectorWindow() {
	ImGui::Begin("Select preset", NULL, ImGuiWindowFlags_NoCollapse);
	ImGui::SetWindowFocus();
	ImGui::TextDisabled("(!!)");
	if (ImGui::BeginItemTooltip()) {
		ImGui::PushTextWrapPos(ImGui::GetFontSize() * 0.35f);
		ImGui::TextUnformatted("Your preset file MUST be in the local storage directory for the program. See the predicted path to find that folder.");
		ImGui::PopTextWrapPos();
		ImGui::EndTooltip();
	}
	if (ImGui::Button("Clear selection")) {
		presetPath = "";
	}
	ImGui::Separator();

	for (std::string s : existingProjectPaths) {
		std::wstring Ws = std::filesystem::path(s).filename();
		std::string presetName = std::string(Ws.begin(), Ws.end());
		presetName = presetName.substr(0, presetName.length() - 5); //To cut out the .psim at the end
		if (ImGui::Button(presetName.c_str())) {
			presetPath = s;
		}
	}
	std::string confirm_text;
	if (presetPath == "")
		confirm_text = "No preset selected";
	else {
		confirm_text = "Preset selected: ";
		std::wstring filename = std::filesystem::path(presetPath).filename();
		confirm_text += std::string(filename.begin(), filename.end());
	}
	ImGui::Text(confirm_text.c_str());

	ImGui::PushStyleColor(ImGuiCol_Button, (ImVec4)ImColor::HSV(0.0f, 0.6f, 0.6f));
	ImGui::PushStyleColor(ImGuiCol_ButtonHovered, (ImVec4)ImColor::HSV(0.0f, 0.7f, 0.7f));
	ImGui::PushStyleColor(ImGuiCol_ButtonActive, (ImVec4)ImColor::HSV(0.0f, 0.8f, 0.8f));
	if (ImGui::Button("Close this window"))
		preset_window = false;
	ImGui::PopStyleColor(3);

	ImGui::End();
}