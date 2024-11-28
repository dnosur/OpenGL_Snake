#include "Apple.h"

void Apple::Initialize()
{
	images.Load(
		"Dodge/images/Lorenzo's_New_SnakeGameAssests/PNGs/LorenzosNewSnakeAssets/food/32px/apple.png",
		"Apple"
	);

	images.Load(
		"Dodge/images/Lorenzo's_New_SnakeGameAssests/PNGs/LorenzosNewSnakeAssets/food/32px/orange.png",
		"Orange"
	);

	images.Load(
		"Dodge/images/Lorenzo's_New_SnakeGameAssests/PNGs/LorenzosNewSnakeAssets/food/32px/grapes.png",
		"Grapes"
	);

	images.Load(
		"Dodge/images/Lorenzo's_New_SnakeGameAssests/PNGs/LorenzosNewSnakeAssets/food/32px/mouse.png",
		"Mouse"
	);
}

void Apple::Spawn(Snake& snake)
{
	if (spawnPoints.size() <= 0 || images.GetSize() <= 0) {
		return;
	}

	do {
		spawnIndex = rand() % spawnPoints.size();
	} while (snake.CollisionEnter(*spawnPoints[spawnIndex]));

	imageIndex = rand() % images.GetSize();

	spawnPoints[spawnIndex]->GetMaterial()->SetDiffuseMap(images[imageIndex]);
}

Apple::Apple(std::vector<IGameObject*> spawnPoints)
{
	spawnIndex = imageIndex = -1;
	this->spawnPoints = spawnPoints;

	Initialize();
}

Apple::~Apple()
{
}

void Apple::Update(Snake* snake)
{
	if (spawnPoints.size() <= 0) {
		return;
	}

	if (spawnIndex < 0) {
		Spawn(*snake);
	}

	if (snake->CollisionEnter(*spawnPoints[spawnIndex])) {
		snake->Eat();
		Spawn(*snake);
	}

	spawnPoints[spawnIndex]->Update();
}

void Apple::Respawn()
{
	spawnIndex = -1;
}
