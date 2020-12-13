#ifndef TEXTURE_HPP
#define TEXTURE_HPP

// Load a .BMP file using our custom loader
GLuint loadBMP_custom(const wchar_t * imagepath);

//// Since GLFW 3, glfwLoadTexture2D() has been removed. You have to use another texture loading library, 
//// or do it yourself (just like loadBMP_custom and loadDDS)
//// Load a .TGA file using GLFW's own loader
//GLuint loadTGA_glfw(const char * imagepath);

// Load a .DDS file using GLFW's own loader
bool loadDDS(const wchar_t* imagepath, GLuint& outImageFormat, GLuint& outMipMapCount, GLuint& outWidth, GLuint& outHeight, unsigned char*& outImage);
GLuint GLCreateTextureDDS(GLuint format, GLuint mipMapCount, GLuint width, GLuint height, unsigned char* buffer);

#endif