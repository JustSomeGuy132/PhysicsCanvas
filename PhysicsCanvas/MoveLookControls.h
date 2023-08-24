#pragma once

#include "pch.h"
#include "..\Common\DeviceResources.h"
#include "..\Common\StepTimer.h"
#include "..\Common\DirectXHelper.h"

namespace PhysicsCanvas {
	ref class MoveLookControls {
	public:
		void OnPointerPressed(
			Windows::UI::Core::CoreWindow^ sender,
			Windows::UI::Core::PointerEventArgs^ args);
		void OnPointerMoved(
			Windows::UI::Core::CoreWindow^ sender,
			Windows::UI::Core::PointerEventArgs^ args
		);

		void OnPointerReleased(
			Windows::UI::Core::CoreWindow^ sender,
			Windows::UI::Core::PointerEventArgs^ args
		);

		void OnKeyDown(
			Windows::UI::Core::CoreWindow^ sender,
			Windows::UI::Core::KeyEventArgs^ args
		);

		void OnKeyUp(
			Windows::UI::Core::CoreWindow^ sender,
			Windows::UI::Core::KeyEventArgs^ args
		);

		// Set up the Controls that this controller supports
		void Initialize(Windows::UI::Core::CoreWindow^ window);

		void Update(Windows::UI::Core::CoreWindow^ window);
	internal:
		// Accessor to set position of controller
		void SetPosition(DirectX::XMFLOAT3 pos);

		// Accessor to set position of controller
		void SetOrientation(float pitch, float yaw);

		// Returns the position of the controller object
		DirectX::XMFLOAT3 get_Position();

		// Returns the point  which the controller is facing
		DirectX::XMFLOAT3 get_LookPoint();
	private:
		// Properties of the controller object
		DirectX::XMFLOAT3 m_position;               // The position of the controller
		float m_pitch, m_yaw;           // Orientation euler angles in radians

		DirectX::XMFLOAT3 m_moveCommand;            // The net command from the move control

		// Properties of the Look control
		bool m_lookInUse;               // Specifies whether the look control is in use
		uint32 m_lookPointerID;         // Id of the pointer in this control
		DirectX::XMFLOAT2 m_lookLastPoint;          // Last point (from last frame)
		DirectX::XMFLOAT2 m_lookLastDelta;          // For smoothing

		bool m_forward, m_back;         // States for movement
		bool m_left, m_right;
		bool m_up, m_down;
	};
}