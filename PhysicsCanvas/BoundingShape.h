#pragma once
#include "pch.h"
#include "..\Common\DirectXHelper.h"
#include "PhysicsBody.h"
#include "PhysMaths.h"

namespace PhysicsCanvas {
	struct Edge {
		DirectX::XMFLOAT3 vert1;
		DirectX::XMFLOAT3 vert2;
	};
	struct CuboidFace {
		DirectX::XMFLOAT3 vert1;
		DirectX::XMFLOAT3 vert2;
		DirectX::XMFLOAT3 vert3;
		DirectX::XMFLOAT3 vert4;
	};

	class BoundingShape {
	public:
		static enum BoundType {
			Cuboid, Sphere
		};

		BoundingShape(BoundType type_, DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rot, DirectX::XMFLOAT3 dims)
			: type(type_), position(pos), rotation(rot), dimensions(dims) {}

		void SetPosition(DirectX::XMFLOAT3 pos) { position = pos; }
		void SetRotation(DirectX::XMFLOAT3 rot) { rotation = rot; }
		void SetDimensions(DirectX::XMFLOAT3 dims) { dimensions = dims; }

		static bool PointCollidingWithObject(DirectX::XMFLOAT3 point, std::shared_ptr<BoundingShape> object) {
			switch (object->type) {
			case Cuboid:
				DirectX::XMFLOAT3 half(object->dimensions.x / 2.f, object->dimensions.y / 2.f, object->dimensions.z / 2.f);
				DirectX::XMFLOAT3 min1(object->position.x - half.x, object->position.y - half.y, object->position.z - half.z);
				DirectX::XMFLOAT3 max1(object->position.x + half.x, object->position.y + half.y, object->position.z + half.z);
				return (point.x >= min1.x && point.x <= max1.x) &&
					(point.y >= min1.y && point.y <= max1.y) &&
					(point.z >= min1.z && point.z <= max1.z);
				break;
			case Sphere:
				return PhysMaths::Distance(point, object->position) <= object->dimensions.x;
				break;
			}
		}

