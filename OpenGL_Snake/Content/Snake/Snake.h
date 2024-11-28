#pragma once
#include "../../Dodge/pawn/Pawn.h"
#include <deque>

class Snake : public Pawn
{
	Keyboard* keyboard;
	std::vector<IGameObject*> snakeBody;

	Coord firstPos;

	const int snakeBodyOffset = 30;

	void Initialize() override;
	void Draw() override;

	void Move();
	void MoveVerticale(float impulse);
	void MoveHorizontal(float impulse);

	void CreateSnakeBody(IGameObject* body, SpriteAnimation anim);

	std::vector<float> GetVerticesByDirection();

	struct TurnPoint {
		Coord position;
		Directions direction;
		int crossings;

		TurnPoint(Coord pos, Directions dir) : position(pos), direction(dir), crossings(0) {}
	};
	std::deque<TurnPoint> turnPoints;
	const size_t MAX_TURN_POINTS = 50;

	float CalculateTurnProgress(Coord current, Coord turn, float offset);

	bool IsCloseToTurnPoint(const Coord& position, const Coord& turnPosition, float threshold);

	void CleanupTurnPoints();
public:
	Snake(
		const char* title, Window& window,
		ICollision* collision, Material* material, Directions moveDirection,
		Coord pos, Size size, float speed, float maxSpeed, float minSpeed,
		float health, float maxHealth, bool isPlayable, bool isKinematic, bool isHidden
	);

	void Update() override;

	int GetBodySize();

	void Restart();

	void Eat();
};