//
//  main.cpp
//  3D Classroom
//
//  A simple classroom scene built from course-style primitives, in the same style
//  as the course's "3D_CubeTransformation_Color" and "Lighting" lab projects.
//
//  Project requirements covered:
//    1. 3D transformation      -> objects are placed with translate/rotate/scale
//    2. Viewing transformation -> free-fly Camera class (WASD + mouse look) with perspective projection
//    3. A moving object        -> the classroom door slides open/closed, and the ceiling fan spins
//    4. Two kinds of light     -> 4 point lights (ceiling lights) + 1 spotlight (board/podium spotlight)
//    5. Different colors       -> floor tiles, walls, desks, chairs, door, board etc. all use different colors
//

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "shader.h"
#include "camera.h"
#include "pointLight.h"
#include "spotLight.h"

#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

// ------------------------------------------------------------------------
// function prototypes
// ------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void mouse_callback(GLFWwindow* window, double xpos, double ypos);
void processInput(GLFWwindow* window);

void drawCube(unsigned int& cubeVAO, Shader& shader, glm::mat4 parentModel, glm::vec3 position, glm::vec3 size, glm::vec3 color, float rotYDeg = 0.0f);
void drawRoom(unsigned int& cubeVAO, Shader& shader);
void drawBlackboard(unsigned int& cubeVAO, Shader& shader);
void drawDoor(unsigned int& cubeVAO, Shader& shader, float slideAmount);
void drawDesk(unsigned int& cubeVAO, Shader& shader, glm::vec3 pos, glm::vec3 topColor);
void drawChair(unsigned int& cubeVAO, Shader& shader, glm::vec3 pos, glm::vec3 seatColor, float rotYDeg);
void drawTeacherTable(unsigned int& cubeVAO, Shader& shader, glm::vec3 pos);
void drawCeilingFan(unsigned int& cubeVAO, Shader& shader, glm::vec3 pos, float angleDeg);
void drawCylinder(unsigned int& cylinderVAO, Shader& shader, glm::mat4 parentModel, glm::vec3 position, float radius, float height, glm::vec3 color);

// ------------------------------------------------------------------------
// settings
// ------------------------------------------------------------------------
unsigned int SCR_WIDTH = 1100;
unsigned int SCR_HEIGHT = 750;

// room dimensions (world units)
const float ROOM_W = 12.0f;   // X extent
const float ROOM_D = 12.0f;   // Z extent
const float ROOM_H = 6.0f;    // ceiling height

// Global transformation controls, kept in the same style as the Kitchen
// / 3D_CubeTransformation examples. These transform the whole classroom.
float translate_X = 0.0f, translate_Y = 0.0f, translate_Z = 0.0f;
float rotateAngle_X = 0.0f, rotateAngle_Y = 0.0f, rotateAngle_Z = 0.0f;
float scale_X = 1.0f, scale_Y = 1.0f, scale_Z = 1.0f;
float rotateAxis_X = 1.0f, rotateAxis_Y = 0.0f, rotateAxis_Z = 0.0f;
glm::mat4 currentSceneTransform(1.0f);

// camera (starts just inside the sliding door, facing the blackboard)
Camera camera(glm::vec3(-2.5f, 1.6f, -8.5f));

// Mouse look: only active while the LEFT mouse button is held.
bool leftMouseHeld = false;
bool mouseLookInitialized = false;
double lastMouseX = 0.0, lastMouseY = 0.0;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// ---- moving object #1: the sliding door ----
bool doorOpenTarget = false;
float doorSlide = 0.0f;     // 0 = fully closed .. 1 = fully open
const float DOOR_SPEED = 2.5f;

// ---- moving object #2 (bonus): the ceiling fan ----
bool fanOn = true;
float fanAngle = 0.0f;
const float FAN_SPEED = 160.0f; // degrees per second

// ---- light toggles ----
bool pointLightsOn = true;
bool spotLightOn = true;

// point light positions (4 ceiling lights spread across the room)
glm::vec3 pointLightPositions[4] = {
    glm::vec3(-3.0f, ROOM_H - 0.25f, -2.0f),
    glm::vec3(3.0f,  ROOM_H - 0.25f, -2.0f),
    glm::vec3(-3.0f, ROOM_H - 0.25f,  2.6f),
    glm::vec3(3.0f,  ROOM_H - 0.25f,  2.6f)
};

PointLight pointlight1(
    pointLightPositions[0].x, pointLightPositions[0].y, pointLightPositions[0].z,
    0.08f, 0.08f, 0.07f,      // ambient
    0.75f, 0.75f, 0.68f,      // diffuse
    0.4f, 0.4f, 0.4f,         // specular
    1.0f, 0.09f, 0.032f,      // k_c, k_l, k_q
    1
);
PointLight pointlight2(
    pointLightPositions[1].x, pointLightPositions[1].y, pointLightPositions[1].z,
    0.08f, 0.08f, 0.07f,
    0.75f, 0.75f, 0.68f,
    0.4f, 0.4f, 0.4f,
    1.0f, 0.09f, 0.032f,
    2
);
PointLight pointlight3(
    pointLightPositions[2].x, pointLightPositions[2].y, pointLightPositions[2].z,
    0.08f, 0.08f, 0.07f,
    0.75f, 0.75f, 0.68f,
    0.4f, 0.4f, 0.4f,
    1.0f, 0.09f, 0.032f,
    3
);
PointLight pointlight4(
    pointLightPositions[3].x, pointLightPositions[3].y, pointLightPositions[3].z,
    0.08f, 0.08f, 0.07f,
    0.75f, 0.75f, 0.68f,
    0.4f, 0.4f, 0.4f,
    1.0f, 0.09f, 0.032f,
    4
);

