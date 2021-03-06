#include "PanelEditor.h"
#include "Application.h"
#include "ModuleRender.h"
#include "ModuleTime.h"
#include "ModuleScene.h"



PanelEditor::PanelEditor()
{
	show = true;
}


PanelEditor::~PanelEditor()
{
}

void PanelEditor::Draw()
{
	ImGui::Begin("Editor", &show);
	if (ImGui::ArrowButton("Play", ImGuiDir_Right)) 
	{
		if (App->time->gameState == State::STOP) 
		{
			App->scene->SaveSceneJSON("/Library/Scene/temp_scene.json");
			App->time->Start();
		}
		else 
		{
			App->time->Stop();
			App->scene->cleanScene = true;
			App->scene->loadScene = true;
			App->scene->sceneName = "/Library/Scene/temp_scene.json";
		}
	}
	ImGui::SameLine();
	if (ImGui::Button("||", { 23,19 })) 
	{
		if (App->time->gameState == State::RUN) 
		{
			App->time->Pause(true);
		}
		else if (App->time->gameState == State::PAUSE) 
		{
			App->time->Pause(false);
		}
	}
	ImGui::SameLine();
	ImGui::Checkbox("Axis", &App->renderer->showAxis);
	ImGui::SameLine();
	ImGui::Checkbox("Grid", &App->renderer->showGrid);
	ImGui::SameLine();
	ImGui::Checkbox("Frustum culling", &App->renderer->frustumCulling);
	ImGui::SameLine();
	ImGui::Checkbox("MSAA", &App->renderer->msaa);
	ImGui::SameLine();
	if (ImGui::Checkbox("VSync", &App->renderer->enableVSync)) 
	{
		if (App->renderer->enableVSync) 
		{
			SDL_GL_SetSwapInterval(1);
		}
		else 
		{
			SDL_GL_SetSwapInterval(0);
		}
	}
	ImGui::SameLine();
	const char* items[] = { "Translate", "Rotate", "Scale" };
	ImGui::PushItemWidth(120);
	ImGui::Combo("Guizmo", &App->renderer->guizmoType, items, IM_ARRAYSIZE(items));
	ImGui::PopItemWidth();
	ImGui::End();
}
