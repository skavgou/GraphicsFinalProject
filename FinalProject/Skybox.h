#ifndef SKYBOX_H
#define SKYBOX_H

#include <glad/gl.h>
#include <GL/gl.h>

GLuint LoadTextureTileBox(const char *texture_file_path);

struct Skybox {
    glm::vec3 position;
    glm::vec3 scale;

    static const GLfloat vertex_buffer_data[72];
    static const GLfloat color_buffer_data[72];
    static const GLuint index_buffer_data[36];
    static const GLfloat uv_buffer_data[48];

    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint indexBufferID;
    GLuint colorBufferID;
    GLuint uvBufferID;
    GLuint textureID;

    GLuint mvpMatrixID;
    GLuint textureSamplerID;
    GLuint programID;

    // Initialize the building
    void initialize(glm::vec3 position, glm::vec3 scale);

    // Render the building
    void render(glm::mat4 cameraMatrix);

    // Clean up resources
    void cleanup();
};

#endif