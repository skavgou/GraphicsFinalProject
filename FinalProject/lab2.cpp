#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Building.h"
#include "render/shader.h"

GLuint boxShaderID;
GLuint buildingTex1;
GLuint buildingTex2;
GLuint buildingTex3;
GLuint buildingTex4;
GLuint buildingTex5;
GLuint buildingTex6;

// Function prototypes
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);

// OpenGL camera view parameters
static glm::vec3 eye_center(0, 0, 500);
static glm::vec3 lookat(0, 0, 0);
static glm::vec3 up(0, 1, 0);
static glm::vec3 cameraDirection = glm::normalize(lookat - eye_center);

// Camera movement parameters
static float viewAzimuth = 0.f;
static float viewPolar = 0.f;
static float viewDistance = 500.0f;
static float cameraSpeed = 10.0f;

// Mouse state
static bool firstMouse = true;
static float lastX = 512, lastY = 384; // Assume window center
static float yaw = -90.0f; // Yaw starts facing down -Z axis
static float pitch = 0.0f;

// Tile structure
struct Tile {
    std::vector<Building> buildings;
    glm::vec3 position; // Position of the tile
};

GLuint toTexID(int texID) {
    GLuint textureID;
    switch (texID) {
        case 1: textureID = buildingTex1; break;
        case 2: textureID = buildingTex2; break;
        case 3: textureID = buildingTex3; break;
        case 4: textureID = buildingTex4; break;
        case 5: textureID = buildingTex5; break;
        case 6: textureID = buildingTex6; break;
        default: textureID = buildingTex4; break;
    }
    return textureID;
}

// Function to create a grid of buildings
void createGridLayout(std::vector<Building> &buildings, int rows, int cols, int spacing) {
    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            Building b;
            int height = (rand() % 100) / 5 + 10;
            b.initialize(
                glm::vec3(i * spacing, height, j * spacing),  // Position
                glm::vec3((rand() % 50) / 10 + 5, height, (rand() % 50) / 10 + 5), // Scale
                toTexID(rand() % 6 + 1),  // Texture ID,
                boxShaderID
            );
            buildings.push_back(b);
        }
    }
}

// Function to create a floor for a tile
Building createFloor(glm::vec3 position, glm::vec3 scale, GLuint shaderID, GLuint textureID) {
    Building floor;
    floor.initialize(
        position, // Position of the floor
        scale,    // Scale: very flat
        textureID, // Texture ID
        shaderID   // Shader program ID
    );
    return floor;
}


// Function to create a tile at a specific position
// Function to create a tile at a specific position
Tile createTile(int rows, int cols, int spacing, glm::vec3 position) {
    Tile tile;
    tile.position = position; // Assign specific position

    // Create buildings for the tile
    createGridLayout(tile.buildings, rows, cols, spacing);

    // Add a floor to the tile
    Building floor = createFloor(
        glm::vec3(0, 0, 0),             // Position relative to the tile
        glm::vec3(rows * spacing, 1, cols * spacing), // Scale to cover the tile
        boxShaderID,                    // Use the same shader
        buildingTex1                    // Example texture for the floor
    );
    tile.buildings.push_back(floor);

    return tile;
}


// Function to arrange tiles in a grid
std::vector<Tile> createTileGrid(int numRows, int numCols, int tileSpacing, int tileSize, int buildingSpacing) {
    std::vector<Tile> tiles;

    for (int row = 0; row < numRows; ++row) {
        for (int col = 0; col < numCols; ++col) {
            glm::vec3 tilePosition(col * (tileSize + tileSpacing), 0, row * (tileSize + tileSpacing));
            tiles.push_back(createTile(10, 10, buildingSpacing, tilePosition));
        }
    }

    return tiles;
}





