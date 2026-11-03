#include <GL/glew.h>
#include <GL/glfw3.h>
#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/type_ptr.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "Obj.h"
#include <cmath>

struct Ball {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 speed = glm::vec3(0.0f);
};

GLFWwindow* window = nullptr;
GLuint shaderProgramID = 0;
GLint modelLocation = -1, viewLocation = -1;
GLint projectionLocation = -1, colorLocation = -1;
GLuint cubevao = 0, cubevbo = 0, cubeebo = 0;
GLsizei cubeIndexCount = 0;
GLuint spherevao = 0, spherevbo = 0, sphereebo = 0;
GLsizei sphereIndexCount = 0;
GLuint planevao = 0, planevbo = 0, planeebo = 0;
constexpr GLsizei planeIndexCount = 6;
float deltaTime = 0.0f;

glm::vec3 cameraPosition(0.0f,2.0f, 8.0f);
glm::vec3 cameraTarget(0.0f,2.0f,0.0f);

glm::mat4 Zmove(1.0f);
glm::mat4 Yrotate(1.0f);
glm::mat4 Zrotate(1.0f);
bool isDragging = false;
double previousMouseX = 0.0;
float rotateZ = 0.0f;
float moveZ = 0.0f;
float rotateY = 0.0f;
struct Box {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 speed = glm::vec3(0.0f);
    float size = 0.4f;
};
Box boxes[3];
bool floorOpen = false;
bool isOpressed = false;
Ball ball[5];
int ballcount = 1;
bool isBpressed = false;


