#pragma once
#include "pch.h"
#include "..\Common\DirectXHelper.h"
#include "PhysicsBody.h"

namespace PhysicsCanvas {
	class BoundingShape {
	public:
		static enum BoundType {
			Cube, Sphere
		};

		BoundingShape(BoundType type_, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rot, DirectX::XMFLOAT3 dims)
			: type(type_), position(pos), rotation(rot), dimensions(dims) {}

		void SetPosition(DirectX::XMFLOAT3 pos) { position = pos; }
		void SetRotation(DirectX::XMFLOAT3 rot) { rotation = rot; }
		void SetDimensions(DirectX::XMFLOAT3 dims) { dimensions = dims; }

		static bool IsColliding(std::shared_ptr<BoundingShape> first, std::shared_ptr<BoundingShape> other) {
			if (first->type == BoundType::Cube) {
				//cube-cube collisions
				if (other->type == BoundType::Cube) {
					DirectX::XMFLOAT3 half(first->dimensions.x / 2.f, first->dimensions.y / 2.f, first->dimensions.z / 2.f);
					DirectX::XMFLOAT3 oHalf(other->dimensions.x / 2.f,
											other->dimensions.y / 2.f, 
											other->dimensions.z / 2.f);

					DirectX::XMFLOAT3 min1(first->position.x - half.x,
										first->position.y - half.y,
										first->position.z - half.z);

					DirectX::XMFLOAT3 max1(first->position.x + half.x, first->position.y + half.y, first->position.z + half.z);

					DirectX::XMFLOAT3 min2(other->position.x - oHalf.x,
										other->position.y - oHalf.y,
										other->position.z - oHalf.z);

					DirectX::XMFLOAT3 max2(other->position.x + oHalf.x,
										other->position.y + oHalf.y,
										other->position.z + oHalf.z);

					// Check for overlap along each axis (X, Y, Z)
					bool overlapX = (max1.x >= min2.x) && (min1.x <= max2.x);
					bool overlapY = (max1.y >= min2.y) && (min1.y <= max2.y);
					bool overlapZ = (max1.z >= min2.z) && (min1.z <= max2.z);

					//if there is a collision, return true. otherwise, return false
					if (overlapX && overlapY && overlapZ) {
						return true;
					}
					else {
						return false;
					}
				}
				//cube-sphere collisions
				else {

				}
			}
			else {
				//sphere-cube collisions
				if (other->type == BoundType::Cube) {

				}
				//sphere-sphere collisions
				else {
					float dist = sqrt(	//use 3D pythagoras to find distance between the two spheres
						((first->position.x - other->position.x) * 
							(first->position.x - other->position.x)) +

						((first->position.y - other->position.y) * 
							(first->position.y - other->position.y)) +

						((first->position.z - other->position.z) * 
							(first->position.z - other->position.z))
					);
					//if distance is less than sum of radii, they are colliding so return true. else, return false
					if (dist < first->dimensions.x + other->dimensions.x) {		//we can just take x since it will be all the same
						return true;
					}
					else {
						return false;
					}
				}
			}
		}

		static std::vector<DirectX::XMFLOAT3> ResolveCollisions(std::shared_ptr<BoundingShape> first, std::shared_ptr<BoundingShape> other) {
			DirectX::XMFLOAT3 min1(first->position.x - first->dimensions.x * 0.5f,
							first->position.y - first->dimensions.y * 0.5f,
							first->position.z - first->dimensions.z * 0.5f);
			
			DirectX::XMFLOAT3 max1(first->position.x + first->dimensions.x * 0.5f,
						first->position.y + first->dimensions.y * 0.5f,
						first->position.z + first->dimensions.z * 0.5f);
			
			DirectX::XMFLOAT3 min2(other->position.x - other->dimensions.x * 0.5f,
						other->position.y - other->dimensions.y * 0.5f,
						other->position.z - other->dimensions.z * 0.5f);
			
			DirectX::XMFLOAT3 max2(other->position.x + other->dimensions.x * 0.5f,
						other->position.y + other->dimensions.y * 0.5f,
						other->position.z + other->dimensions.z * 0.5f);

			float overlapX = min(max1.x, max2.x) - max(min1.x, min2.x);
			float overlapY = min(max1.y, max2.y) - max(min1.y, min2.y);
			float overlapZ = min(max1.z, max2.z) - max(min1.z, min2.z);

			DirectX::XMFLOAT3 trans1 = DirectX::XMFLOAT3();
			DirectX::XMFLOAT3 trans2 = DirectX::XMFLOAT3();

			// Choose the axis with the smallest overlap and move the cubes apart
			if (overlapX <= overlapY && overlapX <= overlapZ) {
				float penetrationDepth = std::abs(overlapX);
				float moveDistance = penetrationDepth * 0.5f;
				trans1.x += moveDistance;
				trans2.x -= moveDistance;
			}
			else if (overlapY <= overlapX && overlapY <= overlapZ) {
				float penetrationDepth = std::abs(overlapY);
				float moveDistance = penetrationDepth * 0.5f;
				trans1.y += moveDistance;
				trans2.y -= moveDistance;
			}
			else {
				float penetrationDepth = std::abs(overlapZ);
				float moveDistance = penetrationDepth * 0.5f;
				trans1.z += moveDistance;
				trans2.z -= moveDistance;
			}
			std::vector<DirectX::XMFLOAT3> ret;
			ret.push_back(trans1);
			ret.push_back(trans2);
			return ret;
		}


	private:
		BoundType type;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 rotation;
		DirectX::XMFLOAT3 dimensions;

	};
}