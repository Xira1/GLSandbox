#pragma once

#include "../../Types/Texture.h"

namespace BakeQueue {
	void QueueTextureForBaking(Texture* texture);
	void RemoveQueuedTextureBakeByJobID(int jobID);
	void ImmediateBakeAllTexture();
	const int GetQueuedTextureBakeJobCount();
	QueuedTextureBake* GetNextQueuedTextureBake();
	QueuedTextureBake* GetQueuedTextureByJobID(int jobID);
}