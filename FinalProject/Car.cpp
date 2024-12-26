#include <iostream>
#include <fstream>
#include <vector>
#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <tiny_gltf.h>
#include <GL/glew.h>

// Function to load the model
bool loadGLTFModel(const std::string& gltfPath, tinygltf::Model& model) {
    tinygltf::TinyGLTF loader;
    std::string err, warn;

    bool ret = loader.LoadASCIIFromFile(&model, &err, &warn, gltfPath);
    if (!warn.empty()) std::cout << "Warning: " << warn << std::endl;
    if (!err.empty()) std::cerr << "Error: " << err << std::endl;

    return ret;
}

// Function to upload data to OpenGL
void uploadMeshToGPU(const tinygltf::Model& model, GLuint& VAO, GLuint& VBO, GLuint& EBO) {
    const auto& mesh = model.meshes[0]; // Assume single mesh for simplicity

    // Extract first primitive from the mesh
    const auto& primitive = mesh.primitives[0];
    const auto& positionAccessor = model.accessors[primitive.attributes.at("POSITION")];
    const auto& indexAccessor = model.accessors[primitive.indices];

    const auto& positionBufferView = model.bufferViews[positionAccessor.bufferView];
    const auto& indexBufferView = model.bufferViews[indexAccessor.bufferView];

    const auto& positionBuffer = model.buffers[positionBufferView.buffer];
    const auto& indexBuffer = model.buffers[indexBufferView.buffer];

    const float* positions = reinterpret_cast<const float*>(
        &positionBuffer.data[positionBufferView.byteOffset + positionAccessor.byteOffset]);
    const unsigned int* indices = reinterpret_cast<const unsigned int*>(
        &indexBuffer.data[indexBufferView.byteOffset + indexAccessor.byteOffset]);

    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    // Upload vertex data
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, positionAccessor.count * 3 * sizeof(float), positions, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // Upload index data
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, indexAccessor.count * sizeof(unsigned int), indices, GL_STATIC_DRAW);

    glBindVertexArray(0);
}

int main() {
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW" << std::endl;
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(800, 600, "GLTF Viewer", nullptr, nullptr);
    if (!window) {
        std::cerr << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    if (glewInit() != GLEW_OK) {
        std::cerr << "Failed to initialize GLEW" << std::endl;
        return -1;
    }

    // Load GLTF model
    tinygltf::Model model;
    if (!loadGLTFModel("path/to/scene.gltf", model)) {
        std::cerr << "Failed to load GLTF model" << std::endl;
        return -1;
    }

    // Setup OpenGL buffers
    GLuint VAO, VBO, EBO;
    uploadMeshToGPU(model, VAO, VBO, EBO);

    // Shader setup (minimal shader with vertex position only)
    GLuint shaderProgram = glCreateProgram();
    // Attach vertex and fragment shaders here
    glUseProgram(shaderProgram);

    // Rendering loop
    while (!glfwWindowShouldClose(window)) {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);
        glDrawElements(GL_TRIANGLES, model.accessors[model.meshes[0].primitives[0].indices].count, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    // Cleanup
    glDeleteBuffers(1, &VBO);
    glDeleteBuffers(1, &EBO);
    glDeleteVertexArrays(1, &VAO);

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
