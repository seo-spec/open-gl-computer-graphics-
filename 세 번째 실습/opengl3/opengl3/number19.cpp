#include <GL/glew.h>
#include <GL/glfw3.h>
#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/type_ptr.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "Obj.h"
#include <random>

using namespace std;

GLFWwindow* window = nullptr;
GLuint shaderProgramID = 0;
GLint modelLocation = -1, viewLocation = -1;
GLint projectionLocation = -1, colorLocation = -1;
GLuint cubevao = 0, cubevbo = 0, cubeebo = 0;
GLsizei cubeIndexCount = 0;
float deltaTime = 0.0f;
random_device rd;
mt19937 gen(rd());
uniform_real_distribution<float> Colordist(0.0f,1.0f);

bool isDpressed = false;
bool islowerChange = false;
bool isLpressed = false;
bool isSwapping = false;
bool isChanged = false;
bool isYpressed = false;
bool isCameraRotating = false; // 회전 중인지
bool isRpressed = false;
bool isCameraOrbiting = false;
bool isOpressed = false;
bool isCpressed = false;

float cameraOrbitDirection = 1.0f;
float cameraRotateDirection = 1.0f;
float Changespeed = 0.1f;
int lowerStep = 0;          // 처음에는 기본 크기
float targetHeight = 0.3f; // 기본 크기 변수
float UpperMoveX[2] = { 0.0f,0.0f };


struct Part {
    glm::vec3 position = glm::vec3(0.0f);
    glm::vec3 rotation = glm::vec3(0.0f); // degrees
    glm::vec3 scale = glm::vec3(1.0f);
    glm::vec3 color = glm::vec3(1.0f);
};

Part lower[2], center, upper[2], barrel[2], flagpole[2];
Part initialParts[9]; // Startup values, including the original random colors.
glm::vec3 tankPosition(0.0f);
glm::vec3 cameraPosition(4.0f, 3.0f, 5.0f);
glm::vec3 camerDirection = glm::normalize(glm::vec3(0.0f) - cameraPosition);


