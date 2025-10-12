#pragma once

#include <glad/glad.h>
#include <cstdint>
#include <iostream>

struct PBO {
public:
	void Init(size_t size) {
		m_size = size;
		glGenBuffers(1, &m_handle);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, m_handle);
		glBufferStorage(GL_PIXEL_UNPACK_BUFFER, size, nullptr, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		m_persistentBuffer = (GLubyte*)glMapBufferRange(GL_PIXEL_UNPACK_BUFFER, 0, size, GL_MAP_WRITE_BIT | GL_MAP_PERSISTENT_BIT | GL_MAP_COHERENT_BIT);
		glBindBuffer(GL_PIXEL_UNPACK_BUFFER, 0);
	}

	bool IsSyncComplete() const {
		return !m_syncInProgress;
	}

	bool IsSyncInProgress() const {
		return m_syncInProgress;
	}

	void SyncStart() {
		if (m_sync) {
			glDeleteSync(m_sync);
		}
		m_sync = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		glWaitSync(m_sync, GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
		m_syncInProgress = true;
	}
	
	bool UpdatePBOState() {
		if (m_sync) {
			GLenum res = glClientWaitSync(m_sync, GL_SYNC_FLUSH_COMMANDS_BIT, 0);
			if (res == GL_ALREADY_SIGNALED || res == GL_CONDITION_SATISFIED) {
				glDeleteSync(m_sync);
				m_sync = nullptr;
				m_syncInProgress = false;
			}
			else {
				m_syncInProgress = true;
			}
		}
		else {
			m_syncInProgress = false;
		}

		return m_syncInProgress;
	}

	GLubyte* GetPersistentBuffer() const {
		return m_persistentBuffer;
	}

	uint32_t GetHandle() const {
		return m_handle;
	}

	void SetCustomValue(size_t value) {
		m_customValue = value;
	}

	int16_t GetCustomValue() {
		return m_customValue;
	}

private:
	GLubyte* m_persistentBuffer = nullptr;
	uint32_t m_handle = 0;
	size_t m_size = 0;
	GLsync m_sync = nullptr;
	bool m_syncInProgress = false;
	int16_t m_customValue = -1;
};