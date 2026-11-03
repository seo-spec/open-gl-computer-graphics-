#include <GL/glew.h>
#include <GL/glfw3.h>
#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/type_ptr.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "Obj.h"
#include <cmath>

GLFWwindow* window = nullptr;
GLuint shaderProgramID = 0;
GLint modelLocation = -1, viewLocation = -1;
GLint projectionLocation = -1, colorLocation = -1;
GLuint cubevao = 0, cubevbo = 0, cubeebo = 0;
GLsizei cubeIndexCount = 0;
GLuint planevao = 0, planevbo = 0, planeebo = 0;
constexpr GLsizei planeIndexCount = 6;
float deltaTime = 0.0f;
bool isBpressed = false;
bool isStarting = false;
bool isperspective = true;
bool isPpressed = false;
bool isRpressed = false;

// 실습 19와 같은 부품 구조체. 회전 각도의 단위는 도(degree).
struct Part {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f);
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 color = glm::vec3(1.0f);
};

Part belt[2];
Part stripes[20];
Part legs[14];
Part boxes[5];
float stripeDistance[20] = {};
float boxDistance[5] = {};
constexpr float stripeRadius = 0.5f;
constexpr float stripeStraightLength = 3.0f;
constexpr float stripeCornerLength = stripeRadius * 1.57079632679f;
constexpr float stripePathLength = 2.0f * stripeStraightLength + stripeCornerLength;

glm::vec3 cameraPosition(1.0f, 4.0f, 7.0f);
glm::vec3 cameraTarget(1.0f, 0.8f, 1.0f);

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

// Follow the belt centerline and turn to match its tangent.
void UpdatePathTransform(Part& part, float distance, float height) {
    if (distance < stripeStraightLength) {
        part.position = glm::vec3(0.0f, height, 3.5f - distance);
        part.rotation.y = 0.0f;
    }
    else if (distance < stripeStraightLength + stripeCornerLength) {
        // Arc length = radius * angle (radians).
        float angle = (distance - stripeStraightLength) / stripeRadius;
        part.position = glm::vec3(
            stripeRadius - stripeRadius * std::cos(angle),
            height,
            stripeRadius - stripeRadius * std::sin(angle));
        part.rotation.y = -glm::degrees(angle);
    }
    else {
        float straightDistance = distance - stripeStraightLength - stripeCornerLength;
        part.position = glm::vec3(stripeRadius + straightDistance, height, 0.0f);
        part.rotation.y = -90.0f;
    }
}

void UpdateStripeTransform(int i) {
    UpdatePathTransform(stripes[i], stripeDistance[i], 1.01f);
}

