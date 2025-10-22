#include "AssetManager.h"

#include "BakeQueue/BakeQueue.h"
#include "../Util.hpp"
#include "../API/OpenGL/GL_backend.h"
#include "../Types/TextureTools/TextureTools.h"
#include "../World/Room/Room.hpp"

namespace AssetManager {
	// Thread parts
	std::queue<PendingModel> g_importTasks;
	std::queue<std::pair<Model*, ModelData>> g_pendingUploads;
	std::mutex g_taskMutex;
	std::mutex g_queueMutex;
	std::unordered_set<std::string> g_scheduledModels;
	std::atomic<bool> g_running = false;
	static std::thread loaderThread;

	// Asset parts
	std::deque<Model> g_models;
	std::vector<OpenGLDetachedMesh> g_meshes;
	std::vector<Texture> g_textures;
	std::vector<Material> g_materials;
	std::unordered_map<std::string, int> g_modelIndexMap;
	std::unordered_map<std::string, int> g_textureIndexMap;
	std::unordered_map<std::string, int> g_materialIndexMap;

	// Models
	void LoadModelsAsync();
	void LoadPendingModelsAsync();

	// Textures
	void LoadTextureMinimum();
	void LoadTexturesAsync();
	void LoadTexture(Texture* texture);
	void LoadPendingTexturesAsync();
	void CompressMissingDDSTexutres();

	// Materials
	void BuildMaterials();
	bool IsAlbedo(const FileInfo& fileInfo);
	std::string GetMaterialNameFromFileInfo(const FileInfo& fileInfo);

	void Init() {
		CompressMissingDDSTexutres();
		HardcodedRoom::LoadHardcodedModelRoom();
		LoadTextureMinimum();
		LoadModelsAsync();
		LoadTexturesAsync();
		BuildMaterials();
	}

	void Update() {
		for (Texture& texture : g_textures) {
			texture.CheckForBakeCompletion();
		}
	}

	/*
	███    ███  ██████  ██████  ███████ ██      ███████
	████  ████ ██    ██ ██   ██ ██      ██      ██
	██ ████ ██ ██    ██ ██   ██ █████   ██      ███████
	██  ██  ██ ██    ██ ██   ██ ██      ██           ██
	██      ██  ██████  ██████  ███████ ███████ ███████ */

	void LoadModelsAsync() {
		g_running = true;
		std::vector<std::future<Model*>> futures;

		for (FileInfo& fileInfo : Util::IterateDirectory("res/models", { "obj", "fbx" })) {
			std::string name = Util::GetFileName(fileInfo.path);
			if (g_scheduledModels.find(name) != g_scheduledModels.end())
				continue;
			g_scheduledModels.insert(name);

			PendingModel task;
			task.filePath = fileInfo.path;
			futures.push_back(task.promise.get_future());

			std::lock_guard<std::mutex> lock(g_taskMutex);
			g_importTasks.push(std::move(task));
		}

		loaderThread = std::thread([] {
			while (g_running) {
				PendingModel task;

				{
					std::lock_guard<std::mutex> lock(g_taskMutex);
					if (!g_importTasks.empty()) {
						task = std::move(g_importTasks.front());
						g_importTasks.pop();
					}
					else {
						std::this_thread::sleep_for(std::chrono::milliseconds(10));
						continue;
					}
				}

				if (!task.filePath.empty()) {
					ModelData modelData = AssimpImporter::ImportModel(task.filePath);

					Model model;
					model.SetName(modelData.name);

					{
						std::lock_guard<std::mutex> lock(g_queueMutex);
						int modelIndex = static_cast<int>(g_models.size());
						g_models.push_back(std::move(model));

						g_modelIndexMap[g_models[modelIndex].GetName()] = modelIndex;
						g_pendingUploads.emplace(&g_models[modelIndex], std::move(modelData));
					}

					task.promise.set_value(&g_models.back());
				}
			}
			});

		for (auto& f : futures) {
			f.get();
		}

		LoadPendingModelsAsync();

		g_running = false;
		if (loaderThread.joinable()) {
			loaderThread.join();
		}
	}

