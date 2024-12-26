#ifndef BUILDING_H
#define BUILDING_H

#include <glm/glm.hpp>
#include <GL/gl.h>

#include <tuple>
#include <functional>

// Custom hash function for std::tuple<int, int>
namespace std {
	template <>
	struct hash<std::tuple<int, int>> {
		std::size_t operator()(const std::tuple<int, int>& key) const {
			// Combine hashes of individual elements in the tuple
			std::size_t h1 = std::hash<int>{}(std::get<0>(key));
			std::size_t h2 = std::hash<int>{}(std::get<1>(key));
			return h1 ^ (h2 << 1); // Combine with XOR and bit shifting
		}
	};
}

// Forward declaration if the function exists in another file
GLuint LoadTextureTileBox(const char* texture_file_path);

struct Building2 {
	glm::vec3 position;
	glm::vec3 scale;

	static const GLfloat vertex_buffer_data[72];
	static const GLfloat color_buffer_data[72];
	static const GLuint index_buffer_data[36];
	static const GLfloat uv_buffer_data[48];
	static const GLfloat normal_buffer_data[72];

	GLuint vertexArrayID;
	GLuint vertexBufferID;
	GLuint indexBufferID;
	GLuint colorBufferID;
	GLuint uvBufferID;
	GLuint textureID;

	GLuint mvpMatrixID;
	GLuint textureSamplerID;
	GLuint programID;
	GLuint normalBufferID;

	GLuint shadowDepthMapID;     // Shadow depth map uniform location
	GLuint lightSpaceMatrixID;
	GLuint lightPositionID;
	GLuint lightIntensityID;


	// Initialize the Building2
	void initialize(glm::vec3 position, glm::vec3 scale, GLuint texID, GLuint shaderID);
	void render(glm::mat4 cameraMatrix, glm::mat4 lightSpaceMatrix, GLuint shadowDepthMap, glm::vec3 lightPosition, glm::vec3 lightIntensity) const;


	// Clean up resources
	void cleanup();
};

#endif
