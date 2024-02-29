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

		DirectX::XMFLOAT3 GetMaxPoint() {
			switch (type) {
			case Cuboid:
				DirectX::XMFLOAT3 maxP = {};
				for (DirectX::XMFLOAT3 vert : CuboidVertices()) {
					if (vert.x >= maxP.x && vert.y >= maxP.y && vert.z >= maxP.z)
						maxP = vert;
				}
				return maxP;
				break;
			case Sphere:
				return PhysMaths::Float3Add(position, PhysMaths::VecTimesByConstant({ 1,1,1 }, dimensions.x));
				break;
			}
		}
		DirectX::XMFLOAT3 GetMinPoint() {
			switch (type) {
			case Cuboid:
				DirectX::XMFLOAT3 minP = {};
				for (DirectX::XMFLOAT3 vert : CuboidVertices()) {
					if (vert.x <= minP.x && vert.y <= minP.y && vert.z <= minP.z)
						minP = vert;
				}
				return minP;
				break;
			case Sphere:
				return PhysMaths::Float3Add(position, PhysMaths::VecTimesByConstant({ -1,-1,-1 }, dimensions.x));
				break;
			}
		}

		static bool PointCollidingWithObject(DirectX::XMFLOAT3 point, std::shared_ptr<BoundingShape> object) {
			switch (object->type) {
			case Cuboid:
				return (point.x >= object->GetMinPoint().x && point.x <= object->GetMaxPoint().x
						&& point.y >= object->GetMinPoint().y && point.y <= object->GetMaxPoint().y
						&& point.z >= object->GetMinPoint().z && point.z <= object->GetMaxPoint().z);
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
					for (DirectX::XMFLOAT3 v1 : first->CuboidVertices()) {
						if (PointCollidingWithObject(v1, other))
							return true;
					}
					for (DirectX::XMFLOAT3 v2 : other->CuboidVertices()) {
						if (PointCollidingWithObject(v2, first))
							return true;
					}
					//if there is a collision, return true. otherwise, return false
					return false;
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
			DirectX::XMFLOAT3 half(first->dimensions.x / 2.f, first->dimensions.y / 2.f, first->dimensions.z / 2.f);
			DirectX::XMFLOAT3 oHalf(other->dimensions.x / 2.f, other->dimensions.y / 2.f, other->dimensions.z / 2.f);

			DirectX::XMFLOAT3 min1(-half.x, -half.y, -half.z);
			
			min1 = PhysMaths::Float3Add(first->position, min1);min1 = PhysMaths::RotateVector(min1, first->rotation);

			DirectX::XMFLOAT3 max1(half.x, half.y, half.z);
			
			max1 = PhysMaths::Float3Add(first->position, max1);max1 = PhysMaths::RotateVector(max1, first->rotation);

			DirectX::XMFLOAT3 min2(-oHalf.x, -oHalf.y, -oHalf.z);
			
			min2 = PhysMaths::Float3Add(other->position, min2);min2 = PhysMaths::RotateVector(min2, other->rotation);

			DirectX::XMFLOAT3 max2(oHalf.x, oHalf.y, oHalf.z);
			
			max2 = PhysMaths::Float3Add(other->position, max2);max2 = PhysMaths::RotateVector(max2, other->rotation);

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

		std::vector<DirectX::XMFLOAT3> CuboidVertices() {
			assert(type == BoundType::Cuboid && "Not a cuboid!!");
			DirectX::XMFLOAT3 halves(dimensions.x / 2.0f, dimensions.y / 2.0f, dimensions.z / 2.0f);
			//declare positions of vertices
			DirectX::XMFLOAT3 v1(halves.x, halves.y, halves.z);
			v1 = PhysMaths::RotateVector(v1, rotation);
			v1 = PhysMaths::Float3Add(v1, position);
			DirectX::XMFLOAT3 v2(halves.x, halves.y, -halves.z);
			v2 = PhysMaths::RotateVector(v2, rotation);
			v2 = PhysMaths::Float3Add(v2, position);
			DirectX::XMFLOAT3 v3(halves.x, -halves.y, -halves.z);
			v3 = PhysMaths::RotateVector(v3, rotation);
			v3 = PhysMaths::Float3Add(v3, position);
			DirectX::XMFLOAT3 v4(halves.x, -halves.y, halves.z);
			v4 = PhysMaths::RotateVector(v4, rotation);
			v4 = PhysMaths::Float3Add(v4, position);
			DirectX::XMFLOAT3 v5(-halves.x, halves.y, halves.z);
			v5 = PhysMaths::RotateVector(v5, rotation);
			v5 = PhysMaths::Float3Add(v5, position);
			DirectX::XMFLOAT3 v6(-halves.x, halves.y, -halves.z);
			v6 = PhysMaths::RotateVector(v6, rotation);
			v6 = PhysMaths::Float3Add(v6, position);
			DirectX::XMFLOAT3 v7(-halves.x, -halves.y, -halves.z);
			v7 = PhysMaths::RotateVector(v7, rotation);
			v7 = PhysMaths::Float3Add(v7, position);
			DirectX::XMFLOAT3 v8(-halves.x, -halves.y, halves.z);
			v8 = PhysMaths::RotateVector(v8, rotation);
			v8 = PhysMaths::Float3Add(v8, position);
			return { v1, v2, v3, v4, v5, v6, v7, v8 };
		}

		std::vector<Edge> CuboidEdges() {
			assert(type == BoundType::Cuboid && "Not a cuboid!!");
			
			std::vector<Edge> ret;
			std::vector<DirectX::XMFLOAT3> verts = CuboidVertices();
			
			//create edges
			ret.push_back({ verts[0], verts[1] });
			ret.push_back({ verts[1], verts[3] });
			ret.push_back({ verts[1], verts[2] });
			ret.push_back({ verts[2], verts[3] });
			ret.push_back({ verts[0], verts[4] });
			ret.push_back({ verts[1], verts[5] });
			ret.push_back({ verts[2], verts[6] });
			ret.push_back({ verts[3], verts[7] });
			ret.push_back({ verts[4], verts[5] });
			ret.push_back({ verts[4], verts[7] });
			ret.push_back({ verts[5], verts[6] });
			ret.push_back({ verts[6], verts[7] });
			return ret;
		}

		std::vector<CuboidFace> CuboidFaces() {
			assert(type == BoundType::Cuboid && "Not a cuboid!!");
			
			std::vector<CuboidFace> ret;
			std::vector<DirectX::XMFLOAT3> verts = CuboidVertices();
			
			//create faces
			ret.push_back({ verts[0], verts[1], verts[2], verts[3] }); //right face
			ret.push_back({ verts[0], verts[1], verts[5], verts[4] }); //top face
			ret.push_back({ verts[0], verts[3], verts[7], verts[4] }); //front face
			ret.push_back({ verts[1], verts[2], verts[6], verts[5] }); //back face
			ret.push_back({ verts[4], verts[5], verts[6], verts[7] }); //left face
			ret.push_back({ verts[2], verts[3], verts[7], verts[6]}); //bottom face
			return ret;
		}
		static DirectX::XMFLOAT3 CuboidFaceCentre(CuboidFace face) {
			
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
			return (PhysMaths::Float3Dot(posToFac, normal) / (PhysMaths::Magnitude(posToFac) * PhysMaths::Magnitude(normal))) > 0 ?
				normal : PhysMaths::VecTimesByConstant(normal, -1);
		}

		//To find the contact points on object1 with object2, call this method on object1 and pass in object2 as the argument
		std::vector<DirectX::XMFLOAT3> ContactPointsTo(std::shared_ptr<BoundingShape> obj2) {
			std::vector<DirectX::XMFLOAT3> ret;
			if (type == BoundType::Cuboid) {
				std::vector<Edge> edges = CuboidEdges();
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
				/*std::vector<DirectX::XMFLOAT3> otherPs;
				for (DirectX::XMFLOAT3 oP : obj2->ContactPointsTo(std::make_shared<BoundingShape>(type, position, rotation, dimensions))) {
					bool alreadyContained = false;
					for (DirectX::XMFLOAT3 vert : ret) {
						alreadyContained = (oP.x == vert.x) && (oP.y == vert.y) && (oP.z == vert.z);
						if (alreadyContained)
							break;
					}
					if (!alreadyContained)
						ret.push_back(oP);
				}*/
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