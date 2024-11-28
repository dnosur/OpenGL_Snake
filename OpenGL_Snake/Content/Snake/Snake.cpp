#include "Snake.h"

#include "../../Dodge/collisions/BoxCollision.h"
#include "../../Dodge/figures/Rect.h"

void Snake::Initialize()
{
	WindowPointer<Keyboard>* keyboardPointer = WindowPointerController::GetValue<Keyboard>(window->GetWindow(), "Keyboard");
	if (keyboardPointer != nullptr) {
		keyboard = &keyboardPointer->GetValue();
	}

	firstPos = pos;

	CreateSnakeBody(
		new Rect("Head", *window, pos, size, Color(1, 1, 1)),
		SpriteAnimation(
			"HeadAnimation",
			"Dodge/images/Lorenzo's_New_SnakeGameAssests/PNGs/LorenzosNewSnakeAssets/head",
			5,
			window
		)
	);

	collision = snakeBody[0]->GetCollision();

	CreateSnakeBody(
		new Rect("body1", *window, Coord(pos.X, pos.Y - (snakeBodyOffset * snakeBody.size())), size, Color(1, 1, 1)),
		SpriteAnimation(
			"BodyAnimation1",
			"Dodge/images/Lorenzo's_New_SnakeGameAssests/PNGs/LorenzosNewSnakeAssets/body/verticalBody32",
			5,
			window
		)
	);

	CreateSnakeBody(
		new Rect("Tail", *window, Coord(pos.X, pos.Y - snakeBodyOffset * snakeBody.size()), size, Color(1, 1, 1)),
		SpriteAnimation(
			"TailAnimation",
			"Dodge/images/Lorenzo's_New_SnakeGameAssests/PNGs/LorenzosNewSnakeAssets/tail/16px",
			5,
			window
		)
	);


}

void Snake::Draw()
{
	for (IGameObject* rect : snakeBody) {
		rect->GetMaterial()->Use(this);
		glPushAttrib(GL_ALL_ATTRIB_BITS);
		std::vector<float> vertices = Rect::GetVerticesByDirection(*(Rect*)rect, rect->GetMoveDirection(), true);

		unsigned int VBO, VAO;

		glGenBuffers(1, &VBO);
		glGenVertexArrays(1, &VAO);

		glBindVertexArray(VAO);
		glBindBuffer(GL_ARRAY_BUFFER, VBO);

		glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
		glEnableVertexAttribArray(0);

		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
		glEnableVertexAttribArray(1);

		glDrawArrays(GL_QUADS, 0, vertices.size() / 5);
		glPopAttrib();
		rect->GetMaterial()->Disable(this);
	}
}

