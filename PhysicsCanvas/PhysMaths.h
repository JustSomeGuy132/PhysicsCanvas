#pragma once
#include "pch.h"
#include "..\Common\DirectXHelper.h"

namespace PhysicsCanvas {
	class PhysMaths {
		
	public:
		static float Magnitude(DirectX::XMFLOAT3 vec) {
			return sqrt(pow(vec.x, 2) + pow(vec.y, 2) + pow(vec.z, 2));
		}
		static float Distance(DirectX::XMFLOAT3 v1, DirectX::XMFLOAT3 v2) {
			return Magnitude(DirectX::XMFLOAT3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z));
		}
		static DirectX::XMFLOAT3 VecTimesByConstant(DirectX::XMFLOAT3 vec, float c) {
			return DirectX::XMFLOAT3(vec.x * c, vec.y * c, vec.z * c);
		}
		static DirectX::XMFLOAT3 VecDivByConstant(DirectX::XMFLOAT3 vec, float c) {
			return DirectX::XMFLOAT3(vec.x / c, vec.y / c, vec.z / c);
		}

		static float Float3Dot(DirectX::XMFLOAT3 vec1, DirectX::XMFLOAT3 vec2) {
			return (vec1.x * vec2.x) + (vec1.y * vec2.y) + (vec1.z * vec2.z);
		}
		static DirectX::XMFLOAT3 Float3Cross(DirectX::XMFLOAT3 vec1, DirectX::XMFLOAT3 vec2) {
			return DirectX::XMFLOAT3((vec1.y * vec2.z) - (vec1.z * vec2.y), (vec1.z * vec2.x) - (vec1.x * vec2.z), (vec1.x * vec2.y) - (vec1.y * vec2.x));
		}
		static float PerpendicularDist(DirectX::XMFLOAT3 vec1, DirectX::XMFLOAT3 vec2) {
			return abs(Magnitude(vec1) * sinf(acosf(Float3Dot(vec1, vec2) / (Magnitude(vec1) * Magnitude(vec2)))));
		}
	};
}