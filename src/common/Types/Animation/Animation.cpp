#include "Animation.h"

void Animation::SetFileInfo(FileInfo fileInfo) {
	m_fileInfo = fileInfo;
}

void Animation::SetLoadingState(LoadingState loadingState) {
	m_loadingState = loadingState;
}

FileInfo Animation::GetFileInfo() {
	return m_fileInfo;
}

LoadingState Animation::GetLoadingState() const {
	return m_loadingState;
}

float Animation::GetTicksPerSecond() {
	return m_ticksPerSec != 0 ? m_ticksPerSec : 25.0f;
}

const std::string& Animation::GetName() {
	return m_fileInfo.name;
}

void Animation::PrintNodeNames() {
	std::cout << m_fileInfo.name << "\n";

	for (AnimatedNode node : m_animNodes) {
		std::cout << " - " << node.m_nodeName << "\n";
	}
}