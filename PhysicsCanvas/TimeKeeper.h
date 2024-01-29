#pragma once
#include "pch.h"
#include "..\Common\DirectXHelper.h"

namespace PhysicsCanvas {
	struct Record {
		float time;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 rotation;
		DirectX::XMFLOAT3 velocity;
		DirectX::XMFLOAT3 ang_velocity;
	};
	class TimeKeeper {
	public:
		void RecordData(float timestamp, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rot, DirectX::XMFLOAT3 vel, DirectX::XMFLOAT3 ang_vel) {
			Record data = { timestamp, pos, rot, vel, ang_vel };
			records.push_back(data);
		}
		Record Retrieve(float timestamp) {
			for (Record r : records) {
				if (r.time == timestamp) {
					return r;
				}
			}
		}
		void Overwrite(float timestamp, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rot, DirectX::XMFLOAT3 vel, DirectX::XMFLOAT3 ang_vel) {
			for (Record& r : records) {
				if (r.time == timestamp) {
					r = { r.time, pos, rot, vel, ang_vel };
				}
			}
		}
		void Wipe(float currentTime) {
			Record currentState = Retrieve(currentTime);
			currentState.time = 0;
			records.clear();
			records.push_back(currentState);
		}
	private:
		std::vector<Record> records;
	};
}