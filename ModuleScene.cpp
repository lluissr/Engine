#include "ModuleScene.h"
#include "GameObject.h"
#include "ModuleModelLoader.h"
#include "ModuleCamera.h"


ModuleScene::ModuleScene()
{
}


ModuleScene::~ModuleScene()
{
}

bool ModuleScene::Init()
{
	root = new GameObject();
	root->name = "Scene Root";

	gameCamera = CreateCamera();

	return true;
}


update_status ModuleScene::PreUpdate()
{
	if (cleanScene)
	{
		selectedGO = nullptr;
		if (gameCamera != nullptr)
		{
			if (gameCamera->componentCamera->uuid == App->camera->selectedCamera->uuid)
			{
				App->camera->selectedCamera = nullptr;
			}
			gameCamera = nullptr;
		}
		delete root;
		root = new GameObject();
		root->name = "Scene Root";

		cleanScene = false;
	}

	if (loadScene && !cleanScene)
	{
		LoadSceneJSON();
		CalculateGlobalMatrix(root);
		root->UpdateBoundingBox();
		loadScene = false;
	}

	root->RemoveChild();

	if (selectedGO != nullptr && selectedGO->toDelete)
	{
		if (selectedGO->uuid == gameCamera->uuid)
		{
			if (gameCamera->componentCamera->uuid == App->camera->selectedCamera->uuid)
			{
				App->camera->selectedCamera = nullptr;
			}
			gameCamera = nullptr;
		}
		delete selectedGO;
		selectedGO = nullptr;
	}
	else
	{
		if (gameCamera != nullptr && gameCamera->toDelete)
		{
			if (gameCamera->componentCamera->uuid == App->camera->selectedCamera->uuid)
			{
				App->camera->selectedCamera = nullptr;
			}
			delete gameCamera;
			gameCamera = nullptr;
		}
	}

	return UPDATE_CONTINUE;
}


update_status ModuleScene::Update()
{
	return UPDATE_CONTINUE;
}

bool ModuleScene::CleanUp()
{
	selectedGO = nullptr;
	delete root;
	root = nullptr;

	return true;
}

GameObject* ModuleScene::CreateGameObject()
{
	return nullptr;
}

void ModuleScene::CleanRootGameObjects()
{
	for (std::list<GameObject*>::iterator it = root->childrens.begin(); it != root->childrens.end(); ++it)
		delete *it;

	root->childrens.clear();
}

void ModuleScene::SelectGameObject(GameObject* go)
{
	assert(go != NULL);

	if (selectedGO != nullptr)
		selectedGO->SetIsSelected(false);

	selectedGO = go;
	if (go != nullptr)
		go->SetIsSelected(true);
}

void ModuleScene::CalculateGlobalMatrix(GameObject* go)
{
	if (go->parent == nullptr)
	{
		go->globalMatrix = go->localMatrix;
	}
	else
	{
		go->globalMatrix = go->parent->globalMatrix*go->localMatrix;
	}

	for each (GameObject* gameObject in go->childrens)
	{
		CalculateGlobalMatrix(gameObject);
	}
}

void ModuleScene::UseAsGameCamera(GameObject* go)
{
	if (App->camera->selectedCamera == nullptr || (App->scene->gameCamera != nullptr && App->scene->gameCamera->componentCamera->uuid == App->camera->selectedCamera->uuid))
	{
		App->camera->selectedCamera = go->componentCamera;
	}
	App->scene->gameCamera = go;
}


GameObject* ModuleScene::CreateCamera()
{
	GameObject* go = new GameObject();
	go->name = "Game camera";
	go->CreateComponent(ComponentType::CAMERA);
	go->parent = root;
	root->childrens.push_back(go);
	return go;
}

GameObject* ModuleScene::GetGameObjectByUUID(GameObject* gameObject, const std::string& uuid)
{
	GameObject* result = nullptr;

	if (result == nullptr && gameObject->uuid == uuid)
	{
		result = gameObject;
	}
	else
	{
		for (GameObject* gameObjectChild : gameObject->childrens)
		{
			if (gameObjectChild->childrens.size() > 0)
			{
				result = GetGameObjectByUUID(gameObjectChild, uuid);
			}

			if (result == nullptr && gameObjectChild->uuid == uuid)
			{
				result = gameObjectChild;
				break;
			}
			else if (result != nullptr)
			{
				break;
			}
		}
	}

	return result;
}

void ModuleScene::SaveSceneJSON()
{
	LOG("Starting saving scene");
	Config* config = new Config();
	config->StartObject("scene");
	config->AddFloat("ambientLight", ambient);
	config->AddFloat3("lightPosition", lightPosition);

	if (gameCamera != nullptr)
	{
		config->AddString("gameCamera", gameCamera->uuid.c_str());
	}

	config->StartArray("gameObjects");
	SaveGameObjectsJSON(config, root);
	config->EndArray();
	config->EndObject();

	config->WriteToDisk();
	LOG("Scene saved succesfully: Library/Scene/scene.json");
	delete config;
}

void ModuleScene::SaveGameObjectsJSON(Config* config, GameObject* gameObject)
{
	gameObject->SaveJSON(config);

	if (gameObject->childrens.size() > 0)
	{
		for (std::list<GameObject*>::iterator iterator = gameObject->childrens.begin(); iterator != gameObject->childrens.end(); ++iterator)
		{
			SaveGameObjectsJSON(config, (*iterator));
		}
	}
}

void ModuleScene::LoadSceneJSON()
{
	LOG("Starting scene loading: Library/Scene/scene.json");
	Config* config = new Config();
	rapidjson::Document document = config->LoadFromDisk();

	if (!document.HasParseError())
	{
		rapidjson::Value& scene = document["scene"];
		ambient = config->GetFloat("ambientLight", scene);
		lightPosition = config->GetFloat3("lightPosition", scene);

		const char* gameCamerauuid = nullptr;
		if (scene.HasMember("gameCamera"))
		{
			gameCamerauuid = config->GetString("gameCamera", scene);
		}

		rapidjson::Value gameObjects = scene["gameObjects"].GetArray();
		for (rapidjson::Value::ValueIterator it = gameObjects.Begin(); it != gameObjects.End(); it++)
		{
			if ((*it).HasMember("parent"))
			{
				GameObject* go = new GameObject();
				go->LoadJSON(config, *it);
				root->AddChild(go, std::string(config->GetString("parent", (*it))));
				if (gameCamerauuid != nullptr && gameCamera == nullptr && go->uuid == gameCamerauuid)
				{
					gameCamera = go;
				}
			}
			else
			{
				root->uuid = std::string(config->GetString("uuid", (*it)));
			}
		}

		LOG("Scene loaded succesfully");
	}
	else
	{
		LOG("Error loading scene");
	}

	delete config;
}