bool InitShaderProgram() {
    // Reuse the model/view/projection and solid-color shaders from practice 18.
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

void InitTank() {
    // TODO 1: Set each part's position, scale, and color.
    // 하부 몸체 초기화
    lower[0].position = glm::vec3(-1.0f, 0.15f, 0.0f);
    lower[1].position = glm::vec3(1.0f, 0.15f, 0.0f);
    float r = Colordist(gen);
    float g = Colordist(gen);
    float b = Colordist(gen);
    for (int i = 0;i < 2;i++) {
        lower[i].color = glm::vec3(r, g, b);
        lower[i].scale = glm::vec3(1.0f, 0.3f, 0.8f);
    }
    //중간 몸체 초기화
    center.position = glm::vec3(0.0f, 0.45f, 0.0f);
    center.scale = glm::vec3(1.3f, 0.3f, 0.8f);
    center.color = glm::vec3(Colordist(gen), Colordist(gen), Colordist(gen));
    //상부 몸체 초기화
    upper[0].position = glm::vec3(-0.8f, 0.75f, 0.0f);
    upper[1].position = glm::vec3(0.8f, 0.75f, 0.0f);
    upper[0].color = glm::vec3(Colordist(gen), Colordist(gen), Colordist(gen));
    upper[1].color = upper[0].color;
    for (int i = 0; i < 2; i++) {
        upper[i].scale = glm::vec3(0.9f, 0.3f, 0.8f);
    }
    //포신 위치 초기화
    barrel[0].position = glm::vec3(-0.8f, 0.75f, 0.4f);
    barrel[1].position = glm::vec3(0.8f, 0.75f, 0.4f);
    barrel[0].color = glm::vec3(Colordist(gen), Colordist(gen), Colordist(gen));
    barrel[1].color = barrel[0].color;
    for (int i = 0; i < 2; i++) {
        barrel[i].scale = glm::vec3(0.1f, 0.1f, 1.5f);
    }
    flagpole[0].position = glm::vec3(-0.8f, 1.2f, 0.0f);
    flagpole[1].position = glm::vec3(0.8f, 1.2f, 0.0f);
    flagpole[0].color = glm::vec3(Colordist(gen), Colordist(gen), Colordist(gen));
    flagpole[1].color = flagpole[0].color;
    for (int i = 0;i < 2;i++) {
        flagpole[i].scale = glm::vec3(0.1f, 0.6f, 0.1f);
    }
}

void ChangeObjectColors() {
    lower[0].color = glm::vec3(Colordist(gen), Colordist(gen), Colordist(gen));
    center.color = glm::vec3(Colordist(gen), Colordist(gen), Colordist(gen));
    upper[0].color = glm::vec3(Colordist(gen), Colordist(gen), Colordist(gen));
    barrel[0].color = glm::vec3(Colordist(gen), Colordist(gen), Colordist(gen));
    flagpole[0].color = glm::vec3(Colordist(gen), Colordist(gen), Colordist(gen));
    // Keep matching parts the same color, as in InitTank().
    lower[1].color = lower[0].color;
    upper[1].color = upper[0].color;
    barrel[1].color = barrel[0].color;
    flagpole[1].color = flagpole[0].color;
}

void ResetScene() {
    for (int i = 0; i < 2; i++) {
        lower[i] = initialParts[i];
        upper[i] = initialParts[3 + i];
        barrel[i] = initialParts[5 + i];
        flagpole[i] = initialParts[7 + i];
        UpperMoveX[i] = 0.0f;
    }
    center = initialParts[2];
    tankPosition = glm::vec3(0.0f);
    cameraPosition = glm::vec3(4.0f, 3.0f, 5.0f);
    camerDirection = glm::normalize(glm::vec3(0.0f) - cameraPosition);
    lowerStep = 0;
    targetHeight = 0.3f;
    islowerChange = false;
    isSwapping = false;
    isChanged = false;
    isCameraRotating = false;
    isCameraOrbiting = false;
    cameraRotateDirection = 1.0f;
    cameraOrbitDirection = 1.0f;
}

void InputProcess() {
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        if (!isOpressed) {
            ChangeObjectColors();
        }
        isOpressed = true;
    }
    else {
        isOpressed = false;
    }

    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (!isCpressed) {
            ResetScene();
        }
        isCpressed = true;
        // Do not apply other held commands during a reset.
        return;
    }
    else {
        isCpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS)
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    // TODO 4: Tank movement and animation toggles.
    // 상하좌우 방향키 처리
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if (!isDpressed) {
            lowerStep = (lowerStep + 1) % 4;

            if (lowerStep == 1) {
                targetHeight = 0.6f;   // 확대
            }
            else if (lowerStep == 3) {
                targetHeight = 0.2f;   // 축소
            }
            else {
                targetHeight = 0.3f;   // 기본 복귀
            }

            islowerChange = true;
        }
        isDpressed = true;
    }
    else {
        isDpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        if (!isLpressed) {
            isSwapping = true;
        }
        isLpressed = true;
    }
    else {
        isLpressed = false;
    }
    float moveSpeed = 2.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        bool shiftPressed =
            glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
        if (shiftPressed) {
            cameraPosition.z -= moveSpeed;
        }
        else {
            cameraPosition.z += moveSpeed;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        bool shiftPressed =
            glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
            glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;
        if (shiftPressed) {
            cameraPosition.x -= moveSpeed;
        }
        else {
            cameraPosition.x += moveSpeed;
        }
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        if (!isYpressed) {
            bool shiftPressed =
                glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;

            float direction = shiftPressed ? 1.0f : -1.0f;

            if (isCameraRotating && cameraRotateDirection == direction) {
                isCameraRotating = false;
            }
            else {
                cameraRotateDirection = direction;
                isCameraRotating = true;
            }
        }
        isYpressed = true;
    }
    else {
        isYpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        if (!isRpressed) {
            bool shiftPressed =
                glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS ||
                glfwGetKey(window, GLFW_KEY_RIGHT_SHIFT) == GLFW_PRESS;

            float direction = shiftPressed ? 1.0f : -1.0f;

            if (isCameraOrbiting && cameraOrbitDirection == direction) {
                isCameraOrbiting = false;
            }
            else {
                cameraOrbitDirection = direction;
                isCameraOrbiting = true;
            }
        }
        isRpressed = true;
    }
    else {
        isRpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        barrel[0].rotation.y -= moveSpeed * 25.0f;
        barrel[1].rotation.y += moveSpeed * 25.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        flagpole[0].rotation.x -= moveSpeed * 25.0f;
        flagpole[1].rotation.x += moveSpeed * 25.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        center.rotation.y += moveSpeed*25.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS)
    {
        tankPosition.z += moveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS)
    {
        tankPosition.z -= moveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS)
    {
        tankPosition.x -= moveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS)
    {
        tankPosition.x += moveSpeed;
    }
}

