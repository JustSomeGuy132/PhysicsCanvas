#pragma once
#include "pch.h"
#include "Common/DeviceResources.h"
#include "Common/DirectXHelper.h"
#include "MoveLookControls.h"
#include "imgui/imgui.h"
#include "imgui/imgui_impl_dx11.h"

namespace PhysicsCanvas {
	class ProjectLib {
	public:
		ProjectLib(const std::shared_ptr<DX::DeviceResources>& deviceResources);
		~ProjectLib() {};
		void CreateWindowSizeDependentResources();
		void CreateDeviceDependentResources();
		void Refresh();
		Platform::String^ ChosenFile();
		Platform::String^ ChosenPreset();
		void Render();
		void NewFileWindow();
		void PresetSelectorWindow();

		Windows::Storage::StorageFolder^ getLocalFolder() { return localFolder; }

	private:
		Windows::Storage::StorageFolder^ localFolder;
		std::shared_ptr<DX::DeviceResources> m_deviceResources;
		std::vector<std::string> existingProjectPaths;
		std::string chosenPath;
		std::string presetPath;
		bool newproj_window = false, preset_window = false;

	};
}