// spotlight: mounted on the ceiling above the teacher's table, shining straight down
// like a presentation / podium spotlight
glm::vec3 spotLightPosition(0.0f, ROOM_H - 0.25f, 4.3f);
SpotLight spotlight1(
    spotLightPosition.x, spotLightPosition.y, spotLightPosition.z,   // position
    0.0f, -1.0f, 0.15f,                                              // direction (down, tilted slightly toward the board)
    0.05f, 0.05f, 0.05f,                                             // ambient
    0.95f, 0.9f, 0.75f,                                              // diffuse (warm spotlight color)
    1.0f, 1.0f, 1.0f,                                                // specular
    1.0f, 0.045f, 0.0075f,                                           // k_c, k_l, k_q (longer range)
    15.0f, 23.0f                                                     // inner / outer cone angle (degrees)
);



void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
    camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

int main()
{
    // glfw: initialize and configure
    // ------------------------------
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    // glfw window creation
    // --------------------
    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "3D Classroom - Computer Graphics Project", NULL, NULL);
    if (window == NULL)
    {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSetScrollCallback(window, scroll_callback);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    // Keep the cursor visible. Camera rotates only while LEFT mouse is held.
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);

    // glad: load all OpenGL function pointers
    // ---------------------------------------
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    // configure global opengl state
    // -----------------------------
    glEnable(GL_DEPTH_TEST);

    // build and compile our shader programs
    // --------------------------------------
    Shader roomShader("classroomVertexShader.vs", "classroomFragmentShader.fs");
    Shader lampShader("vertexShader.vs", "fragmentShader.fs");

    // ------------------------------------------------------------------
    // set up a single unit cube (position + normal) for the main classroom furniture.
    // The trash bin below uses a separate curved cylinder mesh.
    // ------------------------------------------------------------------
    float cube_vertices[] = {
        // positions      // normals
        0.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 0.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 0.0f, -1.0f,

        1.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f,
        1.0f, 1.0f, 1.0f, 1.0f, 0.0f, 0.0f,

        0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        1.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f,

        0.0f, 0.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 1.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 1.0f, 0.0f, -1.0f, 0.0f, 0.0f,
        0.0f, 0.0f, 0.0f, -1.0f, 0.0f, 0.0f,

        1.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,
        1.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 0.0f,
        0.0f, 1.0f, 1.0f, 0.0f, 1.0f, 0.0f,

        0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f,
        1.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f,
        0.0f, 0.0f, 1.0f, 0.0f, -1.0f, 0.0f
    };
    unsigned int cube_indices[] = {
        0, 3, 2, 2, 1, 0,
        4, 5, 7, 7, 6, 4,
        8, 9, 10, 10, 11, 8,
        12, 13, 14, 14, 15, 12,
        16, 17, 18, 18, 19, 16,
        20, 21, 22, 22, 23, 20
    };

    unsigned int cubeVAO, cubeVBO, cubeEBO;
    glGenVertexArrays(1, &cubeVAO);
    glGenBuffers(1, &cubeVBO);
    glGenBuffers(1, &cubeEBO);

    glBindVertexArray(cubeVAO);

    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(cube_vertices), cube_vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(cube_indices), cube_indices, GL_STATIC_DRAW);

    // position attribute
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    // normal attribute
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // second VAO for the unlit light-bulb markers (same VBO/EBO, position only)
    unsigned int lampVAO;
    glGenVertexArrays(1, &lampVAO);
    glBindVertexArray(lampVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cubeVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeEBO);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    // ------------------------------------------------------------------
    // Cylinder mesh for the classroom trash bin.
    // This is a real curved 3D primitive (not a scaled cube).
    // ------------------------------------------------------------------
    const int CYLINDER_SEGMENTS = 32;
    const float cylinderHeight = 1.0f;
    std::vector<float> cylinderVertices;
    std::vector<unsigned int> cylinderIndices;
    cylinderVertices.reserve((CYLINDER_SEGMENTS * 2 + 2) * 6);

    // Side vertices: bottom and top rings.
    for (int i = 0; i < CYLINDER_SEGMENTS; ++i)
    {
        float a = 2.0f * 3.14159265358979323846f * (float)i / (float)CYLINDER_SEGMENTS;
        float x = cosf(a);
        float z = sinf(a);
        // bottom
        cylinderVertices.insert(cylinderVertices.end(), {x, 0.0f, z, x, 0.0f, z});
        // top
        cylinderVertices.insert(cylinderVertices.end(), {x, cylinderHeight, z, x, 0.0f, z});
    }

    // Side triangles.
    for (int i = 0; i < CYLINDER_SEGMENTS; ++i)
    {
        int n = (i + 1) % CYLINDER_SEGMENTS;
        unsigned int b0 = 2 * i;
        unsigned int t0 = 2 * i + 1;
        unsigned int b1 = 2 * n;
        unsigned int t1 = 2 * n + 1;
        cylinderIndices.insert(cylinderIndices.end(), {b0, b1, t1, t1, t0, b0});
    }

    // Bottom-center vertex (normal down) and top-center vertex (normal up).
    unsigned int bottomCenter = (unsigned int)(cylinderVertices.size() / 6);
    cylinderVertices.insert(cylinderVertices.end(), {0.0f, 0.0f, 0.0f, 0.0f, -1.0f, 0.0f});
    unsigned int topCenter = (unsigned int)(cylinderVertices.size() / 6);
    cylinderVertices.insert(cylinderVertices.end(), {0.0f, cylinderHeight, 0.0f, 0.0f, 1.0f, 0.0f});

    // Caps.
    for (int i = 0; i < CYLINDER_SEGMENTS; ++i)
    {
        int n = (i + 1) % CYLINDER_SEGMENTS;
        unsigned int b0 = 2 * i;
        unsigned int b1 = 2 * n;
        unsigned int t0 = 2 * i + 1;
        unsigned int t1 = 2 * n + 1;
        cylinderIndices.insert(cylinderIndices.end(), {bottomCenter, b1, b0});
        cylinderIndices.insert(cylinderIndices.end(), {topCenter, t0, t1});
    }

    unsigned int cylinderVAO, cylinderVBO, cylinderEBO;
    glGenVertexArrays(1, &cylinderVAO);
    glGenBuffers(1, &cylinderVBO);
    glGenBuffers(1, &cylinderEBO);

    glBindVertexArray(cylinderVAO);
    glBindBuffer(GL_ARRAY_BUFFER, cylinderVBO);
    glBufferData(GL_ARRAY_BUFFER, cylinderVertices.size() * sizeof(float), cylinderVertices.data(), GL_STATIC_DRAW);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cylinderEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cylinderIndices.size() * sizeof(unsigned int), cylinderIndices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    std::cout << "\n=================  3D CLASSROOM CONTROLS  =================\n";
    std::cout << " Start view      : outside the closed sliding door\n";
    std::cout << " SPACE           : open/close sliding door\n";
    std::cout << " W / S           : move forward / backward (after door opens)\n";
    std::cout << " A / D           : rotate camera left / right (after door opens)\n";
    std::cout << " LEFT MOUSE + DRAG: look around (after door opens)\n";
    std::cout << " Q / E           : move down / up\n";
    std::cout << " R               : rotate selected axis\n";
    std::cout << " X / Y / Z       : select rotation axis\n";
    std::cout << " I / K           : translate Y\n";
    std::cout << " J / L           : translate X\n";
    std::cout << " O / P           : translate Z\n";
    std::cout << " C / V           : scale X\n";
    std::cout << " B / N           : scale Y\n";
    std::cout << " M / U           : scale Z\n";
    std::cout << " F               : fan on/off\n";
    std::cout << " 1               : point lights on/off\n";
    std::cout << " 2               : spotlight on/off\n";
    std::cout << " ESC             : quit\n";
    std::cout << " Mouse           : disabled\n";
    std::cout << "=============================================================\n\n";

    // render loop
    // -----------
    while (!glfwWindowShouldClose(window))
    {
        // per-frame time logic
        float currentFrame = static_cast<float>(glfwGetTime());
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        processInput(window);

        // Same global model transformation pattern used by the Kitchen project.
        glm::mat4 identityMatrix(1.0f);
        glm::mat4 translateMatrix = glm::translate(identityMatrix, glm::vec3(translate_X, translate_Y, translate_Z));
        glm::mat4 rotateXMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_X), glm::vec3(1.0f, 0.0f, 0.0f));
        glm::mat4 rotateYMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Y), glm::vec3(0.0f, 1.0f, 0.0f));
        glm::mat4 rotateZMatrix = glm::rotate(identityMatrix, glm::radians(rotateAngle_Z), glm::vec3(0.0f, 0.0f, 1.0f));
        glm::mat4 scaleMatrix = glm::scale(identityMatrix, glm::vec3(scale_X, scale_Y, scale_Z));
        currentSceneTransform = translateMatrix * rotateXMatrix * rotateYMatrix * rotateZMatrix * scaleMatrix;

        // -------------------- update animations --------------------
        // The door moves only after SPACE toggles it.
        float doorTarget = doorOpenTarget ? 1.0f : 0.0f;
        if (doorSlide < doorTarget)
        {
            doorSlide += DOOR_SPEED * deltaTime;
            if (doorSlide > doorTarget) doorSlide = doorTarget;
        }
        else if (doorSlide > doorTarget)
        {
            doorSlide -= DOOR_SPEED * deltaTime;
            if (doorSlide < doorTarget) doorSlide = doorTarget;
        }

        if (fanOn)
            fanAngle = fmodf(fanAngle + deltaTime * FAN_SPEED, 360.0f);

        // -------------------- render --------------------
        glClearColor(0.55f, 0.65f, 0.75f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        roomShader.use();
        roomShader.setVec3("viewPos", camera.Position);

        // lights
        pointlight1.setUpPointLight(roomShader);
        pointlight2.setUpPointLight(roomShader);
        pointlight3.setUpPointLight(roomShader);
        pointlight4.setUpPointLight(roomShader);
        spotlight1.setUpSpotLight(roomShader);

        roomShader.use();

        // viewing + projection transformation
        float aspect = (float)SCR_WIDTH / (float)SCR_HEIGHT;
        glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), aspect, 0.1f, 100.0f);
        roomShader.setMat4("projection", projection);

        glm::mat4 view = camera.GetViewMatrix();
        roomShader.setMat4("view", view);

        // -------------------- draw the scene (all 3D transformations) --------------------
        drawRoom(cubeVAO, roomShader);
        drawBlackboard(cubeVAO, roomShader);
        drawDoor(cubeVAO, roomShader, doorSlide);           // moving object: sliding door
        drawCeilingFan(cubeVAO, roomShader, glm::vec3(0.0f, ROOM_H, 0.8f), fanAngle); // moving object: fan
        drawCylinder(cylinderVAO, roomShader, glm::mat4(1.0f), glm::vec3(4.6f, 0.0f, -4.55f), 0.38f, 0.75f, glm::vec3(0.18f, 0.20f, 0.22f)); // curved object: trash bin

        drawTeacherTable(cubeVAO, roomShader, glm::vec3(0.0f, 0.0f, 4.3f));
        drawChair(cubeVAO, roomShader, glm::vec3(0.0f, 0.0f, 5.05f), glm::vec3(0.55f, 0.15f, 0.15f), 180.0f);

        // rows of student desks + chairs, each column in a different chair color
        glm::vec3 chairColors[3] = {
            glm::vec3(0.80f, 0.20f, 0.20f), // red
            glm::vec3(0.20f, 0.45f, 0.80f), // blue
            glm::vec3(0.25f, 0.65f, 0.30f)  // green
        };
        glm::vec3 deskColor(0.65f, 0.48f, 0.30f);

        for (int row = 0; row < 3; row++)
        {
            float z = -1.6f + row * 1.9f;
            for (int col = 0; col < 3; col++)
            {
                float x = -3.2f + col * 3.2f;
                drawDesk(cubeVAO, roomShader, glm::vec3(x, 0.0f, z), deskColor);
                drawChair(cubeVAO, roomShader, glm::vec3(x, 0.0f, z - 0.75f), chairColors[col], 0.0f);
            }
        }

        // -------------------- draw the light-bulb markers (unlit) --------------------
        lampShader.use();
        lampShader.setMat4("projection", projection);
        lampShader.setMat4("view", view);
        glBindVertexArray(lampVAO);

        for (int i = 0; i < 4; i++)
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), pointLightPositions[i]);
            model = glm::scale(model, glm::vec3(0.18f));
            lampShader.setMat4("model", model);
            lampShader.setVec3("color", pointLightsOn ? glm::vec3(0.95f, 0.9f, 0.6f) : glm::vec3(0.3f, 0.3f, 0.3f));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }
        // spotlight housing
        {
            glm::mat4 model = glm::translate(glm::mat4(1.0f), spotLightPosition);
            model = glm::scale(model, glm::vec3(0.22f, 0.12f, 0.22f));
            lampShader.setMat4("model", model);
            lampShader.setVec3("color", spotLightOn ? glm::vec3(1.0f, 0.85f, 0.5f) : glm::vec3(0.3f, 0.3f, 0.3f));
            glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(1, &cubeVAO);
    glDeleteVertexArrays(1, &lampVAO);
    glDeleteVertexArrays(1, &cylinderVAO);
    glDeleteBuffers(1, &cubeVBO);
    glDeleteBuffers(1, &cubeEBO);
    glDeleteBuffers(1, &cylinderVBO);
    glDeleteBuffers(1, &cylinderEBO);

    glfwTerminate();
    return 0;
}

