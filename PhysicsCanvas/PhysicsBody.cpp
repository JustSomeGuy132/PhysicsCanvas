#include "PhysicsBody.h"

using namespace PhysicsCanvas;

void PhysicsBody::Create(const UINT shape, const std::shared_ptr<DX::DeviceResources>& deviceResources) {
	CreateMesh(shape, deviceResources);
	position = XMFLOAT3();
	rotation = XMFLOAT3();
	obj_type = Kinematic;

	switch (shape) {
	case CUBE:
		dimensions = XMFLOAT3(1.0f, 1.0f, 1.0f);	//the faces are all 0.5 units away from the centre
		bounds = std::make_shared<BoundingShape>(BoundingShape::Cuboid, position, rotation, dimensions);
		break;
	case SPHERE:
		dimensions = XMFLOAT3(1.0f, 1.0f, 1.0f);	//radius from centre is 1 unit in all directions
		bounds = std::make_shared<BoundingShape>(BoundingShape::Sphere, position, rotation, dimensions);
		ApplyScale(XMFLOAT3(0.5f, 0.5f, 0.5f));
		break;
	case FLOOR:
		position = XMFLOAT3(0.0f, -2.0f, 0.0f);
		dimensions = XMFLOAT3(200.0f, 4.0f, 200.0f); //vertical faces are 100 from the centre, horizontal faces are 0.25
		bounds = std::make_shared<BoundingShape>(BoundingShape::Cuboid, position, rotation, dimensions);
		isFloor = true;
		break;
	}
	//if its not the floor, give a default mass of 1kg. else, make it very large
	if (shape != FLOOR) {
		mass = 1.0f;
		Force weight(mass);
		weight.SetFrom(position);
		AddEvent(std::make_shared<Force>(weight));
	}
	else {
		mass = 5.97e+24; // 5.97*10^24 kg
	}
}

Mesh& PhysicsBody::GetMesh() { return _mesh; }

std::string PhysicsBody::BodyData() {
	std::ostringstream data;
	data << "OBJECT KINEMATIC\n"
		<< "NAME " << name << "\n"
		<< "COL " << _mesh.GetColour().x << " " << _mesh.GetColour().y << " " << _mesh.GetColour().z << "\n"
		<< "SHAPE " << (bounds->GetType() == BoundingShape::Cuboid ? "Cuboid" : "Sphere") << "\n"
		<< "DIMS " << (bounds->GetType() == BoundingShape::Cuboid ? std::to_string(dimensions.x) + " "
			+ std::to_string(dimensions.y) + " "
			+ std::to_string(dimensions.z)
			: std::to_string(dimensions.x)) << "\n"
		<< "POS " << timeKeeper.Retrieve(0).position.x << " " << timeKeeper.Retrieve(0).position.y << " " << timeKeeper.Retrieve(0).position.z << "\n"
		<< "ROT " << timeKeeper.Retrieve(0).rotation.x << " " << timeKeeper.Retrieve(0).rotation.y << " " << timeKeeper.Retrieve(0).rotation.z << "\n"
		<< "VEL " << timeKeeper.Retrieve(0).velocity.x << " " << timeKeeper.Retrieve(0).velocity.y << " " << timeKeeper.Retrieve(0).velocity.z << "\n"
		<< "AVEL " << timeKeeper.Retrieve(0).ang_velocity.x << " " << timeKeeper.Retrieve(0).ang_velocity.y << " " << timeKeeper.Retrieve(0).ang_velocity.z << "\n"
		<< "MASS " << mass << "\n";
	for (std::shared_ptr<PEvent> e : pEvents) {
		Force* eForce = dynamic_cast<Force*>(e.get());
		if (eForce)
			data << eForce->EData() << "\n";
	}
	data << "ENDOBJECT\n";
	return data.str();
}

void PhysicsBody::SetTransform(XMFLOAT3 pos, XMFLOAT3 rot, XMFLOAT3 scale) {
	XMFLOAT3 posChange(pos.x - position.x, pos.y - position.y, pos.z - position.z);
	XMFLOAT3 rotChange(rot.x - rotation.x, rot.y - rotation.y, rot.z - rotation.z);
	//Update the fromPoint member on all forces
	for (std::shared_ptr<PEvent> e : pEvents) {
		Force* eForce = dynamic_cast<Force*>(e.get());
		if (eForce) {
			XMFLOAT3 rotPosChange(PhysMaths::RotateVector(PhysMaths::Float3Minus(eForce->GetFrom(), position), rotChange));
			eForce->SetFrom(PhysMaths::Float3Add(PhysMaths::Float3Add(eForce->GetFrom(), posChange), rotPosChange));
		}
	}

	position = pos;
	rotation = rot;
	dimensions = scale;

	_mesh.SetWorldMat(position, rotation, dimensions);
	bounds->SetPosition(position);
	bounds->SetRotation(rotation);
	bounds->SetDimensions(dimensions);
}
void PhysicsBody::ApplyTranslation(XMFLOAT3 translation) {
	if (isFloor) return;
	float _x = position.x + translation.x;
	float _y = position.y + translation.y;
	float _z = position.z + translation.z;
	SetTransform(XMFLOAT3(_x, _y, _z), rotation, dimensions);
}
void PhysicsBody::ApplyRotation(XMFLOAT3 rot) {
	if (isFloor) return;
	float _roll = rotation.x + rot.x;
	float _pitch = rotation.z + rot.z;
	float _yaw = rotation.y + rot.y;
	SetTransform(position, XMFLOAT3(_roll, _pitch, _yaw), dimensions);
}
void PhysicsBody::ApplyScale(XMFLOAT3 scale_) {
	dimensions.x = scale_.x;
	dimensions.y = scale_.y;
	dimensions.z = scale_.z;
	SetTransform(position, rotation, dimensions);
}

