#include <GL/glew.h>
#include <GL/glfw3.h>
#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/type_ptr.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "Obj.h"

GLFWwindow* window = nullptr;
GLuint shaderProgramID = 0;
GLint modelLocation = -1;
GLint colorLocation = -1;
GLuint cubevao = 0;
GLuint cubevbo = 0;
GLuint cubeebo = 0;
GLuint spherevao = 0;
GLuint spherevbo = 0;
GLuint sphereebo = 0;
GLuint axisvao;//좌표계
GLuint axisvbo;//좌표계
GLsizei cubeIndexCount = 0;
GLsizei sphereIndexCount = 0;

struct MeshBuffer {
    GLuint vao = 0;
    GLuint vbo = 0;
    GLuint ebo = 0;
    GLsizei indexCount = 0;
};
MeshBuffer pyramidMesh;
MeshBuffer coneMesh;
bool useAlternateShapes = false;
bool isCpressed = false;

struct ObjectState {
    explicit ObjectState(float initialScale) : scale(initialScale) {}

    bool visible = true;
    bool rotatingX = false;
    bool rotatingY = false;
    bool orbitingY = false;
    bool scaling = false;
    bool depthScaling = false; // V: 앞뒤 위치에 따른 반복 신축
    bool originScaling = false;
    bool movingX = false;
    bool movingY = false;
    float rotationX = 0.0f;
    float rotationY = 0.0f;
    float rotateSpeedX = 45.0f;
    float rotateSpeedY = 45.0f;
    float orbitAngleY = 0.0f;
    float orbitSpeed = 180.0f;
    float scale;
    float scaleSpeed = 0.1f;
    float originScale = 1.0f;
    float originSpeed = 0.1f;
    float moveX = 0.0f;
    float moveY = 0.0f;
    float moveZ = 0.0f;
    float moveSpeedX = 0.3f;
    float moveSpeedY = 0.3f;
};

ObjectState cube(0.3f);
ObjectState sphere(0.4f);
int selectedObject = 3; // 1: left cube, 2: right sphere, 3: both

bool isXpressed = false;
bool isSpressed = false;
bool isYpressed = false;
bool isRpressed = false;
bool isApressed = false;
bool isBpressed = false;
bool isDpressed = false;
bool isEpressed = false;
bool isTpressed = false;
bool isSwapping = false;
bool isUpressed = false;
bool isVpressed = false;
bool swapViaOffset = false; // false: T 원점 경유, true: U 위/아래 경유

glm::vec3 cubeSwapStart(0.0f);
glm::vec3 sphereSwapStart(0.0f);

float deltaTime = 0.0f;
float swapProgress = 0.0f;
float swapDuration = 2.0f; // 전체 자리 교환에 걸리는 시간(초)

bool InitShaderProgram() {
    shaderProgramID = InitShader("vertex.glsl", "fragment.glsl");
    if (shaderProgramID == 0) return false;

    modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
    colorLocation = glGetUniformLocation(shaderProgramID, "faceColor");
    return true;
}