void Snake::Move()
{
	if (!health) {
		return;
	}

	KeyboardKey key = keyboard->GetKey();

	Coord lastHeadPos = snakeBody[0]->GetPos();
	Coord offset = Coord(0, 0);

	Directions lastDir = snakeBody[0]->GetMoveDirection();
	bool press = key.action < GLFW_PRESS;

	if (key.Pressed(KeyboardKeys::W) && lastDir != Directions::DOWN ||
		(press && lastDir == Directions::UP)) {
		moveDirection = Directions::UP;
		offset = Coord(0, -1 * snakeBodyOffset);
		MoveHorizontal(speed * -0.1f);
	}

	if (key.Pressed(KeyboardKeys::S) && lastDir != Directions::UP ||
		(press && lastDir == Directions::DOWN)) {
		moveDirection = Directions::DOWN;
		offset = Coord(0, snakeBodyOffset);
		MoveHorizontal(speed * 0.1f);
	}

	if (key.Pressed(KeyboardKeys::A) && lastDir != Directions::RIGHT ||
		(press && lastDir == Directions::LEFT)) {
		moveDirection = Directions::LEFT;
		offset = Coord(-1 * snakeBodyOffset, 0);
		MoveVerticale(speed * -0.1f);
	}

	if (key.Pressed(KeyboardKeys::D) && lastDir != Directions::LEFT ||
		(press && lastDir == Directions::RIGHT)) {
		moveDirection = Directions::RIGHT;
		offset = Coord(snakeBodyOffset, 0);
		MoveVerticale(speed * 0.1f);
	}

	if (pos == lastHeadPos) {
		return;
	}

	snakeBody[0]->SetPos(pos);

	if (snakeBody[0]->GetMoveDirection() != moveDirection) {
		turnPoints.push_front(TurnPoint(pos, moveDirection));
		if (turnPoints.size() > MAX_TURN_POINTS) {
			turnPoints.pop_back();
		}
		snakeBody[0]->SetMoveDirection(moveDirection);
	}


	if (collision != nullptr) {
		snakeBody[0]->GetCollision()->SetPoints({ pos });
		//std::cout << "Head Collision: X: " << pos.X << " Y: " << pos.Y << std::endl;
	}

	for (size_t i = 1; i < snakeBody.size(); i++) {
		Coord prevPos = snakeBody[i]->GetPos();
		Directions prevDir = snakeBody[i]->GetMoveDirection();

		float dx = lastHeadPos.X - prevPos.X;
		float dy = lastHeadPos.Y - prevPos.Y;
		float distance = sqrt(dx * dx + dy * dy);

		if (distance > snakeBodyOffset) {
			float correction = distance - snakeBodyOffset;
			prevPos.X += (dx / distance) * correction;
			prevPos.Y += (dy / distance) * correction;
		}

		if (distance < snakeBodyOffset) {
			float correction = snakeBodyOffset - distance;
			prevPos.X -= (dx / distance) * correction;
			prevPos.Y -= (dy / distance) * correction;
		}

		bool isTurnPoint = false;
		for (TurnPoint& turn : turnPoints) {
			if (IsCloseToTurnPoint(prevPos, turn.position, snakeBodyOffset * 0.5f)) {
				float turnProgress = CalculateTurnProgress(prevPos, turn.position, snakeBodyOffset);
				prevPos.X = prevPos.X + (turn.position.X - prevPos.X) * turnProgress;
				prevPos.Y = prevPos.Y + (turn.position.Y - prevPos.Y) * turnProgress;

				snakeBody[i]->SetPos(prevPos);
				snakeBody[i]->SetMoveDirection(turn.direction);

				turn.crossings++;
				isTurnPoint = true;
				break;
			}
		}

		if (!isTurnPoint) {
			snakeBody[i]->SetPos(prevPos);
			snakeBody[i]->SetMoveDirection(lastDir);

			if (collision != nullptr) {
				snakeBody[i]->GetCollision()->SetPoints({ prevPos });
				//DrawSymbol(prevPos, '!', window->GetSize());
			}
		}

		lastHeadPos = prevPos;
		lastDir = prevDir;
	}

	CleanupTurnPoints();
}

void Snake::MoveVerticale(float impulse)
{
	pos.X += impulse;
	if (pos.X >= window->GetSize().GetWidth()) {
		pos.X = 1.0f;
	}

	if (pos.X <= 0) {
		pos.X = window->GetSize().GetWidth() - 1.0f;
	}
}

void Snake::MoveHorizontal(float impulse)
{
	pos.Y += impulse;
	if (pos.Y >= window->GetSize().GetHeight()) {
		pos.Y = 1.0f;
	}

	if (pos.Y <= 0) {
		pos.Y = window->GetSize().GetHeight() - 1.0f;
	}
}

void Snake::CreateSnakeBody(IGameObject* body, SpriteAnimation anim)
{
	anim.SetRepeat(true);
	anim.SetRootTile(body->GetMaterial()->GetDiffuseMap());
	body->SetCollision(
		new BoxCollision(
			body->GetPos(),
			dynamic_cast<BoxCollision*>(collision)->GetSize(),
			snakeBody.size() - 1,
			(char*)body->GetTitle(),
			(char*)"Snake Body"
		)
	);

	snakeBody.push_back(body);
	animations.AddAnimation(anim);
}