void PhysicsBody::SetMass(float m) {
	mass = m;
	for (std::shared_ptr<PEvent> e : pEvents) {
		if (e->GetId() == "Weight") {
			Force* eForce = dynamic_cast<Force*>(e.get());
			if (eForce) {
				eForce->SetDirection(XMFLOAT3(0.0f, -9.81f * mass, 0.0f));
				return;
			}
		}
	}
}


void PhysicsBody::AddForce(Force::ForceType type_, XMFLOAT3 dir_, float time) {	//if the parameters for a force object are passed in
	Force f(type_, dir_);
	f.SetStart(time);
	forces.push_back(f);
}

void PhysicsBody::AddEvent(std::shared_ptr<PEvent> e) {
	pEvents.push_back(e);
}

bool PhysicsBody::HasCollider(std::string n) {
	for (std::shared_ptr<PhysicsBody> coll : collisions) {
		if (coll->GetName() == n)
			return true;
	}
	return false;
}

void PhysicsBody::RegisterCollision(std::shared_ptr<PhysicsBody>& coll, float time) {
	//Handling reaction forces for the objects having contact
	std::vector<XMFLOAT3> contacts = bounds->ContactPointsTo(coll->GetBounds());
	XMFLOAT3 direction = {};
	switch (coll->GetBounds()->GetType()) {
	case BoundingShape::Sphere:
		direction = { coll->position.x - position.x, coll->position.y - position.y, coll->position.z - position.z };
		break;
	case BoundingShape::Cuboid:
		std::vector<CuboidFace> faces = coll->GetBounds()->CuboidFaces();
		//check this object's position relative to all of the faces
		for (CuboidFace face : faces) {
			if (PhysMaths::Float3CosTheta(
				coll->GetBounds()->CuboidFaceNormal(face), PhysMaths::Float3Minus(position, BoundingShape::CuboidFaceCentre(face))) >= 0) {

				direction = coll->GetBounds()->CuboidFaceNormal(face);
				break;
			}
		}
		break;
	}
	//store perpendicular distance of each contact point from the centre
	std::vector<float> perpDists;
	for (XMFLOAT3 Cpoint : contacts) {
		perpDists.push_back(PhysMaths::PerpendicularDist(PhysMaths::Float3Minus(Cpoint, position), direction));
	}
	float l = 0; //sum of all perDists
	for (float x : perpDists)
		l += x;
	//magnitude of reaction force, all the reactions will sum to this
	std::list<Force> allForcesExcludingReaction;
	for (Force f : ActiveForces(time)) {
		if (f.GetId().find(/*"Reaction force due to " + */coll->GetName()) == std::string::npos) {
			allForcesExcludingReaction.push_back(f);
		}
	}
	float reactionMag = abs(PhysMaths::Float3Dot(Force::ResultantF(allForcesExcludingReaction).GetDirection(), direction) / PhysMaths::Magnitude(direction));
	//now to create a reaction force at each contact point, scaling it according to perpendicular distance proportions
	int index = 0;
	for (XMFLOAT3 Cpoint : contacts) {
		std::ostringstream fName;
		fName << "Reaction force due to " + coll->GetName() + "(" << index << ")";
		Force reaction(Force::Reaction,
			PhysMaths::VecTimesByConstant(direction, (perpDists[perpDists.size() - index - 1] / l) * (reactionMag / PhysMaths::Magnitude(direction)))
		);
		reaction.SetId(fName.str());
		reaction.SetFrom(Cpoint);
		reaction.SetColour(XMFLOAT3(1, 0.1, 0.1));
		bool flag0 = false;
		for (Force& f : forces) {
			if (f.GetId() == reaction.GetId()) {
				flag0 = true;
				f = reaction;
				break;
			}
		}
		if (!flag0)
			forces.push_back(reaction);
		fName.flush();
		index++;
	}
	if (!HasCollider(coll->GetName())) {
		collisions.push_back(coll);
		timestamps.push_back(std::make_tuple(time, "Collision with " + coll->GetName()));
		//find direction from this to coll
		XMFLOAT3 dir = direction;
		dir = PhysMaths::VecDivByConstant(dir, pow(PhysMaths::Magnitude(dir), 2));
		dir.x *= -Momentum().x / 0.003f;	//
		dir.y *= -Momentum().y / 0.003f;	// F = p/t
		dir.z *= -Momentum().z / 0.003f;	//

		Force f(Force::Impulse, XMFLOAT3(dir.x, dir.y, dir.z));
		f.SetStart(time);
		std::string fName = "Collision force due to " + coll->GetName();
		f.SetId(fName);
		f.SetFrom(position);
		std::ostringstream outstr;
		bool flag = false;
		for (Force& f0 : forces) {
			if (f0.GetId() == fName) {
				flag = true;
				f0 = f;
				break;
			}
		}
		if (!flag)
			AddForce(f);
	}

}