void UpdateAnimation() {
    // TODO 5: Update positions/angles using speed * deltaTime.
    float step = Changespeed * deltaTime;
    if (islowerChange) {
        for (int i = 0; i < 2; i++) {
            if (lower[i].scale.y < targetHeight) {
                lower[i].scale.y =
                    glm::min(lower[i].scale.y + step, targetHeight);
            }
            else if (lower[i].scale.y > targetHeight) {
                lower[i].scale.y =
                    glm::max(lower[i].scale.y - step, targetHeight);
            }
        }

        if (lower[0].scale.y == targetHeight &&
            lower[1].scale.y == targetHeight) {
            islowerChange = false;
        }
    }
    if (isSwapping) {
        float step = 0.3f * deltaTime;
        if (!isChanged) {
            UpperMoveX[0] = glm::min(UpperMoveX[0] + step, 1.6f);
            UpperMoveX[1] = glm::max(UpperMoveX[1] - step, -1.6f);

            if (UpperMoveX[0] == 1.6f && UpperMoveX[1] == -1.6f) {
                isSwapping = false;
                isChanged = true;
            }
        }
        else {
            UpperMoveX[0] = glm::max(UpperMoveX[0] - step, 0.0f);
            UpperMoveX[1] = glm::min(UpperMoveX[1] + step, 0.0f);

            if (UpperMoveX[1] == 0.0f && UpperMoveX[0] == 0.0f) {
                isSwapping = false;
                isChanged = false;
            }
        }
    }
    if (isCameraRotating) {
        float angle = glm::radians(45.0f * cameraRotateDirection * deltaTime);

        glm::mat4 rotation = glm::rotate(glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));

        camerDirection = glm::normalize(glm::vec3(rotation * glm::vec4(camerDirection, 0.0f)));
    }
    if (isCameraOrbiting) {
        float angle = glm::radians(45.0f * cameraOrbitDirection * deltaTime);

        glm::mat4 rotation = glm::rotate(
            glm::mat4(1.0f), angle, glm::vec3(0.0f, 1.0f, 0.0f));
        // Orbit around the world origin: rotate both position and view direction.
        cameraPosition = glm::vec3(rotation * glm::vec4(cameraPosition, 1.0f));
        camerDirection = glm::normalize(glm::vec3(
            rotation * glm::vec4(camerDirection, 0.0f)));
    }
}

// Pass a finished model matrix, including the part's scale.
void DrawCube(const glm::mat4& model, const glm::vec3& color) {
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
    glUniform3fv(colorLocation, 1, glm::value_ptr(color));
    glBindVertexArray(cubevao);
    glDrawElements(GL_TRIANGLES, cubeIndexCount, GL_UNSIGNED_INT, nullptr);
    glBindVertexArray(0);
}

void DrawFloor() {
    // TODO 2: Draw the floor on the XZ plane.
    glm::mat4 model(1.0f);

    model = glm::translate(model,glm::vec3(0.0f, -0.1f, 0.0f));
    model = glm::scale(model, glm::vec3(6.0f, 0.2f, 6.0f));

    DrawCube(model, glm::vec3(0.5f, 0.5f, 0.5f));
}