std::vector<float> Snake::GetVerticesByDirection()
{
	Coord& vertex1 = vertexes[0];
	Coord& vertex2 = vertexes[1];

	const bool isHasDiffuseVertexs = material->GetDiffuseMapVerticies().size() >= 2 && material->GetDiffuseMap() != nullptr;
	const Coord& textCoord1 = isHasDiffuseVertexs ? material->GetDiffuseMapVerticies()[0] : Coord(0, 0);
	const Coord& textCoord2 = isHasDiffuseVertexs ? material->GetDiffuseMapVerticies()[1] : Coord(1, 1);

	if (moveDirection == Directions::UP) {
		return std::vector<float> {
			(float)vertex2.X, (float)vertex2.Y, 0.0f, (float)textCoord1.X, (float)textCoord1.Y,
			(float)vertex1.X, (float)vertex2.Y, 0.0f, (float)textCoord2.X, (float)textCoord1.Y,
			(float)vertex1.X, (float)vertex1.Y, 0.0f, (float)textCoord2.X, (float)textCoord2.Y,
			(float)vertex2.X, (float)vertex1.Y, 0.0f, (float)textCoord1.X, (float)textCoord2.Y
		};
	}

	if (moveDirection == Directions::DOWN) {
		return std::vector<float> {
			(float)vertex1.X, (float)vertex1.Y, 0.0f, (float)textCoord1.X, (float)textCoord1.Y,
			(float)vertex2.X, (float)vertex1.Y, 0.0f, (float)textCoord2.X, (float)textCoord1.Y,
			(float)vertex2.X, (float)vertex2.Y, 0.0f, (float)textCoord2.X, (float)textCoord2.Y,
			(float)vertex1.X, (float)vertex2.Y, 0.0f, (float)textCoord1.X, (float)textCoord2.Y
		};
	}

	if (moveDirection == Directions::LEFT) {
		return std::vector<float> {
			(float)vertex1.X, (float)vertex1.Y, 0.0f, (float)textCoord1.X, (float)textCoord1.Y,
			(float)vertex1.X, (float)vertex2.Y, 0.0f, (float)textCoord2.X, (float)textCoord1.Y,
			(float)vertex2.X, (float)vertex2.Y, 0.0f, (float)textCoord2.X, (float)textCoord2.Y,
			(float)vertex2.X, (float)vertex1.Y, 0.0f, (float)textCoord1.X, (float)textCoord2.Y
		};
	}

	if (moveDirection == Directions::RIGHT) {
		return std::vector<float> {
			(float)vertex2.X, (float)vertex2.Y, 0.0f, (float)textCoord1.X, (float)textCoord1.Y,
			(float)vertex2.X, (float)vertex1.Y, 0.0f, (float)textCoord2.X, (float)textCoord1.Y,
			(float)vertex1.X, (float)vertex1.Y, 0.0f, (float)textCoord2.X, (float)textCoord2.Y,
			(float)vertex1.X, (float)vertex2.Y, 0.0f, (float)textCoord1.X, (float)textCoord2.Y
		};
	}
}

float Snake::CalculateTurnProgress(Coord current, Coord turn, float offset)
{
	float distance = CalculateDistance(current, turn);
	return std::clamp(1.0f - (distance / offset), 0.0f, 1.0f);
}

bool Snake::IsCloseToTurnPoint(const Coord& position, const Coord& turnPosition, float threshold)
{
	return fabs(position.X - turnPosition.X) <= threshold && fabs(position.Y - turnPosition.Y) <= threshold;
}

void Snake::CleanupTurnPoints() {
	while (!turnPoints.empty()) {
		const auto& lastTurn = turnPoints.back();
		bool allPassed = true;

		for (const auto& segment : snakeBody) {
			if (IsCloseToTurnPoint(segment->GetPos(), lastTurn.position, snakeBodyOffset * snakeBody.size()) || 
				lastTurn.crossings >= snakeBody.size()) {
				allPassed = false;
				break;
			}
		}

		if (allPassed) {
			turnPoints.pop_back();
		}
		else {
			break;
		}
	}
}

