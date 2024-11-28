#pragma once
#include "ICollision.h"

class PoligonCollision 
	: public ICollision
{
	int root_id;
	char* root_title;
	char* type;

	std::vector<Coord> points;
	std::vector<IGameObject*> gameObjects;
public:
	PoligonCollision();
	PoligonCollision(std::vector<Coord> points, int root_id, char* root_title = nullptr, char* type = nullptr);

	static bool IsPointInPolygon(const Coord& point, const std::vector<Coord>& polygon);

	int GetRootId();

	char* GetRootTitle();
	char* GetType();

	std::vector<Coord> GetPoints();
	void SetPoints(std::vector<Coord> points);

	PoligonCollision* Clone() const override;

	bool IsCollisionEnter(IGameObject* gameObject);
	bool IsCollisionStay(IGameObject* gameObject);
	bool IsCollisionExit(IGameObject* gameObject);

	bool IsExist(IGameObject* gameObject);

	bool operator==(const PoligonCollision& other) const;
	bool operator!=(const PoligonCollision& other) const;
	PoligonCollision& operator=(const PoligonCollision& other);
};

