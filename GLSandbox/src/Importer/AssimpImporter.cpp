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

		// Generate tangent for models if imported model has no tangent
		for (int i = 0; i < meshData.indices.size(); i += 3) {
			Vertex* vert0 = &meshData.vertices[meshData.indices[i]];
			Vertex* vert1 = &meshData.vertices[meshData.indices[i + 1]];
			Vertex* vert2 = &meshData.vertices[meshData.indices[i + 2]];
			glm::vec3 deltaPos1 = vert1->position - vert0->position;
			glm::vec3 deltaPos2 = vert2->position - vert0->position;
			glm::vec2 deltaUV1 = vert1->uv - vert0->uv;
			glm::vec2 deltaUV2 = vert2->uv - vert0->uv;
			float det = (deltaUV1.x * deltaUV2.y - deltaUV1.y * deltaUV2.x);
			float invDet = 1.0f / det;
			glm::vec3 tangent = invDet * (-deltaUV2.y * deltaPos1 - deltaUV1.y * deltaPos2);
			glm::vec3 bitangent = invDet * (-deltaUV2.x * deltaPos1 + deltaUV1.x * deltaPos2);
			tangent = glm::normalize(tangent);
			bitangent = glm::normalize(bitangent);
			vert0->tangent = tangent;
			vert1->tangent = tangent;
			vert2->tangent = tangent;
		}

		// Compute model AABB
		modelData.aabbMin = Util::Vec3Min(modelData.aabbMin, meshData.aabbMin);
		modelData.aabbMax = Util::Vec3Max(modelData.aabbMax, meshData.aabbMax);
	}

	importer.FreeScene();
	return modelData;
}