Snake::Snake(
	const char* title, Window& window,
	ICollision* collision, Material* material, Directions moveDirection, 
	Coord pos, Size size, float speed, float maxSpeed, float minSpeed, 
	float health, float maxHealth, bool isPlayable, bool isKinematic, bool isHidden
) : Pawn(
	title, window,
	collision, material, moveDirection, pos, 
	size, speed, maxSpeed, minSpeed, health, maxHealth, 
	isPlayable, isKinematic, isHidden
){
	Initialize();
}

void Snake::Update()
{
	Move();
	Draw();

	for (int i = 2; i < snakeBody.size(); i++) {
		if (snakeBody[i]->CollisionEnter(*snakeBody[0])) {
			Die();
			return;
		}
	}

	if (health) {
		animations.PlayAll();
	}
	//std::cout << "Snake "<< title << " Update"  << std::endl;
}

int Snake::GetBodySize()
{
	return snakeBody.size();
}

void Snake::Restart()
{
	health = 100;
	pos = firstPos;
	moveDirection = Directions::DOWN;

	snakeBody[0]->SetPos(firstPos);
	snakeBody[0]->SetMoveDirection(moveDirection);

	if (snakeBody.size() >= 3) {
		std::swap(snakeBody[2], snakeBody[snakeBody.size() - 1]);
		for (int i = 3; i < snakeBody.size(); i++) {
			RemoveFromVector(snakeBody, snakeBody[i]);
			i--;
		}
	}

	snakeBody[1]->SetPos(Coord(pos.X, pos.Y - (snakeBodyOffset * snakeBody.size())));
	snakeBody[2]->SetPos(Coord(pos.X, pos.Y - (snakeBodyOffset * snakeBody.size())));
}

void Snake::Eat()
{
	WindowPointer<AudioController>* audioController = WindowPointerController::GetValue<AudioController>(window->GetWindow(), "audioController");
	if (audioController) {
		audioController->GetValue().Play("eat");
	}

	std::string body = "body" + std::to_string(snakeBody.size() + 1);
	std::string bodyAnimation = "BodyAnimation" + std::to_string(snakeBody.size() + 1);
	
	IGameObject* tail = snakeBody[snakeBody.size() - 1];

	Coord oldPos = tail->GetPos();
	Coord newPos;

	if (tail->GetMoveDirection() == Directions::UP) {
		newPos = Coord(oldPos.X, oldPos.Y - snakeBodyOffset * snakeBody.size());
	}

	if (tail->GetMoveDirection() == Directions::DOWN) {
		newPos = Coord(oldPos.X, oldPos.Y + snakeBodyOffset * snakeBody.size());
	}

	if (tail->GetMoveDirection() == Directions::LEFT) {
		newPos = Coord(oldPos.X - snakeBodyOffset * snakeBody.size(), oldPos.Y);
	}

	if (tail->GetMoveDirection() == Directions::RIGHT) {
		newPos = Coord(oldPos.X + snakeBodyOffset * snakeBody.size(), oldPos.Y);
	}

	tail->SetPos(newPos);

	CreateSnakeBody(
		new Rect(
			body.c_str(), 
			*window, 
			oldPos, 
			size, 
			Color(1, 1, 1)
		),
		SpriteAnimation(
			bodyAnimation.c_str(),
			"Dodge/images/Lorenzo's_New_SnakeGameAssests/PNGs/LorenzosNewSnakeAssets/body/verticalBody32",
			5,
			window,
			animations[animations.GetSize() - 2]->GetCurrentSpriteIndex() != 1
		)
	);

	std::swap(snakeBody[snakeBody.size() - 1], snakeBody[snakeBody.size() - 2]);
}
