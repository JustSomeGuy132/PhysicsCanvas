#pragma once
#include "pch.h"
#include "..\Common\DeviceResources.h"
#include "..\Common\DirectXHelper.h"
#include "..\Content\ShaderStructures.h"
#include "Mesh.h"
#include <list>
#include "Force.h"
#include "BoundingShape.h"
#include <sstream>



namespace PhysicsCanvas {
	class PhysicsBody {
	public:
		void Create(const UINT shape, const std::shared_ptr<DX::DeviceResources>& deviceResources) {
			CreateMesh(shape, deviceResources);
			position = DirectX::XMFLOAT3();
			rotation = DirectX::XMFLOAT3();
			
			switch (shape) {
			case CUBE:
				dimensions = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);	//the faces are all 0.5 units away from the centre
				bounds = std::make_shared<BoundingShape>(BoundingShape::Cube, position, rotation, dimensions);
				break;
			case SPHERE:
				dimensions = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);	//radius from centre is 1 unit in all directions
				bounds = std::make_shared<BoundingShape>(BoundingShape::Sphere, position, rotation, dimensions);
				break;
			case FLOOR:
				position = DirectX::XMFLOAT3(0.0f, -2.0f, 0.0f);
				dimensions = DirectX::XMFLOAT3(200.0f, 4.0f, 200.0f); //vertical faces are 100 from the centre, horizontal faces are 0.25
				bounds = std::make_shared<BoundingShape>(BoundingShape::Cube, position, rotation, dimensions);
				isFloor = true;
				break;
			}
			//if its not the floor, give a default mass of 1kg. else, make it very large
			if(shape != FLOOR) {
				mass = 1.0f;
				AddForce(mass);
			}
			else {
				mass = 10000000000000000.0f;
			}
		}

		std::string GetName() { return name; }
		void GiveName(std::string n) { name = n; }

		void CreateMesh(const UINT shape, const std::shared_ptr<DX::DeviceResources>& deviceResources) {
			_mesh.Create(shape, deviceResources);
		}
		void Render(DirectX::XMMATRIX viewprojMat) {
			_mesh.Render(viewprojMat);
		}
		void SetTransform(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rot) {
			position = pos;
			rotation = rot;
			_mesh.SetWorldMat(position, rotation);
			bounds->SetPosition(pos);
			bounds->SetRotation(rot);
		}

		float GetMass() { return mass; }

		DirectX::XMFLOAT3 GetPosition() { return position; }
		DirectX::XMFLOAT3 GetRotation() { return rotation; }
		DirectX::XMFLOAT3 GetDimensions() { return dimensions; }
		DirectX::XMFLOAT3 GetVelocity() { return velocity; }

		DirectX::XMFLOAT3 Momentum() {
			return DirectX::XMFLOAT3(mass * velocity.x, mass * velocity.y, mass * velocity.z);
		}

		void AddForce(Force f) {	//if a whole force object is passed in
			forces.push_back(f);
		}
		void AddForce(Force::ForceType type_, DirectX::XMFLOAT3 dir_, float time) {	//if the parameters for a force object are passed in
			Force f(type_, dir_);
			f.SetStart(time);
			forces.push_back(f);
		}
		void AddForce(float m) {	//if only mass is passed in, so basically its the weight of the object
			Force f(m);
			std::string n = "Weight of " + GetName();
			f.SetId(n);
			forces.push_back(f);
		}
		bool HasForce(std::string n) {
			for (Force f : forces) {
				if (f.GetId() == n) {
					return true;
				}
			}
			return false;
		}
		bool HasExactForce(Force _for) {
			for (Force f : forces) {
				if (f == _for) {
					return true;
				}
			}
			return false;
		}
		
		bool HasCollider(std::string n) {
			for (std::tuple<std::shared_ptr<PhysicsBody>, std::string> coll : collisions) {
				if (std::get<0>(coll)->GetName() == n)
					return true;
			}
			return false;
		}

		void RegisterCollision(std::shared_ptr<PhysicsBody>& coll, float time) {
			if (HasCollider(coll->GetName())) {
				return;
			} 
			
			//handle reaction forces for the objects having contact
			std::string fName = "Reaction force due to " + coll->GetName();
			for (Force f : forces) {
				if (f.GetId() == fName && time < f.GetEnd()) {
					return; //if a force due to this collision already exists and is currently being applied, do not register a new collision
				}
			}
			collisions.push_back(std::make_tuple(coll, fName));	//register the PhysicsBody "coll" as one that is colliding with this body
			DirectX::XMFLOAT3 Rdir(coll->position.x - position.x, coll->position.y - position.y, coll->position.z - position.z);
			Rdir.x *= ResultantActiveForces(time).GetDirection().x;
			Rdir.y *= ResultantActiveForces(time).GetDirection().y;
			Rdir.z *= ResultantActiveForces(time).GetDirection().z;
			Force reactionF(Force::Constant, Rdir);
			reactionF.SetId(fName);
			reactionF.SetStart(time);
			reactionF.SetEnd(-0.5f);
			AddForce(reactionF);
			std::ostringstream oss;
			oss << "Added a collision to " << GetName() << ", force(" << Rdir.x << ", " << Rdir.y << ", " << Rdir.z << ")\n";
			OutputDebugString(oss.str().c_str());
			
			//find direction from this to coll
			DirectX::XMFLOAT3 dir(position.x - coll->GetPosition().x,
					position.y - coll->GetPosition().y,
					position.z - coll->GetPosition().z
			);
			dir.x *= Momentum().x / 0.002f;	//
			dir.y *= Momentum().y / 0.002f;	// F = p/t
			dir.z *= Momentum().z / 0.002f;	//

			Force f(Force::Impulse, DirectX::XMFLOAT3(-dir.x, -dir.y, -dir.z));
			f.SetStart(time);
			fName = "Collision force due to " + coll->GetName();
			f.SetId(fName);
			std::ostringstream outstr;
			AddForce(f);
			outstr << "Added force --" << fName << "-- to " << GetName()
				<< " With direction = (" << -dir.x << ", " << -dir.y << ", " << -dir.z << ")" << "\n";
			OutputDebugString(outstr.str().c_str());
		}

		void UpdateCollisionForces(float time) {
			for (std::vector<std::tuple<std::shared_ptr<PhysicsBody>, std::string>>::iterator coll = collisions.begin(); coll != collisions.end();) {
				//if this body is no longer colliding with a body with which collisions were registered,
				if (!BoundingShape::IsColliding(bounds, std::get<0>(*coll)->GetBounds())) {
					for (Force& f : forces) {											//find the force which this collision caused
						if (f.GetId() == std::get<1>(*coll) && f.GetEnd() == -0.5f) {	//if we find it, and it has not already been deactivated,
							f.SetEnd(time);												//set that force to end at this time, deactivating it
							coll = collisions.erase(coll);
							OutputDebugString("REMOVED A COLLIDER");
							break;
						}
					}
				}
				else
					++coll;		//move the iterator forward if we don't deregister the current collider
			}
		}

		Force ResultantActiveForces(float time) {
			std::list<Force> activeForces;
			for (Force f : forces) {
				if (time >= f.GetStart()) {
					//if it's weight OR if it's a reaction force OR if it's a different type that is still meant to be applied
					if (f.GetEnd() == -1 || f.GetEnd() == -0.5f || time < f.GetEnd()) {
						activeForces.push_back(f);
					}
				}
			}
			return Force::ResultantF(activeForces);
		}

		void Step(float time) {
			UpdateCollisionForces(time);
			
			Force sumF = ResultantActiveForces(time);
			DirectX::XMFLOAT3 a = sumF / mass;	// acceleration = Force / mass
			velocity.x = velocity.x + (a.x * 0.001f);	//each step increases time by 0.001s
			velocity.y = velocity.y + (a.y * 0.001f);	//calculating u for next Step()
			velocity.z = velocity.z + (a.z * 0.001f);	//v = u + at
			//s = ut + at^2
			DirectX::XMFLOAT3 translation(
				(velocity.x * 0.001f) + (0.5f * a.x * 0.001f * 0.001f),
				(velocity.y * 0.001f) + (0.5f * a.y * 0.001f * 0.001f),
				(velocity.z * 0.001f) + (0.5f * a.z * 0.001f * 0.001f)
			);
			std::ostringstream oss;
			oss << "Velocity of " << GetName() << " = (" << velocity.x << ", " << velocity.y << ", " << velocity.z << ")\n"
				<< "Where resultant force = (" << sumF.GetDirection().x << ", " << sumF.GetDirection().y << ", " << sumF.GetDirection().z << ")\n"
				<< "Position = (" << position.x << ", " << position.y << ", " << position.z << ")\n";
			OutputDebugString(oss.str().c_str());
			std::list<Force> activeForces;
			for (Force f : forces) {
				if (time >= f.GetStart()) {
					//if it's weight OR if it's a different type that is still meant to be applied
					if (f.GetEnd() == -1 || f.GetEnd() == -0.5f || time < f.GetEnd()) {
						activeForces.push_back(f);
					}
				}
			}
			for (Force foo : activeForces) {
				std::ostringstream str;
				str << foo.GetId() << "= (" << foo.GetDirection().x << ", " << foo.GetDirection().y << ", " << foo.GetDirection().z  << ")\n";
				OutputDebugString(str.str().c_str());
			}
			ApplyTranslation(translation);
		}
		void RevStep(float time);


		std::shared_ptr<BoundingShape> GetBounds() { return bounds; }

		void ApplyTranslation(DirectX::XMFLOAT3 translation) {
			if (isFloor) return;
			float _x = position.x + translation.x;
			float _y = position.y + translation.y;
			float _z = position.z + translation.z;
			SetTransform(DirectX::XMFLOAT3(_x, _y, _z), rotation);
		}
		void ApplyRotation(DirectX::XMFLOAT3 rot) {
			float _roll = rotation.x + rot.x;
			float _pitch = rotation.y + rot.y;
			float _yaw = rotation.z + rot.z;
			SetTransform(position, DirectX::XMFLOAT3(_roll, _pitch, _yaw));
		}
		void ApplyScale(DirectX::XMFLOAT3 scale_) {
			dimensions.x += scale_.x;
			dimensions.y += scale_.y;
			dimensions.z += scale_.z;
			_mesh.Scale(dimensions);
			bounds->SetDimensions(dimensions);
		}

		
		void ReleaseResources() {
			_mesh.ReleaseResources();
		}
		
	private:
		Mesh _mesh;
		std::string name;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 rotation;
		DirectX::XMFLOAT3 dimensions;

		//for collisions, register the collision body as well as the name of the subsequent collision
		std::vector<std::tuple<std::shared_ptr<PhysicsBody>, std::string>> collisions;

		std::shared_ptr<BoundingShape> bounds;
		bool isFloor = false;
		std::list<Force> forces;
		DirectX::XMFLOAT3 velocity;

		//have to change later since wave bodies, if implemented, will not have these

		float mass;
		float volume;
	};

}