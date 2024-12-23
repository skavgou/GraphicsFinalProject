//
// Created by seank on 24/10/2024.
//
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/gtc/matrix_transform.hpp>

#include <render/shader.h>
#include "Skybox.h"
#include <iostream>

static GLFWwindow *window;
static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);

// OpenGL camera view parameters
static glm::vec3 eye_center;
static glm::vec3 lookat(0, 0, 0);
static glm::vec3 up(0, 1, 0);

// View control
static float viewAzimuth = 0.f;
static float viewPolar = 0.f;
static float viewDistance = 200.0f;

const GLfloat Skybox::vertex_buffer_data[72] = {	// Vertex definition for a canonical box
            // Front face
            -1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,

            // Back face
            1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,

            // Left face
            -1.0f, -1.0f, -1.0f,
            -1.0f, -1.0f, 1.0f,
            -1.0f, 1.0f, 1.0f,
            -1.0f, 1.0f, -1.0f,

            // Right face
            1.0f, -1.0f, 1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, 1.0f, -1.0f,
            1.0f, 1.0f, 1.0f,

            // Top face
            -1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, -1.0f,
            -1.0f, 1.0f, -1.0f,

            // Bottom face
            -1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, -1.0f,
            1.0f, -1.0f, 1.0f,
            -1.0f, -1.0f, 1.0f,
    };

const GLfloat Skybox::color_buffer_data[72] = {
            // Front, red
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,

            // Back, yellow
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,
            1.0f, 1.0f, 0.0f,

            // Left, green
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,

            // Right, cyan
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,
            0.0f, 1.0f, 1.0f,

            // Top, blue
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,
            0.0f, 0.0f, 1.0f,

            // Bottom, magenta
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
            1.0f, 0.0f, 1.0f,
    };

const GLuint Skybox::index_buffer_data[36] = {		// 12 triangle faces of a box
            0, 1, 2,
            0, 2, 3,

            4, 5, 6,
            4, 6, 7,

            8, 9, 10,
            8, 10, 11,

            12, 13, 14,
            12, 14, 15,

            16, 17, 18,
            16, 18, 19,

            20, 21, 22,
            20, 22, 23,
    };

    // TODO: Define UV buffer data
    // ---------------------------
const GLfloat Skybox::uv_buffer_data[48] = {
        // Neg X :)
        0.75f, 1.0f / 3.0f,
        0.5f, 1.0f / 3.0f,
        0.5f, 2.0f / 3.0f,
        0.75f, 2.0f / 3.0f,
        // Pos X :)
        0.25f, 1.0f / 3.0f,
        0.0f, 1.0f / 3.0f,
        0.0f, 2.0f / 3.0f,
        0.25f, 2.0f / 3.0f,
        // Neg Z
        1.0f, 1.0f / 3.0f,
        0.75f, 1.0f / 3.0f,
        0.75f, 2.0f / 3.0f,
        1.0f, 2.0f / 3.0f,
        // Pos Z :)
        0.5f, 1.0f / 3.0f,
        0.25f, 1.0f / 3.0f,
        0.25f, 2.0f / 3.0f,
        0.5f, 2.0f / 3.0f,
        // Neg Y

        0.5f, 1.0f,
        0.5f, 2.0f / 3.0f,
        0.25f, 2.0f / 3.0f,
        0.25f, 1.0f,

        // Pos Y
        0.25f, 0.0f,
        0.25f, 1.0f / 3.0f,
        0.5f, 1.0f / 3.0f,
        0.5f, 0.0f,

    };
    // ---------------------------

    // OpenGL buffers
/*GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint indexBufferID;
    GLuint colorBufferID;
    GLuint uvBufferID;
    GLuint textureID;

    // Shader variable IDs
    GLuint mvpMatrixID;
    GLuint textureSamplerID;
    GLuint programID;*/

void Skybox::initialize(glm::vec3 position, glm::vec3 scale) {
    // Define scale of the building geometry
    this->position = position;
    this->scale = scale;

    // Create a vertex array object
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    // Create a vertex buffer object to store the vertex data
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

    // Create a vertex buffer object to store the color data
    // TODO:
    glGenBuffers(1, &colorBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);

    //for (int i = 0; i < 24; ++i) uv_buffer_data[2*i+1] *= 3;

    // TODO: Create a vertex buffer object to store the UV data
    // --------------------------------------------------------
    // Create a vertex buffer object to store the UV data
    glGenBuffers(1, &uvBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(uv_buffer_data), uv_buffer_data,
                 GL_STATIC_DRAW);
    // --------------------------------------------------------

    // Create an index buffer object to store the index data that defines triangle faces
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);

    // Create and compile our GLSL program from the shaders
    programID = LoadShadersFromFile("../FinalProject/shader/box.vert", "../FinalProject/shader/box.frag");
    if (programID == 0)
    {
        std::cerr << "Failed to load shaders." << std::endl;
    }

    // Get a handle for our "MVP" uniform
    mvpMatrixID = glGetUniformLocation(programID, "MVP");

    // TODO: Load a texture
    // --------------------
    textureID = LoadTextureTileBox("../FinalProject/Textures/sky_debug.png");
    // --------------------

    // TODO: Get a handle to texture sampler
    // -------------------------------------
    // Get a handle for our "textureSampler" uniform
    textureSamplerID = glGetUniformLocation(programID,"textureSampler");
    // -------------------------------------
}

void Skybox::render(glm::mat4 cameraMatrix) {
    glUseProgram(programID);

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

    // TODO: Model transform
    // -----------------------
    glm::mat4 modelMatrix = glm::mat4();
    // Translate the building to its position
    modelMatrix = glm::translate(modelMatrix, position);
    // Scale the box along each axis to make it look like a building
    modelMatrix = glm::scale(modelMatrix, scale);
    // -----------------------

    // Set model-view-projection matrix
    glm::mat4 mvp = cameraMatrix * modelMatrix;
    glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

    // TODO: Enable UV buffer and texture sampler
    // ------------------------------------------
    glEnableVertexAttribArray(2);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);
    // Set textureSampler to use texture unit 0
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);
    glUniform1i(textureSamplerID, 0);

    // ------------------------------------------

    // Draw the box
    glDrawElements(
            GL_TRIANGLES,      // mode
            36,    			   // number of indices
            GL_UNSIGNED_INT,   // type
            (void*)0           // element array buffer offset
    );

    glDisableVertexAttribArray(0);
    glDisableVertexAttribArray(1);
    //glDisableVertexAttribArray(2);
}

void Skybox::cleanup() {
    glDeleteBuffers(1, &vertexBufferID);
    glDeleteBuffers(1, &colorBufferID);
    glDeleteBuffers(1, &indexBufferID);
    glDeleteVertexArrays(1, &vertexArrayID);
    //glDeleteBuffers(1, &uvBufferID);
    //glDeleteTextures(1, &textureID);
    glDeleteProgram(programID);
}
