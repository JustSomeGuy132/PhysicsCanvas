#pragma once
#include "..\Common\DirectXHelper.h"
#include <DirectXMath.h>
#include <list>
#include "PEvent.h"
#include <sstream>

namespace PhysicsCanvas {
	class Force : public PEvent {
	public:
		static enum ForceType {
			Impulse,
			Constant,
			Weight	// weight is its own forcetype because it is infinitely applied
		};
		//for all regular forces that can be applied on objects
		Force(ForceType type_, DirectX::XMFLOAT3 dir_) : type(type_), direction(dir_) {
			if (type == ForceType::Impulse) {
				SetEnd(0.002f);
			}
		}

		//if only mass is passed in, that force is automatically assumed to be weight and will be classed as such
		Force(float mass) : type(ForceType::Weight), direction(DirectX::XMFLOAT3(0.0f, mass * -9.81f, 0.0f)) {
			SetEnd(-1.0f);
		}

		void SetStart(float start) {
			startT = start;
			if (type == ForceType::Impulse) {
				SetEnd(startT + 0.002f);
			}
		}

		float Magnitude() {		//applies 3D Pythagoras to this force object
			float result = 0.0f;
			result += direction.x * direction.x;
			result += direction.y * direction.y;
			result += direction.z * direction.z;
			result = sqrt(result);
			return result;
		}

		DirectX::XMFLOAT3 GetDirection() {
			return direction;
		}

		static Force ResultantF(std::list<Force> forces) {
			Force result(ForceType::Constant, DirectX::XMFLOAT3(0.0f, 0.0f, 0.0f));
			for (Force f : forces) {
				result.direction.x += f.direction.x;
				result.direction.y += f.direction.y;
				result.direction.z += f.direction.z;
			}
			return result;
		}

		void operator = (Force f1) {
			SetId(f1.GetId());
			type = f1.type;
			SetStart(f1.GetStart());
			SetEnd(f1.GetEnd());
			direction = f1.GetDirection();
		}

		bool operator == (Force f1) {
			if (type != f1.type) {
				return false;
			}
			if (direction.x != f1.direction.x || direction.y != f1.direction.y || direction.z != f1.direction.z) {
				return false;
			}
			if (GetId() != f1.GetId()) {
				return false;
			}
			if (GetStart() != f1.GetStart()) {
				return false;
			}
			if (GetEnd() != f1.GetEnd()) {
				return false;
			}
			return true;
		}

		DirectX::XMFLOAT3 operator / (float m) {
			DirectX::XMFLOAT3 temp(direction);
			temp.x = direction.x / m;
			temp.y = direction.y / m;		//implementing a = F/m for acceleration
			temp.z = direction.z / m;
			return temp;
		}

		static void PrintForce(Force f) {
			std::ostringstream oss;
			oss << "Force name : " << f.GetId() << "\n"
				<< "Direction = (" << f.GetDirection().x << ", " << f.GetDirection().y << ", " << f.GetDirection().z << ")\n"
				<< "Start time = " << f.GetStart() << "\n"
				<< "End time = " << f.GetEnd() << "\n";
			OutputDebugString(oss.str().c_str());
		}

	private:
		ForceType type;
		DirectX::XMFLOAT3 direction;

		float startT = 0.0f;
	};
}