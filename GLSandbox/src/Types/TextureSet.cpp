#include "TextureSet.h"

OpenGLTextureSet::OpenGLTextureSet(int num, int W, int H) {
    nTextures = num;
    texture.resize(num);

    for (int i = 0; i < num; ++i) {
        glCreateTextures(GL_TEXTURE_2D, 1, &texture[i]);
        glTextureStorage2D(texture[i], 1, GL_RGBA32F, W, H);

        glTextureParameteri(texture[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTextureParameteri(texture[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(texture[i], GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTextureParameteri(texture[i], GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    }
}