void InitMeshBuffer(const ObjData& obj, MeshBuffer& mesh) {
    glGenVertexArrays(1, &mesh.vao);
    glBindVertexArray(mesh.vao);
    glGenBuffers(1, &mesh.vbo);
    glBindBuffer(GL_ARRAY_BUFFER, mesh.vbo);
    glBufferData(GL_ARRAY_BUFFER, obj.vertices.size() * sizeof(float),
        obj.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    glGenBuffers(1, &mesh.ebo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj.indices.size() * sizeof(unsigned int),
        obj.indices.data(), GL_STATIC_DRAW);
    mesh.indexCount = static_cast<GLsizei>(obj.indices.size());
    glBindVertexArray(0);
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

void InitaxisBuffer() {
    float axispos[] = {
        -1.0f,0.0f,0.0f,
        1.0f,0.0f,0.0f,
        0.0f,-1.0f,0.0f,
        0.0f,1.0f,0.0f,
        0.0f,0.0f,-1.0f,
        0.0f,0.0f,1.0f
    };

    glGenVertexArrays(1, &axisvao);//정점 배열준비
    glBindVertexArray(axisvao);// 버퍼 선택
    glGenBuffers(1, &axisvbo); // 버퍼 이름 생성
    glBindBuffer(GL_ARRAY_BUFFER, axisvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(axispos), axispos, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    //인자 순서 - 셰이더의 위치, 정점 하나당 x,y,z 세 값, 값의 자료형, 정규화 할 것인지, 정점이 빈틈없이 배치, 버퍼 읽는 위치
    glEnableVertexAttribArray(0); // 설정한 0번 속성 활성화

}

void InitShape() {
    cube = ObjectState(0.3f);
    sphere = ObjectState(0.4f);
    isSwapping = false;
    swapProgress = 0.0f;
    swapViaOffset = false;
    useAlternateShapes = false;
}
void InputProcess() {
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (!isCpressed) useAlternateShapes = !useAlternateShapes;
        isCpressed = true;
    }
    else {
        isCpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) selectedObject = 1;
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) selectedObject = 2;
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) selectedObject = 3;

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (!isSpressed) InitShape();
        isSpressed = true;
    }
    else {
        isSpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        if (!isXpressed) {
            auto applyCommand = [](ObjectState& object) { object.rotatingX = true; object.rotateSpeedX = -object.rotateSpeedX; };
            if (selectedObject == 1 || selectedObject == 3) applyCommand(cube);
            if (selectedObject == 2 || selectedObject == 3) applyCommand(sphere);
        }
        isXpressed = true;
    }
    else {
        isXpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        if (!isYpressed) {
            auto applyCommand = [](ObjectState& object) { object.rotatingY = true; object.rotateSpeedY = -object.rotateSpeedY; };
            if (selectedObject == 1 || selectedObject == 3) applyCommand(cube);
            if (selectedObject == 2 || selectedObject == 3) applyCommand(sphere);
        }
        isYpressed = true;
    }
    else {
        isYpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        if (!isRpressed) {
            auto applyCommand = [](ObjectState& object) { object.orbitingY = true; object.orbitSpeed = -object.orbitSpeed; };
            if (selectedObject == 1 || selectedObject == 3) applyCommand(cube);
            if (selectedObject == 2 || selectedObject == 3) applyCommand(sphere);
        }
        isRpressed = true;
    }
    else {
        isRpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        if (!isApressed) {
            auto applyCommand = [](ObjectState& object) { object.scaling = true; };
            if (selectedObject == 1 || selectedObject == 3) applyCommand(cube);
            if (selectedObject == 2 || selectedObject == 3) applyCommand(sphere);
        }
        isApressed = true;
    }
    else {
        isApressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        if (!isBpressed) {
            auto applyCommand = [](ObjectState& object) { object.originScaling = true; };
            if (selectedObject == 1 || selectedObject == 3) applyCommand(cube);
            if (selectedObject == 2 || selectedObject == 3) applyCommand(sphere);
        }
        isBpressed = true;
    }
    else {
        isBpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if (!isDpressed) {
            auto applyCommand = [](ObjectState& object) { object.moveSpeedX = -object.moveSpeedX; object.movingX = true; };
            if (selectedObject == 1 || selectedObject == 3) applyCommand(cube);
            if (selectedObject == 2 || selectedObject == 3) applyCommand(sphere);
        }
        isDpressed = true;
    }
    else {
        isDpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        if (!isEpressed) {
            auto applyCommand = [](ObjectState& object) { object.moveSpeedY = -object.moveSpeedY; object.movingY = true; };
            if (selectedObject == 1 || selectedObject == 3) applyCommand(cube);
            if (selectedObject == 2 || selectedObject == 3) applyCommand(sphere);
        }
        isEpressed = true;
    }
    else {
        isEpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        if (!isTpressed) {
            swapViaOffset = false;
            swapProgress = 0.0f;
            isSwapping = true;
            cubeSwapStart = glm::vec3(
                -0.6f + cube.moveX, 0.15f + cube.moveY, 0.15f + cube.moveZ);

            sphereSwapStart = glm::vec3(
                0.45f + sphere.moveX, 0.20f + sphere.moveY, 0.20f + sphere.moveZ);
        }
        isTpressed = true;
    }
    else {
        isTpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        if (!isUpressed) {
            swapViaOffset = true;
            swapProgress = 0.0f;
            isSwapping = true;
            cubeSwapStart = glm::vec3(
                -0.6f + cube.moveX, 0.15f + cube.moveY, 0.15f + cube.moveZ);
            sphereSwapStart = glm::vec3(
                0.45f + sphere.moveX, 0.20f + sphere.moveY, 0.20f + sphere.moveZ);
        }
        isUpressed = true;
    }
    else {
        isUpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        if (!isVpressed) {
            // V는 선택과 관계없이 두 도형에 함께 적용한다.
            cube.scaling = false;
            sphere.scaling = false;
            cube.depthScaling = true;
            sphere.depthScaling = true;
            cube.rotatingY = true;
            sphere.rotatingY = true;
            cube.orbitingY = true;
            sphere.orbitingY = true;
        }
        isVpressed = true;
    }
    else {
        isVpressed = false;
    }
}

