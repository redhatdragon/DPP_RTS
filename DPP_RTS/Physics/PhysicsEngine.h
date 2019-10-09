#include "SpatialHashTable.h"
#include <stdio.h>
#include <iostream>
#include <vector>

struct PhysicsBody {
	//int32_t x, y, w, h, vx, vy;
	struct Point location, scale, velocity;
	uint8_t isSolid, isDynamic;
	uint16_t garbage;
	uint32_t ID;
	void* data;
	bool isColliding(const PhysicsBody* other) {
		if (location.x > other->location.x + other->scale.x || location.x + scale.x < other->location.x
			|| location.y > other->location.y + other->scale.y || location.y + scale.y < other->location.y) {
			return false;
		}
		return true;
	}
};

class PhysicsEngine {
	//static std::vector<PhysicsEngine*> engine;
	static PhysicsEngine* engine[10000];
	static uint32_t engineCount;
	SpatialHashTable spatialHashTable = SpatialHashTable(100 * 2*5, 100 * 2*5, 5, 5);
	PhysicsBody* body;
	std::vector <std::vector<PhysicsBody*>> overlap = std::vector <std::vector<PhysicsBody*>>();
	uint32_t bodyCount = 0;
	uint32_t bodyCapacity;
	int32_t* IDIndex;
public:
	float gravityDensity = 0;
	PhysicsEngine(uint32_t _bodyCapacity) {
		bodyCapacity = _bodyCapacity;
		body = new PhysicsBody[_bodyCapacity];
		IDIndex = new int32_t[_bodyCapacity];
		memset(IDIndex, -1, _bodyCapacity);
		for (unsigned int i = 0; i < _bodyCapacity; i++) {
			overlap.push_back(std::vector<PhysicsBody*>());
		}
	}
	const PhysicsBody* getBodyData(int32_t bodyID) {
		return &body[IDIndex[bodyID]];
	}
	static uint32_t initEngine(uint32_t _bodyCapacity) {
		uint32_t engineIndex = engineCount;
		engine[engineCount] = new PhysicsEngine(_bodyCapacity);
		engineCount++;
		return engineIndex;
	}
	static PhysicsEngine* getEngine(uint32_t engineID) {
		return engine[engineID];
	}
	inline int32_t addBody(PhysicsBody newBody) {
		body[bodyCount] = newBody;
		for (uint32_t i = 0; i < bodyCapacity; i++) {
			if (IDIndex[i] == -1) {
				IDIndex[i] = bodyCount;
				body[bodyCount].ID = i;
				spatialHashTable.insertBody(&body[bodyCount], (Rect*)& body[bodyCount].location);
				bodyCount++;
				return i;
			}
		}
		std::cout << this << std::endl;
		std::cout << bodyCount << std::endl;
		std::cout << bodyCapacity << " " << IDIndex[2] << std::endl;
		exit(1);
		return -1;
	}
	inline void removeBody(int32_t id) {
		int32_t index = IDIndex[id];
		//PhysicsBody *b = &body[index];
		//IDIndex[id] = -1;  ////probably what caused the crashing.
		//IDIndex[b->ID] = index;
		//IDIndex[body[index].ID] = index;  //probably what caused the crashing.
		bodyCount--;
		IDIndex[body[bodyCount].ID] = -1;  //Not sure yet...
		spatialHashTable.removeBody(&body[index], (Rect*)& body[index].location);
		memcpy(&body[index], &body[bodyCount], sizeof(PhysicsBody));
	}
	inline std::vector<int32_t> getOverlappingBodyIDs(int32_t id) {
		int32_t index = IDIndex[id];
		std::vector<int32_t> vectorToReturn = std::vector<int32_t>();
		size_t length = overlap[index].size();
		for (size_t i = 0; i < length; i++) {
			vectorToReturn.push_back(overlap[index][i]->ID);
		}
		return vectorToReturn;
	}
	void setBodyVelocity(int32_t id, float vx, float vy) {
		if (isnan(vx)) vx = 0;
		if (isnan(vy)) vy = 0;
		PhysicsBody* b = &body[IDIndex[id]];
		b->velocity.x = vx;
		b->velocity.y = vy;
	}
	void tick() {
		unsigned int bCount = bodyCount;
		for (unsigned int i = 0; i < bCount; i++) {
			overlap[i].clear();
		}

		for (unsigned int i = 0; i < bCount; i++) {
			PhysicsBody* currentBody = &body[i];
			if (!currentBody->isDynamic) continue;
			spatialHashTable.moveBody(currentBody, (Rect*)& currentBody->location,
				&currentBody->velocity);
			std::vector<void*> broadlyCollidingBodies =
				spatialHashTable.getBodies((Rect*)& currentBody->location);
			for (size_t j = 0; j < broadlyCollidingBodies.size(); j++) {
				PhysicsBody* otherBody = (PhysicsBody*)broadlyCollidingBodies[j];
				if (&body[i] == otherBody) continue;
				if (body[i].isColliding(otherBody)) {
					overlap[i].push_back(otherBody);
					overlap[IDIndex[otherBody->ID]].push_back(&body[i]);  //Not sure.
				}
			}
			if (!body[i].isSolid) continue;
			if (overlap[i].size()) {
				for (unsigned int j = 0; j < overlap[i].size(); j++) {
					if (overlap[i][j]->isSolid) {
						body[i].location -= body[i].velocity;
						break;
					}
				}
			}
		}

		/*for(unsigned int i = 0; i < bCount; i++){
			if(!body[i].isDynamic) continue;
			body[i].location+=body[i].velocity;
			for(unsigned int j = i+1; j < bCount; j++){
				if(body[i].isColliding(&body[j])){
					overlap[i].push_back(&body[j]);
					overlap[j].push_back(&body[i]);
				}
			}
			if(!body[i].isSolid) continue;
			if(overlap[i].size()){
				for(unsigned int j = 0; j < overlap[i].size(); j++){
					if(overlap[i][j]->isSolid){
						body[i].location-=body[i].velocity;
						break;
					}
				}
			}
			for(unsigned int j = 0; j < bCount; j++){
				if(j == i) continue;
				if(body[i].isColliding(&body[j]) && body[j].isSolid){
					body[i].location-=body[i].velocity;
					j = 0;
				}
			}
		}*/
	}
private:
	void gravity() {
		if (gravityDensity == 0) return;
		unsigned int bCount = bodyCount;
		for (unsigned int i = 0; i < bCount; i++) {
			body[i].location += body[i].velocity;
			bool hasCollided = false;
			for (unsigned int j = i + 1; j < bCount; j++) {
				if (body[i].isColliding(&body[j])) {
					hasCollided = true;
					bool alreadyHasOverlap = false;
					for (unsigned int k = 0; k < overlap[i].size(); k++) {
						if (overlap[i][k] == &body[j]) {
							alreadyHasOverlap = true;
							break;
						}
					}
					if (!alreadyHasOverlap)
						overlap[i].push_back(&body[j]);
				}
			}
		}
	}
};