		static bool IsColliding(std::shared_ptr<BoundingShape> first, std::shared_ptr<BoundingShape> other) {
			if (first->type == BoundType::Cuboid) {
				//cube-cube collisions
				if (other->type == BoundType::Cuboid) {
					DirectX::XMFLOAT3 half(first->dimensions.x / 2.f, first->dimensions.y / 2.f, first->dimensions.z / 2.f);
					DirectX::XMFLOAT3 oHalf(other->dimensions.x / 2.f, other->dimensions.y / 2.f, other->dimensions.z / 2.f);

					DirectX::XMFLOAT3 min1(first->position.x - half.x, first->position.y - half.y, first->position.z - half.z);

					DirectX::XMFLOAT3 max1(first->position.x + half.x, first->position.y + half.y, first->position.z + half.z);

					DirectX::XMFLOAT3 min2(other->position.x - oHalf.x, other->position.y - oHalf.y, other->position.z - oHalf.z);

					DirectX::XMFLOAT3 max2(other->position.x + oHalf.x, other->position.y + oHalf.y, other->position.z + oHalf.z);

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
					return (PointCollidingWithObject(DirectX::XMFLOAT3(other->position.x, other->position.y + other->dimensions.y, other->position.z), first)
						|| PointCollidingWithObject(DirectX::XMFLOAT3(other->position.x, other->position.y - other->dimensions.y, other->position.z), first)
						|| PointCollidingWithObject(DirectX::XMFLOAT3(other->position.x + other->dimensions.x, other->position.y, other->position.z), first)
						|| PointCollidingWithObject(DirectX::XMFLOAT3(other->position.x - other->dimensions.x, other->position.y, other->position.z), first)
						|| PointCollidingWithObject(DirectX::XMFLOAT3(other->position.x, other->position.y, other->position.z + other->dimensions.z), first)
						|| PointCollidingWithObject(DirectX::XMFLOAT3(other->position.x, other->position.y, other->position.z - other->dimensions.z), first)
					);
				}
			}
			else {
				//sphere-cube collisions
				if (other->type == BoundType::Cuboid) {
					return (PointCollidingWithObject(DirectX::XMFLOAT3(first->position.x, first->position.y + first->dimensions.y, first->position.z), other)
						|| PointCollidingWithObject(DirectX::XMFLOAT3(first->position.x, first->position.y - first->dimensions.y, first->position.z), other)
						|| PointCollidingWithObject(DirectX::XMFLOAT3(first->position.x + first->dimensions.x, first->position.y, first->position.z), other)
						|| PointCollidingWithObject(DirectX::XMFLOAT3(first->position.x - first->dimensions.x, first->position.y, first->position.z), other)
						|| PointCollidingWithObject(DirectX::XMFLOAT3(first->position.x, first->position.y, first->position.z + first->dimensions.z), other)
						|| PointCollidingWithObject(DirectX::XMFLOAT3(first->position.x, first->position.y, first->position.z - first->dimensions.z), other)
						);
				}
				//sphere-sphere collisions
				else {
					//if distance is less than sum of radii, they are colliding so return true. else, return false
					return (PhysMaths::Distance(first->position, other->position) < first->dimensions.x + other->dimensions.x);		//we can just take x since it will be all the same
					
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

		std::vector<Edge> CuboidEdges() {
			assert(type == BoundType::Cuboid && "Not a cuboid!!");
			DirectX::XMFLOAT3 halves(dimensions.x / 2.0f, dimensions.y / 2.0f, dimensions.z / 2.0f);
			std::vector<Edge> ret;
			//declare positions of vertices
			DirectX::XMFLOAT3 v1(position.x + halves.x, position.y + halves.y, position.z + halves.z);
			DirectX::XMFLOAT3 v2(position.x + halves.x, position.y + halves.y, position.z - halves.z);
			DirectX::XMFLOAT3 v3(position.x + halves.x, position.y - halves.y, position.z - halves.z);
			DirectX::XMFLOAT3 v4(position.x + halves.x, position.y - halves.y, position.z + halves.z);
			DirectX::XMFLOAT3 v5(position.x - halves.x, position.y + halves.y, position.z + halves.z);
			DirectX::XMFLOAT3 v6(position.x - halves.x, position.y + halves.y, position.z - halves.z);
			DirectX::XMFLOAT3 v7(position.x - halves.x, position.y - halves.y, position.z - halves.z);
			DirectX::XMFLOAT3 v8(position.x - halves.x, position.y - halves.y, position.z + halves.z);
			//create edges
			ret.push_back({ v1, v2 });
			ret.push_back({ v1, v4 });
			ret.push_back({ v2, v3 });
			ret.push_back({ v3, v4 });
			ret.push_back({ v1, v5 });
			ret.push_back({ v2, v6 });
			ret.push_back({ v3, v7 });
			ret.push_back({ v4, v8 });
			ret.push_back({ v5, v6 });
			ret.push_back({ v5, v8 });
			ret.push_back({ v6, v7 });
			ret.push_back({ v7, v8 });
			return ret;
		}

		std::vector<CuboidFace> CuboidFaces() {
			assert(type == BoundType::Cuboid && "Not a cuboid!!");
			DirectX::XMFLOAT3 halves(dimensions.x / 2.0f, dimensions.y / 2.0f, dimensions.z / 2.0f);
			std::vector<CuboidFace> ret;
			//declare positions of vertices
			DirectX::XMFLOAT3 v1(position.x + halves.x, position.y + halves.y, position.z + halves.z);
			DirectX::XMFLOAT3 v2(position.x + halves.x, position.y + halves.y, position.z - halves.z);
			DirectX::XMFLOAT3 v3(position.x + halves.x, position.y - halves.y, position.z - halves.z);
			DirectX::XMFLOAT3 v4(position.x + halves.x, position.y - halves.y, position.z + halves.z);
			DirectX::XMFLOAT3 v5(position.x - halves.x, position.y + halves.y, position.z + halves.z);
			DirectX::XMFLOAT3 v6(position.x - halves.x, position.y + halves.y, position.z - halves.z);
			DirectX::XMFLOAT3 v7(position.x - halves.x, position.y - halves.y, position.z - halves.z);
			DirectX::XMFLOAT3 v8(position.x - halves.x, position.y - halves.y, position.z + halves.z);
			//create faces
			ret.push_back({ v1, v2, v3, v4 }); //right face
			ret.push_back({ v1, v2, v6, v5 }); //top face
			ret.push_back({ v1, v4, v8, v5 }); //front face
			ret.push_back({ v2, v3, v7, v6 }); //back face
			ret.push_back({ v5, v6, v7, v8 }); //left face
			ret.push_back({ v3, v4, v8, v7 }); //bottom face
			return ret;
		}
		DirectX::XMFLOAT3 CuboidFaceCentre(CuboidFace face) {
			assert(type == BoundType::Cuboid && "Not a cuboid!");
			return DirectX::XMFLOAT3((face.vert1.x + face.vert2.x + face.vert3.x + face.vert4.x) / 4.0f,
				(face.vert1.y + face.vert2.y + face.vert3.y + face.vert4.y) / 4.0f,
				(face.vert1.z + face.vert2.z + face.vert3.z + face.vert4.z) / 4.0f);
		}
		DirectX::XMFLOAT3 CuboidFaceNormal(CuboidFace face) {
			assert(type == BoundType::Cuboid && "Not a cuboid!");
			DirectX::XMFLOAT3 posToFac(CuboidFaceCentre(face).x - position.x, CuboidFaceCentre(face).y - position.y, CuboidFaceCentre(face).z - position.z);

			DirectX::XMFLOAT3 vec1(face.vert2.x - face.vert1.x, face.vert2.y - face.vert1.y, face.vert2.z - face.vert1.z);
			DirectX::XMFLOAT3 vec2(face.vert4.x - face.vert1.x, face.vert4.y - face.vert1.y, face.vert4.z - face.vert1.z);

			DirectX::XMFLOAT3 normal = PhysMaths::Float3Cross(vec1, vec2);
			return (PhysMaths::Float3Dot(posToFac, normal) / PhysMaths::Magnitude(posToFac) * PhysMaths::Magnitude(normal)) > 0 ?
				normal : PhysMaths::VecTimesByConstant(normal, -1);
		}

		//To find the contact points on object1 with object2, call this method on object1 and pass in object2 as the argument
		std::vector<DirectX::XMFLOAT3> ContactPointsTo(std::shared_ptr<BoundingShape> obj2) {
			if(type == BoundType::Cuboid) {
				std::vector<Edge> edges = CuboidEdges();
				std::vector<DirectX::XMFLOAT3> ret;
				for (Edge edge : edges) {
					//If both vertices of the edge are colliding in contact with the object, they are both contact points
					if (PointCollidingWithObject(edge.vert1, obj2) && PointCollidingWithObject(edge.vert2, obj2)) {
						//Check that the vertices aren't already in ret to avoid duplicates
						bool alreadyContained = false;
						for (DirectX::XMFLOAT3 vert : ret) {
							alreadyContained = (edge.vert1.x == vert.x) && (edge.vert1.y == vert.y) && (edge.vert1.z == vert.z);
							if (alreadyContained)
								break;
						}
						if (!alreadyContained)
							ret.push_back(edge.vert1);
						//repeat for the second vertex
						alreadyContained = false;
						for (DirectX::XMFLOAT3 vert : ret) {
							alreadyContained = (edge.vert2.x == vert.x) && (edge.vert2.y == vert.y) && (edge.vert2.z == vert.z);
							if (alreadyContained)
								break;
						}
						if (!alreadyContained)
							ret.push_back(edge.vert2);
					}
					//if only the first vertex is in contact with the object
					else if (PointCollidingWithObject(edge.vert1, obj2)) {
						bool alreadyContained = false;
						for (DirectX::XMFLOAT3 vert : ret) {
							alreadyContained = (edge.vert1.x == vert.x) && (edge.vert1.y == vert.y) && (edge.vert1.z == vert.z);
							if (alreadyContained)
								break;
						}
						if (!alreadyContained)
							ret.push_back(edge.vert1);
						//Traverse along this edge to see where it stops being in contact, registering that point as a contact point
						DirectX::XMFLOAT3 dir(edge.vert2.x - edge.vert1.x, edge.vert2.y - edge.vert1.y, edge.vert2.z - edge.vert1.z);
						DirectX::XMFLOAT3 point = edge.vert1;
						DirectX::XMFLOAT3 point2;
						for (float p = 0.01f; p < 1.0f; p += 1.0f) {
							point2 = { edge.vert1.x + (dir.x * p), edge.vert1.y + (dir.y * p), edge.vert1.z + (dir.z * p) };
							if (!PointCollidingWithObject(point2, obj2)) {
								break; //The cut-off point has been found and the previous point is saved to "point" so now we can append it
							}
							else {
								point = point2;
							}
						}
						alreadyContained = false;
						for (DirectX::XMFLOAT3 vert : ret) {
							alreadyContained = (point.x == vert.x) && (point.y == vert.y) && (point.z == vert.z);
							if (alreadyContained)
								break;
						}
						if (!alreadyContained)
							ret.push_back(point);
					}
					//if only the second vertex is in contact
					else if (PointCollidingWithObject(edge.vert2, obj2)) {
						bool alreadyContained = false;
						for (DirectX::XMFLOAT3 vert : ret) {
							alreadyContained = (edge.vert2.x == vert.x) && (edge.vert2.y == vert.y) && (edge.vert2.z == vert.z);
							if (alreadyContained)
								break;
						}
						if (!alreadyContained)
							ret.push_back(edge.vert2);
						//Traverse along this edge to see where it stops being in contact, registering that point as a contact point
						DirectX::XMFLOAT3 dir(edge.vert1.x - edge.vert2.x, edge.vert1.y - edge.vert2.y, edge.vert1.z - edge.vert2.z);
						DirectX::XMFLOAT3 point = edge.vert2;
						DirectX::XMFLOAT3 point2;
						for (float p = 0.01f; p < 1.0f; p += 0.01f) {
							point2 = { edge.vert2.x + (dir.x * p), edge.vert2.y + (dir.y * p), edge.vert2.z + (dir.z * p) };
							if (!PointCollidingWithObject(point2, obj2)) {
								break; //The cut-off point has been found and the previous point is saved to "point" so now we can append it
							}
							else {
								point = point2;
							}
						}
						alreadyContained = false;
						for (DirectX::XMFLOAT3 vert : ret) {
							alreadyContained = (point.x == vert.x) && (point.y == vert.y) && (point.z == vert.z);
							if (alreadyContained)
								break;
						}
						if (!alreadyContained)
							ret.push_back(point);
					}
				}
				return ret;
			}
		}

		//Find closest point on obj2 from obj1 (caller of the method)
		DirectX::XMFLOAT3 ClosestPointOn(std::shared_ptr<BoundingShape> obj2) {
			switch (obj2->GetType()) {
			case BoundType::Cuboid:
				
				break;
			case BoundType::Sphere:
				DirectX::XMFLOAT3 dir(obj2->position.x - position.x, obj2->position.y - position.y, obj2->position.z - position.z);
				dir = PhysMaths::VecTimesByConstant(dir, obj2->dimensions.x / PhysMaths::Magnitude(dir));
				return DirectX::XMFLOAT3(obj2->position.x + dir.x, obj2->position.y + dir.y, obj2->position.z + dir.z);
				break;
			}
		}

		BoundType GetType() { return type; }
	private:
		BoundType type;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 rotation;
		DirectX::XMFLOAT3 dimensions;

	};
}