	void LoadPendingModelsAsync() {
		std::queue<std::pair<Model*, ModelData>> uploads;
		{
			std::lock_guard<std::mutex> lock(g_queueMutex);
			std::swap(uploads, g_pendingUploads);
		}

		while (!uploads.empty()) {
			auto [model, data] = std::move(uploads.front());
			uploads.pop();

			auto start = std::chrono::high_resolution_clock::now();

			LoadModelFromData(*model, data);

			auto end = std::chrono::high_resolution_clock::now();
			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);

			std::cout << "\n\033[32m[Upload]\033[0m Model '" << model->GetName()
				<< "' uploaded to GPU in \033[32m" << duration.count() << "\033[0m ms";

			model->SetLoadStatus(true);
		}
	}

	void LoadModelFromData(Model& model, ModelData& modelData) {
		model.SetName(modelData.name);
		model.SetAABB(modelData.aabbMin, modelData.aabbMax);

		for (MeshData& meshData : modelData.meshes) {
			int meshIndex = CreateMesh(meshData.name, meshData.vertices, meshData.indices, meshData.aabbMin, meshData.aabbMax);
			model.AddMeshIndex(meshIndex);
		}

		model.SetLoadStatus(true);
	}

	Model* GetModelByName(const std::string& name) {
		auto it = g_modelIndexMap.find(name);
		if (it != g_modelIndexMap.end()) {
			return &g_models[it->second];
		}
		std::cout << "AssetManager::GetModelByName() failed because '" << name << "' was not found\n";
		return nullptr;
	}

	Model* GetModelByIndex(int index) {
		if (index >= 0 && index < g_models.size()) {
			return &g_models[index];
		}
		std::cout << "AssetManager::GetModelByIndex() failed because index = " << index << "\n";
		return nullptr;
	}

	Model* CreateModel(const std::string& name) {
		g_models.emplace_back(Model());
		Model* model = &g_models.back();
		model->SetName(name);
		g_modelIndexMap[name] = static_cast<int>(g_models.size() - 1);
		return model;
	}

	int GetModelIndexByName(const std::string& name) {
		auto it = g_modelIndexMap.find(name);
		if (it != g_modelIndexMap.end()) {
			return it->second;
		}

		std::cout << "AssetManager::GetModelIndexByName() failed because '" << name << "' was not found\n";
		return -1;
	}

	/*
	███    ███ ███████ ███████ ██   ██ ███████ ███████
	████  ████ ██      ██      ██   ██ ██      ██
	██ ████ ██ █████   ███████ ███████ █████   ███████
	██  ██  ██ ██           ██ ██   ██ ██           ██
	██      ██ ███████ ███████ ██   ██ ███████ ███████*/

	int CreateMesh(const std::string& name, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices, glm::vec3 aabbMin, glm::vec3 aabbMax) {
		OpenGLDetachedMesh& mesh = g_meshes.emplace_back();
		mesh.SetName(name);
		mesh.UpdateVertexBuffer(vertices, indices);
		mesh.aabbMin = aabbMin;
		mesh.aabbMax = aabbMax;
		return g_meshes.size() - 1;
	}

	int CreateMesh(const std::string& name, std::vector<Vertex>& vertices, std::vector<uint32_t>& indices) {
		glm::vec3 aabbMin = glm::vec3(std::numeric_limits<float>::min());
		glm::vec3 aabbMax = glm::vec3(-std::numeric_limits<float>::max());;
		for (int i = 0; i < indices.size(); i += 3) {
			Vertex* vert0 = &vertices[indices[i]];
			Vertex* vert1 = &vertices[indices[i + 1]];
			Vertex* vert2 = &vertices[indices[i + 2]];
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
			aabbMin = Util::Vec3Min(vert0->position, aabbMin);
			aabbMax = Util::Vec3Max(vert0->position, aabbMax);
			aabbMin = Util::Vec3Min(vert1->position, aabbMin);
			aabbMax = Util::Vec3Max(vert1->position, aabbMax);
			aabbMin = Util::Vec3Min(vert2->position, aabbMin);
			aabbMax = Util::Vec3Max(vert2->position, aabbMax);
		}
		OpenGLDetachedMesh& mesh = g_meshes.emplace_back();
		mesh.SetName(name);
		mesh.UpdateVertexBuffer(vertices, indices);
		mesh.aabbMin = aabbMin;
		mesh.aabbMax = aabbMax;
		return g_meshes.size() - 1;
	}

	OpenGLDetachedMesh* GetMeshByIndex(int index) {
		if (index >= 0 && index < g_meshes.size()) {
			return &g_meshes[index];
		}
		else {
			std::cout << "AssetManager::GetMeshByIndex() failed because " << index << " is out of range. Size is " << g_meshes.size();
			return nullptr;
		}
	}

	/*
	████████ ███████ ██   ██ ████████ ██    ██ ██████  ███████ ███████
	   ██    ██       ██ ██     ██    ██    ██ ██   ██ ██      ██
	   ██    █████     ███      ██    ██    ██ ██████  █████   ███████
	   ██    ██       ██ ██     ██    ██    ██ ██   ██ ██           ██
	   ██    ███████ ██   ██    ██     ██████  ██   ██ ███████ ███████*/

	void LoadTextureMinimum() {
		for (FileInfo& fileInfo : Util::IterateDirectory("res/textures/load_at_init/uncompressed", { "png", "jpg" })) {
			Texture& texture = g_textures.emplace_back();
			texture.SetFileInfo(fileInfo);
			texture.SetImageDataType(ImageDataType::UNCOMPRESSED);
			texture.SetTextureWrapMode(TextureWrapMode::CLAMP_TO_EDGE);
			texture.SetMinFilter(TextureFilter::LINEAR_MIPMAP);
			texture.SetMagFilter(TextureFilter::LINEAR);
			texture.RequestMipMaps();
		}

		for (FileInfo& fileInfo : Util::IterateDirectory("res/textures/load_at_init/uncompressed_no_mipmaps", { "png", "jpg", })) {
			Texture& texture = g_textures.emplace_back();
			texture.SetFileInfo(fileInfo);
			texture.SetImageDataType(ImageDataType::UNCOMPRESSED);
			texture.SetTextureWrapMode(TextureWrapMode::CLAMP_TO_EDGE);
			texture.SetMinFilter(TextureFilter::NEAREST);
			texture.SetMagFilter(TextureFilter::NEAREST);
		}

		LoadPendingTexturesAsync();

		BakeQueue::ImmediateBakeAllTexture();

		for (int i = 0; i < g_textures.size(); i++) {
			g_textureIndexMap[g_textures[i].GetFileInfo().name] = i;
		}
	}

	void LoadTexturesAsync() {
		for (FileInfo& fileInfo : Util::IterateDirectory("res/textures/compressed", { "dds" })) {
			Texture& texture = g_textures.emplace_back();
			texture.SetFileInfo(fileInfo);
			texture.SetImageDataType(ImageDataType::COMPRESSED);
			texture.SetTextureWrapMode(TextureWrapMode::REPEAT);
			texture.SetMinFilter(TextureFilter::LINEAR_MIPMAP);
			texture.SetMagFilter(TextureFilter::LINEAR);
			texture.RequestMipMaps();
		}

		for (FileInfo& fileInfo : Util::IterateDirectory("res/textures/uncompressed_no_mipmaps", { "png", "jpg" })) {
			Texture& texture = g_textures.emplace_back();
			texture.SetFileInfo(fileInfo);
			texture.SetImageDataType(ImageDataType::UNCOMPRESSED);
			texture.SetTextureWrapMode(TextureWrapMode::CLAMP_TO_EDGE);
			texture.SetMinFilter(TextureFilter::NEAREST);
			texture.SetMagFilter(TextureFilter::NEAREST);
		}

		LoadPendingTexturesAsync();

		for (int i = 0; i < g_textures.size(); i++) {
			g_textureIndexMap[g_textures[i].GetFileInfo().name] = i;
		}
	}

	void LoadPendingTexturesAsync() {
		std::vector<std::future<void>> fut;
		for (Texture& texture : g_textures) {
			if (texture.GetLoadingState() == LoadingState::AWAITING_LOADING_FROM_DISK) {
				texture.SetLoadingState(LoadingState::LOADING_FROM_DISK);
				fut.emplace_back(std::async(std::launch::async, LoadTexture, &texture));
			}
		}

		for (auto& future : fut) {
			future.get();
		}

		// Allocate memory
		for (Texture& texture : g_textures) {
			OpenGLBackend::AllocateTextureMemory(texture);
		}
	}

	void CompressMissingDDSTexutres() {
		for (FileInfo& fileInfo : Util::IterateDirectory("res/textures/will_compress", { "png", "jpg", "tga" })) {
			std::string inputFile = fileInfo.path;
			std::string outputFile = "res/textures/compressed/" + fileInfo.name + ".dds";
			if (!Util::FileExist(outputFile)) {
				TextureTools::CreateAndExportDDS(inputFile, outputFile, true);
				std::cout << "Exported " << outputFile << "\n";
			}
		}
	}

	void LoadTexture(Texture* texture) {
		if (texture) {
			texture->Load();
			BakeQueue::QueueTextureForBaking(texture);
		}
	}

	int GetTextureIndexByName(const std::string& name, bool warning) {
		for (int i = 0; i < g_textures.size(); i++) {
			if (g_textures[i].GetFileInfo().name == name) {
				return i;
			}
		}
		if (!warning) {
			std::cout << "AssetManager::GetTextureIndexByName() failed because '" << name << "' was not found" << "\n";
		}
		return -1;
	}

	int GetTextureCount() {
		return g_textures.size();
	}

	Texture* GetTextureByIndex(int index) {
		if (index != -1) {
			return &g_textures[index];
		}
		std::cout << "AssetManager::GetTextureByIndex() failed because index = " << index << "\n";
		return nullptr;
	}

	Texture* GetTextureByName(const std::string& name) {
		for (Texture& texture : g_textures) {
			if (texture.GetFileInfo().name == name) {
				return &texture;
			}
		}

		std::cout << "AssetManager::GetTextureByName() failed because " << name << " was not found" << "\n";
		return nullptr;
	}

	/*
	███    ███  █████  ████████ ███████ ██████  ██  █████  ██      ███████
	████  ████ ██   ██    ██    ██      ██   ██ ██ ██   ██ ██      ██
	██ ████ ██ ███████    ██    █████   ██████  ██ ███████ ██      ███████
	██  ██  ██ ██   ██    ██    ██      ██   ██ ██ ██   ██ ██           ██
	██      ██ ██   ██    ██    ███████ ██   ██ ██ ██   ██ ███████ ███████*/

	bool IsAlbedo(const FileInfo& fileInfo) {
		if (fileInfo.name.size() >= 4 && fileInfo.name.substr(fileInfo.name.size() - 4) == "_ALB") {
			return true;
		}
		return false;
	}

	std::string GetMaterialNameFromFileInfo(const FileInfo& fileInfo) {
		const std::string suff = "_ALB";
		if (fileInfo.name.size() > suff.size() && fileInfo.name.substr(fileInfo.name.size() - suff.size()) == suff) {
			return fileInfo.name.substr(0, fileInfo.name.size() - suff.size());
		}

		return "";
	}

	void BuildMaterials() {
		g_materials.clear();
		for (Texture& texture : g_textures) {
			if (IsAlbedo(texture.GetFileInfo())) {
				Material& mat = g_materials.emplace_back(Material());
				mat.name = GetMaterialNameFromFileInfo(texture.GetFileInfo());
				int baseColorInd = GetTextureIndexByName(mat.name + "_ALB", true);
				int normalInd = GetTextureIndexByName(mat.name + "_NRM", true);
				int rmaInd = GetTextureIndexByName(mat.name + "_RMA", true);
				mat.m_baseColor = baseColorInd;
				mat.m_normal = (normalInd != -1) ? normalInd : GetTextureIndexByName("DefaultNRM");
				mat.m_rma = (rmaInd != -1) ? rmaInd : GetTextureIndexByName("DefaultRMA");
			}
		}

		for (int i = 0; i < g_materials.size(); i++) {
			g_materialIndexMap[g_materials[i].name] = i;
		}
	}

	int GetMaterialIndex(const std::string& name) {
		auto it = g_materialIndexMap.find(name);
		if (it != g_materialIndexMap.end()) {
			return it->second;
		}
		else {
			std::cout << "AssetManager::GetMaterialIndex() failed because " << name << " was not found" << "\n";
			return -1;
		}
	}

	std::string& GetMaterialNameByIndex(int index) {
		return g_materials[index].name;
	}

	Material* GetMaterialByIndex(int index) {
		if (index >= 0 && index < g_materials.size()) {
			Material* material = &g_materials[index];
			Texture* baseColor = AssetManager::GetTextureByIndex(material->m_baseColor);
			Texture* normal = AssetManager::GetTextureByIndex(material->m_normal);
			Texture* rma = AssetManager::GetTextureByIndex(material->m_rma);

			if (baseColor && baseColor->BakeComplete() &&
				normal && normal->BakeComplete() &&
				rma && rma->BakeComplete()) {
				return &g_materials[index];
			}
			else {
				GetDefaultMaterial();
			}
		}
		else {
			GetDefaultMaterial();
		}
	}

	Material* GetDefaultMaterial() {
		int index = GetMaterialIndex("Default");
		return GetMaterialByIndex(index);
	}
}