#ifndef __ModuleRender_h__
#define __ModuleRender_h__

#include "Module.h"
#include "Globals.h"
#include "ModuleModelLoader.h"
#include <vector>

struct SDL_Texture;
struct SDL_Renderer;
struct SDL_Rect;
class QuadtreeNode;

enum class FrameBufferType
{
	EDITOR,
	GAME
};

struct FrameBuffer
{
	unsigned fbo = 0u;
	unsigned rbo = 0u;
	unsigned fboDepth = 0u;
	unsigned renderTexture = 0u;
	FrameBufferType frameBufferType;

	unsigned msfbo = 0u;
	unsigned msfboColor = 0u;
	unsigned msfbDepth = 0u;

};

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
	void RenderGameObject(GameObject* gameObject, math::float4x4 viewMatrix, math::float4x4 projectionMatrix);
	void RenderMesh(const Mesh& mesh, const Material& material, math::float4x4 modelMatrix, math::float4x4 viewMatrix, math::float4x4 projectionMatrix, bool active);
	void DrawInFrameBuffer(FrameBuffer& frameBuffer);
	void UpdateDrawDebug(FrameBuffer& frameBuffer, math::float4x4 viewMatrix, math::float4x4 projectionMatrix);
	void DrawQuadTreeNode(QuadtreeNode* node);
	void InitFrameBuffer(int width, int height, FrameBuffer& frameBuffer);
	void DrawImGuizmo(float width, float height);

	void* context;
	FrameBuffer frameBufferScene;
	FrameBuffer frameBufferGame;
	bool renderBoundingBoxes = true;
	bool showAxis = true;
	bool showGrid = true;
	bool frustumCulling = true;
	bool enableVSync = true;
	bool msaa = true;

	std::vector<ComponentMesh*> meshes;

private:
	unsigned GenerateFallback();
	
	unsigned fallback = 0;
	std::vector<GameObject*> gameObjectsCollideQuadtree;
};

#endif