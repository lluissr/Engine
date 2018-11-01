#ifndef __ModuleModelLoader_h__
#define __ModuleModelLoader_h__

#include "Module.h"
#include "Globals.h"
#include <assimp/mesh.h>
#include <assimp/material.h>
#include "GL/glew.h"
#include <vector>



class ModuleModelLoader : public Module
{
public:
	ModuleModelLoader();
	~ModuleModelLoader();

	bool Init();
	bool CleanUp();

	void GenerateMeshData(const aiMesh* aiMesh);
	void GenerateMaterialData(const aiMaterial* aiMaterial);


	struct Mesh
	{
		unsigned vbo = 0;
		unsigned ibo = 0;
		unsigned material = 0;
		unsigned numVertices = 0;
		unsigned numIndices = 0;
	};

	struct Material
	{
		unsigned texture0 = 0;
	};

	std::vector<Mesh> meshes;
	std::vector<Material> materials;
};

#endif
