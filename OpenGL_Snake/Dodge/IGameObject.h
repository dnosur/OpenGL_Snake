#pragma once
#include "Window.h"

#include "collisions/ICollision.h"

class Material;
class CollisionsController;
enum Directions;

__interface IGameObject;
__interface ICollision;

typedef void(__stdcall* MouseHoverHandler)(IGameObject* object, GLFWwindow* window);
typedef void(__stdcall* MouseClickHandler)(IGameObject* object, GLFWwindow* window);

typedef void(__stdcall* OnCollisionEnter)(IGameObject* object, IGameObject* sender, GLFWwindow* window);

__interface IGameObject
{
	void Draw();
	void Update();

	bool MouseHover(Mouse& mouse);
	bool CollisionEnter(IGameObject& gameObject);

	Window* GetWindow();

	Coord GetPos();
	std::vector<Coord> GetVertices();

	void SetColor(Color color);
	Color GetColor();

	void SetPos(std::vector<Coord> vertices);
	void SetPos(Coord pos);

	Directions GetMoveDirection();

	void SetMoveDirection(Directions moveDirection);

	void RotateToDirection(Directions direction);

	Color GetBaseColor();

	Size GetSize();

	void SetMaterial(Material* material);

	Material* GetMaterial();
	void SetCollision(ICollision* collision);
	ICollision* GetCollision();

	const char* GetTitle();
	void SetTitle(const char* title);

	const bool IsMouseOverlap();

	void HookMouseHover(MouseHoverHandler handler);
	void HookMouseOver(MouseHoverHandler handler);

	void HookMouseClick(MouseClickHandler handler);

	void HookOnCollisionEnter(OnCollisionEnter handler);
};