#include "AssimpImporter.h"

#include "Common.h"
#include <assimp/matrix3x3.h>
#include <assimp/matrix4x4.h>
#include <assimp/Importer.hpp>
#include <assimp/Scene.h>
#include <assimp/PostProcess.h>
#include "../Util.hpp"

ModelData AssimpImporter::ImportModel(const std::string filePath)
{
	ModelData modelData;
	Assimp::Importer importer;

	importer.SetPropertyBool(AI_CONFIG_PP_FD_REMOVE, true);

	const aiScene* scene = importer.ReadFile(filePath,
		aiProcess_Triangulate | 
		aiProcess_FlipUVs | 
		aiProcess_ImproveCacheLocality | 
		aiProcess_JoinIdenticalVertices |
		aiProcess_RemoveRedundantMaterials
		);
	if (!scene) {
		std::cerr << "AssimpImporter::ImportModel() failed " << filePath << "\n";
		return modelData;
	}

	modelData.name = Util::GetFileName(filePath);
	modelData.meshCount = scene->mNumMeshes;
	modelData.meshes.resize(modelData.meshCount);

	// Preallocate vector memory 
	for (int i = 0; i < modelData.meshes.size(); i++) {
		MeshData& meshData = modelData.meshes[i];
		meshData.vertexCount = scene->mMeshes[i]->mNumVertices;
		meshData.indicesCount = scene->mMeshes[i]->mNumFaces * 3;
		meshData.name = scene->mMeshes[i]->mName.C_Str();
		meshData.vertices.resize(meshData.vertexCount);
		meshData.indices.resize(meshData.indicesCount);
		meshData.name = meshData.name.substr(0, meshData.name.find('.'));
	}

	// Populate vectors
	for (int i = 0; i < modelData.meshes.size(); i++) {
		MeshData& meshData = modelData.meshes[i];
		const aiMesh* assimpMesh = scene->mMeshes[i];

		// Vertex
		for (unsigned int j = 0; j < meshData.vertexCount; j++) {
			meshData.vertices[j] = (Vertex{
				// Position
				glm::vec3(assimpMesh->mVertices[j].x, assimpMesh->mVertices[j].y, assimpMesh->mVertices[j].z),
				// Normals
				glm::vec3(assimpMesh->mNormals[j].x, assimpMesh->mNormals[j].y, assimpMesh->mNormals[j].z),
				// UV
				assimpMesh->HasTextureCoords(0) ? glm::vec2(assimpMesh->mTextureCoords[0][j].x, assimpMesh->mTextureCoords[0][j].y) : glm::vec2(0.0f, 0.0f),
				// Tangent
				assimpMesh->HasTangentsAndBitangents() ? glm::vec3(assimpMesh->mTangents[j].x, assimpMesh->mTangents[j].y, assimpMesh->mTangents[j].z) : glm::vec3(0.0f, 0.0f, 0.0f)
				});

			// Compute mesh AABB
			meshData.aabbMin = Util::Vec3Min(meshData.vertices[j].position, meshData.aabbMin);
			meshData.aabbMax = Util::Vec3Max(meshData.vertices[j].position, meshData.aabbMax);
		}

		// Get indices
		for (unsigned int j = 0; j < assimpMesh->mNumFaces; j++) {
			const aiFace& faces = assimpMesh->mFaces[j];
			unsigned int baseInd = j * 3;
			meshData.indices[baseInd] = faces.mIndices[0];
			meshData.indices[baseInd + 1] = faces.mIndices[1];
			meshData.indices[baseInd + 2] = faces.mIndices[2];
		}

		// Normalize vertex if not normalized
		for (Vertex& vert : meshData.vertices) {
			vert.normal = glm::normalize(vert.normal);
		}

		// TODO: Generate tangent for models if imported model has no tangent


		// Compute model AABB
		modelData.aabbMin = Util::Vec3Min(modelData.aabbMin, meshData.aabbMin);
		modelData.aabbMax = Util::Vec3Max(modelData.aabbMax, meshData.aabbMax);
	}

	importer.FreeScene();
	return modelData;
}