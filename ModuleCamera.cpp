#include "Application.h"

#include "ModuleCamera.h"
#include "ModuleInput.h"
#include "ModuleModelLoader.h"
#include "ModuleScene.h"
#include "ModuleEditor.h"
#include "ModuleRender.h"

#include "GL/glew.h"
#include "SDL.h"
#include "MathGeoLib.h"
#include "ImGuizmo.h"

ModuleCamera::ModuleCamera()
{

}

ModuleCamera::~ModuleCamera()
{
}

bool ModuleCamera::Init()
{
	sceneCamera = new ComponentCamera(nullptr, ComponentType::CAMERA);
	selectedCamera = sceneCamera;
	sceneCamera->frustum.pos = math::float3(0.0f, 100.0f, 1000.0f);
	sceneCamera->frustum.front = -float3::unitZ;
	sceneCamera->frustum.farPlaneDistance = 100000.0f;
	sceneCamera->yaw = 0;

	return true;
}

update_status ModuleCamera::PreUpdate()
{
	if (selectedCamera != nullptr && selectedCamera->myGameObject != nullptr && !selectedCamera->myGameObject->isActive)
	{
		return UPDATE_CONTINUE;
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_REPEAT)
	{
		if (App->input->GetKey(SDL_SCANCODE_Q) == KEY_REPEAT)
		{
			Move(Directions::UP);
		}
		if (App->input->GetKey(SDL_SCANCODE_E) == KEY_REPEAT)
		{
			Move(Directions::DOWN);
		}
		if (App->input->GetKey(SDL_SCANCODE_W) == KEY_REPEAT)
		{
			Move(Directions::FORWARD);
		}
		if (App->input->GetKey(SDL_SCANCODE_S) == KEY_REPEAT)
		{
			Move(Directions::BACKWARD);
		}
		if (App->input->GetKey(SDL_SCANCODE_A) == KEY_REPEAT)
		{
			Move(Directions::LEFT);
		}
		if (App->input->GetKey(SDL_SCANCODE_D) == KEY_REPEAT)
		{
			Move(Directions::RIGHT);
		}
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_DOWN)
		{
			selectedCamera->mSpeed = selectedCamera->mSpeed * 2;
			selectedCamera->rSpeed = selectedCamera->rSpeed * 2;
		}
		if (App->input->GetKey(SDL_SCANCODE_LSHIFT) == KEY_UP)
		{
			selectedCamera->mSpeed = selectedCamera->mSpeed / 2;
			selectedCamera->rSpeed = selectedCamera->rSpeed / 2;
		}

		MouseUpdate();
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_RIGHT) == KEY_UP)
	{
		if (selectedCamera != nullptr)
			selectedCamera->firstMouse = true;
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_X1) == KEY_DOWN)
	{
		selectedCamera->fovX -= 1;
		//SetHorizontalFOV(selectedCamera->fovX);
	}
	else if (App->input->GetMouseButtonDown(SDL_BUTTON_X2) == KEY_DOWN)
	{
		selectedCamera->fovX += 1;
		//SetHorizontalFOV(selectedCamera->fovX);
	}

	if (App->input->GetKey(SDL_SCANCODE_F) == KEY_DOWN)
	{
		Focus();
	}

	if (App->input->GetKey(SDL_SCANCODE_LALT) == KEY_REPEAT && App->input->GetMouseButtonDown(SDL_BUTTON_LEFT))
	{
		Orbit();
	}

	if (App->input->GetMouseButtonDown(SDL_BUTTON_LEFT) == KEY_DOWN && App->editor->overEditorViewport && !ImGuizmo::IsOver())
	{
		PickGameObject();
	}


	return UPDATE_CONTINUE;
}



update_status ModuleCamera::Update()
{
	return UPDATE_CONTINUE;
}


void ModuleCamera::PickGameObject()
{
	iPoint mousePosition = App->input->GetMousePosition();

	float normalizedX = -(1.0f - (float(mousePosition.x - App->editor->editorViewportX) * 2.0f) / sceneCamera->screenWidth);
	float normalizedY = 1.0f - (float(mousePosition.y - App->editor->editorViewportY) * 2.0f) / sceneCamera->screenHeight;

	math::LineSegment pickingLine = sceneCamera->frustum.UnProjectLineSegment(normalizedX, normalizedY);

	objectsPossiblePick.clear();
	App->scene->quadTree.CollectIntersections(objectsPossiblePick, pickingLine);

	for (std::vector<ComponentMesh*>::iterator it = App->renderer->meshes.begin(); it != App->renderer->meshes.end(); ++it)
	{
		if (!(*it)->myGameObject->isStatic && (*it)->mesh != nullptr && pickingLine.Intersects((*it)->mesh->globalBoundingBox))
		{
			objectsPossiblePick.push_back((*it)->myGameObject);
		}
	}

	float minDistance = -100.0f;
	GameObject* gameObjectHit = nullptr;
	if (objectsPossiblePick.size() > 0)
	{
		for (std::vector<GameObject*>::iterator it = objectsPossiblePick.begin(); it != objectsPossiblePick.end(); ++it)
		{
			Mesh* mesh = (*it)->componentMesh->mesh;
			math::LineSegment localTransformPikingLine(pickingLine);
			localTransformPikingLine.Transform((*it)->globalMatrix.Inverted());

			math::Triangle triangle;
			for (unsigned i = 0; i < mesh->numIndices; i += 3)
			{
				triangle.a = { mesh->vertices[mesh->indices[i] * 3], mesh->vertices[mesh->indices[i] * 3 + 1], mesh->vertices[mesh->indices[i] * 3 + 2] };
				triangle.b = { mesh->vertices[mesh->indices[i + 1] * 3], mesh->vertices[mesh->indices[i + 1] * 3 + 1], mesh->vertices[mesh->indices[i + 1] * 3 + 2] };
				triangle.c = { mesh->vertices[mesh->indices[i + 2] * 3], mesh->vertices[mesh->indices[i + 2] * 3 + 1], mesh->vertices[mesh->indices[i + 2] * 3 + 2] };

				float triangleDistance;
				float3 hitPoint;
				if (localTransformPikingLine.Intersects(triangle, &triangleDistance, &hitPoint))
				{
					if (minDistance == -100.0f || triangleDistance < minDistance)
					{
						minDistance = triangleDistance;
						gameObjectHit = *it;
					}
				}
			}
		}
	}

	if (gameObjectHit != nullptr)
	{
		App->scene->selectedGO = gameObjectHit;
	}
}


