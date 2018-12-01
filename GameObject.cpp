#include "GameObject.h"
#include "ComponentMesh.h"
#include "ComponentMaterial.h"


GameObject::GameObject()
{
	uuid = xg::newGuid().str();
}


GameObject::GameObject(GameObject* parent)
{
	this->parent = parent;
	uuid = xg::newGuid().str();
}

GameObject::~GameObject()
{
	for (std::list<Component*>::iterator it = components.begin(); it != components.end(); ++it)
		delete *it;

	components.clear();

	for (std::list<GameObject*>::iterator it = gameObjects.begin(); it != gameObjects.end(); ++it)
		delete *it;

	gameObjects.clear();
}

void GameObject::Update()
{
}

Component* GameObject::CreateComponent(ComponentType type)
{
	Component* ret = nullptr;

	switch (type)
	{
	case ComponentType::MESH:
		mesh = new ComponentMesh(this, type);
		ret = mesh;
		break;
	case ComponentType::MATERIAL:
		material = new ComponentMaterial(this, type);
		ret = material;
		break;
	}

	components.push_back(ret);
	return ret;
}

void GameObject::SetIsSelected(bool selected)
{
	isSelected = selected;

	if (parent != nullptr)
		parent->SetIsSelected(selected);
}

void GameObject::Duplicate()
{
}

void GameObject::RemoveChild()
{
	if (!gameObjects.empty())
	{
		gameObjects.remove_if([](GameObject* i) {return i->toDelete;});

		for each (GameObject* go in gameObjects)
		{
			go->RemoveChild();
		}
	}
}