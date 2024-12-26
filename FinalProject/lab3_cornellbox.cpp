#include <glad/gl.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <GL/gl.h>
#include <cmath>
#include <Building2.h>

#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb/stb_image_write.h>

#include <render/shader.h>

#include <vector>
#include <iostream>
#define _USE_MATH_DEFINES

//#define STB_IMAGE_IMPLEMENTATION
//#include <stb/stb_image.h>
#define _USE_MATH_DEFINES

GLuint boxShaderID;
GLuint skyboxShaderID;
GLuint skyboxTexID;
GLuint buildingTex1;
GLuint buildingTex2;
GLuint buildingTex3;
GLuint buildingTex4;
GLuint buildingTex5;
GLuint buildingTex6;

static GLFWwindow *window;
static int windowWidth = 1024;
static int windowHeight = 768;

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode);
static void cursor_callback(GLFWwindow *window, double xpos, double ypos);

// OpenGL camera view parameters
static glm::vec3 eye_center(-278.0f, 273.0f, 800.0f);
static glm::vec3 lookat(-278.0f, 273.0f, 0.0f);
static glm::vec3 up(0.0f, 1.0f, 0.0f);
static float FoV = 45.0f;
static float zNear = 600.0f;
static float zFar = 15000.0f;

// Lighting control
const glm::vec3 wave500(0.0f, 255.0f, 146.0f);
const glm::vec3 wave600(255.0f, 190.0f, 0.0f);
const glm::vec3 wave700(205.0f, 0.0f, 0.0f);
static glm::vec3 lightIntensity = 5.0f * (8.0f * wave500 + 15.6f * wave600 + 18.4f * wave700);
static glm::vec3 lightPosition(-275.0f, 500.0f, -275.0f);

// Shadow mapping
static glm::vec3 lightUp(0, 0, 1);
static int shadowMapWidth = 0;
static int shadowMapHeight = 0;

// TODO: set these parameters
static float depthFoV = 140.0f;
static float depthNear = 1.0f;
static float depthFar = 600.0f;

// Helper flag and function to save depth maps for debugging
static bool saveDepth = false;

// This function retrieves and stores the depth map of the default frame buffer
// or a particular frame buffer (indicated by FBO ID) to a PNG image.
static void saveDepthTexture(GLuint fbo, std::string filename) {
    int width = shadowMapWidth;
    int height = shadowMapHeight;
    if (shadowMapWidth == 0 || shadowMapHeight == 0) {
        width = windowWidth;
        height = windowHeight;
    }
    int channels = 3;

    std::vector<float> depth(width * height);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);
    glReadBuffer(GL_DEPTH_COMPONENT);
    glReadPixels(0, 0, width, height, GL_DEPTH_COMPONENT, GL_FLOAT, depth.data());
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    std::vector<unsigned char> img(width * height * 3);
    for (int i = 0; i < width * height; ++i) img[3*i] = img[3*i+1] = img[3*i+2] = depth[i] * 255;

    stbi_write_png(filename.c_str(), width, height, channels, img.data(), width * channels);
}

