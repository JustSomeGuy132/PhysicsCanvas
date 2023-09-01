#pragma once


namespace PhysicsCanvas {
	class PEvent {
	public:
		PEvent() {}

		void SetStart(float start) { startT = start; }
		void SetEnd(float end) { endT = end; }

		float GetStart() { return startT; }
		float GetEnd() { return endT; }

		std::string GetId() const { return Id; }
		void SetId(std::string id) { Id = id; }

	private:
		float startT = 0.0f;
		float endT = 1.0f;
		std::string Id;
	};
}