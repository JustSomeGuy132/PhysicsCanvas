#pragma once
#include "pch.h"
#include "..\Common\DirectXHelper.h"
#include "PhysicsBody.h"
#include "PhysMaths.h"

using namespace DirectX;

namespace PhysicsCanvas {
	struct Edge {
		XMFLOAT3 vert1;
		XMFLOAT3 vert2;
	};
	struct CuboidFace {
		XMFLOAT3 vert1;
		XMFLOAT3 vert2;
		XMFLOAT3 vert3;
		XMFLOAT3 vert4;
	};

	class BoundingShape {
	public:
		static enum BoundType {
			Cuboid, Sphere
		};

		BoundingShape(BoundType type_, XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 dims)
			: type(type_), position(pos), rotation(rot), dimensions(dims) {}

		void SetPosition(XMFLOAT3 pos) { position = pos; }

		void SetRotation(XMFLOAT3 rot) { rotation = rot; }
		
		void SetDimensions(XMFLOAT3 dims) { dimensions = dims; }

		XMFLOAT3 GetMaxPoint();

		XMFLOAT3 GetMinPoint();

		static bool PointCollidingWithObject(XMFLOAT3 point, std::shared_ptr<BoundingShape> object);

		static bool IsColliding(std::shared_ptr<BoundingShape> first, std::shared_ptr<BoundingShape> other);

		static std::vector<XMFLOAT3> ResolveCollisions(std::shared_ptr<BoundingShape> first, std::shared_ptr<BoundingShape> other);

		std::vector<XMFLOAT3> CuboidVertices();

		std::vector<Edge> CuboidEdges();

		std::vector<CuboidFace> CuboidFaces();

		static XMFLOAT3 CuboidFaceCentre(CuboidFace face);

		XMFLOAT3 CuboidFaceNormal(CuboidFace face);

		//To find the contact points on object1 with object2, call this method on object1 and pass in object2 as the argument
		std::vector<XMFLOAT3> ContactPointsTo(std::shared_ptr<BoundingShape> obj2);

		//Find closest point on obj2 from obj1 (caller of the method)
		XMFLOAT3 ClosestPointOn(std::shared_ptr<BoundingShape> obj2);

		BoundType GetType() { return type; }
	private:
		BoundType type;
		XMFLOAT3 position;
		XMFLOAT3 rotation;
		XMFLOAT3 dimensions;

	};
}