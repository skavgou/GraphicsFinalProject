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

void Skybox::initialize(glm::vec3 position, glm::vec3 scale, int texID, GLuint shaderID) {
	this->position = position;
	this->scale = scale;
	programID = shaderID;


    // Create and bind VAO
    glGenVertexArrays(1, &vertexArrayID);
    glBindVertexArray(vertexArrayID);

    // Create and bind vertex buffer
    glGenBuffers(1, &vertexBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

    // Create and bind color buffer
    glGenBuffers(1, &colorBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);

    // Create modifiable UV buffer
    GLfloat modifiable_uv_buffer_data[48];
    memcpy(modifiable_uv_buffer_data, uv_buffer_data, sizeof(uv_buffer_data));
    for (int i = 0; i < 24; ++i) {
        //modifiable_uv_buffer_data[2 * i + 1] *= scale[2] / 10;
    }

    // Create and bind UV buffer
    glGenBuffers(1, &uvBufferID);
    glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
    glBufferData(GL_ARRAY_BUFFER, sizeof(modifiable_uv_buffer_data), modifiable_uv_buffer_data, GL_STATIC_DRAW);

    // Create and bind index buffer
    glGenBuffers(1, &indexBufferID);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);

    // Get uniform locations
    mvpMatrixID = glGetUniformLocation(programID, "MVP");

    // Load texture based on texID
    textureID = texID;

    // Get texture sampler location
    textureSamplerID = glGetUniformLocation(programID, "textureSampler");
}

void Skybox::render(glm::mat4 cameraMatrix, glm::mat4* lightSpaceMatrix) const {
	glUseProgram(programID);

	glEnableVertexAttribArray(0);
	glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

	glEnableVertexAttribArray(1);
	glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

	if (!lightSpaceMatrix) {
		// Enable UV buffer and texture sampler only for normal rendering
		glEnableVertexAttribArray(2);
		glBindBuffer(GL_ARRAY_BUFFER, uvBufferID);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, 0);

		// Set texture sampler to use texture unit 0
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, textureID);
		glUniform1i(textureSamplerID, 0);
	}

	// Model transform
	glm::mat4 modelMatrix = glm::mat4();
	modelMatrix = glm::translate(modelMatrix, position);
	modelMatrix = glm::scale(modelMatrix, scale);

	// Use the appropriate matrix
	glm::mat4 mvp = lightSpaceMatrix ? (*lightSpaceMatrix) * modelMatrix : cameraMatrix * modelMatrix;
	glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);

	// Draw the box
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, (void*)0);

	glDisableVertexAttribArray(0);
	glDisableVertexAttribArray(1);
	if (!lightSpaceMatrix) {
		glDisableVertexAttribArray(2);
	}
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
