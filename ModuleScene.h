#ifndef __ModuleScene_h__
#define __ModuleScene_h__

#include "Module.h"
#include "MathGeoLib.h"
#include <list>
#include "Config.h"
#include "QuadTree.h"

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
	void SelectGameObject(GameObject* go);
	void CalculateGlobalMatrix(GameObject* go);
	void UseAsGameCamera(GameObject* go);
	void SaveSceneJSON(const char* name);
	GameObject* GetGameObjectByUUID(GameObject* gameObject, const std::string& uuid);

	GameObject* root = nullptr;
	GameObject* selectedGO = nullptr;
	GameObject* gameCamera = nullptr;

	Quadtree quadTree;
	bool drawQuadTree = false;

	math::float3 lightPosition = math::float3(0.0f, 1000.0f, 1000.0f);
	float ambient = 0.3f;

	bool cleanScene = false;
	bool loadScene = false;
	const char* sceneName = nullptr;

private:
	GameObject* CreateGameObject();
	void CleanRootGameObjects();
	void LoadSceneJSON(const char* name);
	void SaveGameObjectsJSON(const Config* config, GameObject* gameObject);
};

#endif