static void computeNormals(const GLfloat* vertex_buffer_data, int vertex_count, GLfloat* normal_buffer_data) {
    // Each face has 4 vertices, thus we need to iterate in steps of 4
    for (int i = 0; i < vertex_count; i += 12) { // 4 vertices * 3 coordinates per vertex = 12
        // Get vertices for the face
        GLfloat v0[3] = { vertex_buffer_data[i], vertex_buffer_data[i + 1], vertex_buffer_data[i + 2] };
        GLfloat v1[3] = { vertex_buffer_data[i + 3], vertex_buffer_data[i + 4], vertex_buffer_data[i + 5] };
        GLfloat v2[3] = { vertex_buffer_data[i + 6], vertex_buffer_data[i + 7], vertex_buffer_data[i + 8] };

        // Compute the two edges
        GLfloat edge1[3] = { v1[0] - v0[0], v1[1] - v0[1], v1[2] - v0[2] };
        GLfloat edge2[3] = { v2[0] - v0[0], v2[1] - v0[1], v2[2] - v0[2] };

        // Compute the normal using the cross product
        GLfloat normal[3] = {
                edge1[1] * edge2[2] - edge1[2] * edge2[1], // x
                edge1[2] * edge2[0] - edge1[0] * edge2[2], // y
                edge1[0] * edge2[1] - edge1[1] * edge2[0]  // z
        };

        // Normalize the normal vector
        GLfloat length = sqrt(normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
        if (length != 0) {
            normal[0] /= length;
            normal[1] /= length;
            normal[2] /= length;
        }

        // Assign the normal to each vertex of the face
        for (int j = 0; j < 4; ++j) {
            size_t normal_index = i + j * 3;
            normal_buffer_data[normal_index] = normal[0];
            normal_buffer_data[normal_index + 1] = normal[1];
            normal_buffer_data[normal_index + 2] = normal[2];
        }
    }
}





struct CornellBox {

    // Refer to original Cornell Box data
    // from https://www.graphics.cornell.edu/online/box/data.html

    GLfloat vertex_buffer_data[60] = {
            // Floor
            -552.8, 0.0, 0.0,
            0.0, 0.0,   0.0,
            0.0, 0.0, -559.2,
            -549.6, 0.0, -559.2,

            // Ceiling
            -556.0, 548.8, 0.0,
            -556.0, 548.8, -559.2,
            0.0, 548.8, -559.2,
            0.0, 548.8,   0.0,

            // Left wall
            -552.8,   0.0,   0.0,
            -549.6,   0.0, -559.2,
            -556.0, 548.8, -559.2,
            -556.0, 548.8,   0.0,

            // Right wall
            0.0,   0.0, -559.2,
            0.0,   0.0,   0.0,
            0.0, 548.8,   0.0,
            0.0, 548.8, -559.2,

            // Back wall
            -549.6,   0.0, -559.2,
            0.0,   0.0, -559.2,
            0.0, 548.8, -559.2,
            -556.0, 548.8, -559.2
    };

    // TODO: set vertex normals properly
    GLfloat normal_buffer_data[60] = {
            // Floor
            0.0, 1.0, 0.0,
            0.0, 1.0, 0.0,
            0.0, 1.0, 0.0,
            0.0, 1.0, 0.0,

            // Ceiling
            0.0, -1.0, 0.0,
            0.0, -1.0, 0.0,
            0.0, -1.0, 0.0,
            0.0, -1.0, 0.0,

            // Left wall
            1.0, 0.0, 0.0,
            1.0, 0.0, 0.0,
            1.0, 0.0, 0.0,
            1.0, 0.0, 0.0,

            // Right wall
            -1.0, 0.0, 0.0,
            -1.0, 0.0, 0.0,
            -1.0, 0.0, 0.0,
            -1.0, 0.0, 0.0,

            // Back wall
            0.0, 0.0, 1.0,
            0.0, 0.0, 1.0,
            0.0, 0.0, 1.0,
            0.0, 0.0, 1.0
    };

    GLfloat color_buffer_data[60] = {
            // Floor
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,

            // Ceiling
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,

            // Left wall
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,
            1.0f, 0.0f, 0.0f,

            // Right wall
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,
            0.0f, 1.0f, 0.0f,

            // Back wall
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f,
            1.0f, 1.0f, 1.0f
    };

    GLuint index_buffer_data[30] = {
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
    };

    // OpenGL buffers
    GLuint vertexArrayID;
    GLuint vertexBufferID;
    GLuint indexBufferID;
    GLuint colorBufferID;
    GLuint normalBufferID;

    // Shader variable IDs
    GLuint mvpMatrixID;
    GLuint lMapMatrixID;
    GLuint lightPositionID;
    GLuint lightIntensityID;
    GLuint programID;
    GLuint shadowDepthMapID;

    void initialize() {
        computeNormals(vertex_buffer_data, 60, normal_buffer_data);
        // Create a vertex array object
        glGenVertexArrays(1, &vertexArrayID);
        glBindVertexArray(vertexArrayID);

        // Create a vertex buffer object to store the vertex data
        glGenBuffers(1, &vertexBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertex_buffer_data), vertex_buffer_data, GL_STATIC_DRAW);

        // Create a vertex buffer object to store the color data
        glGenBuffers(1, &colorBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
        glBufferData(GL_ARRAY_BUFFER, sizeof(color_buffer_data), color_buffer_data, GL_STATIC_DRAW);

        // Create a vertex buffer object to store the vertex normals
        glGenBuffers(1, &normalBufferID);
        glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);

        glBufferData(GL_ARRAY_BUFFER, sizeof(normal_buffer_data), normal_buffer_data, GL_STATIC_DRAW);

        // Create an index buffer object to store the index data that defines triangle faces
        glGenBuffers(1, &indexBufferID);
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index_buffer_data), index_buffer_data, GL_STATIC_DRAW);

        // Create and compile our GLSL program from the shaders
        programID = LoadShadersFromFile("../FinalProject/shader/box2.vert", "../FinalProject/shader/box2.frag");
        if (programID == 0)
        {
            std::cerr << "Failed to load shaders." << std::endl;
        }

        // Get a handle for our "MVP" uniform
        mvpMatrixID = glGetUniformLocation(programID, "MVP");
        lMapMatrixID = glGetUniformLocation(programID, "LMAP");
        lightPositionID = glGetUniformLocation(programID, "lightPosition");
        lightIntensityID = glGetUniformLocation(programID, "lightIntensity");
        shadowDepthMapID = glGetUniformLocation(programID, "shadowDepthMap");
    }

    void render(glm::mat4 cameraMatrix, glm::mat4 lightMatrix, GLuint shadowFBO) {
        glUseProgram(programID);

        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexBufferID);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(1);
        glBindBuffer(GL_ARRAY_BUFFER, colorBufferID);
        glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glEnableVertexAttribArray(2);
        glBindBuffer(GL_ARRAY_BUFFER, normalBufferID);
        glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, 0);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexBufferID);

        // Set model-view-projection matrix
        glm::mat4 mvp = cameraMatrix;
        glUniformMatrix4fv(mvpMatrixID, 1, GL_FALSE, &mvp[0][0]);
        glm::mat4 lmap = lightMatrix;
        glUniformMatrix4fv(lMapMatrixID, 1, GL_FALSE, &lmap[0][0]);

        // Set light data
        glUniform3fv(lightPositionID, 1, &lightPosition[0]);
        glUniform3fv(lightIntensityID, 1, &lightIntensity[0]);
        glUniform1i(shadowDepthMapID, shadowFBO);

        // Draw the box
        glDrawElements(
                GL_TRIANGLES,      // mode
                30,    			   // number of indices
                GL_UNSIGNED_INT,   // type
                (void*)0           // element array buffer offset
        );

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);
        glDisableVertexAttribArray(2);
    }

    void cleanup() {
        glDeleteBuffers(1, &vertexBufferID);
        glDeleteBuffers(1, &colorBufferID);
        glDeleteBuffers(1, &indexBufferID);
        glDeleteBuffers(1, &normalBufferID);
        glDeleteVertexArrays(1, &vertexArrayID);
        glDeleteProgram(programID);
    }
};



