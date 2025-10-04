#include "AssetManager.h"

#include "../Util.hpp"

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
	std::vector<OpenGLDetachedMesh> g_meshes; // TODO: make it unique ptr
	std::vector<std::unique_ptr<Model>> g_models;
	std::unordered_map<std::string, int> g_modelIndexMap;

	void LoadModelsAsync();
	void ProcessPendingUploads();

	void Init() {
		LoadModelsAsync();
	}

	void Update() {
		ProcessPendingUploads();

		// TODO: Texture baking
	}
	/*
	███    ███  ██████  ██████  ███████ ██      ███████
	████  ████ ██    ██ ██   ██ ██      ██      ██
	██ ████ ██ ██    ██ ██   ██ █████   ██      ███████
	██  ██  ██ ██    ██ ██   ██ ██      ██           ██
	██      ██  ██████  ██████  ███████ ███████ ███████ */

	void LoadModelsAsync() {
		g_running = true;
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

					auto model = std::make_unique<Model>();
					model->SetName(modelData.name);
					Model* rawModel = model.get();

					{
						std::lock_guard<std::mutex> lock(g_queueMutex);

						int modelIndex = static_cast<int>(g_models.size());
						g_models.push_back(std::move(model));
						g_modelIndexMap[rawModel->GetName()] = modelIndex;

						g_pendingUploads.emplace(rawModel, std::move(modelData));
					}

					task.promise.set_value(rawModel);
				}
			}
			});

		for (FileInfo& fileInfo : Util::IterateDirectory("res/models")) {
			std::string name = Util::GetFileName(fileInfo.path);
			if (g_scheduledModels.find(name) != g_scheduledModels.end())
				return;
			g_scheduledModels.insert(name);

			PendingModel task;
			task.filePath = fileInfo.path;

			std::lock_guard<std::mutex> lock(g_taskMutex);
			g_importTasks.push(std::move(task));
		}
	}

	void ProcessPendingUploads() {
		using clock = std::chrono::high_resolution_clock;

		std::queue<std::pair<Model*, ModelData>> uploads;
		{
			std::lock_guard<std::mutex> lock(g_queueMutex);
			std::swap(uploads, g_pendingUploads);
		}

		while (!uploads.empty()) {
			auto [model, data] = std::move(uploads.front());
			uploads.pop();

			auto start = clock::now();

			LoadModelFromData(*model, data);

			auto end = clock::now();

			auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(end - start);
			std::cout << "\n[Upload] Model " << "'" + model->GetName() + "'" << " uploaded to GPU in " << duration.count() << " ms";

			/*g_modelIndexMap[model->GetName()] = static_cast<int>(g_models.size() - 1);*/
			model->SetLoadStatus(true);
		}
	}

	void ShutdownThread() {
		g_running = false;
		if (loaderThread.joinable()) {
			loaderThread.join();
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
			return g_models[it->second].get();
		}
		return nullptr;
	}

	Model* GetModelByIndex(int index) {
		if (index != -1) {
			return g_models[index].get();
		}
		std::cout << "AssetManager::GetModelByIndex() failed because index = " << index << "\n";
		return nullptr;
	}

	Model* CreateModel(const std::string& name) {
		g_models.emplace_back(std::make_unique<Model>());
		Model* model = g_models.back().get();
		model->SetName(name);
		return model;
	}

	int GetModelIndexByName(const std::string& name) {
		auto it = g_modelIndexMap.find(name);
		if (it != g_modelIndexMap.end()) {
			return it->second;
		}

		std::cout << "AssetManager::GetModelIndexByName() failed because " << name << " was not found" << "\n";
		return -1;
	}

	/*
	███    ███ ███████ ███████ ██   ██ ███████ ███████
	████  ████ ██      ██      ██   ██ ██      ██
	██ ████ ██ █████   ███████ ███████ █████   ███████
	██  ██  ██ ██           ██ ██   ██ ██           ██
	██      ██ ███████ ███████ ██   ██ ███████ ███████*/

	int CreateMesh(const std::string& name, std::vector<Vertex>& vertices, std::vector<uint32_t> indices, glm::vec3 aabbMin, glm::vec3 aabbMax) {
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
}