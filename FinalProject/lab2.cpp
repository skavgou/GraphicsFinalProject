#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <vector>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include "Building.h"
#include "Skybox.h"
#include "render/shader.h"

GLuint boxShaderID;
GLuint skyboxShaderID;
GLuint skyboxTexID;
GLuint buildingTex1;
GLuint buildingTex2;
GLuint buildingTex3;
GLuint buildingTex4;
GLuint buildingTex5;
GLuint buildingTex6;

Skybox skybox;

// Function prototypes
void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
void mouse_callback(GLFWwindow *window, double xpos, double ypos);

// OpenGL camera view parameters
static glm::vec3 eye_center(0, 20, 500);
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

// Sun movement variables
glm::vec3 sunCenter(0.0f, 0.0f, 0.0f);   // Center of the sun's orbit
float sunRadius = 1000.0f;               // Radius of the sun's orbit
float sunSpeed = 0.01f;                  // Speed of the sun's movement (radians per frame)
float sunAngle = 0.0f;                   // Current angle of the sun
glm::vec3 lightPosition(0.0f, 1000.0f, 0.0f); // Initial light position


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

void initializeSkybox() {
    skybox.initialize(glm::vec3(200, 0, 200), glm::vec3(-1000, -1000, -1000), skyboxTexID, skyboxShaderID);
}

