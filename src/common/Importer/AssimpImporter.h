#pragma once

#include "Common/Types.h"
#include "FileFormats.h"

namespace AssimpImporter {
	ModelData ImportModel(const std::string filePath);
}