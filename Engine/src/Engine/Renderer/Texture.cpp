#include "Texture.h"
#include "Platform/OpenGL/gl.h"
#include "stb_image.h"
#include "feline_load.h"
#include <iostream> // For error messages

namespace Engine
{
    Ref<Texture> Texture::Create(int width, int height, void* data)
    {
        return CreateRef<Texture>(width, height, data);
    }
    Ref<Texture> Texture::Create(const char* filename, uint32_t texType, uint32_t slot)
    {
        return CreateRef<Texture>(filename, texType, slot);
    }

    Texture::Texture(int width, int height, void* data)
    {
        type = GL_TEXTURE_2D;
		glGenTextures(1, &ID);
		glBindTexture(type, ID);
		glTexParameteri(type, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
		glTexParameteri(type, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
		glTexParameteri(type, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(type, GL_TEXTURE_WRAP_T, GL_REPEAT);
        glTexImage2D(type, 0, GL_RGBA8, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glBindTexture(type, 0);
    }

    Texture::Texture(const char* filename, uint32_t texType, uint32_t slot)
    {
        type = texType;

        int widthImg, heightImg, numColCh;

        File f = feline_load_file(filename, false);
        if (!f.buffer || f.size == 0)
        {
            std::cerr << "Failed to load file: " << filename << std::endl;
            return;
        }

        unsigned char* bytes = stbi_load_from_memory(f.buffer, f.size, &widthImg, &heightImg, &numColCh, 0);
        if (!bytes)
        {
            std::cerr << "Failed to decode image: " << filename << std::endl;
            return;
        }

        uint32_t m_InternalFormat = 0, m_DataFormat = 0;
        if (numColCh == 4)
        {
            m_InternalFormat = GL_RGBA8;
            m_DataFormat = GL_RGBA;
        }
        else if (numColCh == 3)
        {
            m_InternalFormat = GL_RGB8;
            m_DataFormat = GL_RGB;
        }
        else if (numColCh == 2)
        {
            m_InternalFormat = GL_RG8;
            m_DataFormat = GL_RG;
        }
        else if (numColCh == 1)
        {
            m_InternalFormat = GL_R8;
            m_DataFormat = GL_RED;
        }
        else
        {
            std::cerr << "Unsupported number of channels: " << numColCh << " in file " << filename << std::endl;
            stbi_image_free(bytes);
            return;
        }

        glGenTextures(1, &ID);
        glActiveTexture(GL_TEXTURE0 + slot);
        glBindTexture(texType, ID);

        glTexParameteri(texType, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
        glTexParameteri(texType, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexParameteri(texType, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTexParameteri(texType, GL_TEXTURE_WRAP_T, GL_REPEAT);

        // Optional: Uncomment to use GL_CLAMP_TO_BORDER
        // float flatColor[] = {1.0f, 1.0f, 1.0f, 1.0f};
        // glTexParameterfv(texType, GL_TEXTURE_BORDER_COLOR, flatColor);

        glTexImage2D(texType, 0, m_InternalFormat, widthImg, heightImg, 0, m_DataFormat, GL_UNSIGNED_BYTE, bytes);
        //glGenerateMipmap(texType);

        stbi_image_free(bytes);
        glBindTexture(texType, 0);

        // Optional: if your loader allocates resources dynamically
        // feline_free_file(&f);
    }

    Texture::~Texture()
    {
        glDeleteTextures(1, &ID);
    }

    void Texture::texUnit(Ref<Shader> shader, const char* uniform, uint32_t unit)
    {
        GLuint texUni = glGetUniformLocation(shader->ID, uniform);
        shader->Activate();
        glUniform1i(texUni, unit);
    }

    void Texture::Bind() const
    {
        glBindTexture(type, ID);
    }

    void Texture::Unbind() const
    {
        glBindTexture(type, 0);
    }

    
}
