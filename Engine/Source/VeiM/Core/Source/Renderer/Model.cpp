#include "Model.h"
#include "Test/TestRenderer.h"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

namespace VeiM
{

	void Model::Draw(Shader& shader)
	{

		for (unsigned int i = 0; i < m_Meshes.size(); i++)
			TestRenderer::RenderMesh(m_Meshes[i], shader);
	}

	void Model::loadModel(fs::path filepath)
	{
		Assimp::Importer importerr;
		const aiScene* scene = importerr.ReadFile(filepath.string(), aiProcess_Triangulate | aiProcess_FlipUVs);

		if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode)
		{
			VM_CORE_ASSERT(false);
			VM_CORE_ERROR("[ASSIMP] {0}", importerr.GetErrorString());
			return;
		}
		m_Directory = filepath.parent_path();
		processNode(scene->mRootNode, scene);
	}

	void Model::processNode(aiNode* node, const aiScene* scene)
	{
		for (uint32 i = 0; i < node->mNumMeshes; i++)
		{
			aiMesh* mesh = scene->mMeshes[node->mMeshes[i]];
			m_Meshes.push_back(processMesh(mesh, scene));
		}

		for (uint32 i = 0; i < node->mNumChildren; i++)
		{
			processNode(node->mChildren[i], scene);
		}
	}

	VeiM::IMesh* Model::processMesh(aiMesh* mesh, const aiScene* scene)
	{
		std::vector<glm::vec3> positions;
		std::vector<glm::vec2> uvs;
		std::vector<glm::vec3> normals;
		std::vector<uint32> indices;
		Texture diffuseTex;
		Texture specualrTex;

		positions.resize(mesh->mNumVertices);
		normals.resize(mesh->mNumVertices);
		if (mesh->mNumUVComponents[0] > 0)
		{
			uvs.resize(mesh->mNumVertices);
		}

		indices.resize(mesh->mNumFaces * 3);

		for (uint32 i = 0; i < mesh->mNumVertices; i++)
		{
			positions[i] = glm::vec3(mesh->mVertices[i].x, mesh->mVertices[i].y, mesh->mVertices[i].z);
			normals[i] = glm::vec3(mesh->mNormals[i].x, mesh->mNormals[i].y, mesh->mNormals[i].z);
			if (mesh->mTextureCoords[0])
			{
				uvs[i] = glm::vec2(mesh->mTextureCoords[0][i].x, mesh->mTextureCoords[0][i].y);
			}
		}
		for (int32 f = 0; f < mesh->mNumFaces; f++)
		{
			aiFace face = mesh->mFaces[f];
			VM_CORE_ASSERT(face.mNumIndices == 3, "Non-triangulated face detected!");
			for (uint32 i = 0; i < 3; i++)
			{
				indices[f * 3 + i] = face.mIndices[i];
			}
		}
		if (mesh->mMaterialIndex >= 0)
		{
			aiMaterial* material = scene->mMaterials[mesh->mMaterialIndex];
			std::vector<Texture> diffuseMaps = loadMaterialTextures(material, aiTextureType_DIFFUSE, StringID("texture_diffuse"));

			if (!diffuseMaps.empty())
				diffuseTex = diffuseMaps[0];

			std::vector<Texture> specularMaps = loadMaterialTextures(material, aiTextureType_SPECULAR, StringID("texture_specular"));
			if (!specularMaps.empty())
				specualrTex = specularMaps[0];
		}
		IMesh* newMesh = new IMesh();
		newMesh->Positions = positions;
		newMesh->UV = uvs;
		newMesh->Normals = normals;
		newMesh->Indices = indices;
		newMesh->Topology = ETopology::Triangles;
		newMesh->Tdiffuse = diffuseTex;
		newMesh->Tspecualr = specualrTex;
		newMesh->Finilize();
		return newMesh;
	}

	std::vector<Texture> Model::loadMaterialTextures(aiMaterial* mat, aiTextureType type, StringID typeName)
	{
		std::vector<Texture> textures;
		for (uint32 i = 0; i < mat->GetTextureCount(type); i++)
		{
			aiString str;
			mat->GetTexture(type, i, &str);
			bool skip = false;
			for (uint32 j = 0; j < m_TexturesLoaded.size(); j++)
			{
				if (m_TexturesLoaded[j].Path == str.C_Str())
				{
					textures.push_back(m_TexturesLoaded[j]);
					skip = true;
					break;
				}
			}
			if (!skip)
			{
				ETextureColorSpace colorSpace = ETextureColorSpace::Linear;
				if (typeName == StringID("texture_diffuse"))
					colorSpace = ETextureColorSpace::sRGB;
				Texture texture = TextureFromFile(m_Directory / str.C_Str(), colorSpace);
				texture.Type = typeName;
				texture.Path = str.C_Str();
				textures.push_back(texture);
				m_TexturesLoaded.push_back(texture);
			}
		}
		return textures;
	}

}

