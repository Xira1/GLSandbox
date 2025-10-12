#pragma once

#include <string>
#include <memory>
#include "../API/OpenGL/Types/GL_texture.h"
#include "../Common/Enums.h"

struct Texture {
public:
	Texture() = default;
	void Load();
	void SetLoadingState(LoadingState loadingState);
	void SetFileInfo(FileInfo fileInfo);
	void SetImageDataType(ImageDataType imageDataType);
	void SetTextureWrapMode(TextureWrapMode wrapMode);
	void SetMinFilter(TextureFilter filter);
	void SetMagFilter(TextureFilter filter);
	void RequestMipMaps();
	void SetTextureDataLevelBakeState(int index, BakeState state);
	void CheckForBakeCompletion();
	const bool BakeComplete();
	const bool MipmapsAreRequested();
	const int GetTextureDataCount();
	const int GetWidth(int mipmapLevel);
	const int GetHeight(int mipmapLevel);
	const int GetFormat();
	const int GetInternalFormat();
	const int GetMipmapLevelCount();
	const int GetDataSize(int mipmapLevel);
	const void* GetData(int mipmapLevel);
	const std::string& GetFileName();
	const std::string& GetFilePath();
	const LoadingState GetLoadingState();
	const FileInfo GetFileInfo();
	const ImageDataType GetImageDataType();
	const TextureWrapMode GetTextureWrapMode();
	const TextureFilter GetMinFilter();
	const TextureFilter GetMagFilter();
	OpenGLTexture& GetGLTexture();

private:
	OpenGLTexture m_glTexture;
	LoadingState m_loadingState = LoadingState::AWAITING_LOADING_FROM_DISK;
	ImageDataType m_imageDataType = ImageDataType::UNDEFINED;
	TextureWrapMode m_wrapMode = TextureWrapMode::REPEAT;
	TextureFilter m_minFilter = TextureFilter::NEAREST;
	TextureFilter m_magFilter = TextureFilter::NEAREST;
	FileInfo m_fileInfo;
	std::vector<TextureData> m_textureDataLevels;
	std::vector<BakeState> m_textureDataLevelBakeStates;
	int m_mipmapLevelCount = 0;
	bool m_mipmapsRequested = false;
	bool m_bakeComplete = false;
};