// ------------------------------------------------------------------------
// generic cube drawer: every piece of furniture/room geometry is built from
// this. 'position' is the pivot: bottom-center of the box (so objects can be
// stacked on top of the floor / on top of each other easily), matching the
// convention used in the course's bed()/drawCube() sample code.
// ------------------------------------------------------------------------
void drawCube(unsigned int& cubeVAO, Shader& shader, glm::mat4 parentModel, glm::vec3 position, glm::vec3 size, glm::vec3 color, float rotYDeg)
{
    shader.use();

    glm::mat4 model = currentSceneTransform * glm::translate(parentModel, position);
    if (rotYDeg != 0.0f)
        model = glm::rotate(model, glm::radians(rotYDeg), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::scale(model, size);
    model = glm::translate(model, glm::vec3(-0.5f, 0.0f, -0.5f));

    shader.setVec3("material.ambient", color);
    shader.setVec3("material.diffuse", color);
    shader.setVec3("material.specular", glm::vec3(0.3f, 0.3f, 0.3f));
    shader.setFloat("material.shininess", 32.0f);
    shader.setMat4("model", model);

    glBindVertexArray(cubeVAO);
    glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

// ------------------------------------------------------------------------
// the room shell: checkered floor, ceiling, four walls, windows, bulletin board
// ------------------------------------------------------------------------
void drawRoom(unsigned int& cubeVAO, Shader& shader)
{
    glm::mat4 I(1.0f);
    float halfW = ROOM_W / 2.0f;
    float halfD = ROOM_D / 2.0f;

    // ---- checkered floor ----
    int tiles = 6;
    float tileSize = ROOM_W / tiles;
    for (int i = 0; i < tiles; i++)
    {
        for (int j = 0; j < tiles; j++)
        {
            float x = -halfW + tileSize * i + tileSize / 2.0f;
            float z = -halfD + tileSize * j + tileSize / 2.0f;
            glm::vec3 col = ((i + j) % 2 == 0) ? glm::vec3(0.80f, 0.72f, 0.55f) : glm::vec3(0.62f, 0.50f, 0.35f);
            drawCube(cubeVAO, shader, I, glm::vec3(x, 0.0f, z), glm::vec3(tileSize, 0.12f, tileSize), col);
        }
    }

    // ---- ceiling ----
    drawCube(cubeVAO, shader, I, glm::vec3(0.0f, ROOM_H, 0.0f), glm::vec3(ROOM_W, 0.15f, ROOM_D), glm::vec3(0.93f, 0.93f, 0.90f));

    // ---- walls ----
    glm::vec3 wallColorNS(0.90f, 0.87f, 0.76f);  // front / back wall
    glm::vec3 wallColorEW(0.84f, 0.89f, 0.92f);  // left / right wall

    // Front wall with a real doorway opening.
    const float doorCenterX = -2.5f;
    const float doorWidth = 1.6f;
    const float doorHeight = 2.6f;
    const float doorLeft = doorCenterX - doorWidth / 2.0f;
    const float doorRight = doorCenterX + doorWidth / 2.0f;

    drawCube(cubeVAO, shader, I,
        glm::vec3((-halfW + doorLeft) / 2.0f, 0.0f, -halfD),
        glm::vec3(doorLeft + halfW, ROOM_H, 0.2f), wallColorNS);
    drawCube(cubeVAO, shader, I,
        glm::vec3((doorRight + halfW) / 2.0f, 0.0f, -halfD),
        glm::vec3(halfW - doorRight, ROOM_H, 0.2f), wallColorNS);
    drawCube(cubeVAO, shader, I,
        glm::vec3(doorCenterX, (doorHeight + ROOM_H) / 2.0f, -halfD),
        glm::vec3(doorWidth, ROOM_H - doorHeight, 0.2f), wallColorNS);
    drawCube(cubeVAO, shader, I, glm::vec3(0.0f, 0.0f, halfD), glm::vec3(ROOM_W, ROOM_H, 0.2f), wallColorNS); // back (board) wall
    drawCube(cubeVAO, shader, I, glm::vec3(-halfW, 0.0f, 0.0f), glm::vec3(0.2f, ROOM_H, ROOM_D), wallColorEW); // left (window) wall
    drawCube(cubeVAO, shader, I, glm::vec3(halfW, 0.0f, 0.0f), glm::vec3(0.2f, ROOM_H, ROOM_D), wallColorEW); // right wall

    // ---- windows on the left wall ----
    // (the interior face of the left wall is at -halfW+0.1; positive offsets
    // from there stick INTO the room, matching the door/blackboard convention)
    float innerX_left = -halfW + 0.1f;
    glm::vec3 frameColor(0.95f, 0.95f, 0.95f);
    glm::vec3 glassColor(0.65f, 0.85f, 0.92f);
    for (int k = -1; k <= 1; k++)
    {
        float z = k * 3.2f;
        drawCube(cubeVAO, shader, I, glm::vec3(innerX_left + 0.04f, 1.2f, z), glm::vec3(0.08f, 1.4f, 1.6f), frameColor);
        drawCube(cubeVAO, shader, I, glm::vec3(innerX_left + 0.11f, 1.35f, z), glm::vec3(0.05f, 1.1f, 1.3f), glassColor);
    }

    // ---- bulletin board on the right wall ----
    // (the interior face of the right wall is at halfW-0.1; negative offsets
    // from there stick INTO the room)
    float innerX_right = halfW - 0.1f;
    drawCube(cubeVAO, shader, I, glm::vec3(innerX_right - 0.04f, 1.6f, -2.0f), glm::vec3(0.08f, 1.3f, 2.2f), glm::vec3(0.55f, 0.35f, 0.20f));
    drawCube(cubeVAO, shader, I, glm::vec3(innerX_right - 0.10f, 1.7f, -2.0f), glm::vec3(0.03f, 1.0f, 1.9f), glm::vec3(0.95f, 0.80f, 0.20f));
}

// ------------------------------------------------------------------------
// blackboard + wooden frame + chalk tray, mounted on the back wall
// ------------------------------------------------------------------------
void drawBlackboard(unsigned int& cubeVAO, Shader& shader)
{
    glm::mat4 I(1.0f);
    float halfD = ROOM_D / 2.0f;
    float innerZ = halfD - 0.1f; // interior face of the back wall

    drawCube(cubeVAO, shader, I, glm::vec3(0.0f, 1.4f, innerZ - 0.06f), glm::vec3(6.4f, 2.6f, 0.06f), glm::vec3(0.40f, 0.25f, 0.12f));   // frame
    drawCube(cubeVAO, shader, I, glm::vec3(0.0f, 1.55f, innerZ - 0.14f), glm::vec3(6.0f, 2.2f, 0.05f), glm::vec3(0.05f, 0.20f, 0.10f));  // board
    drawCube(cubeVAO, shader, I, glm::vec3(0.0f, 0.55f, innerZ - 0.22f), glm::vec3(6.0f, 0.08f, 0.16f), glm::vec3(0.55f, 0.38f, 0.20f)); // chalk tray
}

// ------------------------------------------------------------------------
// sliding classroom door on the front wall.
// slideAmount: 0.0 = fully closed, 1.0 = fully open
// ------------------------------------------------------------------------
void drawDoor(unsigned int& cubeVAO, Shader& shader, float slideAmount)
{
    glm::mat4 I(1.0f);
    float halfD = ROOM_D / 2.0f;
    float innerZ = -halfD + 0.1f; // interior face of the front wall
    float doorW = 1.6f, doorH = 2.6f;
    float doorX = -2.5f;

    // top rail the door slides along (static)
    drawCube(cubeVAO, shader, I, glm::vec3(doorX + (doorW + 0.4f) / 2.0f, doorH, innerZ + 0.02f), glm::vec3(doorW * 2.1f, 0.08f, 0.10f), glm::vec3(0.25f, 0.25f, 0.28f));

    // the sliding panel itself
    float panelX = doorX + slideAmount * (doorW + 0.4f);
    drawCube(cubeVAO, shader, I, glm::vec3(panelX, 0.0f, innerZ + 0.05f), glm::vec3(doorW, doorH, 0.06f), glm::vec3(0.16f, 0.36f, 0.58f));

    // handle
    drawCube(cubeVAO, shader, I, glm::vec3(panelX + doorW * 0.35f, 1.1f, innerZ + 0.05f), glm::vec3(0.06f, 0.14f, 0.05f), glm::vec3(0.88f, 0.74f, 0.18f));
}

// ------------------------------------------------------------------------
// a simple student desk (tabletop + 4 legs)
// ------------------------------------------------------------------------
void drawDesk(unsigned int& cubeVAO, Shader& shader, glm::vec3 pos, glm::vec3 topColor)
{
    glm::mat4 I(1.0f);
    glm::vec3 legColor(0.25f, 0.25f, 0.28f);
    float w = 0.9f, d = 0.55f, topH = 0.05f, legH = 0.68f, legT = 0.05f;

    drawCube(cubeVAO, shader, I, pos + glm::vec3(0.0f, legH, 0.0f), glm::vec3(w, topH, d), topColor);

    float lx = w / 2.0f - legT / 2.0f - 0.03f;
    float lz = d / 2.0f - legT / 2.0f - 0.03f;
    drawCube(cubeVAO, shader, I, pos + glm::vec3(-lx, 0.0f, -lz), glm::vec3(legT, legH, legT), legColor);
    drawCube(cubeVAO, shader, I, pos + glm::vec3(lx, 0.0f, -lz), glm::vec3(legT, legH, legT), legColor);
    drawCube(cubeVAO, shader, I, pos + glm::vec3(-lx, 0.0f, lz), glm::vec3(legT, legH, legT), legColor);
    drawCube(cubeVAO, shader, I, pos + glm::vec3(lx, 0.0f, lz), glm::vec3(legT, legH, legT), legColor);
}

// ------------------------------------------------------------------------
// a simple chair (seat + backrest + 4 legs). rotYDeg = 0 faces +Z (toward
// the blackboard); rotYDeg = 180 faces -Z (used for the teacher's chair).
// ------------------------------------------------------------------------
void drawChair(unsigned int& cubeVAO, Shader& shader, glm::vec3 pos, glm::vec3 seatColor, float rotYDeg)
{
    glm::mat4 model = glm::translate(glm::mat4(1.0f), pos);
    model = glm::rotate(model, glm::radians(rotYDeg), glm::vec3(0.0f, 1.0f, 0.0f));

    float seatW = 0.45f, seatD = 0.45f, seatH = 0.05f;
    float legH = 0.45f, legT = 0.04f;
    float backH = 0.5f, backT = 0.04f;
    glm::vec3 legColor(0.2f, 0.2f, 0.22f);

    drawCube(cubeVAO, shader, model, glm::vec3(0.0f, legH, 0.0f), glm::vec3(seatW, seatH, seatD), seatColor);
    drawCube(cubeVAO, shader, model, glm::vec3(0.0f, legH + seatH, -seatD / 2.0f + backT / 2.0f), glm::vec3(seatW, backH, backT), seatColor);

    float lx = seatW / 2.0f - legT / 2.0f - 0.02f;
    float lz = seatD / 2.0f - legT / 2.0f - 0.02f;
    drawCube(cubeVAO, shader, model, glm::vec3(-lx, 0.0f, -lz), glm::vec3(legT, legH, legT), legColor);
    drawCube(cubeVAO, shader, model, glm::vec3(lx, 0.0f, -lz), glm::vec3(legT, legH, legT), legColor);
    drawCube(cubeVAO, shader, model, glm::vec3(-lx, 0.0f, lz), glm::vec3(legT, legH, legT), legColor);
    drawCube(cubeVAO, shader, model, glm::vec3(lx, 0.0f, lz), glm::vec3(legT, legH, legT), legColor);
}

// ------------------------------------------------------------------------
// teacher's table: same idea as drawDesk, just bigger
// ------------------------------------------------------------------------
void drawTeacherTable(unsigned int& cubeVAO, Shader& shader, glm::vec3 pos)
{
    glm::mat4 I(1.0f);
    glm::vec3 topColor(0.42f, 0.28f, 0.16f);
    glm::vec3 legColor(0.20f, 0.20f, 0.22f);
    float w = 1.7f, d = 0.7f, topH = 0.06f, legH = 0.75f, legT = 0.06f;

    drawCube(cubeVAO, shader, I, pos + glm::vec3(0.0f, legH, 0.0f), glm::vec3(w, topH, d), topColor);

    float lx = w / 2.0f - legT / 2.0f - 0.04f;
    float lz = d / 2.0f - legT / 2.0f - 0.04f;
    drawCube(cubeVAO, shader, I, pos + glm::vec3(-lx, 0.0f, -lz), glm::vec3(legT, legH, legT), legColor);
    drawCube(cubeVAO, shader, I, pos + glm::vec3(lx, 0.0f, -lz), glm::vec3(legT, legH, legT), legColor);
    drawCube(cubeVAO, shader, I, pos + glm::vec3(-lx, 0.0f, lz), glm::vec3(legT, legH, legT), legColor);
    drawCube(cubeVAO, shader, I, pos + glm::vec3(lx, 0.0f, lz), glm::vec3(legT, legH, legT), legColor);
}

// ------------------------------------------------------------------------
// ceiling fan: a mount rod + hub + 3 blades, all spinning around Y.
// this is a bonus second moving object (the sliding door is the required one).
// ------------------------------------------------------------------------
void drawCeilingFan(unsigned int& cubeVAO, Shader& shader, glm::vec3 pos, float angleDeg)
{
    glm::mat4 base = glm::translate(glm::mat4(1.0f), pos);

    // mount rod hanging from the ceiling
    drawCube(cubeVAO, shader, base, glm::vec3(0.0f, -0.3f, 0.0f), glm::vec3(0.05f, 0.3f, 0.05f), glm::vec3(0.2f, 0.2f, 0.2f));

    glm::mat4 hub = glm::translate(base, glm::vec3(0.0f, -0.33f, 0.0f));
    hub = glm::rotate(hub, glm::radians(angleDeg), glm::vec3(0.0f, 1.0f, 0.0f));

    // motor housing
    drawCube(cubeVAO, shader, hub, glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.22f, 0.08f, 0.22f), glm::vec3(0.32f, 0.32f, 0.35f));

    // 3 blades spaced 120 degrees apart, spinning with the hub
    for (int i = 0; i < 3; i++)
    {
        glm::mat4 blade = glm::rotate(hub, glm::radians(i * 120.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        drawCube(cubeVAO, shader, blade, glm::vec3(0.75f, 0.02f, 0.0f), glm::vec3(1.5f, 0.02f, 0.22f), glm::vec3(0.55f, 0.55f, 0.58f));
    }
}

// ------------------------------------------------------------------------
// curved object: a real cylinder mesh used as the classroom trash bin.
// position is the bottom-center of the cylinder.
// ------------------------------------------------------------------------
void drawCylinder(unsigned int& cylinderVAO, Shader& shader, glm::mat4 parentModel, glm::vec3 position, float radius, float height, glm::vec3 color)
{
    shader.use();

    glm::mat4 model = currentSceneTransform * glm::translate(parentModel, position);
    model = glm::scale(model, glm::vec3(radius, 1.0f, radius));

    shader.setVec3("material.ambient", color);
    shader.setVec3("material.diffuse", color);
    shader.setVec3("material.specular", glm::vec3(0.35f, 0.35f, 0.35f));
    shader.setFloat("material.shininess", 32.0f);
    shader.setMat4("model", model);

    glBindVertexArray(cylinderVAO);
    // 32 side segments + 32 bottom + 32 top = 96 triangles.
    glDrawElements(GL_TRIANGLES, 32 * 6 + 32 * 3 + 32 * 3, GL_UNSIGNED_INT, 0);
}

// ------------------------------------------------------------------------
// Mouse look: ONLY works while the LEFT mouse button is held.
// Releasing the button immediately stops camera rotation.
// ------------------------------------------------------------------------
void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
    int leftButton = glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT);

    if (leftButton != GLFW_PRESS)
    {
        leftMouseHeld = false;
        mouseLookInitialized = false;
        return;
    }

    // Do not jump the camera when the button is first pressed.
    if (!leftMouseHeld || !mouseLookInitialized)
    {
        lastMouseX = xpos;
        lastMouseY = ypos;
        leftMouseHeld = true;
        mouseLookInitialized = true;
        return;
    }

    float xoffset = static_cast<float>(xpos - lastMouseX);
    float yoffset = static_cast<float>(lastMouseY - ypos);
    lastMouseX = xpos;
    lastMouseY = ypos;

    // Camera look is available only after entering through the opened door.
    if (doorSlide > 0.98f)
        camera.ProcessMouseMovement(xoffset, yoffset);
}

// ------------------------------------------------------------------------
// process all input: query GLFW whether relevant keys are held this frame
// ------------------------------------------------------------------------
void processInput(GLFWwindow* window)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, true);

    // Poll SPACE directly. This is deliberately independent of the key callback.
    static bool spaceWasDown = false;
    bool spaceDown = (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS);
    if (spaceDown && !spaceWasDown)
        doorOpenTarget = !doorOpenTarget;
    spaceWasDown = spaceDown;

    // The player can enter only after the door has fully opened.
    if (doorSlide > 0.98f)
    {
        float boost = (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) ? 2.0f : 1.0f;
        float dt = deltaTime * boost;

        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
            camera.ProcessKeyboard(FORWARD, dt);
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
            camera.ProcessKeyboard(BACKWARD, dt);
        // A/D rotate the camera left/right instead of strafing.
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
            camera.ProcessKeyboard(TURN_LEFT, dt);
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
            camera.ProcessKeyboard(TURN_RIGHT, dt);
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
            camera.ProcessKeyboard(DOWN, dt);
        if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS)
            camera.ProcessKeyboard(UP, dt);
    }

    // Keyboard-only zoom; no mouse wheel.
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
        camera.ProcessMouseScroll(1.0f * deltaTime * 30.0f);
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
        camera.ProcessMouseScroll(-1.0f * deltaTime * 30.0f);

    // Kitchen / 3D_CubeTransformation-style transformation controls.
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS)
    {
        if (rotateAxis_X) rotateAngle_X -= 0.1f;
        else if (rotateAxis_Y) rotateAngle_Y -= 0.1f;
        else rotateAngle_Z -= 0.1f;
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) translate_Y += 0.001f;
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) translate_Y -= 0.001f;
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) translate_X += 0.001f;
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) translate_X -= 0.001f;
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) translate_Z += 0.001f;
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) translate_Z -= 0.001f;
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) scale_X += 0.001f;
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) scale_X -= 0.001f;
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) scale_Y += 0.001f;
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) scale_Y -= 0.001f;
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) scale_Z += 0.001f;
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) scale_Z -= 0.001f;

    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS)
    { rotateAxis_X = 1.0f; rotateAxis_Y = 0.0f; rotateAxis_Z = 0.0f; }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS)
    { rotateAxis_X = 0.0f; rotateAxis_Y = 1.0f; rotateAxis_Z = 0.0f; }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS)
    { rotateAxis_X = 0.0f; rotateAxis_Y = 0.0f; rotateAxis_Z = 1.0f; }
}

