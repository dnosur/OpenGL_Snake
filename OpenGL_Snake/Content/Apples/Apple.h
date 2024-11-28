#pragma once
#include "../Snake/Snake.h"

class Apple
{
	std::vector<IGameObject*> spawnPoints;
	ImagesController images;

	int spawnIndex, imageIndex;

	void Initialize();
	void Spawn(Snake& snake);
public:
	Apple(std::vector<IGameObject*> spawnPoints);
	~Apple();

	void Update(Snake* snake);
	void Respawn();
};

