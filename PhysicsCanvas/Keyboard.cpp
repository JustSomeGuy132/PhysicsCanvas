#include "pch.h"
#include "Keyboard.h"
#include <sstream>
#include <iostream>

using namespace PhysicsCanvas;

#pragma comment(lib, "user32.lib")

bool Keyboard::KeyIsPressed(unsigned char keycode) const noexcept {
	return keystates[keycode];
}

Keyboard::Event Keyboard::ReadKey() noexcept {
	if (keybuffer.size() > 0u) {
		Keyboard::Event e = keybuffer.front();
		keybuffer.pop();
		return e;		//dequeue the "keybuffer" queue if there are keys in the queue
	}
	else {
		return Keyboard::Event();
	}
}

bool Keyboard::KeyIsEmpty() const noexcept {
	return keybuffer.empty();
}

char Keyboard::ReadChar() noexcept {
	if (charbuffer.size() > 0u) {
		unsigned char charcode = charbuffer.front();
		charbuffer.pop();
		return charcode;	//dequeue the "charbuffer" queue if there are chars in it
	}
	else {
		return 0;
	}
}

bool Keyboard::CharIsEmpty() const noexcept {
	return charbuffer.empty();
}

void Keyboard::ClearKey() noexcept {
	keybuffer = std::queue<Event>();
}

void Keyboard::ClearChar() noexcept {
	charbuffer = std::queue<char>();
}

void Keyboard::Clear() noexcept {
	ClearKey(); ClearChar();
}

void Keyboard::EnableAutorepeat() noexcept {
	autorepeatEnabled = true;
}

void Keyboard::DisableAutorepeat() noexcept {
	autorepeatEnabled = false;
}

bool Keyboard::AutorepeatEnabled() const noexcept {
	return autorepeatEnabled;
}

void Keyboard::OnKeyPressed(unsigned char keycode) noexcept {
	keystates[keycode] = true;
	keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Press, keycode));
	TrimBuffer(keybuffer);
	std::ostringstream oss;
	oss << "Pressed the " << keycode << " key";
	MessageBox(nullptr, oss.str().c_str(), "Pressed a key!", MB_OK | MB_ICONINFORMATION);
}

void Keyboard::OnKeyReleased(unsigned char keycode) noexcept {
	keystates[keycode] = false;
	keybuffer.push(Keyboard::Event(Keyboard::Event::Type::Release, keycode));
	TrimBuffer(keybuffer);
}

void Keyboard::OnChar(char character) noexcept {
	charbuffer.push(character);
	TrimBuffer(charbuffer);
}

void Keyboard::ClearState() noexcept {
	keystates.reset();
}

template<typename T>
void Keyboard::TrimBuffer(std::queue<T>& buffer) noexcept {
	while (buffer.size() > bufferSize) {
		buffer.pop();
	}
}