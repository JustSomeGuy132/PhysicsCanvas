#pragma once
#include "pch.h"
#include "..\Common\DeviceResources.h"
#include "..\Common\DirectXHelper.h"
#include "..\Content\ShaderStructures.h"
#include "Mesh.h"
#include <list>
#include "Force.h"

namespace PhysicsCanvas {
	class PhysicsBody {
	public:
		void Create(const std::shared_ptr<DX::DeviceResources>& deviceResources) {
			CreateMesh(deviceResources);
			SetTransform(DirectX::XMFLOAT3(), DirectX::XMFLOAT3());
			mass = 1.0f;
			AddForce(mass);
		}

		void CreateMesh(const std::shared_ptr<DX::DeviceResources>& deviceResources) {
			mesh.Create(deviceResources);
		}
		void Render(DirectX::XMMATRIX viewprojMat) {
			mesh.Render(viewprojMat);
		}
		void SetTransform(DirectX::XMFLOAT3 pos, DirectX::XMFLOAT3 rot) {
			position = pos;
			rotation = rot;
			mesh.SetWorldMat(position, rotation);
		}

		void AddForce(Force f) {	//if a whole force object is passed in
			forces.push_front(f);
		}
		void AddForce(Force::ForceType type_, DirectX::XMFLOAT3 dir_) {	//if the parameters for a force object are passed in
			Force f(type_, dir_);
			forces.push_front(f);
		}
		void AddForce(float m) {	//if only mass is passed in, so basically its the weight of the object
			Force f(m);
			forces.push_front(f);
		}

		void Step(float time) {
			std::list<Force> activeForces;
			for (Force f : forces) {
				if (time >= f.GetStart()) {
					//if it's weight OR if it's no longer meant to be applied
					if(f.GetEnd() == -1 || time <= f.GetEnd()) {
						activeForces.push_front(f);
					}
				}
			}
			Force sumF = Force::ResultantF(activeForces);
			DirectX::XMFLOAT3 a = sumF / mass;	// acceleration = Force / mass
			//s = ut + at^2
			DirectX::XMFLOAT3 translation(
				(velocity.x * time) + (0.5f * a.x * time * time),
				(velocity.y * time) + (0.5f * a.y * time * time),
				(velocity.z * time) + (0.5f * a.z * time * time)
			);
			velocity.x = velocity.x + (a.x * 0.001f);	//each step increases time by 0.001s
			velocity.y = velocity.y + (a.y * 0.001f);	//calculating u for next Step()
			velocity.z = velocity.z + (a.z * 0.001f);	//v = u + at
			
			ApplyTranslation(translation);
		}
		void RevStep(float time);


		void ApplyTranslation(DirectX::XMFLOAT3 translation) {
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

		void ReleaseResources() {
			mesh.ReleaseResources();
		}
		
	private:
		Mesh mesh;
		DirectX::XMFLOAT3 position;
		DirectX::XMFLOAT3 rotation;
		
		std::list<PEvent> events;

		std::list<Force> forces;
		DirectX::XMFLOAT3 velocity;

		//have to change later since wave bodies, if implemented, will not have these

		float mass;
		float volume;
	};

}