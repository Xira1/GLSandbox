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
	}
}

