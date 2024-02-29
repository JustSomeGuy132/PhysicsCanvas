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
		std::string LoopAndReturnFileData();
		void Render();
		Concurrency::task<Windows::Storage::StorageFile^> FilePicker();
	private:
		std::shared_ptr<DX::DeviceResources> m_deviceResources;
		std::vector<std::string> existingProjectPaths;
		std::string chosenPath;

		MoveLookControls^ input;
	};
}