int main(void)
{
    // Initialise GLFW
    if (!glfwInit())
    {
        std::cerr << "Failed to initialize GLFW." << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE); // For MacOS
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // Open a window and create its OpenGL context
    window = glfwCreateWindow(windowWidth, windowHeight, "Building2 Scene", NULL, NULL);
    if (window == NULL)
    {
        std::cerr << "Failed to open a GLFW window." << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    // Ensure we can capture the escape key being pressed below
    glfwSetInputMode(window, GLFW_STICKY_KEYS, GL_TRUE);
    glfwSetKeyCallback(window, key_callback);

    glfwSetCursorPosCallback(window, cursor_callback);

    // Load OpenGL functions
    int version = gladLoadGL(glfwGetProcAddress);
    if (version == 0)
    {
        std::cerr << "Failed to initialize OpenGL context." << std::endl;
        return -1;
    }

    // Prepare shadow map size
    glfwGetFramebufferSize(window, &shadowMapWidth, &shadowMapHeight);

    // Background
    glClearColor(0.2f, 0.2f, 0.25f, 0.0f);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);

    buildingTex1 = LoadTextureTileBox("../FinalProject/Textures/facade1.jpg");
    buildingTex2 = LoadTextureTileBox("../FinalProject/Textures/facade2.jpg");
    buildingTex3 = LoadTextureTileBox("../FinalProject/Textures/facade3.jpg");
    buildingTex4 = LoadTextureTileBox("../FinalProject/Textures/facade4.jpg");
    buildingTex5 = LoadTextureTileBox("../FinalProject/Textures/facade5.jpg");
    buildingTex6 = LoadTextureTileBox("../FinalProject/Textures/facade0.jpg");
    skyboxTexID = LoadTextureTileBox("../FinalProject/Textures/sky.png");

    boxShaderID = LoadShadersFromFile("../FinalProject/shader/box2.vert", "../FinalProject/shader/box2.frag");

    GLuint shadowFBO, shadowDepthTexture;
    glm::mat4 lightProjectionMatrix, lightViewMatrix, lightSpaceMatrix;

    glGenFramebuffers(1, &shadowFBO);
    glGenTextures(1, &shadowDepthTexture);
    glBindTexture(GL_TEXTURE_2D, shadowDepthTexture);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, shadowMapWidth, shadowMapHeight, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowDepthTexture, 0);
    glDrawBuffer(GL_NONE);
    glReadBuffer(GL_NONE);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);

	// Create the classical Cornell Box
	CornellBox b;
	b.initialize();

    // Create buildings
    std::vector<Building2> buildings;

    // Initialize buildings to mimic the Cornell Boxes
    Building2 b1;
    b1.initialize(glm::vec3(-278.0f, 165.0f, 0.0f), glm::vec3(150.0f, 330.0f, 150.0f), buildingTex1, boxShaderID);
    //buildings.push_back(b1);

    Building2 b2;
    b2.initialize(glm::vec3(-160.0f, 82.5f, -150.0f), glm::vec3(82.0f, 165.0f, 82.0f), buildingTex2, boxShaderID);
    //buildings.push_back(b2);

    Building2 b3;
    b3.initialize(glm::vec3(-278.0f, 330.0f, -278.0f), glm::vec3(15.0f, 33.0f, 15.0f), buildingTex4, boxShaderID);
    buildings.push_back(b3);

    // Camera setup
    glm::mat4 viewMatrix, projectionMatrix;
    projectionMatrix = glm::perspective(glm::radians(FoV), (float)windowWidth / windowHeight, zNear, zFar);

    lightProjectionMatrix = glm::perspective(glm::radians(depthFoV),
                                             (float)shadowMapWidth / shadowMapHeight, depthNear, depthFar);

    do
    {
        // Shadow Pass
        glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
        glViewport(0, 0, shadowMapWidth, shadowMapHeight);
        glClear(GL_DEPTH_BUFFER_BIT);

        // Render scene to shadow framebuffer
        lightViewMatrix = glm::lookAt(lightPosition, lightPosition + glm::vec3(0, -1, 0), lightUp);
        glm::mat4 lp = lightProjectionMatrix * lightViewMatrix;

        b.render(lp, lp, shadowFBO);

        for (auto& building : buildings)
        {
            building.render(lp, lp, shadowFBO);
        }



        glBindFramebuffer(GL_FRAMEBUFFER, 0);
        glViewport(0, 0, windowWidth, windowHeight);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glActiveTexture(GL_TEXTURE1); // Activate texture unit 1
        glBindTexture(GL_TEXTURE_2D, shadowDepthTexture);
        glUniform1i(glGetUniformLocation(boxShaderID, "shadowDepthMap"), 1);

        // Main rendering pass
        viewMatrix = glm::lookAt(eye_center, lookat, up);
        glm::mat4 vp = projectionMatrix * viewMatrix;

    	b.render(vp, lp, shadowFBO);
    	//c.render(vp, lp, shadowFBO);
    	//d.render(vp, lp, shadowFBO);

        for (auto& building : buildings)
        {
            building.render(vp, lp, shadowFBO);
        }

        // Save depth map if requested
        if (saveDepth)
        {
            std::string filename = "C:\\Users\\Sean\\Downloads\\FinalProject\\FinalProject\\depth_camera.png";
            saveDepthTexture(shadowFBO, filename);
            std::cout << "Depth texture saved to " << filename << std::endl;
            saveDepth = false;
        }

        // Swap buffers
        glfwSwapBuffers(window);
        glfwPollEvents();

    } while (!glfwWindowShouldClose(window));

    // Cleanup
    for (auto& building : buildings)
    {
        building.cleanup();
    }

	b.cleanup();

    glfwTerminate();
    return 0;
}