// ------------------------------------------------------------------------
// one-shot key presses (toggles)
// ------------------------------------------------------------------------
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action != GLFW_PRESS)
        return;

    // toggle the ceiling fan
    if (key == GLFW_KEY_F)
        fanOn = !fanOn;

    // toggle the point lights (ceiling lights)
    if (key == GLFW_KEY_1)
    {
        pointLightsOn = !pointLightsOn;
        if (pointLightsOn) { pointlight1.turnOn(); pointlight2.turnOn(); pointlight3.turnOn(); pointlight4.turnOn(); }
        else { pointlight1.turnOff(); pointlight2.turnOff(); pointlight3.turnOff(); pointlight4.turnOff(); }
    }

    // toggle the spotlight
    if (key == GLFW_KEY_2)
    {
        spotLightOn = !spotLightOn;
        if (spotLightOn) spotlight1.turnOn();
        else spotlight1.turnOff();
    }

}

// ------------------------------------------------------------------------
// glfw: whenever the window size changes, keep the viewport (and aspect
// ratio used for the projection matrix) in sync
// ------------------------------------------------------------------------
void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
    glViewport(0, 0, width, height);
    if (width > 0 && height > 0)
    {
        SCR_WIDTH = width;
        SCR_HEIGHT = height;
    }
}