void InitScene() {
    // TODO: 바닥, 벨트, 다리, 상자의 초기 상태를 함께 작성.
    belt[0].position = glm::vec3(0.0f, 1.0f, 2.0f);
    belt[0].scale = glm::vec3(1.0f, 1.0f, 3.0f);
    belt[0].color = glm::vec3(0.2f, 0.25f, 0.35f);
    belt[1].position = glm::vec3(1.5f, 1.0f, 0.0f);
    belt[1].scale = glm::vec3(4.0f, 1.0f, 1.0f);
    belt[1].color = glm::vec3(0.2f, 0.25f, 0.35f);
    for (int i = 0;i < 20;i++) {
        // Leave a side margin so neighboring stripes do not meet at the inner turn.
        stripes[i].scale = glm::vec3(0.8f, 0.02f, 0.05f);
        stripes[i].color = glm::vec3(0.0f);
        stripeDistance[i] = (i + 0.5f) * stripePathLength / 20.0f;
        UpdateStripeTransform(i);
    }

    legs[0].position = glm::vec3(-0.4f, 0.74f, 3.4f);
    legs[0].scale = glm::vec3(0.1f, 0.5f, 0.1f);
    legs[0].color = glm::vec3(0.0f);

    legs[1].position = glm::vec3(0.4f, 0.74f, 3.4f);
    legs[1].scale = glm::vec3(0.1f, 0.5f, 0.1f);
    legs[1].color = glm::vec3(0.0f);

    legs[2].position = glm::vec3(-0.4f, 0.74f, 2.4f);
    legs[2].scale = glm::vec3(0.1f, 0.5f, 0.1f);
    legs[2].color = glm::vec3(0.0f);

    legs[3].position = glm::vec3(0.4f, 0.74f, 2.4f);
    legs[3].scale = glm::vec3(0.1f, 0.5f, 0.1f);
    legs[3].color = glm::vec3(0.0f);

    legs[4].position = glm::vec3(-0.4f, 0.74f, 1.4f);
    legs[4].scale = glm::vec3(0.1f, 0.5f, 0.1f);
    legs[4].color = glm::vec3(0.0f);

    legs[5].position = glm::vec3(0.4f, 0.74f, 1.4f);
    legs[5].scale = glm::vec3(0.1f, 0.5f, 0.1f);
    legs[5].color = glm::vec3(0.0f);

    legs[6].position = glm::vec3(-0.4f, 0.74f, -0.4f);
    legs[6].scale = glm::vec3(0.1f, 0.5f, 0.1f);
    legs[6].color = glm::vec3(0.0f);

    legs[7].position = glm::vec3(0.4f, 0.74f,0.4f);
    legs[7].scale = glm::vec3(0.1f, 0.5f, 0.1f);
    legs[7].color = glm::vec3(0.0f);

    legs[8].position = glm::vec3(1.4f, 0.74f, -0.4f);
    legs[8].scale = glm::vec3(0.1f, 0.5f, 0.1f);
    legs[8].color = glm::vec3(0.0f);

    legs[9].position = glm::vec3(1.4f, 0.74f, 0.4f);
    legs[9].scale = glm::vec3(0.1f, 0.5f, 0.1f);
    legs[9].color = glm::vec3(0.0f);

    legs[10].position = glm::vec3(2.4f, 0.74f, -0.4f);
    legs[10].scale = glm::vec3(0.1f, 0.5f, 0.1f);
    legs[10].color = glm::vec3(0.0f);

    legs[11].position = glm::vec3(2.4f, 0.74f, 0.4f);
    legs[11].scale = glm::vec3(0.1f, 0.5f, 0.1f);
    legs[11].color = glm::vec3(0.0f);

    legs[12].position = glm::vec3(3.4f, 0.74f, -0.4f);
    legs[12].scale = glm::vec3(0.1f, 0.5f, 0.1f);
    legs[12].color = glm::vec3(0.0f);

    legs[13].position = glm::vec3(3.4f, 0.74f, 0.4f);
    legs[13].scale = glm::vec3(0.1f, 0.5f, 0.1f);
    legs[13].color = glm::vec3(0.0f);

    boxes[0].position = glm::vec3(0.0f, 1.2f, 3.1f);
    boxes[0].scale = glm::vec3(0.4f, 0.4f, 0.4f);
    boxes[0].color = glm::vec3(0.0f, 1.0f, 0.0f);

    boxes[1].position = glm::vec3(0.0f, 1.2f, 2.0f);
    boxes[1].scale = glm::vec3(0.4f, 0.4f, 0.4f);
    boxes[1].color = glm::vec3(1.0f, 0.0f, 0.0f);

    boxes[2].position = glm::vec3(0.0f, 1.2f, 0.9f);
    boxes[2].scale = glm::vec3(0.4f, 0.4f, 0.4f);
    boxes[2].color = glm::vec3(0.0f, 0.0f, 1.0f);

    boxes[3].position = glm::vec3(1.1f, 1.2f, 0.0f);
    boxes[3].scale = glm::vec3(0.4f, 0.4f, 0.4f);
    boxes[3].color = glm::vec3(1.0f, 0.5f, 0.0f);

    boxes[4].position = glm::vec3(2.2f, 1.2f, 0.0f);
    boxes[4].scale = glm::vec3(0.4f, 0.4f, 0.4f);
    boxes[4].color = glm::vec3(1.0f, 1.0f, 0.0f);

    // Convert the initial positions to distances along the shared belt path.
    for (int i = 0; i < 5; ++i) {
        boxDistance[i] = boxes[i].position.z > 0.0f
            ? 3.5f - boxes[i].position.z
            : stripeStraightLength + stripeCornerLength
                + boxes[i].position.x - stripeRadius;
        UpdatePathTransform(boxes[i], boxDistance[i], 1.2f);
    }
}