static void key_callback(GLFWwindow *window, int key, int scancode, int action, int mode)
{
    if (key == GLFW_KEY_R && action == GLFW_PRESS)
    {
        eye_center = glm::vec3(-278.0f, 273.0f, 800.0f);
        lightPosition = glm::vec3(-275.0f, 500.0f, -275.0f);

    }

    if (key == GLFW_KEY_UP && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        eye_center.y += 20.0f;
    }

    if (key == GLFW_KEY_DOWN && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        eye_center.y -= 20.0f;
    }

    if (key == GLFW_KEY_LEFT && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        eye_center.x -= 20.0f;
    }

    if (key == GLFW_KEY_RIGHT && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        eye_center.x += 20.0f;
    }

    if (key == GLFW_KEY_W && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        lightPosition.z -= 20.0f;
    }

    if (key == GLFW_KEY_S && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        lightPosition.z += 20.0f;
    }

    if (key == GLFW_KEY_SPACE && (action == GLFW_REPEAT || action == GLFW_PRESS))
    {
        saveDepth = true;
    }

    if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GL_TRUE);
}

static void cursor_callback(GLFWwindow *window, double xpos, double ypos) {
    if (xpos < 0 || xpos >= windowWidth || ypos < 0 || ypos > windowHeight)
        return;

    // Normalize to [0, 1]
    float x = xpos / windowWidth;
    float y = ypos / windowHeight;

    // To [-1, 1] and flip y up
    x = x * 2.0f - 1.0f;
    y = 1.0f - y * 2.0f;

    const float scale = 250.0f;
    lightPosition.x = x * scale - 278;
    lightPosition.y = y * scale + 278;

    //std::cout << lightPosition.x << " " << lightPosition.y << " " << lightPosition.z << std::endl;
}