int main() {

    srand(static_cast<unsigned int>(time(0)));

    // Initialize GLFW
    if (!glfwInit()) {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    GLFWwindow *window = glfwCreateWindow(1024, 768, "Adjacent Tile Configurations", nullptr, nullptr);
    if (window == nullptr) {
        std::cerr << "Failed to open a GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Hide and lock cursor

    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);

    // Load OpenGL functions
    if (!gladLoadGL(glfwGetProcAddress)) {
        std::cerr << "Failed to initialize OpenGL context." << std::endl;
        return -1;
    }

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    boxShaderID = LoadShadersFromFile("../FinalProject/shader/box.vert", "../FinalProject/shader/box.frag");
    if (boxShaderID == 0) {
        std::cerr << "Failed to load shaders." << std::endl;
    }

    buildingTex1 = LoadTextureTileBox("../FinalProject/Textures/facade1.jpg");
    buildingTex2 = LoadTextureTileBox("../FinalProject/Textures/facade2.jpg");
    buildingTex3 = LoadTextureTileBox("../FinalProject/Textures/facade3.jpg");
    buildingTex4 = LoadTextureTileBox("../FinalProject/Textures/facade4.jpg");
    buildingTex5 = LoadTextureTileBox("../FinalProject/Textures/facade5.jpg");
    buildingTex6 = LoadTextureTileBox("../FinalProject/Textures/facade0.jpg");

    // Load floor texture
    GLuint floorTexture = LoadTextureTileBox("../FinalProject/Textures/floor.jpg");

    // Create a grid of tiles
    int numTileRows = 20;      // Number of tile rows
    int numTileCols = 20;      // Number of tile columns
    int tileSpacing = 100;    // Spacing between tiles
    int tileSize = 500;       // Approximate size of each tile
    int buildingSpacing = 50; // Spacing between buildings in each tile

    std::vector<Tile> tiles = createTileGrid(numTileRows, numTileCols, tileSpacing, tileSize, buildingSpacing);

    // Camera setup
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.1f, 3000.0f);

    // Main rendering loop
    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::vec3 center = eye_center + cameraDirection;
        glm::mat4 viewMatrix = glm::lookAt(eye_center, center, up);
        glm::mat4 vp = projectionMatrix * viewMatrix;

        // Render all tiles
        for (Tile &tile : tiles) {
            glm::mat4 tileTransform = glm::translate(glm::mat4(1.0f), tile.position);
            for (Building &building : tile.buildings) {
                glm::mat4 model = tileTransform; // Apply tile transform to each building
                building.programID = boxShaderID;
                building.render(vp * model);
            }
        }

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } while (!glfwWindowShouldClose(window));

    // Cleanup
    for (Tile &tile : tiles) {
        for (Building &building : tile.buildings) {
            building.cleanup();
        }
    }

    glfwTerminate();
    return 0;
}

// Function to handle mouse input
void mouse_callback(GLFWwindow *window, double xpos, double ypos) {
    if (firstMouse) {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }

    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // Reversed: y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;

    float sensitivity = 0.1f; // Adjust mouse sensitivity
    xoffset *= sensitivity;
    yoffset *= sensitivity;

    yaw += xoffset;
    pitch += yoffset;

    // Constrain pitch
    if (pitch > 89.0f) pitch = 89.0f;
    if (pitch < -89.0f) pitch = -89.0f;

    // Update camera direction
    glm::vec3 direction;
    direction.x = cos(glm::radians(yaw)) * cos(glm::radians(pitch));
    direction.y = sin(glm::radians(pitch));
    direction.z = sin(glm::radians(yaw)) * cos(glm::radians(pitch));
    cameraDirection = glm::normalize(direction);
}

// Function to handle keyboard input
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode) {
    glm::vec3 right = glm::normalize(glm::cross(cameraDirection, up));
    glm::vec3 forward = glm::normalize(glm::vec3(cameraDirection.x, 0, cameraDirection.z));
    glm::vec3 upward = glm::vec3(0, 1, 0);

    if (key == GLFW_KEY_W && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        eye_center += cameraDirection * cameraSpeed;
        lookat += cameraDirection * cameraSpeed;
    }
    if (key == GLFW_KEY_S && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        eye_center -= cameraDirection * cameraSpeed;
        lookat += cameraDirection * cameraSpeed;
    }
    if (key == GLFW_KEY_A && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        eye_center -= right * cameraSpeed;
        lookat -= right * cameraSpeed;
    }
    if (key == GLFW_KEY_D && (action == GLFW_PRESS || action == GLFW_REPEAT)) {
        eye_center += right * cameraSpeed;
        lookat += right * cameraSpeed;
    }
    if (key == GLFW_KEY_R && action == GLFW_PRESS) {
        eye_center = glm::vec3(0, 0, 500);
        lookat = glm::vec3(0, 0, 0);
        yaw = -90.0f;
        pitch = 0.0f;
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}