bool InitShaderProgram() {
    shaderProgramID = InitShader("vertex18.glsl", "fragment18.glsl");
    if (!shaderProgramID) return false;
    modelLocation = glGetUniformLocation(shaderProgramID, "model");
    viewLocation = glGetUniformLocation(shaderProgramID, "view");
    projectionLocation = glGetUniformLocation(shaderProgramID, "projection");
    colorLocation = glGetUniformLocation(shaderProgramID, "objectColor");
    return true;
}
void InitCubeBuffer(const ObjData& obj) {
    glGenVertexArrays(1, &cubevao);
    glBindVertexArray(cubevao);
    glGenBuffers(1, &cubevbo);
    glBindBuffer(GL_ARRAY_BUFFER, cubevbo);
    glBufferData(GL_ARRAY_BUFFER, obj.vertices.size() * sizeof(float),
        obj.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    glGenBuffers(1, &cubeebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj.indices.size() * sizeof(unsigned int),
        obj.indices.data(), GL_STATIC_DRAW);
    cubeIndexCount = static_cast<GLsizei>(obj.indices.size());
    glBindVertexArray(0);
}

void InitSphereBuffer(const ObjData& obj) {
    glGenVertexArrays(1, &spherevao);
    glBindVertexArray(spherevao);
    glGenBuffers(1, &spherevbo);
    glBindBuffer(GL_ARRAY_BUFFER, spherevbo);
    glBufferData(GL_ARRAY_BUFFER, obj.vertices.size() * sizeof(float),
        obj.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    glGenBuffers(1, &sphereebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, sphereebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj.indices.size() * sizeof(unsigned int),
        obj.indices.data(), GL_STATIC_DRAW);
    sphereIndexCount = static_cast<GLsizei>(obj.indices.size());
    glBindVertexArray(0);
}

void InitPlaneBuffer() {
    // Unit XZ plane centered at the origin; all Y coordinates are zero.
    const float vertices[] = {
        -0.5f, 0.0f, -0.5f,
        -0.5f, 0.0f,  0.5f,
         0.5f, 0.0f,  0.5f,
         0.5f, 0.0f, -0.5f
    };
    // Two triangles, facing +Y.
    const unsigned int indices[] = { 0, 1, 2, 0, 2, 3 };

    glGenVertexArrays(1, &planevao);
    glBindVertexArray(planevao);
    glGenBuffers(1, &planevbo);
    glBindBuffer(GL_ARRAY_BUFFER, planevbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    glGenBuffers(1, &planeebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, planeebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);
    glBindVertexArray(0);
}

// 장면 구성과 실습 동작은 다음 단계에서 작성.
void InitScene() {
    for (int i = 0; i < 3; ++i) {
        boxes[i].size = 0.4f + 0.2f * i;
        boxes[i].position = glm::vec3(0.0f, boxes[i].size * 0.5f, 1.0f - i);
        boxes[i].speed = glm::vec3(0.0f);
    }
    for (int i = 0;i < ballcount;i++) {
        ball[i].position = glm::vec3(0.0f, 2.0f, 0.0f);
        ball[i].speed = glm::vec3(1.0f, 0.8f, 0.6f);
    }
}

void InputProcess() {
    bool oPressed = glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS;
    if (oPressed && !isOpressed) floorOpen = !floorOpen;
    isOpressed = oPressed;
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        if (!isBpressed) {
            if (ballcount < 5) {
                ball[ballcount].position = glm::vec3(0.0f, 2.0f, 0.0f);
                ball[ballcount].speed = glm::vec3(1.0f, 0.8f, 0.6f);
                ballcount++;
            }
        }
        isBpressed = true;
    }
    else {
        isBpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        bool shiftPressed =
            glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
        if (shiftPressed) {
            moveZ -= 2.0f * deltaTime;
        }
        else {
            moveZ += 2.0f * deltaTime;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        bool shiftPressed =
            glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
        if (shiftPressed) {
            rotateY -= 45.0f * deltaTime;
        }
        else {
            rotateY += 45.0f * deltaTime;
        }
    }
}

void ProcessMouse() {
    if ((glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS ||
         glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) &&
        glfwGetWindowAttrib(window, GLFW_FOCUSED)) {
        double mouseX, mouseY;
        glfwGetCursorPos(window, &mouseX, &mouseY);
        if (isDragging) {
            // Right drag: clockwise; left drag: counterclockwise.
            rotateZ -= static_cast<float>(mouseX - previousMouseX) * 0.3f;
        }
        previousMouseX = mouseX;
        isDragging = true;
    }
    else {
        isDragging = false;
    }
}

void UpdateAnimation() {
    // Convert world downward gravity into the box's local coordinates.
    glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), glm::radians(rotateZ), glm::vec3(0, 0, 1));
    rotation = glm::rotate(rotation, glm::radians(rotateY), glm::vec3(0, 1, 0));
    glm::vec3 gravity = glm::transpose(glm::mat3(rotation)) * glm::vec3(0.0f, -4.0f, 0.0f);
    // Limit large frame steps and subdivide for stable gravity/collision updates.
    float frameTime = glm::min(deltaTime, 0.1f);
    int steps = static_cast<int>(std::ceil(frameTime / 0.008f));
    if (steps < 1) steps = 1;
    float stepTime = frameTime / steps;
    for (int step = 0; step < steps; ++step) {
        for (Box& box : boxes) {
            float half = box.size * 0.5f;
            box.speed += gravity * stepTime;
            box.position += box.speed * stepTime;
            // Once below the room, objects fall freely outside its walls.
            if (box.position.y + half >= 0.0f) {
                for (int axis : { 0, 2 }) {
                    float limit = 2.0f - half;
                    if (box.position[axis] > limit) {
                        box.position[axis] = limit;
                        box.speed[axis] = glm::min(box.speed[axis], 0.0f);
                    }
                    if (box.position[axis] < -limit) {
                        box.position[axis] = -limit;
                        box.speed[axis] = glm::max(box.speed[axis], 0.0f);
                    }
                }
                if (box.position.y > 4.0f - half) {
                    box.position.y = 4.0f - half;
                    box.speed.y = glm::min(box.speed.y, 0.0f);
                }
                if (!floorOpen && box.position.y < half && box.position.y >= 0.0f) {
                    box.position.y = half;
                    box.speed.y = glm::max(box.speed.y, 0.0f);
                    box.speed.x *= std::exp(-2.0f * stepTime);
                    box.speed.z *= std::exp(-2.0f * stepTime);
                }
            }
        }

    for (int i = 0;i < ballcount;i++) {
        if (floorOpen || ball[i].position.y < 0.0f)
            ball[i].speed += gravity * stepTime;
        ball[i].position += ball[i].speed * stepTime;
        if (ball[i].position.y < -0.2f) continue;
        if (ball[i].position.x >= 1.8f) {
            ball[i].position.x = 1.8f;
            ball[i].speed.x = -std::abs(ball[i].speed.x);
        }
        else if (ball[i].position.x <= -1.8f) {
            ball[i].position.x = -1.8f;
            ball[i].speed.x = std::abs(ball[i].speed.x);
        }
        if (ball[i].position.y >= 3.8f) {
            ball[i].position.y = 3.8f;
            ball[i].speed.y = -std::abs(ball[i].speed.y);
        }
        else if (!floorOpen && ball[i].position.y >= 0.0f && ball[i].position.y <= 0.2f) {
            ball[i].position.y = 0.2f;
            ball[i].speed.y = std::abs(ball[i].speed.y);
        }
        if (ball[i].position.z >= 1.8f) {
            ball[i].position.z = 1.8f;
            ball[i].speed.z = -std::abs(ball[i].speed.z);
        }
        else if (ball[i].position.z <= -1.8f) {
            ball[i].position.z = -1.8f;
            ball[i].speed.z = std::abs(ball[i].speed.z);
        }
    }
}
}

void DrawCube(const glm::mat4& model, const glm::vec3& color) {
    glm::mat4 finalModel = Zmove * Zrotate * Yrotate * model;
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(finalModel));
    glUniform3fv(colorLocation, 1, glm::value_ptr(color));
    glBindVertexArray(cubevao);
    glDrawElements(GL_TRIANGLES, cubeIndexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void DrawSphere(const glm::mat4& model, const glm::vec3& color) {
    glm::mat4 finalModel = Zmove * Zrotate * Yrotate * model;
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(finalModel));
    glUniform3fv(colorLocation, 1, glm::value_ptr(color));
    glBindVertexArray(spherevao);
    glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void DrawPlane(const glm::mat4& model, const glm::vec3& color) {
    glm::mat4 finalModel = Zmove * Zrotate * Yrotate * model;
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(finalModel));
    glUniform3fv(colorLocation, 1, glm::value_ptr(color));
    glBindVertexArray(planevao);
    glDrawElements(GL_TRIANGLES, planeIndexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void Drawfloor() {
    glm::mat4 planemodel(1.0f);

    planemodel = glm::scale(planemodel, glm::vec3(4.0f, 1.0f, 4.0f));

    DrawPlane(planemodel, glm::vec3(0.5f, 0.5f, 0.5f));
}

void DrawBackWall() {
    glm::mat4 backmodel(1.0f);

    backmodel = glm::translate(backmodel, glm::vec3(0.0f, 2.0f, -2.0f));
    backmodel = glm::rotate(backmodel, glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    backmodel = glm::scale(backmodel, glm::vec3(4.0f, 1.0f, 4.0f));
    
    DrawPlane(backmodel, glm::vec3(0.5f, 0.3f, 0.5f));
}

void DrawleftWall() {
    glm::mat4 leftmodel(1.0f);

    leftmodel = glm::translate(leftmodel, glm::vec3(-2.0f, 2.0f, 0.0f));
    leftmodel = glm::rotate(leftmodel, glm::radians(-90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    leftmodel = glm::scale(leftmodel, glm::vec3(4.0f, 1.0f, 4.0f));

    DrawPlane(leftmodel, glm::vec3(0.3f, 0.3f, 0.5f));
}

void DrawrightWall() {
    glm::mat4 rightmodel(1.0f);

    rightmodel = glm::translate(rightmodel, glm::vec3(2.0f, 2.0f, 0.0f));
    rightmodel = glm::rotate(rightmodel, glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));
    rightmodel = glm::scale(rightmodel, glm::vec3(4.0f, 1.0f, 4.0f));

    DrawPlane(rightmodel, glm::vec3(0.3f, 0.5f, 0.3f));
}

void DrawCeling() {
    glm::mat4 celingmodel(1.0f);

    celingmodel = glm::translate(celingmodel, glm::vec3(0.0f, 4.0f, 0.0f));
    celingmodel = glm::rotate(celingmodel, glm::radians(-180.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    celingmodel = glm::scale(celingmodel, glm::vec3(4.0f, 1.0f, 4.0f));

    DrawPlane(celingmodel, glm::vec3(0.7f, 0.5f, 0.4f));
}

void DrawBoxes() {
    for (const Box& box : boxes) {
        glm::mat4 boxmodel = glm::translate(glm::mat4(1.0f), box.position);
        boxmodel = glm::scale(boxmodel, glm::vec3(box.size));
        DrawCube(boxmodel, glm::vec3(1.0f, 0.0f, 0.0f));
    }
}

void DrawBall() {
    for (int i = 0;i < ballcount;i++) {
        glm::mat4 ballmodel(1.0f);

        ballmodel = glm::translate(ballmodel, ball[i].position);
        ballmodel = glm::scale(ballmodel, glm::vec3(0.4f, 0.4f, 0.4f));

        DrawSphere(ballmodel, glm::vec3(0.0f, 1.0f, 0.0f));
    }
}

void DrawScene() {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    if (width <= 0 || height <= 0) return;
    glViewport(0, 0, width, height);
    glClearColor(0.08f, 0.08f, 0.10f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);

    glm::mat4 view = glm::lookAt(cameraPosition, cameraTarget,
        glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
        static_cast<float>(width) / height, 0.1f, 100.0f);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

    Zmove = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, moveZ));

    Yrotate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f));
    Yrotate = glm::rotate(Yrotate, glm::radians(rotateY), glm::vec3(0.0f, 1.0f, 0.0f));
    Yrotate = glm::translate(Yrotate, glm::vec3(0.0f, -2.0f, 0.0f));

    Zrotate = glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 2.0f, 0.0f));
    Zrotate = glm::rotate(Zrotate, glm::radians(rotateZ), glm::vec3(0.0f, 0.0f, 1.0f));
    Zrotate = glm::translate(Zrotate, glm::vec3(0.0f, -2.0f, 0.0f));

    // TODO: 평면으로 공간을 만들고, 공과 육면체 그리기.
    if (!floorOpen) Drawfloor();
    DrawBackWall();
    DrawleftWall();
    DrawrightWall();
    DrawCeling();
    DrawBoxes();
    DrawBall();
}

void Cleanup() {
    glDeleteBuffers(1, &sphereebo);
    glDeleteBuffers(1, &spherevbo);
    glDeleteVertexArrays(1, &spherevao);
    glDeleteBuffers(1, &planeebo);
    glDeleteBuffers(1, &planevbo);
    glDeleteVertexArrays(1, &planevao);
    glDeleteBuffers(1, &cubeebo);
    glDeleteBuffers(1, &cubevbo);
    glDeleteVertexArrays(1, &cubevao);
    glDeleteProgram(shaderProgramID);
    glfwDestroyWindow(window);
    glfwTerminate();
}

int main() {
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(1200, 900, "Practice 21", nullptr, nullptr);
    if (!window) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE);
    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);

    ObjData cube, sphere;
    if (!InitShaderProgram() || !Loadobj("cube.obj", cube) ||
        !Loadobj("sphere.obj", sphere)) {
        Cleanup();
        return 1;
    }
    // cube.obj의 [0, 1] 좌표를 원점 중심 [-0.5, 0.5]로 변환.
    for (float& coordinate : cube.vertices) coordinate -= 0.5f;
    // sphere.obj is already centered at the origin with radius 0.5.
    InitSphereBuffer(sphere);
    InitCubeBuffer(cube);
    InitPlaneBuffer();
    InitScene();

    double lastTime = glfwGetTime();
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;
        glfwPollEvents();
        InputProcess();
        ProcessMouse();
        UpdateAnimation();
        DrawScene();
        glfwSwapBuffers(window);
    }
    Cleanup();
    return 0;
}