void ModuleCamera::Orbit()
{

}

void ModuleCamera::Focus()
{
	if (selectedCamera == sceneCamera)
	{
		math::float3 center = math::float3(0.0f, 1.0f, 10.0f);
		if (App->scene->selectedGO != nullptr && App->scene->selectedGO->componentMesh != nullptr)
		{
			center = App->scene->selectedGO->componentMesh->mesh->globalBoundingBox.CenterPoint();
		}

		//Reset all variables (position, front, up, fov, pitch, yaw, firstmouse)
		selectedCamera->frustum.pos = center;
		selectedCamera->frustum.front = math::float3(0.0f, 0.0f, -1.0f);
		selectedCamera->frustum.up = math::float3(0.0f, 1.0f, 0.0f);
		selectedCamera->frustum.verticalFov = math::pi / 4.0f;
		selectedCamera->frustum.horizontalFov = 2.f * atanf(tanf(selectedCamera->frustum.verticalFov * 0.5f) * ((float)screenWidth / (float)screenHeight));
		selectedCamera->fovY = 45.0f;
		selectedCamera->fovX = 45.0f;
		selectedCamera->pitch = 0;
		selectedCamera->yaw = 0;
		selectedCamera->firstMouse = true;

		if (App->scene->selectedGO != nullptr && App->scene->selectedGO->componentMesh != nullptr)
		{
			//Add distance still we can see all the bounding box
			int count = 0;
			while (!selectedCamera->frustum.Contains(App->scene->selectedGO->componentMesh->mesh->globalBoundingBox) && count < 100)
			{
				selectedCamera->frustum.pos.z += 100;
				count++;
			}
		}
	}
}


bool ModuleCamera::CleanUp()
{
	selectedCamera = nullptr;
	RELEASE(sceneCamera);
	return true;
}


void ModuleCamera::Move(Directions dir)
{
	switch (dir) {
	case Directions::UP:
		selectedCamera->frustum.pos += selectedCamera->frustum.up.Normalized() * selectedCamera->mSpeed;
		break;
	case Directions::DOWN:
		selectedCamera->frustum.pos -= selectedCamera->frustum.up.Normalized() * selectedCamera->mSpeed;
		break;
	case Directions::FORWARD:
		selectedCamera->frustum.pos += selectedCamera->frustum.front.Normalized() * selectedCamera->mSpeed;
		break;
	case Directions::BACKWARD:
		selectedCamera->frustum.pos -= selectedCamera->frustum.front.Normalized() * selectedCamera->mSpeed;
		break;
	case Directions::LEFT:
		selectedCamera->frustum.pos += selectedCamera->frustum.up.Cross(selectedCamera->frustum.front).Normalized() * selectedCamera->mSpeed;
		break;
	case Directions::RIGHT:
		selectedCamera->frustum.pos -= selectedCamera->frustum.up.Cross(selectedCamera->frustum.front).Normalized() * selectedCamera->mSpeed;
		break;
	}
}


void ModuleCamera::MouseUpdate()
{
	if (selectedCamera == nullptr)
	{
		return;
	}

	iPoint mousePosition = App->input->GetMousePosition();

	if (selectedCamera->firstMouse) {
		selectedCamera->lastX = mousePosition.x;
		selectedCamera->lastY = mousePosition.y;
		selectedCamera->firstMouse = false;
	}

	float xoffset = (float)mousePosition.x - (float)selectedCamera->lastX;
	float yoffset = (float)selectedCamera->lastY - (float)mousePosition.y;
	selectedCamera->lastX = mousePosition.x;
	selectedCamera->lastY = mousePosition.y;

	xoffset *= 0.5f;
	yoffset *= 0.5f;

	selectedCamera->yaw += xoffset;
	selectedCamera->pitch += yoffset;

	selectedCamera->pitch = math::Clamp(selectedCamera->pitch, -89.0f, 89.0f);

	math::float3 front;

	front.x = SDL_sinf(math::DegToRad(selectedCamera->yaw)) * SDL_cosf(math::DegToRad(selectedCamera->pitch));
	front.y = SDL_sinf(math::DegToRad(selectedCamera->pitch));
	front.z = -SDL_cosf(math::DegToRad(selectedCamera->yaw)) * SDL_cosf(math::DegToRad(selectedCamera->pitch));
	selectedCamera->frustum.front = front.Normalized();
}

void ModuleCamera::SetPlaneDistances(float nearDist, float farDist)
{
	if (nearDist > 0.0f && nearDist < selectedCamera->frustum.farPlaneDistance)
	{
		selectedCamera->frustum.nearPlaneDistance = nearDist;
	}

	if (farDist > 0.0f && farDist > selectedCamera->frustum.nearPlaneDistance)
	{
		selectedCamera->frustum.farPlaneDistance = farDist;
	}
}


