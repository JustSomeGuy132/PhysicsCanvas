#pragma once
#include "pch.h"
#include "..\Common\DeviceResources.h"
#include "..\Common\DirectXHelper.h"
#include "Mesh.h"
#include <list>
#include "PEvent.h"
#include "Force.h"
#include "BoundingShape.h"
#include "PhysMaths.h"
#include "TimeKeeper.h"
#include <sstream>

using namespace DirectX;

namespace PhysicsCanvas {
	class PhysicsBody {
	public:
		static enum o_type {
			Kinematic,
		};

		virtual void Create(const UINT shape, const std::shared_ptr<DX::DeviceResources>& deviceResources);

		std::string GetName() { return name; }
		void GiveName(std::string n) { name = n; }

		o_type GetType() { return obj_type; }

		virtual void CreateMesh(const UINT shape, const std::shared_ptr<DX::DeviceResources>& deviceResources) {
			_mesh.Create(shape, deviceResources);
		}
		virtual void Render(XMMATRIX viewprojMat) {
			_mesh.Render(viewprojMat);
		}
		Mesh& GetMesh() { return _mesh; }

		std::string BodyData();

		void SetTransform(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scale);
		
		void ApplyTranslation(XMFLOAT3 translation);
		
		void ApplyRotation(XMFLOAT3 rot);

		void ApplyScale(XMFLOAT3 scale_);

		float GetMass() { return mass; }

		void SetMass(float m);

		XMFLOAT3 GetPosition() { return position; }
		
		XMFLOAT3 GetRotation() { return rotation; }
		
		XMFLOAT3 GetDimensions() { return dimensions; }
		
		XMFLOAT3 GetVelocity() { return velocity; }
		
		void SetVelocity(XMFLOAT3 vel) { velocity = vel; }
		
		XMFLOAT3 GetAngularVelocity() { return ang_velocity; }
		
		void SetAngVelocity(XMFLOAT3 aVel) { ang_velocity = aVel; }
		
		std::list<Force>& GetForces() { return forces; }
		
		std::vector<std::shared_ptr<PEvent>> GetEvents() { return pEvents; }
		
		std::shared_ptr<BoundingShape> GetBounds() { return bounds; }
		
		TimeKeeper& GetTimeKeeper() { return timeKeeper; }
		
		std::vector<std::tuple<float, std::string>>& GetTimestamps() { return timestamps; }

		void AddForce(Force f) {	//if a whole force object is passed in
			forces.push_back(f);
		}

		void AddForce(Force::ForceType type_, XMFLOAT3 dir_, float time);

		void AddEvent(std::shared_ptr<PEvent> e);

		bool HasCollider(std::string n);

		void RegisterCollision(std::shared_ptr<PhysicsBody>& coll, float time);

		void UpdateCollisionForces(float time);

		XMFLOAT3 Torque(float time);

		std::list<Force> ActiveForces(float time);

		void Step(float time);

		void TimeJump(float time);

		XMFLOAT3 Momentum() {
			return XMFLOAT3(mass * velocity.x, mass * velocity.y, mass * velocity.z);
		}

		float KineticEnergy() {
			return (0.5f * mass * pow(PhysMaths::Magnitude(velocity), 2)) + (0.5f * mass * pow(PhysMaths::Magnitude(ang_velocity), 2));
		}

		float RelativeGPEnergy() {
			return (mass * 9.81 * position.y);
		}
		
		void ReleaseResources() {
			_mesh.ReleaseResources();
		}
		
	private:
		Mesh _mesh;
		std::string name;
		XMFLOAT3 position;
		XMFLOAT3 rotation;
		XMFLOAT3 dimensions;

		o_type obj_type;

		//for collisions, register the collision body ~~as well as the name of the subsequent collision~~
		std::vector<std::shared_ptr<PhysicsBody>> collisions;

		std::shared_ptr<BoundingShape> bounds;
		bool isFloor = false;
		std::list<Force> forces;
		XMFLOAT3 velocity;
		XMFLOAT3 ang_velocity;
		float mass;
		float volume;

		std::vector<std::shared_ptr<PEvent>> pEvents;
		TimeKeeper timeKeeper;
		std::vector<std::tuple<float, std::string>> timestamps;
	};

}