void UpdateAnimation() {
    if (cube.rotatingX) {
        cube.rotationX += cube.rotateSpeedX * deltaTime;
    }
    if (sphere.rotatingX) {
        sphere.rotationX += sphere.rotateSpeedX * deltaTime;
    }
    if (cube.rotatingY) {
        cube.rotationY += cube.rotateSpeedY * deltaTime;
    }
    if (sphere.rotatingY) {
        sphere.rotationY += sphere.rotateSpeedY * deltaTime;
    }
    if (cube.orbitingY) {
        cube.orbitAngleY += cube.orbitSpeed * deltaTime;
    }
    if (sphere.orbitingY) {
        sphere.orbitAngleY += sphere.orbitSpeed * deltaTime;
    }
    if (cube.scaling) {
        cube.scale += cube.scaleSpeed * deltaTime;
        if (cube.scaleSpeed > 0.0f && cube.scale >= 1.0f) {
            cube.scale = 1.0f;
            cube.scaling = false;
            cube.scaleSpeed = -cube.scaleSpeed;
        }
        else if (cube.scaleSpeed < 0.0f && cube.scale <= 0.2f) {
            cube.scale = 0.2f;
            cube.scaling = false;
            cube.scaleSpeed = -cube.scaleSpeed;
        }
    }
    if (sphere.scaling) {
        sphere.scale += sphere.scaleSpeed * deltaTime;
        if (sphere.scaleSpeed > 0.0f && sphere.scale >= 1.0f) {
            sphere.scale = 1.0f;
            sphere.scaling = false;
            sphere.scaleSpeed = -sphere.scaleSpeed;
        }
        else if (sphere.scaleSpeed < 0.0f && sphere.scale <= 0.2f) {
            sphere.scale = 0.2f;
            sphere.scaling = false;
            sphere.scaleSpeed = -sphere.scaleSpeed;
        }
    }
    if (cube.originScaling) {
        cube.originScale += cube.originSpeed * deltaTime;
        if (cube.originSpeed > 0.0f && cube.originScale >= 1.4f) {
            cube.originScale = 1.4f;
            cube.originScaling = false;
            cube.originSpeed = -cube.originSpeed;
        }
        else if (cube.originSpeed < 0.0f && cube.originScale <= 0.2f) {
            cube.originScale = 0.2f;
            cube.originScaling = false;
            cube.originSpeed = -cube.originSpeed;
        }
    }
    if (sphere.originScaling) {
        sphere.originScale += sphere.originSpeed * deltaTime;
        if (sphere.originSpeed > 0.0f && sphere.originScale >= 1.4f) {
            sphere.originScale = 1.4f;
            sphere.originScaling = false;
            sphere.originSpeed = -sphere.originSpeed;
        }
        else if (sphere.originSpeed < 0.0f && sphere.originScale <= 0.2f) {
            sphere.originScale = 0.2f;
            sphere.originScaling = false;
            sphere.originSpeed = -sphere.originSpeed;
        }
    }
    if (cube.movingX) {
        cube.moveX += cube.moveSpeedX * deltaTime;
        float halfSize = cube.scale * 0.5f;
        float centerX = -0.6f + cube.moveX;
        float leftLimit = -1.0f + halfSize;
        float rightLimit = 1.0f - halfSize;

        if (cube.moveSpeedX < 0.0f && centerX <= leftLimit) {
            cube.moveX = leftLimit - (-0.6f);
            cube.movingX = false;
        }
        else if (cube.moveSpeedX > 0.0f && centerX >= rightLimit) {
            cube.moveX = rightLimit - (-0.6f);
            cube.movingX = false;
        }

    }
    if (sphere.movingX) {
        sphere.moveX += sphere.moveSpeedX * deltaTime;
        float halfSize = sphere.scale * 0.5f;
        float centerX = 0.45f + sphere.moveX;
        float leftLimit = -1.0f + halfSize;
        float rightLimit = 1.0f - halfSize;

        if (sphere.moveSpeedX < 0.0f && centerX <= leftLimit) {
            sphere.moveX = leftLimit - 0.45f;
            sphere.movingX = false;
        }
        else if (sphere.moveSpeedX > 0.0f && centerX >= rightLimit) {
            sphere.moveX = rightLimit - 0.45f;
            sphere.movingX = false;
        }
    }
    if (cube.movingY) {
        cube.moveY += cube.moveSpeedY * deltaTime;
        float halfSize = cube.scale * 0.5f;
        float centerY = 0.15f + cube.moveY;
        float BottomLimit = -1.0f + halfSize;
        float UpperLimit = 1.0f - halfSize;

        if (cube.moveSpeedY < 0.0f && centerY <= BottomLimit) {
            cube.moveY = BottomLimit - 0.15f;
            cube.movingY = false;
        }
        else if (cube.moveSpeedY > 0.0f && centerY >= UpperLimit) {
            cube.moveY = UpperLimit - 0.30f;
            cube.movingY = false;
        }
    }
    if (sphere.movingY) {
        sphere.moveY += sphere.moveSpeedY * deltaTime;
        float halfSize = sphere.scale * 0.5f;
        float centerY = 0.20f + sphere.moveY;
        float bottomLimit = -1.0f + halfSize;
        float topLimit = 1.0f - halfSize;

        if (sphere.moveSpeedY < 0.0f && centerY <= bottomLimit) {
            sphere.moveY = bottomLimit - 0.20f;
            sphere.movingY = false;
        }
        else if (sphere.moveSpeedY > 0.0f && centerY >= topLimit) {
            sphere.moveY = topLimit - 0.20f;
            sphere.movingY = false;
        }
    }
    if (isSwapping) {
        swapProgress += deltaTime / swapDuration;

        if (swapProgress >= 1.0f) {
            swapProgress = 1.0f;
            isSwapping = false;
        }

        glm::vec3 cubeCenter;
        glm::vec3 sphereCenter;
        glm::vec3 cubeWaypoint(0.0f);
        glm::vec3 sphereWaypoint(0.0f);
        if (swapViaOffset) {
            cubeWaypoint = glm::vec3(0.0f, 0.55f, 0.0f);
            sphereWaypoint = glm::vec3(0.0f, -0.55f, 0.0f);
        }

        if (swapProgress <= 0.5f) {
            float t = swapProgress * 2.0f;

            cubeCenter = glm::mix(cubeSwapStart, cubeWaypoint, t);
            sphereCenter = glm::mix(sphereSwapStart, sphereWaypoint, t);
        }
        else {
            float t = (swapProgress - 0.5f) * 2.0f;

            cubeCenter = glm::mix(cubeWaypoint, sphereSwapStart, t);
            sphereCenter = glm::mix(sphereWaypoint, cubeSwapStart, t);
        }
        cube.moveX = cubeCenter.x - (-0.6f);
        cube.moveY = cubeCenter.y - 0.15f;
        cube.moveZ = cubeCenter.z - 0.15f;

        sphere.moveX = sphereCenter.x - 0.45f;
        sphere.moveY = sphereCenter.y - 0.20f;
        sphere.moveZ = sphereCenter.z - 0.20f;
    }
}

