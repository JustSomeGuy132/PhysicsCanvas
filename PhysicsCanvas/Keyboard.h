#pragma once
#include <queue>
#include <bitset>

namespace PhysicsCanvas {
	class Keyboard {
		friend class PhysicsCanvasMain;  //allows Window class to access some methods
	public:
		class Event {
		public:
			enum class Type { Press, Release, Invalid };
		private:
			Type type;
			unsigned char code;
		public:
			Event() noexcept : type(Type::Invalid), code(0u) {}	//constructor if type is not passed in, inits to invalid
			Event(Type type, unsigned char code) noexcept : type(type), code(code) {}//constructor if type is passed in
			bool IsPress() const noexcept {
				return type == Type::Press;
			}
			bool IsRelease() const noexcept {
				return type == Type::Release;
			}
			bool IsValid() const noexcept {
				return type != Type::Invalid;
			}
		};
	public:
		Keyboard() = default;
		Keyboard(const Keyboard&) = delete;
		Keyboard& operator=(const Keyboard&) = delete;

		bool KeyIsPressed(unsigned char keycode) const noexcept;
		Event ReadKey() noexcept;
		bool KeyIsEmpty() const noexcept;
		void ClearKey() noexcept;  //empty the queue of keys "keybuffer"

		char ReadChar() noexcept;
		bool CharIsEmpty() const noexcept;
		void ClearChar() noexcept;  //empty the queue of chars "charbuffer"
		void Clear() noexcept;  //empty both queues

		void EnableAutorepeat() noexcept;
		void DisableAutorepeat() noexcept;
		bool AutorepeatEnabled() const noexcept;
	private:		//Window class needs these methods here
		void OnKeyPressed(unsigned char keycode) noexcept;
		void OnKeyReleased(unsigned char keycode) noexcept;
		void OnChar(char character) noexcept;
		void ClearState() noexcept;  //clears the "keystates" bitset
		template<typename T>
		static void TrimBuffer(std::queue<T>& buffer) noexcept;  //ensures the buffer of 16u does not go over
	private:
		static constexpr unsigned int nKeys = 256u;  //because keycodes don't exceed 1 byte
		static constexpr unsigned int bufferSize = 16u;
		bool autorepeatEnabled = false;
		std::bitset<nKeys> keystates;  //allows to index the keystates in nKeys as single bits
		std::queue<Event> keybuffer;
		std::queue<char> charbuffer;
	};
}