void DrawTank() {
    // TODO 3: Draw the nine parts, inheriting parent translation/rotation.
    // Keep the parent's shape scale out of the child's coordinate frame.
    glm::mat4 tankmodel(1.0f);
    //하부
    tankmodel = translate(tankmodel, tankPosition);
    for (int i = 0;i < 2;i++) {
        glm::mat4 lowermodel = tankmodel;
        lowermodel = glm::translate(lowermodel, lower[i].position);
        lowermodel = glm::scale(lowermodel, lower[i].scale);
        DrawCube(lowermodel, lower[i].color);
    }
    //중앙
    tankmodel = glm::translate(tankmodel, glm::vec3(0.0f, 0.45f, 0.0f));
    tankmodel = glm::rotate(tankmodel,glm::radians(center.rotation.y),glm::vec3(0.0f, 1.0f, 0.0f));
    tankmodel = glm::translate(tankmodel, glm::vec3(0.0f, -0.45f, 0.0f));
    glm::mat4 centermodel = tankmodel;
    centermodel = glm::translate(centermodel, center.position);
    centermodel = glm::scale(centermodel, center.scale);
    DrawCube(centermodel, center.color);
    //상부
    for (int i = 0;i < 2;i++) {

        glm::mat4 uppermodel = tankmodel;
        uppermodel = glm::translate(uppermodel, glm::vec3(UpperMoveX[i], 0.0f, 0.0f));
        uppermodel = glm::translate(uppermodel, upper[i].position);
        uppermodel = glm::scale(uppermodel, upper[i].scale);
        DrawCube(uppermodel, upper[i].color);
    }
    //포신
    for (int i = 0;i < 2;i++) {
        glm::mat4 barrelmodel = tankmodel;
        //상부 교환
        barrelmodel = glm::translate(barrelmodel, glm::vec3(UpperMoveX[i], 0.0f, 0.0f));
        // 포신만 이동
        barrelmodel = glm::translate(barrelmodel, upper[i].position);
        barrelmodel = glm::rotate(barrelmodel, glm::radians(barrel[i].rotation.y),glm::vec3(0.0f, 1.0f, 0.0f));
        barrelmodel = glm::translate(barrelmodel, -upper[i].position);
        //포신 상대좌표 이동
        barrelmodel = glm::translate(barrelmodel, barrel[i].position);
        barrelmodel = glm::scale(barrelmodel, barrel[i].scale);
        DrawCube(barrelmodel, barrel[i].color);
    }
    //깃발
    for (int i = 0;i < 2;i++) {
        glm::mat4 flagmodel = tankmodel;
        flagmodel = glm::translate(flagmodel, glm::vec3(UpperMoveX[i], 0.0f, 0.0f));
        // 깃발 이동
        glm::vec3 pivot = upper[i].position +
            glm::vec3(0.0f, upper[i].scale.y / 2.0f, 0.0f);
        flagmodel = glm::translate(flagmodel, pivot);
        flagmodel = glm::rotate(flagmodel, glm::radians(flagpole[i].rotation.x), glm::vec3(1.0f, 0.0f, 0.0f));
        flagmodel = glm::translate(flagmodel, -pivot);

        flagmodel = glm::translate(flagmodel, flagpole[i].position);
        flagmodel = glm::scale(flagmodel, flagpole[i].scale);
        DrawCube(flagmodel, flagpole[i].color);
    }
}

void DrawScene() {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    if (width <= 0 || height <= 0) return;
    glViewport(0, 0, width, height);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);
    glm::mat4 view = glm::lookAt(cameraPosition, cameraPosition+camerDirection,
        glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 projection = glm::perspective(glm::radians(45.0f),
        static_cast<float>(width) / height, 0.1f, 100.0f);
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));
    DrawFloor();
    DrawTank();
}

int main() {
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(1200, 900, "Practice 19", nullptr, nullptr);
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
        if (shaderProgramID) glDeleteProgram(shaderProgramID);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    // cube.obj uses [0, 1]; center it at the origin for easier transforms.
    for (float& coordinate : cube.vertices) coordinate -= 0.5f;
    InitCubeBuffer(cube);
    InitTank();
    for (int i = 0; i < 2; i++) {
        initialParts[i] = lower[i];
        initialParts[3 + i] = upper[i];
        initialParts[5 + i] = barrel[i];
        initialParts[7 + i] = flagpole[i];
    }
    initialParts[2] = center;
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
    glDeleteBuffers(1, &cubeebo);
    glDeleteBuffers(1, &cubevbo);
    glDeleteVertexArrays(1, &cubevao);
    glDeleteProgram(shaderProgramID);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
