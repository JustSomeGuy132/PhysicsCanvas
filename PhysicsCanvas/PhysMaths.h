#pragma once
#include "pch.h"
#include "..\Common\DirectXHelper.h"

using namespace DirectX;

namespace PhysicsCanvas {
	class PhysMaths {
		
	public:
		static float Magnitude(XMFLOAT3 vec) {
			return sqrt(pow(vec.x, 2) + pow(vec.y, 2) + pow(vec.z, 2));
		}
		static float Distance(XMFLOAT3 v1, XMFLOAT3 v2) {
			return Magnitude(XMFLOAT3(v2.x - v1.x, v2.y - v1.y, v2.z - v1.z));
		}
		static XMFLOAT3 VecTimesByConstant(XMFLOAT3 vec, float c) {
			return XMFLOAT3(vec.x * c, vec.y * c, vec.z * c);
		}
		static XMFLOAT3 VecDivByConstant(XMFLOAT3 vec, float c) {
			return XMFLOAT3(vec.x / c, vec.y / c, vec.z / c);
		}
		static XMFLOAT3 Float3Add(XMFLOAT3 vec1, XMFLOAT3 vec2) {
			return XMFLOAT3(vec1.x + vec2.x, vec1.y + vec2.y, vec1.z + vec2.z);
		}
		static XMFLOAT3 Float3Minus(XMFLOAT3 vec1, XMFLOAT3 vec2) {
			return XMFLOAT3(vec1.x - vec2.x, vec1.y - vec2.y, vec1.z - vec2.z);
		}
		static float Float3Dot(XMFLOAT3 vec1, XMFLOAT3 vec2) {
			return (vec1.x * vec2.x) + (vec1.y * vec2.y) + (vec1.z * vec2.z);
		}
		static XMFLOAT3 Float3Cross(XMFLOAT3 vec1, XMFLOAT3 vec2) {
			return XMFLOAT3((vec1.y * vec2.z) - (vec1.z * vec2.y), (vec1.z * vec2.x) - (vec1.x * vec2.z), (vec1.x * vec2.y) - (vec1.y * vec2.x));
		}
		static float PerpendicularDist(XMFLOAT3 vec1, XMFLOAT3 vec2) {
			return abs(Magnitude(vec1) * sinf(acosf(Float3Dot(vec1, vec2) / (Magnitude(vec1) * Magnitude(vec2)))));
		}
		static XMFLOAT3 RotateVector(XMFLOAT3 vec, XMFLOAT3 rot) {
			XMFLOAT3 ret = {};
			XMVECTOR vector = XMLoadFloat3(&vec);
			XMVECTOR rotation = XMQuaternionRotationRollPitchYaw(rot.z, rot.y, rot.x);
			XMVECTOR result = XMVector3Rotate(vector, rotation);
			XMStoreFloat3(&ret, result);
			return ret;
		}
		static float Float3CosTheta(XMFLOAT3 vec1, XMFLOAT3 vec2) {
			return Float3Dot(vec1, vec2) / (Magnitude(vec1) * Magnitude(vec2));
		}
	};
}