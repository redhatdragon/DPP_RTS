#include "engine.h"
#include "IO_API/IO_API.h"
#include "JSON.h/json.h"
#include "mapper.h"
#include "Physics/PhysicsEngine.h"
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


struct json_object_element_s *jsonGetVariable(struct json_object_s *object, const char *variableName) {
	unsigned int length = object->length;
	for (unsigned int i = 0; i < length; i++) {
		struct json_object_element_s *element = object->start;
		for (unsigned int j = 0; j < i; j++) {
			element = element->next;
		}
		struct json_string_s *name = element->name;
		if (strcmp(name->string, variableName) == 0) {
			return element;
		}
	}
	return 0;
}

struct GameObject {
	uint32 bodyID = -1;
	struct Texture *texture = nullptr;
	PhysicsEngine *registeredPhysicsEngine = nullptr;	
	inline void setVelocity(float vx, float vy) {
		registeredPhysicsEngine->setBodyVelocity(bodyID, vx, vy);
	}
	virtual void addBody(float x, float y, float w, float h) {
		PhysicsBody body = { {x,y}, {w,h}, {0,0}, false, false };
		body.data = this;
		bodyID = registeredPhysicsEngine->addBody(body);
	}
	virtual void display(float translateX, float translateY) {
		const PhysicsBody *body = registeredPhysicsEngine->getBodyData(bodyID);
		drawTexture(texture, (int)body->location.x+translateX,
			(int)body->location.y+translateY);
	}
	virtual void run() {}
};
struct Wall : public GameObject {
	virtual void addBody(float x, float y, float w, float h) {
		PhysicsBody body = { {x,y}, {w,h}, {0,0}, true, false };
		body.data = this;
		bodyID = registeredPhysicsEngine->addBody(body);
	}
	virtual void display(float translateX, float translateY) {
		const PhysicsBody* body = registeredPhysicsEngine->getBodyData(bodyID);
		drawRect((int)body->location.x+translateX, (int)body->location.y+translateY,
			(int)body->scale.x, (int)body->scale.y, 125, 125, 125, 255);
	}
};

struct Unit : public GameObject {
	int8 team = -1;
};

struct PartPixel {
	uint8 x, y;
	uint16 health;
};
struct ShipPart {
	int32 value;  //thrust, shield, ect
	uint8 x, y, width, height;
	std::vector<PartPixel> pixel;
	virtual void damage(uint8 _x, uint8 _y, uint16 _val) {

	}
	virtual void damageAOE(uint8 _x, uint8 _y, uint8 _area, uint16 _val) {

	}
};
struct ShipEngine: public ShipPart {
	
};

struct Ship: public Unit {
	//ShipPart *engine, *shieldCapaciter, *hull;
};

struct Fighter : public Ship {
	virtual void addBody(float x, float y, float w, float h) {
		PhysicsBody body = { {x,y}, {w,h}, {0,0}, true, true };
		body.data = this;
		bodyID = registeredPhysicsEngine->addBody(body);
	}
	virtual void display(float translateX, float translateY) {
		const PhysicsBody* body = registeredPhysicsEngine->getBodyData(bodyID);
		drawRect((int)body->location.x+translateX, (int)body->location.y+translateY,
			(int)body->scale.x, (int)body->scale.y, 255, 0, 255, 255);
	}
	virtual void run() {
		uint8 keyStates[256] = { 0 };
		float velocityX = 0; float velocityY = 0;
		constexpr float speed = 4;
		getKeyboardState(keyStates);
		if (keyStates['D'])
			velocityX += speed;
		if (keyStates['A'])
			velocityX -= speed;
		if (keyStates['S'])
			velocityY += speed;
		if (keyStates['W'])
			velocityY -= speed;
		registeredPhysicsEngine->setBodyVelocity(bodyID, velocityX, velocityY);
	}
};
struct Bullet : public Ship {
	virtual void addBody(float x, float y, float w, float h) {
		PhysicsBody body = { {x,y}, {w,h}, {0,0}, true, true };
		body.data = this;
		bodyID = registeredPhysicsEngine->addBody(body);
	}
	virtual void display(float translateX, float translateY) {
		const PhysicsBody* body = registeredPhysicsEngine->getBodyData(bodyID);
		drawRect((int)body->location.x + translateX, (int)body->location.y + translateY,
			(int)body->scale.x, (int)body->scale.y, 155, 255, 0, 255);
	}
};

std::vector<GameObject*> gameObject;
PhysicsEngine *physicsEngine;

size_t levelGetGameObjectCount() {
	return gameObject.size();
}

void levelDisplay() {
	size_t size = gameObject.size();
	const Point camera = physicsEngine->getBodyData(gameObject[0]->bodyID)->location;
	float translateX = -(camera.x-cWidth/2);
	float translateY = -(camera.y-cHeight/2);
	for (size_t i = 0; i < size; i++) {
		gameObject[i]->display(translateX, translateY);
	}
}
void levelRun() {
	size_t size = gameObject.size();
	for (size_t i = 0; i < size; i++) {
		gameObject[i]->run();
	}
	const Point camera = physicsEngine->getBodyData(gameObject[0]->bodyID)->location;
	uint8 keyStates[256];
	getKeyboardState(keyStates);
	static int timer = 0;
	constexpr int timer_reset = 4;
	if (timer > 0) {
		timer--;
		physicsEngine->tick();
		return;
	}
	timer = timer_reset;
	if (keyStates['F']) {
		GameObject* newGameObject = new Bullet();
		newGameObject->registeredPhysicsEngine = physicsEngine;
		newGameObject->addBody(camera.x, camera.y-20, 5, 5);
		newGameObject->setVelocity(0, -5);
		gameObject.push_back(newGameObject);
	}
	physicsEngine->tick();
}
void levelLoad() {
	physicsEngine = new PhysicsEngine(100000);
	GameObject *newGameObject = new Fighter();
	newGameObject->registeredPhysicsEngine = physicsEngine;
	newGameObject->addBody(30, 30, 20, 20);
	gameObject.push_back(newGameObject);

	newGameObject = new Wall();
	newGameObject->registeredPhysicsEngine = physicsEngine;
	newGameObject->addBody(0, 0, 2000, 20);
	gameObject.push_back(newGameObject);

	newGameObject = new Wall();
	newGameObject->registeredPhysicsEngine = physicsEngine;
	newGameObject->addBody(0, 0, 20, 2000);
	gameObject.push_back(newGameObject);
}