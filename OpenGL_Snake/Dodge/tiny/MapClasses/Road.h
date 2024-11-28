#pragma once
#include "../TinyMap.h"

class Road :
    public TinyMap
{
	void OnTriggerEnter(IGameObject* object, IGameObject* triggeredObject) override;
public:
	Road(Window* window, TileMap* tileMap, Coord pos = Coord());

	void Update() override;
	bool Trigger(IGameObject* object);
};

