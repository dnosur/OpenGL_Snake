#include "Road.h"

void Road::OnTriggerEnter(IGameObject* object, IGameObject* triggeredObject)
{
}

Road::Road(Window* window, TileMap* tileMap, Coord pos)
	: TinyMap(window, tileMap, pos)
{
}

void Road::Update()
{
	for (IGameObject* gameObject : gameObjects) {
		//if (gameObject->GetCollision() != nullptr) {
		//	DrawSymbols(gameObject->GetPos(), (char*)gameObject->GetTitle(), gameObject->GetWindow()->GetSize(), Color(0, 0, 1));
		//}
		gameObject->Update();
	}

	//for (IGameObject* gameClass : gameClasses) {
	//	//DrawSymbols(gameClass->GetPos(), (char*)std::to_string(i).c_str(), gameClass->GetWindow()->GetSize(), Color(0, 0, 1));
	//	//gameClass->SetTitle((char*)std::to_string(i).c_str());
	//	gameClass->Update();
	//}
}

bool Road::Trigger(IGameObject* object)
{
	bool collisionEnter = false;
	for (IGameObject* gameClass : gameClasses) {
		if (gameClass->CollisionEnter(*object)) {
			collisionEnter = true;
		}
	}

	//std::cout << "Collision enter: " << collisionEnter << std::endl;
	for (IGameObject* gameObject : gameObjects) {
		if (gameObject->GetCollision() == nullptr) {
			continue;
		}

		gameObject->CollisionEnter(*object);
	}

	return collisionEnter;
}
