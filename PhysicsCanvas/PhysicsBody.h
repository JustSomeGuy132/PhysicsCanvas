#pragma once
#include "pch.h"
#include "..\Common\DeviceResources.h"
#include "..\Common\DirectXHelper.h"
#include "..\Content\ShaderStructures.h"
#include "Mesh.h"
#include <list>
#include "PEvent.h"
#include "Force.h"
#include "BoundingShape.h"
#include "PhysMaths.h"
#include "TimeKeeper.h"
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
				bounds = std::make_shared<BoundingShape>(BoundingShape::Cuboid, position, rotation, dimensions);
				break;
			case SPHERE:
				dimensions = DirectX::XMFLOAT3(1.0f, 1.0f, 1.0f);	//radius from centre is 1 unit in all directions
				bounds = std::make_shared<BoundingShape>(BoundingShape::Sphere, position, rotation, dimensions);
				ApplyScale(DirectX::XMFLOAT3(0.5f, 0.5f, 0.5f));
				break;
			case FLOOR:
				position = DirectX::XMFLOAT3(0.0f, -2.0f, 0.0f);
				dimensions = DirectX::XMFLOAT3(200.0f, 4.0f, 200.0f); //vertical faces are 100 from the centre, horizontal faces are 0.25
				bounds = std::make_shared<BoundingShape>(BoundingShape::Cuboid, position, rotation, dimensions);
				isFloor = true;
				break;
			}
			//if its not the floor, give a default mass of 1kg. else, make it very large
			if(shape != FLOOR) {
				mass = 1.0f;
				Force weight(mass);
				weight.SetFrom(position);
				AddEvent(std::make_shared<Force>(weight));
			}
			else {
				mass = 5970000000000000000000000.0f; // 5.97*10^24 kg
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
		Mesh& GetMesh() { return _mesh; }

		void SetTransform(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rot, DirectX::XMFLOAT3 scale) {
			position = pos;
			rotation = rot;
			dimensions = scale;

			_mesh.SetWorldMat(position, rotation, dimensions);
			bounds->SetPosition(position);
			bounds->SetRotation(rotation);
			bounds->SetDimensions(dimensions);
		}
		void ApplyTranslation(DirectX::XMFLOAT3 translation) {
			if (isFloor) return;
			float _x = position.x + translation.x;
			float _y = position.y + translation.y;
			float _z = position.z + translation.z;

			//Update the fromPoint member on all forces
			for (std::shared_ptr<PEvent> e : pEvents) {
				Force* eForce = dynamic_cast<Force*>(e.get());
				if (eForce) {
					eForce->SetFrom(DirectX::XMFLOAT3(eForce->GetFrom().x + translation.x, eForce->GetFrom().y + translation.y, eForce->GetFrom().z + translation.z));
				}
			}
			for (Force& f : forces) {
				f.SetFrom(DirectX::XMFLOAT3(f.GetFrom().x + translation.x, f.GetFrom().y + translation.y, f.GetFrom().z + translation.z));
			}

			SetTransform(DirectX::XMFLOAT3(_x, _y, _z), rotation, dimensions);
		}
		void ApplyRotation(DirectX::XMFLOAT3 rot) {
			float _roll = rotation.x + rot.x;
			float _pitch = rotation.y + rot.y;
			float _yaw = rotation.z + rot.z;
			SetTransform(position, DirectX::XMFLOAT3(_roll, _pitch, _yaw), dimensions);
		}
		void ApplyScale(DirectX::XMFLOAT3 scale_) {
			dimensions.x = scale_.x;
			dimensions.y = scale_.y;
			dimensions.z = scale_.z;
			SetTransform(position, rotation, dimensions);
		}

		float GetMass() { return mass; }
		void SetMass(float m) {
			mass = m;
			for (std::shared_ptr<PEvent> e : pEvents) {
				if (e->GetId() == "Weight") {
					Force* eForce = dynamic_cast<Force*>(e.get());
					if(eForce) {
						eForce->SetDirection(DirectX::XMFLOAT3(0.0f, -9.81f * mass, 0.0f));
						return;
					}
				}
			}
		}

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
		
		void AddEvent(std::shared_ptr<PEvent> e) {
			pEvents.push_back(e);
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
			std::vector<DirectX::XMFLOAT3> contacts = bounds->ContactPointsTo(coll->GetBounds());
			DirectX::XMFLOAT3 direction;
			switch (coll->GetBounds()->GetType()) {
			case BoundingShape::Sphere:
				direction = { coll->position.x - position.x, coll->position.y - position.y, coll->position.z - position.z };
				break;
			case BoundingShape::Cuboid:
				std::vector<CuboidFace> faces = coll->GetBounds()->CuboidFaces();
				//if this object is to the right of coll
				if (position.x > coll->GetBounds()->CuboidFaceCentre(faces[0]).x){
					direction = coll->GetBounds()->CuboidFaceNormal(faces[0]);
				}
				//if this object is to the left of coll
				else if (position.x < coll->GetBounds()->CuboidFaceCentre(faces[4]).x) {
					direction = coll->GetBounds()->CuboidFaceNormal(faces[4]);
				}
				//if this object is in front of coll
				else if (position.z > coll->GetBounds()->CuboidFaceCentre(faces[2]).z) {
					direction = coll->GetBounds()->CuboidFaceNormal(faces[2]);
				}
				//if this object is behind coll
				else if (position.z < coll->GetBounds()->CuboidFaceCentre(faces[3]).z) {
					direction = coll->GetBounds()->CuboidFaceNormal(faces[3]);
				}
				//if this object is above coll
				else if (position.y > coll->GetBounds()->CuboidFaceCentre(faces[1]).y) {
					direction = coll->GetBounds()->CuboidFaceNormal(faces[1]);
				}
				//if this object is below coll
				else if (position.y < coll->GetBounds()->CuboidFaceCentre(faces[5]).y) {
					direction = coll->GetBounds()->CuboidFaceNormal(faces[5]);
				}
				break;
			}
			//store perpendicular distance of each contact point from the centre
			std::vector<float> perpDists;
			for (DirectX::XMFLOAT3 Cpoint : contacts) {
				perpDists.push_back(PhysMaths::PerpendicularDist(DirectX::XMFLOAT3(Cpoint.x - position.x, Cpoint.y - position.y, Cpoint.z - position.z), direction));
			}
			float l = 0; //sum of all perDists
			for (float x : perpDists)
				l += x;
			//magnitude of reaction force, all the reactions will sum to this
			float reactionMag = abs(PhysMaths::Float3Dot(Force::ResultantF(ActiveForces(time)).GetDirection(), direction) / PhysMaths::Magnitude(direction));
			//now to create a reaction force at each contact point, scaling it according to perpendicular distance proportions
			int index = 0;
			for (DirectX::XMFLOAT3 Cpoint : contacts) {
				std::ostringstream fName;
				fName << "Reaction force due to " + coll->GetName() + "(" << index << ")";
				Force reaction(Force::Reaction,
					PhysMaths::VecTimesByConstant(direction, (perpDists[perpDists.size() - index - 1] / l) * (reactionMag / PhysMaths::Magnitude(direction)))
				);
				reaction.SetId(fName.str());
				reaction.SetFrom(Cpoint);
				bool flag0 = false;
				for (Force f : forces) {
					if (f.GetId() == reaction.GetId()) {
						flag0 = true;
						break;
					}
				}
				if(!flag0)
					forces.push_back(reaction);
				fName.flush();
				index++;
			}
			//find direction from this to coll
			DirectX::XMFLOAT3 dir(position.x - coll->GetPosition().x,
					position.y - coll->GetPosition().y,
					position.z - coll->GetPosition().z
			);
			dir = PhysMaths::VecDivByConstant(dir, PhysMaths::Magnitude(dir));
			dir.x *= -Momentum().x / 0.003f;	//
			dir.y *= -Momentum().y / 0.003f;	// F = p/t
			dir.z *= -Momentum().z / 0.003f;	//

			Force f(Force::Impulse, DirectX::XMFLOAT3(dir.x, dir.y, dir.z));
			f.SetStart(time);
			std::string fName = "Collision force due to " + coll->GetName();
			f.SetId(fName);
			std::ostringstream outstr;
			bool flag = false;
			for (Force& f0 : forces) {
				if (f0.GetId() == fName) {
					f0 = f;
					flag = true;
					break;
				}
			}
			if (!flag)
				AddForce(f);
		}

		void UpdateCollisionForces(float time) {
			for (std::vector<std::tuple<std::shared_ptr<PhysicsBody>, std::string>>::iterator coll = collisions.begin(); coll != collisions.end();) {
				//if this body is no longer colliding with a body with which collisions were registered,
				if (!BoundingShape::IsColliding(bounds, std::get<0>(*coll)->GetBounds())) {
					for (Force& f : forces) {											//find the force which this collision caused
						if (f.GetId().find(std::get<1>(*coll)) != std::string::npos 
							&& f.GetForceType() == Force::Reaction) {	//if we find it, and it has not already been deactivated,
							
							f.SetForceType(Force::Constant);
							f.SetEnd(time);												//set that force to end at this time, deactivating it
							coll = collisions.erase(coll);
							break;
						}
					}
				}
				else
					++coll;		//move the iterator forward if we don't deregister the current collider
			}
		}

		std::list<Force> ActiveForces(float time) {
			std::list<Force> activeForces;
			for (std::shared_ptr<PEvent> e : pEvents) {
				if (e->GetEventType() == PEvent::Force && e->GetToggle()) {
					Force* eForce = dynamic_cast<Force*>(e.get());
					if(eForce) {
						if (time >= eForce->GetStart()) {
							if (eForce->GetForceType() == Force::Weight || time < eForce->GetEnd()) {
								activeForces.push_back(*eForce);
							}
						}
					}
				}
			}
			for (Force f : forces) {
				if (time >= f.GetStart()) {
					//if if it's a reaction force OR if it's a different type that is still meant to be applied
					if (f.GetForceType() == Force::Reaction || time < f.GetEnd()) {
						activeForces.push_back(f);
					}
				}
			}
			return activeForces;
		}
		std::list<Force> GetForces() { return forces; }
		std::vector<std::shared_ptr<PEvent>> GetEvents() { return pEvents; }

		void Step(float time) {
			UpdateCollisionForces(time);
			
			Force sumF = Force::ResultantF(ActiveForces(time));
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
			
			ApplyTranslation(translation);
		}


		std::shared_ptr<BoundingShape> GetBounds() { return bounds; }

		
		
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
		float mass;
		float volume;

		std::vector<std::shared_ptr<PEvent>> pEvents;
		TimeKeeper timeKeeper;
		
	};

}