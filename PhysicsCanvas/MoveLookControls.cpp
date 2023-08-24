#include "pch.h"
#include "MoveLookControls.h"
#include "..\Common\DirectXHelper.h"
#include <iostream>
#pragma lib(lib, "user32.lib")

using namespace PhysicsCanvas;
using namespace Windows::Foundation;
//using namespace concurrency;
//using namespace Windows::ApplicationModel;
//using namespace Windows::ApplicationModel::Core;
//using namespace Windows::ApplicationModel::Activation;
//using namespace Windows::UI::Core;
//using namespace Windows::UI::Input;
//using namespace Windows::System;
//using namespace Windows::Foundation;
//using namespace Windows::Graphics::Display;

namespace wnUIc = Windows::UI::Core;

void MoveLookControls::Initialize(wnUIc::CoreWindow^ window) {
	window->KeyDown +=
		ref new TypedEventHandler<wnUIc::CoreWindow^, wnUIc::KeyEventArgs^>(this, &MoveLookControls::OnKeyDown);
	window->KeyUp +=
		ref new TypedEventHandler<wnUIc::CoreWindow^, wnUIc::KeyEventArgs^>(this, &MoveLookControls::OnKeyUp);
	window->PointerPressed +=
		ref new TypedEventHandler<wnUIc::CoreWindow^, wnUIc::PointerEventArgs^>(this, &MoveLookControls::OnPointerPressed);
	window->PointerMoved +=
		ref new TypedEventHandler<wnUIc::CoreWindow^, wnUIc::PointerEventArgs^>(this, &MoveLookControls::OnPointerMoved);
	window->PointerReleased +=
		ref new TypedEventHandler<wnUIc::CoreWindow^, wnUIc::PointerEventArgs^>(this, &MoveLookControls::OnPointerReleased);

	m_lookInUse = false;
	m_lookPointerID = 0;

	m_moveCommand = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);

	SetOrientation(0, 0);
}

void MoveLookControls::Update(wnUIc::CoreWindow^ window) {
	//compute final direction of movement
	if (m_forward) m_moveCommand.y += 1.0f;
	if (m_back) m_moveCommand.y -= 1.0f;

	if (m_left) m_moveCommand.x -= 1.0f;
	if (m_right) m_moveCommand.x += 1.0f;

	if (m_up) m_moveCommand.z += 1.0f;
	if (m_down) m_moveCommand.z -= 1.0f;

	// Make sure that 45 degree cases are not faster.
	DirectX::XMFLOAT3 command = m_moveCommand;
	DirectX::XMVECTOR vector;
	vector = DirectX::XMLoadFloat3(&command);

	if (fabsf(command.x) > 0.1f || fabsf(command.y) > 0.1f || fabsf(command.z) > 0.1f)
	{
		vector = DirectX::XMVector3Normalize(vector);
		DirectX::XMStoreFloat3(&command, vector);
	}


	// Rotate command to align with our direction (world coordinates).
	DirectX::XMFLOAT3 wCommand;
	wCommand.x = command.x * cosf(m_yaw) - command.y * sinf(m_yaw);
	wCommand.y = command.x * sinf(m_yaw) + command.y * cosf(m_yaw);
	wCommand.z = command.z;

	// Our velocity is based on the command.
	// Also note that y is the up-down axis. 
	DirectX::XMFLOAT3 Velocity;
	Velocity.x = -wCommand.x;
	Velocity.z = wCommand.y;
	Velocity.y = wCommand.z;

	// Integrate
	m_position.x += Velocity.x / 10.0f;
	m_position.y += Velocity.y / 10.0f;
	m_position.z += Velocity.z / 10.0f;

	// Clear movement input accumulator for use during the next frame.
	m_moveCommand = DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f);
}

void MoveLookControls::OnKeyDown(wnUIc::CoreWindow^ sender, wnUIc::KeyEventArgs^ args) {

	//detect which key was pressed
	Windows::System::VirtualKey key;
	key = args->VirtualKey;
	//handle the keypress
	if(m_lookInUse) {
		if (key == Windows::System::VirtualKey::W) {
			m_forward = true;
		}
		if (key == Windows::System::VirtualKey::A) {
			m_left = true;
		}
		if (key == Windows::System::VirtualKey::S) {
			m_back = true;
		}
		if (key == Windows::System::VirtualKey::D) {
			m_right = true;
		}
		if (key == Windows::System::VirtualKey::Q) {
			m_down = true;
		}
		if (key == Windows::System::VirtualKey::E) {
			m_up = true;
		}
	}
}

