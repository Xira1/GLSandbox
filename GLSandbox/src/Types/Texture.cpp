#include "Texture.h"
#include "TextureTools/TextureTools.h"

void Texture::Load() {
	if (m_imageDataType == ImageDataType::UNCOMPRESSED) {
		m_textureDataLevels = { TextureTools::LoadUncompressedTextureData(m_fileInfo.path) };
	}
	else if (m_imageDataType == ImageDataType::COMPRESSED) {
		m_textureDataLevels = TextureTools::LoadTextureDataFromDDSThreadSafe(m_fileInfo.path);
	}
	else if (m_imageDataType == ImageDataType::EXR) {
		m_textureDataLevels = { TextureTools::LoadEXRData(m_fileInfo.path) };
	}
	m_loadingState = LoadingState::LOADING_COMPLETE;

	m_mipmapLevelCount = 1 + static_cast<int>(std::log2(std::max(GetWidth(0), GetHeight(0))));

	m_textureDataLevelBakeStates.resize(m_textureDataLevels.size(), BakeState::AWAITING_BAKE);
}

const int Texture::GetWidth(int mipmapLevel) {
	if (mipmapLevel >= 0 && mipmapLevel < m_textureDataLevels.size()) {
		return m_textureDataLevels[mipmapLevel].m_width;
	}
	else {
		return 0;
	}
}

const int Texture::GetHeight(int mipmapLevel) {
	if (mipmapLevel >= 0 && mipmapLevel < m_textureDataLevels.size()) {
		return m_textureDataLevels[mipmapLevel].m_height;
	}
	else {
		return 0;
	}
}

void Texture::SetTextureDataLevelBakeState(int index, BakeState state) {
	if (index >= 0 && m_textureDataLevelBakeStates.size() && index < m_textureDataLevelBakeStates.size()) {
		m_textureDataLevelBakeStates[index] = state;
	}
	else {
		std::cout << "Texture::SetTextureDataLevelBakeState() failed, because " << index << " out of range\n";
	}
}

void Texture::CheckForBakeCompletion() {
	if (m_bakeComplete) {
		return;
	}
	else {
		m_bakeComplete = true;
		for (BakeState& state : m_textureDataLevelBakeStates) {
			if (state != BakeState::BAKE_COMPLETE) {
				m_bakeComplete = false;
				return;
			}
		}
	}
}

const bool Texture::BakeComplete() {
	return m_bakeComplete;
}

void Texture::SetLoadingState(LoadingState loadingState) {
	m_loadingState = loadingState;
}

void Texture::SetFileInfo(FileInfo fileInfo) {
	m_fileInfo = fileInfo;
}

void Texture::SetImageDataType(ImageDataType imageDataType) {
	m_imageDataType = imageDataType;
}

void Texture::SetTextureWrapMode(TextureWrapMode wrapMode) {
	m_wrapMode = wrapMode;
}

void Texture::SetMinFilter(TextureFilter filter) {
	m_minFilter = filter;
}

void Texture::SetMagFilter(TextureFilter filter) {
	m_magFilter = filter;
}

void Texture::RequestMipMaps() {
	m_mipmapsRequested = true;
}

const bool Texture::MipmapsAreRequested() {
	return m_mipmapsRequested;
}

const int Texture::GetTextureDataCount() {
	return m_textureDataLevels.size();
}

const int Texture::GetFormat() {
	if (!m_textureDataLevels.empty()) {
		return m_textureDataLevels[0].m_format;
	}
	else {
		return 0;
	}
}

const int Texture::GetInternalFormat() {
	if (!m_textureDataLevels.empty()) {
		return m_textureDataLevels[0].m_internalFormat;
	}
	else {
		return 0;
	}
}

const int Texture::GetMipmapLevelCount() {
	return m_mipmapLevelCount;
}

const int Texture::GetDataSize(int mipmapLevel) {
	if (mipmapLevel >= 0 && mipmapLevel < m_textureDataLevels.size()) {
		return m_textureDataLevels[mipmapLevel].m_dataSize;
	}
	else {
		return 0;
	}
}

const void* Texture::GetData(int mipmapLevel) {
	if (mipmapLevel >= 0 && mipmapLevel < m_textureDataLevels.size()) {
		return m_textureDataLevels[mipmapLevel].m_data;
	}
	else {
		return 0;
	}
}

const std::string& Texture::GetFileName() {
	return m_fileInfo.name;
}

const std::string& Texture::GetFilePath() {
	return m_fileInfo.path;
}

const LoadingState Texture::GetLoadingState() {
	return m_loadingState;
}

const FileInfo Texture::GetFileInfo() {
	return m_fileInfo;
}

const ImageDataType Texture::GetImageDataType() {
	return m_imageDataType;
}

const TextureWrapMode Texture::GetTextureWrapMode() {
	return m_wrapMode;
}

const TextureFilter Texture::GetMinFilter() {
	return m_minFilter;
}

const TextureFilter Texture::GetMagFilter() {
	return m_magFilter;
}

OpenGLTexture& Texture::GetGLTexture() {
	return m_glTexture;
}