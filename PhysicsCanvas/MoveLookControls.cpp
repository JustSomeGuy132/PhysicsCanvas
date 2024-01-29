#include "pch.h"
#include "MoveLookControls.h"
#include "..\Common\DirectXHelper.h"
#include <sstream>

using namespace PhysicsCanvas;
using namespace Windows::Foundation;

namespace wnUIc = Windows::UI::Core;
namespace wnIN = Windows::Devices::Input;

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

	right_pressed = false;
	left_pressed = false;

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
	Windows::System::VirtualKey key;
	key = args->VirtualKey;
	if (ImGui::GetIO().WantCaptureKeyboard) {
		ImGuiKey imKey = VK_To_ImGuiKey(key);
		ImGui::GetIO().AddKeyEvent(imKey, true);
		if (ImGui::GetIO().WantTextInput) {
			wchar_t wch = 0;
			::MultiByteToWideChar(CP_ACP, MB_PRECOMPOSED, (char*)&key, 1, &wch, 1);
			ImGui::GetIO().AddInputCharacter(wch);
		}
		return;
	}
	//handle the keypress for when user is looking around the world
	if (m_lookInUse) {
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

ImGuiKey MoveLookControls::VK_To_ImGuiKey(Windows::System::VirtualKey key) {
	int keyInt = static_cast<int>(key);
	switch (keyInt)
	{
	case VK_TAB: return ImGuiKey_Tab;
	case VK_LEFT: return ImGuiKey_LeftArrow;
	case VK_RIGHT: return ImGuiKey_RightArrow;
	case VK_UP: return ImGuiKey_UpArrow;
	case VK_DOWN: return ImGuiKey_DownArrow;
	case VK_PRIOR: return ImGuiKey_PageUp;
	case VK_NEXT: return ImGuiKey_PageDown;
	case VK_HOME: return ImGuiKey_Home;
	case VK_END: return ImGuiKey_End;
	case VK_INSERT: return ImGuiKey_Insert;
	case VK_DELETE: return ImGuiKey_Delete;
	case VK_BACK: return ImGuiKey_Backspace;
	case VK_SPACE: return ImGuiKey_Space;
	case VK_RETURN: return ImGuiKey_Enter;
	case VK_ESCAPE: return ImGuiKey_Escape;
	case VK_OEM_7: return ImGuiKey_Apostrophe;
	case VK_OEM_COMMA: return ImGuiKey_Comma;
	case VK_OEM_MINUS: return ImGuiKey_Minus;
	case VK_OEM_PERIOD: return ImGuiKey_Period;
	case VK_OEM_2: return ImGuiKey_Slash;
	case VK_OEM_1: return ImGuiKey_Semicolon;
	case VK_OEM_PLUS: return ImGuiKey_Equal;
	case VK_OEM_4: return ImGuiKey_LeftBracket;
	case VK_OEM_5: return ImGuiKey_Backslash;
	case VK_OEM_6: return ImGuiKey_RightBracket;
	case VK_OEM_3: return ImGuiKey_GraveAccent;
	case VK_CAPITAL: return ImGuiKey_CapsLock;
	case VK_SCROLL: return ImGuiKey_ScrollLock;
	case VK_NUMLOCK: return ImGuiKey_NumLock;
	case VK_SNAPSHOT: return ImGuiKey_PrintScreen;
	case VK_PAUSE: return ImGuiKey_Pause;
	case VK_NUMPAD0: return ImGuiKey_Keypad0;
	case VK_NUMPAD1: return ImGuiKey_Keypad1;
	case VK_NUMPAD2: return ImGuiKey_Keypad2;
	case VK_NUMPAD3: return ImGuiKey_Keypad3;
	case VK_NUMPAD4: return ImGuiKey_Keypad4;
	case VK_NUMPAD5: return ImGuiKey_Keypad5;
	case VK_NUMPAD6: return ImGuiKey_Keypad6;
	case VK_NUMPAD7: return ImGuiKey_Keypad7;
	case VK_NUMPAD8: return ImGuiKey_Keypad8;
	case VK_NUMPAD9: return ImGuiKey_Keypad9;
	case VK_DECIMAL: return ImGuiKey_KeypadDecimal;
	case VK_DIVIDE: return ImGuiKey_KeypadDivide;
	case VK_MULTIPLY: return ImGuiKey_KeypadMultiply;
	case VK_SUBTRACT: return ImGuiKey_KeypadSubtract;
	case VK_ADD: return ImGuiKey_KeypadAdd;
	case VK_LSHIFT: return ImGuiKey_LeftShift;
	case VK_LCONTROL: return ImGuiKey_LeftCtrl;
	case VK_LMENU: return ImGuiKey_LeftAlt;
	case VK_LWIN: return ImGuiKey_LeftSuper;
	case VK_RSHIFT: return ImGuiKey_RightShift;
	case VK_RCONTROL: return ImGuiKey_RightCtrl;
	case VK_RMENU: return ImGuiKey_RightAlt;
	case VK_RWIN: return ImGuiKey_RightSuper;
	case VK_APPS: return ImGuiKey_Menu;
	case '0': return ImGuiKey_0;
	case '1': return ImGuiKey_1;
	case '2': return ImGuiKey_2;
	case '3': return ImGuiKey_3;
	case '4': return ImGuiKey_4;
	case '5': return ImGuiKey_5;
	case '6': return ImGuiKey_6;
	case '7': return ImGuiKey_7;
	case '8': return ImGuiKey_8;
	case '9': return ImGuiKey_9;
	case 'A': return ImGuiKey_A;
	case 'B': return ImGuiKey_B;
	case 'C': return ImGuiKey_C;
	case 'D': return ImGuiKey_D;
	case 'E': return ImGuiKey_E;
	case 'F': return ImGuiKey_F;
	case 'G': return ImGuiKey_G;
	case 'H': return ImGuiKey_H;
	case 'I': return ImGuiKey_I;
	case 'J': return ImGuiKey_J;
	case 'K': return ImGuiKey_K;
	case 'L': return ImGuiKey_L;
	case 'M': return ImGuiKey_M;
	case 'N': return ImGuiKey_N;
	case 'O': return ImGuiKey_O;
	case 'P': return ImGuiKey_P;
	case 'Q': return ImGuiKey_Q;
	case 'R': return ImGuiKey_R;
	case 'S': return ImGuiKey_S;
	case 'T': return ImGuiKey_T;
	case 'U': return ImGuiKey_U;
	case 'V': return ImGuiKey_V;
	case 'W': return ImGuiKey_W;
	case 'X': return ImGuiKey_X;
	case 'Y': return ImGuiKey_Y;
	case 'Z': return ImGuiKey_Z;
	case VK_F1: return ImGuiKey_F1;
	case VK_F2: return ImGuiKey_F2;
	case VK_F3: return ImGuiKey_F3;
	case VK_F4: return ImGuiKey_F4;
	case VK_F5: return ImGuiKey_F5;
	case VK_F6: return ImGuiKey_F6;
	case VK_F7: return ImGuiKey_F7;
	case VK_F8: return ImGuiKey_F8;
	case VK_F9: return ImGuiKey_F9;
	case VK_F10: return ImGuiKey_F10;
	case VK_F11: return ImGuiKey_F11;
	case VK_F12: return ImGuiKey_F12;
	default: return ImGuiKey_None;
	}
}

void MoveLookControls::OnKeyUp(wnUIc::CoreWindow^ sender, wnUIc::KeyEventArgs^ args) {

	//detect which key was released
	Windows::System::VirtualKey key;
	key = args->VirtualKey;

	ImGuiKey imKey = VK_To_ImGuiKey(key);
	ImGui::GetIO().AddKeyEvent(imKey, false);
	
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

	right_pressed = args->CurrentPoint->Properties->IsRightButtonPressed;
	left_pressed = args->CurrentPoint->Properties->IsLeftButtonPressed;

	if (ImGui::GetIO().WantCaptureMouse) {
		ImGui::GetIO().AddMouseButtonEvent(right_pressed, true);	//if right was pressed, it passes in true = 1 = code for RMB 
		return;														//else pass in false = 0 = code for LMB, since only 1 can fire this event
	}

	if (right_pressed) {
		//handle mouse press
		m_lookInUse = true;
		m_lookLastPoint = position;
		m_lookPointerID = args->CurrentPoint->PointerId;
		m_lookLastDelta.x = m_lookLastDelta.y = 0;
	}
	else if (left_pressed) {
		clickCoords = { position.x * 1.254901961f, position.y * 1.254901961f };
	}
}

void MoveLookControls::OnPointerMoved(wnUIc::CoreWindow^ sender, wnUIc::PointerEventArgs^ args) {
	uint32 pointerID = args->CurrentPoint->PointerId;
	DirectX::XMFLOAT2 position = DirectX::XMFLOAT2(args->CurrentPoint->Position.X, args->CurrentPoint->Position.Y);
	ImGui::GetIO().AddMousePosEvent(position.x * 1.254901961f, position.y * 1.254901961f);

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

	bool isRightButton = !args->CurrentPoint->Properties->IsRightButtonPressed && right_pressed;	//right was pressed but is no longer

	bool isLeftButton = !args->CurrentPoint->Properties->IsLeftButtonPressed && left_pressed;

	if (isRightButton)
		right_pressed = false;

	if (isLeftButton)
		left_pressed = false;

	if (ImGui::GetIO().WantCaptureMouse) {
		if (isRightButton)
			ImGui::GetIO().AddMouseButtonEvent(1, false);
		else if (isLeftButton)
			ImGui::GetIO().AddMouseButtonEvent(0, false);
		return;
	}
	if (isRightButton) {
		//stop looking around
		m_lookInUse = false;
		m_lookPointerID = 0;
	}
	else if (isLeftButton) {
		if (clickCoords.x != -2 && clickCoords.y != -2) {
			clickCoords = { -2, -2 };
		}
	}
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