void MoveLookControls::OnKeyUp(wnUIc::CoreWindow^ sender, wnUIc::KeyEventArgs^ args) {

	//detect which key was released
	Windows::System::VirtualKey key;
	key = args->VirtualKey;
	//handle the key release
	if (key == Windows::System::VirtualKey::W) {
		m_forward = false;
	}
	if (key == Windows::System::VirtualKey::A) {
		m_left = false;
	}
	if (key == Windows::System::VirtualKey::S) {
		m_back = false;
	}
	if (key == Windows::System::VirtualKey::D) {
		m_right = false;
	}
	if (key == Windows::System::VirtualKey::Q) {
		m_down = false;
	}
	if (key == Windows::System::VirtualKey::E) {
		m_up = false;
	}
}

void MoveLookControls::OnPointerPressed(wnUIc::CoreWindow^ sender, wnUIc::PointerEventArgs^ args) {

	uint32 pointerID = args->CurrentPoint->PointerId;
	DirectX::XMFLOAT2 position = DirectX::XMFLOAT2(args->CurrentPoint->Position.X, args->CurrentPoint->Position.Y);

	auto device = args->CurrentPoint->PointerDevice->PointerDeviceType;
	if (device == Windows::Devices::Input::PointerDeviceType::Mouse) {
		//handle mouse press
		m_lookInUse = true;
		m_lookLastPoint = position;
		m_lookPointerID = args->CurrentPoint->PointerId;
		m_lookLastDelta.x = m_lookLastDelta.y = 0;
	}
}

void MoveLookControls::OnPointerMoved(wnUIc::CoreWindow^ sender, wnUIc::PointerEventArgs^ args) {
	uint32 pointerID = args->CurrentPoint->PointerId;
	DirectX::XMFLOAT2 position = DirectX::XMFLOAT2(args->CurrentPoint->Position.X, args->CurrentPoint->Position.Y);
	if (pointerID == m_lookPointerID) {
		//looking around controls
		DirectX::XMFLOAT2 pointerDelta;
		pointerDelta.x = position.x - m_lookLastPoint.x;        // How far did pointer move
		pointerDelta.y = position.y - m_lookLastPoint.y;

		DirectX::XMFLOAT2 rotationDelta;
		rotationDelta.x = pointerDelta.x * 0.004f;   // Scale for control sensitivity.
		rotationDelta.y = pointerDelta.y * 0.004f;

		m_lookLastPoint = position;                     // Save for the next time through.

		// Update our orientation based on the command.
		m_pitch -= rotationDelta.y;                     // Mouse y increases down, but pitch increases up.
		m_yaw -= rotationDelta.x;                       // Yaw is defined as CCW around the y-axis.

		// Limit the pitch to straight up or straight down.
		m_pitch = (float)__max(-DirectX::XM_PI / 2.0f, m_pitch);
		m_pitch = (float)__min(+DirectX::XM_PI / 2.0f, m_pitch);
	}
}

void MoveLookControls::OnPointerReleased(wnUIc::CoreWindow^ sender, wnUIc::PointerEventArgs^ args) {
	uint32 pointerID = args->CurrentPoint->PointerId;
	DirectX::XMFLOAT2 position = DirectX::XMFLOAT2(args->CurrentPoint->Position.X, args->CurrentPoint->Position.Y);
	//stop looking around
	m_lookInUse = false;
	m_lookPointerID = 0;
}

void MoveLookControls::SetPosition(DirectX::XMFLOAT3 pos)
{
	m_position = pos;
}

// Accessor to set the position of the controller.
void MoveLookControls::SetOrientation(float pitch, float yaw)
{
	m_pitch = pitch;
	m_yaw = yaw;
}

// Returns the position of the controller object.
DirectX::XMFLOAT3 MoveLookControls::get_Position()
{
	return m_position;
}

// Returns the point at which the camera controller is facing.
DirectX::XMFLOAT3 MoveLookControls::get_LookPoint()
{
	float y = sinf(m_pitch);        // Vertical
	float r = cosf(m_pitch);        // In the plane
	float z = r * cosf(m_yaw);        // Fwd-back
	float x = r * sinf(m_yaw);        // Left-right
	DirectX::XMFLOAT3 result(x, y, z);
	result.x += m_position.x;
	result.y += m_position.y;
	result.z += m_position.z;

	// Return m_position + DirectX::XMFLOAT3(x, y, z);
	return result;
}