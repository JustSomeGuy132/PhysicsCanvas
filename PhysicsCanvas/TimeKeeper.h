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

		bool operator ==(const Record& r2) {
			if (time != r2.time)
				return false;
			if (position.x != r2.position.x || position.y != r2.position.y || position.z != r2.position.z)
				return false;
			if (rotation.x != r2.rotation.x || position.y != r2.rotation.y || position.z != r2.rotation.z)
				return false;
			if (velocity.x != r2.velocity.x || velocity.y != r2.velocity.y || velocity.z != r2.velocity.z)
				return false;
			if (ang_velocity.x != r2.ang_velocity.x || ang_velocity.y != r2.ang_velocity.y || ang_velocity.z != r2.ang_velocity.z)
				return false;
			return true;
		}
		bool operator !=(const Record& r2) {
			return !(*this == r2);
		}
	};
	const Record NULL_RECORD = { -1, DirectX::XMFLOAT3(), DirectX::XMFLOAT3(), DirectX::XMFLOAT3(), DirectX::XMFLOAT3() };

	class TimeKeeper {
	public:
		void RecordData(float timestamp, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rot, DirectX::XMFLOAT3 vel, DirectX::XMFLOAT3 ang_vel) {
			Record data = { timestamp, pos, rot, vel, ang_vel };
			records.push_back(data);
		}
		void RecordData(Record data) {
			records.push_back(data);
		}
		Record Retrieve(float timestamp) {
			if (records.size() == 0)	return NULL_RECORD;
			
			if (timestamp > records[records.size() - 1].time || timestamp < 0) {
				return NULL_RECORD;
			}
			
			return records[timestamp * 1000];
		}
		
		void Wipe(Record initial) {
			records.clear();
			records.push_back(initial);
		}

		std::vector<Record> GetRecords() { return records; }
	private:
		std::vector<Record> records;
	};
}