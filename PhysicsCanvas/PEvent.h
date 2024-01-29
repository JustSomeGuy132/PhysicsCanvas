#pragma once


namespace PhysicsCanvas {
	class PEvent {
	public:
		PEvent() : toggle(true) {}
		static enum eventType {
			Force,
		};

		virtual void SetStart(float start) { 
			if (start >= 0) 
				startT = start;
			if (startT > endT)
				endT = startT + 1.0f;
		}
		virtual void SetEnd(float end) { if (end > startT) endT = end; }

		virtual float GetStart() { return startT; }
		virtual float GetEnd() { return endT; }

		virtual std::string GetId() const { return Id; }
		virtual void SetId(std::string id) { Id = id; }

		virtual bool GetToggle() { return toggle; }
		virtual void SetToggle(bool newToggle) { toggle = newToggle; }

		virtual void SetEventType(eventType new_type) {
			eType = new_type;
		}

		virtual eventType GetEventType() { return eType; }
	private:
		eventType eType;
		float startT = 0.0f;
		float endT = 1.0f;
		std::string Id;
		bool toggle;
	};
}