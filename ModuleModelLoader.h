#ifndef __ModuleModelLoader_h__
#define __ModuleModelLoader_h__

#include "Application.h"
#include "Module.h"
#include "Globals.h"
#include <assimp/mesh.h>
#include <assimp/material.h>
#include "GL/glew.h"
#include "MathGeoLib.h"
#include <vector>
#include "GameObject.h"
#include <string>
#include "ModuleTextures.h"

struct aiScene;
struct aiNode;
struct par_shapes_mesh_s;

class Mesh
{
public:
	unsigned vbo = 0;
	unsigned ibo = 0;
	unsigned material = 0;
	unsigned numVertices = 0;
	unsigned numIndices = 0;
	unsigned normalsOffset = 0;
	unsigned vertexSize = 0;
	unsigned vao = 0;
	unsigned texturesOffset = 0;
	math::AABB localBoundingBox;
	math::AABB globalBoundingBox;
	bool useWireframe = false;

	float* normals = nullptr;
	unsigned* indices = nullptr;
	float* vertices = nullptr;
	float* texCoords = nullptr;
	std::string meshName;

	Mesh() {}
	~Mesh()
	{
		RELEASE_ARRAY(normals);
		RELEASE_ARRAY(indices);
		RELEASE_ARRAY(vertices);
		RELEASE_ARRAY(texCoords);
		/*glDeleteVertexArrays(1, &vao);
		glDeleteBuffers(1, &vbo);
		glDeleteBuffers(1, &ibo);*/
	}
};

class Material
{
public:
	unsigned diffuseMap = 0u;
	std::string diffuseMapName;
	math::float4 diffuseColor = math::float4(1.0f, 1.0f, 1.0f, 1.0f);
	float k_diffuse = 0.0f;
	int diffuseWidth = 0;
	int diffuseHeight = 0;

	unsigned occlusionMap = 0u;
	std::string occlusionMapName;
	float k_ambient = 0.0f;
	int ambientWidth = 0;
	int ambientHeight = 0;

	unsigned specularMap = 0u;
	std::string specularMapName;
	math::float4 specularColor = math::float4(1.0f, 1.0f, 1.0f, 1.0f);
	float k_specular = 0.0f;
	float shininess = 0.0f;
	int specularWidth = 0;
	int specularHeight = 0;

	unsigned emissiveMap = 0u;
	std::string emissiveMapName;
	math::float4 emissiveColor = math::float4(0.0f, 0.0f, 0.0f, 0.0f);
	int emissiveWidth = 0;
	int emissiveHeight = 0;

	Material() {}
	~Material()
	{
		App->textures->Unload(diffuseMap, diffuseMapName.c_str());
		App->textures->Unload(occlusionMap, occlusionMapName.c_str());
		App->textures->Unload(specularMap, specularMapName.c_str());
		App->textures->Unload(emissiveMap, emissiveMapName.c_str());
	}
};

class ModuleModelLoader : public Module
{
public:
	ModuleModelLoader();
	~ModuleModelLoader();

	bool Init();
	bool CleanUp();

	bool LoadSphere(const char* name, float size, unsigned slices, unsigned stacks, const math::float4& color);
	bool LoadTorus(const char* name, float innerRadius, float outerRadius, unsigned slices, unsigned stacks, const math::float4& color);
	Mesh* CreateMeshFromParShapes(par_shapes_mesh_s* mesh);

	bool Import(const char* path);
	bool ImportMesh(const aiMesh* aiMesh, const char* name);
	bool SaveMesh(Mesh* mesh, std::string& newpath);
	Mesh* Load(const char* path);
	void GenerateVBO(Mesh& mesh);
	void GenerateVAO(Mesh& mesh);
};

#endif