void DrawCube(const glm::mat4& transform) {
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE,
        glm::value_ptr(transform));

    glBindVertexArray(useAlternateShapes ? pyramidMesh.vao : cubevao);
    glDrawElements(GL_TRIANGLES, useAlternateShapes ? pyramidMesh.indexCount : cubeIndexCount,
        GL_UNSIGNED_INT, nullptr);
}

void DrawSphere(const glm::mat4& transform) {
    // 원뿔 OBJ는 0~1 좌표이므로 구와 같은 원점 중심으로 맞춘다.
    glm::mat4 meshTransform = transform;
    if (useAlternateShapes) {
        meshTransform = glm::translate(meshTransform, glm::vec3(-0.5f));
    }
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE,
        glm::value_ptr(meshTransform));

    glBindVertexArray(useAlternateShapes ? coneMesh.vao : spherevao);
    glDrawElements(GL_TRIANGLES, useAlternateShapes ? coneMesh.indexCount : sphereIndexCount,
        GL_UNSIGNED_INT, nullptr);
}

void DrawScene() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);
    glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);//ortho 직교 투영 변환 행렬
    glm::mat4 axisModel(1.0f);
    axisModel = glm::rotate(axisModel, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    axisModel = glm::rotate(axisModel, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 axisTran = projection * axisModel;
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, value_ptr(axisTran));
    glBindVertexArray(axisvao);
    glUniform3f(colorLocation, 1.0f, 0.0f, 0.0f);
    glDrawArrays(GL_LINES, 0, 2);
    glUniform3f(colorLocation, 0.0f, 1.0f, 0.0f);
    glDrawArrays(GL_LINES, 2, 2);
    glUniform3f(colorLocation, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_LINES, 4, 2);

    // 좌표축, 모델 변환, 도형 그리기는 여기서부터 직접 작성한다.
    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    if (cube.visible) {
        glm::mat4 cubemodel(1.0f);
        glm::mat4 orbitModel(1.0f);
        glm::mat4 originScaleModel(1.0f);
        glm::mat4 cubeMovemodel(1.0f);
        //cubemodel = glm::rotate(cubemodel, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //cubemodel = glm::rotate(cubemodel, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        orbitModel = glm::rotate(orbitModel, glm::radians(cube.orbitAngleY), glm::vec3(0.0f, 1.0f, 0.0f));
        cubeMovemodel = glm::translate(cubeMovemodel, glm::vec3(cube.moveX, cube.moveY, cube.moveZ));
        originScaleModel = glm::scale(originScaleModel, glm::vec3(cube.originScale));
        cubemodel = glm::translate(cubemodel, glm::vec3(-1.1f, -0.35f, -0.35f));
        glm::mat4 scaleModel(1.0f);
        scaleModel = glm::translate(scaleModel, glm::vec3(0.5f, 0.5f, 0.5f));
        float cubeDrawScale = cube.scale;
        if (cube.depthScaling) {
            glm::vec4 center = axisModel * cubeMovemodel * orbitModel
                * originScaleModel * cubemodel * glm::vec4(0.5f, 0.5f, 0.5f, 1.0f);
            float depthFactor = glm::clamp(1.0f + 0.5f * center.z, 0.65f, 1.35f);
            cubeDrawScale *= depthFactor;
        }
        scaleModel = glm::scale(scaleModel, glm::vec3(cubeDrawScale));
        scaleModel = glm::translate(scaleModel, glm::vec3(-0.5f, -0.5f, -0.5f));
        glm::mat4 rotateModel(1.0f);
        if (cube.rotatingX) {
            rotateModel = glm::translate(rotateModel, glm::vec3(0.0f, 0.5f, 0.5f));
            rotateModel = glm::rotate(rotateModel, glm::radians(cube.rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
            rotateModel = glm::translate(rotateModel, glm::vec3(0.0f, -0.5f, -0.5f));
        }
        if (cube.rotatingY) {
            rotateModel = glm::translate(rotateModel, glm::vec3(0.5f, 0.0f, 0.5f));
            rotateModel = glm::rotate(rotateModel, glm::radians(cube.rotationY), glm::vec3(.0f, 1.0f, 0.0f));
            rotateModel = glm::translate(rotateModel, glm::vec3(-0.5f, 0.0f, -0.5f));
        }
        DrawCube(projection * axisModel * cubeMovemodel * orbitModel * originScaleModel * cubemodel * scaleModel * rotateModel);
    }
    if (sphere.visible) {
        glm::mat4 spheremodel(1.0f);
        glm::mat4 orbitModel(1.0f);
        glm::mat4 originScaleModel(1.0f);
        glm::mat4 sphereMovemodel(1.0f);
        //spheremodel = glm::rotate(spheremodel, glm::radians(-30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        //spheremodel = glm::rotate(spheremodel, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
        orbitModel = glm::rotate(orbitModel, glm::radians(sphere.orbitAngleY), glm::vec3(0.0f, 1.0f, 0.0f));
        sphereMovemodel = glm::translate(sphereMovemodel, glm::vec3(sphere.moveX, sphere.moveY, sphere.moveZ));
        originScaleModel = glm::scale(originScaleModel, glm::vec3(sphere.originScale));
        spheremodel = glm::translate(spheremodel, glm::vec3(0.45f, 0.20f, 0.20f));
        glm::mat4 scaleModel(1.0f);
        float sphereDrawScale = sphere.scale;
        if (sphere.depthScaling) {
            glm::vec4 center = axisModel * sphereMovemodel * orbitModel
                * originScaleModel * spheremodel * glm::vec4(0.0f, 0.0f, 0.0f, 1.0f);
            float depthFactor = glm::clamp(1.0f + 0.5f * center.z, 0.65f, 1.35f);
            sphereDrawScale *= depthFactor;
        }
        scaleModel = glm::scale(scaleModel, glm::vec3(sphereDrawScale));
        glm::mat4 rotateModel(1.0f);
        if (sphere.rotatingX) {
            rotateModel = glm::rotate(rotateModel, glm::radians(sphere.rotationX), glm::vec3(1.0f, 0.0f, 0.0f));
        }
        if (sphere.rotatingY) {
            rotateModel = glm::rotate(rotateModel, glm::radians(sphere.rotationY), glm::vec3(0.0f, 1.0f, 0.0f));
        }
        DrawSphere(projection * axisModel * sphereMovemodel * orbitModel * originScaleModel * spheremodel * scaleModel * rotateModel);
    }
}

int main() {
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(1200, 1200, "Practice 17", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return 1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    glViewport(0, 0, 1200, 1200);
    glEnable(GL_DEPTH_TEST);

    // 실행 작업 폴더에 vertex.glsl, fragment.glsl, cube.obj가 있어야 한다.
    ObjData cubeobj;
    ObjData sphereobj;
    ObjData pyramidobj;
    ObjData coneobj;
    if (!InitShaderProgram() || !Loadobj("cube.obj", cubeobj)
        || !Loadobj("sphere.obj", sphereobj) || !Loadobj("pyramid.obj", pyramidobj)
        || !Loadobj("cone.obj", coneobj)) {
        if (shaderProgramID) glDeleteProgram(shaderProgramID);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    InitSphereBuffer(sphereobj);
    InitCubeBuffer(cubeobj);
    InitMeshBuffer(pyramidobj, pyramidMesh);
    InitMeshBuffer(coneobj, coneMesh);
    InitaxisBuffer();
    float lastTime = glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        float currentTime = glfwGetTime();
        deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        glfwPollEvents();
        InputProcess();
        UpdateAnimation();
        DrawScene();
        glfwSwapBuffers(window);
    }

    glDeleteBuffers(1, &cubevbo);
    glDeleteBuffers(1, &cubeebo);
    glDeleteVertexArrays(1, &cubevao);
    glDeleteBuffers(1, &pyramidMesh.vbo);
    glDeleteBuffers(1, &pyramidMesh.ebo);
    glDeleteVertexArrays(1, &pyramidMesh.vao);
    glDeleteBuffers(1, &coneMesh.vbo);
    glDeleteBuffers(1, &coneMesh.ebo);
    glDeleteVertexArrays(1, &coneMesh.vao);
    glDeleteProgram(shaderProgramID);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}