void renderSkybox(glm::mat4 projectionMatrix, glm::vec3 cameraPosition) {
    glDepthFunc(GL_LEQUAL); // Change depth function to render the skybox behind all other objects

    // Create a view matrix without translation (only orientation matters for the skybox)
    glm::mat4 viewMatrix = glm::mat4(glm::mat3(glm::lookAt(cameraPosition, cameraPosition + cameraDirection, up)));

    glm::mat4 vpMatrix = projectionMatrix * viewMatrix;

    skybox.render(vpMatrix);

    glDepthFunc(GL_LESS); // Restore default depth function
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

    // Load shaders
    boxShaderID = LoadShadersFromFile("../FinalProject/shader/box.vert", "../FinalProject/shader/box.frag");
    skyboxShaderID = LoadShadersFromFile("../FinalProject/shader/skybox.vert", "../FinalProject/shader/skybox.frag");
    GLuint shadowShaderID = LoadShadersFromFile("../FinalProject/shader/shadow.vert", "../FinalProject/shader/shadow.frag");


    if (boxShaderID == 0 || shadowShaderID == 0) {
        std::cerr << "Failed to load shaders." << std::endl;
        return -1;
    }

    // Load textures
    buildingTex1 = LoadTextureTileBox("../FinalProject/Textures/facade1.jpg");
    buildingTex2 = LoadTextureTileBox("../FinalProject/Textures/facade2.jpg");
    buildingTex3 = LoadTextureTileBox("../FinalProject/Textures/facade3.jpg");
    buildingTex4 = LoadTextureTileBox("../FinalProject/Textures/facade4.jpg");
    buildingTex5 = LoadTextureTileBox("../FinalProject/Textures/facade5.jpg");
    buildingTex6 = LoadTextureTileBox("../FinalProject/Textures/facade0.jpg");
    skyboxTexID = LoadTextureTileBox("../FinalProject/Textures/sky.png");

    // Create a grid of tiles
    int numTileRows = 20;      // Number of tile rows
    int numTileCols = 20;      // Number of tile columns
    int tileSpacing = 100;    // Spacing between tiles
    int tileSize = 500;       // Approximate size of each tile
    int buildingSpacing = 50; // Spacing between buildings in each tile

    std::vector<Tile> tiles = createTileGrid(numTileRows, numTileCols, tileSpacing, tileSize, buildingSpacing);
    initializeSkybox();

    // Camera setup
    glm::mat4 projectionMatrix = glm::perspective(glm::radians(60.0f), 4.0f / 3.0f, 0.1f, 3000.0f);

    // Lighting setup
    glm::vec3 lightPosition(0.0f, 1000.0f, 0.0f); // Move the light higher
    glm::vec3 lightColor(2.0f, 2.0f, 2.0f); // Increase the brightness of the light
    glUniform3fv(glGetUniformLocation(boxShaderID, "lightColor"), 1, glm::value_ptr(lightColor));
    glm::vec3 ambientColor(0.4f, 0.4f, 0.4f); // Increase ambient light
    glUniform3fv(glGetUniformLocation(boxShaderID, "ambientColor"), 1, glm::value_ptr(ambientColor));


    // Shadow map setup
    GLuint shadowMapFBO;
    glGenFramebuffers(1, &shadowMapFBO);

    GLuint shadowDepthMap;
    glGenTextures(1, &shadowDepthMap);
    glBindTexture(GL_TEXTURE_2D, shadowDepthMap);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 1024, 1024, 0, GL_DEPTH_COMPONENT, GL_FLOAT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
    float borderColor[] = { 1.0, 1.0, 1.0, 1.0 };
    glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthMap, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    // Light-space matrix for shadow mapping
    glm::mat4 lightProjection = glm::perspective(glm::radians(90.0f), 1.0f, 1.0f, 1000.0f);
    glm::mat4 lightView = glm::lookAt(lightPosition, glm::vec3(0.0f), glm::vec3(0.0, 1.0, 0.0));
    glm::mat4 lightSpaceMatrix = lightProjection * lightView;


    // Main rendering loop
    do {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Update the sun's position based on a circular trajectory
        sunAngle += sunSpeed;
        if (sunAngle > 2.0f * glm::pi<float>()) {
            sunAngle -= 2.0f * glm::pi<float>(); // Reset angle to avoid overflow
        }

        lightPosition = glm::vec3(
            sunCenter.x + sunRadius * cos(sunAngle),  // X position
            sunCenter.y + sunRadius * sin(sunAngle),  // Y position
            sunCenter.z                               // Z position (constant for simplicity)
        );

        // First pass: render depth to the shadow map
        glBindFramebuffer(GL_FRAMEBUFFER, shadowMapFBO);
        glClear(GL_DEPTH_BUFFER_BIT);
        glUseProgram(shadowShaderID);
        glUniformMatrix4fv(glGetUniformLocation(shadowShaderID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));
        for (Tile &tile : tiles) {
            glm::mat4 tileTransform = glm::translate(glm::mat4(1.0f), tile.position);
            for (Building &building : tile.buildings) {
                building.render(tileTransform, &lightSpaceMatrix); // Shadow pass
            }
        }
        glBindFramebuffer(GL_FRAMEBUFFER, 0);

        // Render the skybox
        renderSkybox(projectionMatrix, eye_center);

        // Second pass: render the scene with updated lighting
        glUseProgram(boxShaderID);

        // Update light position (dynamic lighting)
        glUniform3fv(glGetUniformLocation(boxShaderID, "lightPosition"), 1, glm::value_ptr(lightPosition));

        // Update light properties
        glUniform3fv(glGetUniformLocation(boxShaderID, "lightColor"), 1, glm::value_ptr(lightColor));
        glUniform3fv(glGetUniformLocation(boxShaderID, "ambientColor"), 1, glm::value_ptr(ambientColor));

        // Pass the light-space matrix for shadow mapping
        glUniformMatrix4fv(glGetUniformLocation(boxShaderID, "lightSpaceMatrix"), 1, GL_FALSE, glm::value_ptr(lightSpaceMatrix));

        // Bind the shadow map
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, shadowDepthMap);
        glUniform1i(glGetUniformLocation(boxShaderID, "shadowDepthMap"), 1);

        // Loop through all tiles and buildings
        glm::mat4 viewMatrix = glm::lookAt(eye_center, eye_center + cameraDirection, up);
        glm::mat4 vp = projectionMatrix * viewMatrix;

        for (Tile &tile : tiles) {
            glm::mat4 tileTransform = glm::translate(glm::mat4(1.0f), tile.position);
            for (Building &building : tile.buildings) {
                // Bind building texture
                glActiveTexture(GL_TEXTURE0);
                glBindTexture(GL_TEXTURE_2D, building.textureID); // Assume Building has a `textureID`
                glUniform1i(glGetUniformLocation(boxShaderID, "textureSampler"), 0);

                // Render the building
                building.render(vp * tileTransform); // Standard rendering pass
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
        eye_center = glm::vec3(0, 20, 500);
        lookat = glm::vec3(0, 0, 0);
        yaw = -90.0f;
        pitch = 0.0f;
    }
    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GL_TRUE);
    }
}