void ResetScene() {
    // Restore CPU-side scene data; the existing GPU buffers can be reused.
    for (Part& part : belt) part = Part{};
    for (Part& part : stripes) part = Part{};
    for (Part& part : legs) part = Part{};
    for (Part& part : boxes) part = Part{};
    InitScene();

    cameraPosition = glm::vec3(1.0f, 4.0f, 7.0f);
    cameraTarget = glm::vec3(1.0f, 0.8f, 1.0f);
    isStarting = false;
    isperspective = true;
}

void InputProcess() {
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        if (!isRpressed) {
            ResetScene();
        }
        isRpressed = true;
        // Held keys must not immediately toggle the restored state.
        isBpressed = glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS;
        isPpressed = glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS;
        return;
    }
    else {
        isRpressed = false;
    }
    // TODO: 벨트 시작/정지, 카메라 조작, 투영 전환, 리셋.
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        if (!isBpressed) {
            isStarting = !isStarting;
        }
        isBpressed = true;
    }
    else {
        isBpressed = false;
    }
    if(glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        if (!isPpressed) {
            isperspective = !isperspective;
        }
        isPpressed = true;
    }
    else {
        isPpressed = false;
    }
    float cameraRadians = 0.0f;
    glm::vec3 TargetCamera = cameraPosition - cameraTarget ;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        cameraRadians = -45.0f * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        cameraRadians = 45.0f * deltaTime;
    }
    glm::mat4 cameraRotation = glm::rotate(
        glm::mat4(1.0f),
        glm::radians(cameraRadians),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );

    TargetCamera = glm::vec3(
        cameraRotation * glm::vec4(TargetCamera, 0.0f)
    );

    cameraPosition = cameraTarget + TargetCamera;
    glm::vec3 toTarget = cameraTarget - cameraPosition;
    float cameraDistance = glm::length(toTarget);
    if (cameraDistance > 0.0f) {
        glm::vec3 zoomDirection = glm::normalize(toTarget);
        float zoomStep = 3.0f * deltaTime;
        bool shiftPressed =
            glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;

        // Main keyboard + (Shift and =) or numeric keypad +.
        if ((glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS && shiftPressed) ||
            glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
            float move = glm::min(zoomStep, glm::max(cameraDistance - 1.0f, 0.0f));
            cameraPosition += zoomDirection * move;
        }
        if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
            cameraPosition -= zoomDirection * zoomStep;
        }
    }
}

void UpdateAnimation() {
    // TODO: deltaTime을 사용한 벨트와 상자 이동.
    if (isStarting) {
        for (int i = 0;i < 5;i++) {
            boxDistance[i] = std::fmod(
                boxDistance[i] + 0.5f * deltaTime, stripePathLength);
            UpdatePathTransform(boxes[i], boxDistance[i], 1.2f);
        }
        for (int i = 0;i < 20;i++) {
            stripeDistance[i] = std::fmod(
                stripeDistance[i] + 0.5f * deltaTime, stripePathLength);
            UpdateStripeTransform(i);
        }
    }
}

