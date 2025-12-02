#pragma once

#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <vector>

struct OpenGLTextureSet {
public:
    OpenGLTextureSet() = default;
    OpenGLTextureSet(int num, int W, int H);

    int GetNTexturesCount() const { return nTextures; }

    unsigned int GetColorAttachmentTextureByIndex(int i) const {
        return texture[i];
    }

private:
    int nTextures = 0;
    std::vector<GLuint> texture;
};