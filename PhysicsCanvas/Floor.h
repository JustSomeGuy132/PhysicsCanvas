#pragma once
#include "PhysicsBody.h"

using namespace DirectX;
namespace PhysicsCanvas {
	class Floor : public PhysicsBody {
	public:
		void Create(const std::shared_ptr<DX::DeviceResources> deviceResources) {
			position = XMFLOAT3(0.0f, -2.0f, 0.0f);
			dimensions = XMFLOAT3(200.0f, 4.0f, 200.0f); //vertical faces are 100 from the centre, horizontal faces are 0.25
			bounds = std::make_shared<BoundingShape>(BoundingShape::Cuboid, position, rotation, dimensions);
			mass = 5.97e+24; // 5.97*10^24 kg
			obj_type = Kinematic;
			CreateMesh(deviceResources);
			GiveName("FLOOR");
		}
		void CreateMesh(const std::shared_ptr<DX::DeviceResources> deviceResources) {
			_mesh.Create(FLOOR, deviceResources);
		}
		void Render(XMMATRIX viewProjMat) {
			_mesh.Render(viewProjMat);
			OutputDebugString("WE ARE RENDERING THE FLOOR\n");
		}
		void ApplyTranslation(){}
		void ApplyRotation(){}
		void SetMass(){}

		std::shared_ptr<BoundingShape> GetBounds() {
			return bounds;
		}
	private:
		XMFLOAT3 position;
		XMFLOAT3 dimensions;
		XMFLOAT3 rotation;
		float mass;
		std::shared_ptr<BoundingShape> bounds;
		Mesh _mesh;
		o_type obj_type;
	};
}
