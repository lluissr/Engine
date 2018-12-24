#ifndef __ModuleScene_h__
#define __ModuleScene_h__

#include "Module.h"
#include "MathGeoLib.h"
#include <list>

class GameObject;

class ModuleScene : public Module 
{
public:
	ModuleScene();
	~ModuleScene();

	bool Init() override;
	update_status PreUpdate() override;
	update_status Update() override;
	bool CleanUp() override;

	GameObject* CreateCamera();
	GameObject* CreateGameObject();
	void CleanRootGameObjects();
	void SelectGameObject(GameObject* go);
	void CalculateGlobalMatrix(GameObject* go);
	void UseAsGameCamera(GameObject* go);

	GameObject* root = nullptr;
	GameObject* selectedGO = nullptr;
	GameObject* gameCamera = nullptr;

	math::float3 lightPosition = math::float3(-2.0f, 0.0f, 6.0f);
	float ambient = 0.3f;
};

#endif

