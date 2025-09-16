#pragma once

#include "Common.h"
#include <glad/glad.h>

struct Grid {
	GLuint VAO;

	void Init() {
		glGenVertexArrays(1, &VAO);
		glBindVertexArray(VAO);

		glBindVertexArray(0);
	}
};