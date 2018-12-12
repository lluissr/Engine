#pragma once
#include "Module.h"
#include "Globals.h"
#include "ModuleModelLoader.h"

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;

class ModuleRender : public Module
{
public:
	ModuleRender();
	~ModuleRender();

	bool Init();
	update_status PreUpdate();
	update_status Update();
	update_status PostUpdate();
	bool CleanUp();
	void  RenderGameObject(GameObject* gameObject);
	void RenderMesh(const Mesh& mesh, const Material& material, math::float4x4 modelMatrix);
	void RenderBoundingBox() const;
	void InitFrameBuffer(int width, int height);

	void* context;
	bool renderBoundingBox = false;
	int currentItemSelected = 0;
	bool useCheckerTexture = false;

	unsigned renderTexture = 0u;
private:
	GLuint checkersTexture;
	unsigned fbo = 0u;
	unsigned rbo = 0u;
};