void PhysicsBody::UpdateCollisionForces(float time) {
	for (std::vector<std::shared_ptr<PhysicsBody>>::iterator coll = collisions.begin(); coll != collisions.end();) {
		//if this body is no longer colliding with a body with which collisions were registered,
		for (Force& f : forces) {											//find the force which this collision caused
			if (!BoundingShape::PointCollidingWithObject(f.GetFrom(), (*coll)->GetBounds())) {
				if (f.GetId().find((*coll)->GetName()) != std::string::npos) {	//if we find it, and it has not already been deactivated,
					f.SetToggle(false);
				}
			}
		}
		if (!BoundingShape::IsColliding(bounds, (*coll)->GetBounds())) {
			coll = collisions.erase(coll);
		}
		else
			++coll;		//move the iterator forward if we don't deregister the current collider
	}
}

XMFLOAT3 PhysicsBody::Torque(float time) {
	XMFLOAT3 resultant(0, 0, 0);
	for (Force f : ActiveForces(time)) {
		XMFLOAT3 axis = PhysMaths::Float3Cross(
			XMFLOAT3(position.x - f.GetFrom().x, position.y - f.GetFrom().y, position.z - f.GetFrom().z), f.GetDirection());

		resultant = { resultant.x + axis.x, resultant.y + axis.y, resultant.z + axis.z };
	}
	return resultant;
}

std::list<Force> PhysicsBody::ActiveForces(float time) {
	std::list<Force> activeForces;
	for (std::shared_ptr<PEvent> e : pEvents) {
		if (e->GetEventType() == PEvent::Force && e->GetToggle()) {
			Force* eForce = dynamic_cast<Force*>(e.get());
			if (eForce) {
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
			if (f.GetForceType() == Force::Reaction) {
				if (f.GetToggle())
					activeForces.push_back(f);
			}
			else {
				if (f.GetEnd() <= time)
					activeForces.push_back(f);
			}
		}
	}
	return activeForces;
}

void PhysicsBody::Step(float time) {
	if (timeKeeper.Retrieve(time) != NULL_RECORD) {
		TimeJump(time);
		return;
	}
	UpdateCollisionForces(time);

	Force sumF = Force::ResultantF(ActiveForces(time));
	XMFLOAT3 a = sumF / mass;	// acceleration = Force / mass
	velocity.x = velocity.x + (a.x * 0.001f);	//each step increases time by 0.001s
	velocity.y = velocity.y + (a.y * 0.001f);	//calculating u for next Step()
	velocity.z = velocity.z + (a.z * 0.001f);	//v = u + at
	//s = ut + 1/2 at^2
	XMFLOAT3 translation(
		(velocity.x * 0.001f) + (0.5f * a.x * 0.001f * 0.001f),
		(velocity.y * 0.001f) + (0.5f * a.y * 0.001f * 0.001f),
		(velocity.z * 0.001f) + (0.5f * a.z * 0.001f * 0.001f)
	);
	ApplyTranslation(translation);

	XMFLOAT3 ang_a = PhysMaths::VecDivByConstant(Torque(time), mass);
	ang_velocity.x += ang_a.x * 0.001f;
	ang_velocity.y += ang_a.y * 0.001f;
	ang_velocity.z += ang_a.z * 0.001f;
	//theta = omega(t) + 1/2 aplha(t)^2
	XMFLOAT3 rot(
		(ang_velocity.x * 0.001f) + (0.5f * ang_a.x * 0.001f * 0.001f),
		(ang_velocity.y * 0.001f) + (0.5f * ang_a.y * 0.001f * 0.001f),
		(ang_velocity.z * 0.001f) + (0.5f * ang_a.z * 0.001f * 0.001f)
	);
	ApplyRotation(rot);

	timeKeeper.RecordData(time, position, rotation, velocity, ang_velocity);
}

void PhysicsBody::TimeJump(float time) {
	if (timeKeeper.Retrieve(time) == NULL_RECORD)
		return;

	Record& r = timeKeeper.Retrieve(time);
	SetTransform(r.position, r.rotation, dimensions);
	velocity = r.velocity;
	ang_velocity = r.ang_velocity;
	UpdateCollisionForces(time);
}