void DrawCube(const glm::mat4& model, const glm::vec3& color) {
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(colorLocation, 1, glm::value_ptr(color));
    glBindVertexArray(cubevao);
    glDrawElements(GL_TRIANGLES, cubeIndexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void DrawPlane(const glm::mat4& model, const glm::vec3& color) {
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(colorLocation, 1, glm::value_ptr(color));
    glBindVertexArray(planevao);
    glDrawElements(GL_TRIANGLES, planeIndexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void DrawBelt() {
    for (int i = 0;i < 2;i++) {
        glm::mat4 model(1.0f);
        model = glm::translate(model, belt[i].position);
         model = glm::scale(model, belt[i].scale);
        DrawPlane(model, belt[i].color);
    }
}

void DrawBoxNumber(const glm::mat4& boxmodel, int number) {
    // 3 x 5 digit patterns. Each filled cell is a thin black cube on the lid.
    const char* digits[5][5] = {
        { "010", "110", "010", "010", "111" },
        { "111", "001", "111", "100", "111" },
        { "111", "001", "111", "001", "111" },
        { "101", "101", "111", "001", "001" },
        { "111", "100", "111", "001", "111" }
    };
    for (int row = 0; row < 5; row++) {
        for (int col = 0; col < 3; col++) {
            if (digits[number - 1][row][col] == '1') {
                glm::mat4 numbermodel = boxmodel;
                numbermodel = glm::translate(numbermodel,
                    glm::vec3((col - 1) * 0.16f, 0.51f, (row - 2) * 0.14f));
                numbermodel = glm::scale(numbermodel, glm::vec3(0.14f, 0.01f, 0.12f));
                DrawCube(numbermodel, glm::vec3(0.0f));
            }
        }
    }
}

void DrawBox() {
    for (int i = 0;i < 5;i++) {
        glm::mat4 boxmodel(1.0f);
        boxmodel = glm::translate(boxmodel, boxes[i].position);
        boxmodel = glm::rotate(boxmodel, glm::radians(boxes[i].rotation.y),
            glm::vec3(0.0f, 1.0f, 0.0f));
        boxmodel = glm::scale(boxmodel, boxes[i].scale);
        DrawCube(boxmodel, boxes[i].color);
        DrawBoxNumber(boxmodel, i + 1);
    }
}

void DrawFloor() {
    // TODO: 바닥 그리기는 여기서 함께 작성.
    glm::mat4 model(1.0f);

    model = glm::translate(model, glm::vec3(0.0f, -0.1f, 0.0f));
    model = glm::scale(model, glm::vec3(30.0f, 0.2f, 30.0f));

    DrawCube(model, glm::vec3(0.28f, 0.31f, 0.28f));
}

void DrawStripe() {
    for (int i = 0;i < 20;i++) {
        glm::mat4 stripemodel(1.0f);

        stripemodel = glm::translate(stripemodel, stripes[i].position);
        stripemodel = glm::rotate(stripemodel,glm::radians(stripes[i].rotation.y),glm::vec3(0.0f, 1.0f, 0.0f));
        stripemodel = glm::scale(stripemodel, stripes[i].scale);

        DrawCube(stripemodel, stripes[i].color);
    }
}
void Drawlegs() {
    for (int i = 0;i < 14;i++) {
        glm::mat4 legmodel(1.0f);;

        legmodel = glm::translate(legmodel, legs[i].position);
        legmodel = glm::scale(legmodel, legs[i].scale);

        DrawCube(legmodel, legs[i].color);
    }
}

void DrawScene() {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    if (width <= 0 || height <= 0) return;
    glViewport(0, 0, width, height);
    glClearColor(0.60f, 0.75f, 0.89f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);

    glm::mat4 view = glm::lookAt(cameraPosition, cameraTarget,
        glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection;
    if (isperspective) {
            projection = glm::perspective(glm::radians(45.0f),
            static_cast<float>(width) / height, 0.1f, 100.0f);
    }
    else {
        float aspect = static_cast<float>(width) / height;
        float halfHeight = 4.0f;

        projection = glm::ortho(
            -halfHeight * aspect, halfHeight * aspect,
            -halfHeight, halfHeight,
            0.1f, 100.0f
        );
    }
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

    // TODO: 벨트, 다리, 상자 그리기.
    DrawFloor();
    DrawBelt();
    DrawStripe();
    Drawlegs();
    DrawBox();
}

void Cleanup() {
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
    window = glfwCreateWindow(1200, 900, "Practice 20", nullptr, nullptr);
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

    ObjData cube;
    if (!InitShaderProgram() || !Loadobj("cube.obj", cube)) {
        Cleanup();
        return 1;
    }
    // cube.obj의 [0, 1] 좌표를 원점 중심 [-0.5, 0.5]로 변환.
    for (float& coordinate : cube.vertices) coordinate -= 0.5f;
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
        UpdateAnimation();
        DrawScene();
        glfwSwapBuffers(window);
    }
    Cleanup();
    return 0;
}
