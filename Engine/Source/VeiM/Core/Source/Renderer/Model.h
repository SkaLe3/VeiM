#pragma once
#include "CoreDefines.h"
#include "Renderer/Shader.h"
#include "Renderer/Mesh.h"
#include "Renderer/Texture.h"

#include <assimp/material.h>

struct aiNode;
struct aiMesh;
struct aiScene;


namespace VeiM
{


	class Model
	{
	public:
		Model(fs::path filepath)
		{
			loadModel(filepath);
		}
		void Draw(Shader& shader);
		uint32 GetMeshesCount() { return m_Meshes.size(); }
		IMesh* GetMesh(uint32 index) { return m_Meshes[index]; }
	private:
		// model data
		std::vector<IMesh*> m_Meshes;
		fs::path m_Directory;
		std::vector<Texture> m_TexturesLoaded;

		void loadModel(fs::path filepath);
		void processNode(aiNode* node, const aiScene* scene);
		IMesh* processMesh(aiMesh* mesh, const aiScene* scene);
		std::vector<Texture> loadMaterialTextures(aiMaterial* mat, aiTextureType type,
			StringID typeName);
	};
}