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
GLint modelLocation = -1;
GLint viewLocation = -1;
GLint projectionLocation = -1;
GLint colorLocation = -1;
GLuint spherevao = 0;
GLuint spherevbo = 0;
GLuint sphereebo = 0;
GLsizei sphereIndexCount = 0;

float deltaTime = 0;
random_device rd;
mt19937 gen(rd());

uniform_real_distribution<float> speed(45.0f, 120.0f);
uniform_real_distribution<float> radius(1.5f, 2.5f);
uniform_real_distribution<float> color(0.0f, 1.0f);

struct Moon {
    float orbitAngle;
    float orbitRadius;
    float orbitSpeed;
    float scale;
    glm::vec3 color;
};

struct Planet {
    float orbitAngle;   // 현재 공전 각도
    float orbitRadius;  // 공전 반지름
    float orbitSpeed;   // 공전 속도
    float scale;        // 행성 크기
    float orbitTile; // 얼마나 회전?
    glm::vec3 PlanetColor; // 행성의 색깔
    Moon moon;
};

Planet planets[3];
glm::vec3 scenePosition(0.0f);//wasd 이동량

float PlanetRadius[3] = {};
float MoonRadius[3] = {};
float Scalespeed = 0.3f;
float CenterScale = 1.0f;
float CenterR = 0.0f;
float CenterG = 1.0f;
float CenterB = 0.0f;
float colorTimer = 0.0f;
float moonOrbitTilt = 0.0f;

GLuint orbitvao = 0;
GLuint orbitvbo = 0;
constexpr int orbitVertexCount = 180;

void InitOrbitBuffer() {
    float vertices[orbitVertexCount * 3];
    for (int i = 0; i < orbitVertexCount; ++i) {
        float angle = glm::radians(360.0f * i / orbitVertexCount);
        vertices[i * 3] = glm::cos(angle);
        vertices[i * 3 + 1] = 0.0f;
        vertices[i * 3 + 2] = glm::sin(angle);
    }
    glGenVertexArrays(1, &orbitvao);
    glGenBuffers(1, &orbitvbo);
    glBindVertexArray(orbitvao);
    glBindBuffer(GL_ARRAY_BUFFER, orbitvbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
    glEnableVertexAttribArray(0);
    glBindVertexArray(0);
}

void DrawOrbit(const glm::mat4& frame, float orbitRadius) {
    glm::mat4 orbitModel = glm::scale(frame, glm::vec3(orbitRadius));
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(orbitModel));
    glUniform3f(colorLocation, 0.35f, 0.35f, 0.35f);
    glBindVertexArray(orbitvao);
    glDrawArrays(GL_LINE_LOOP, 0, orbitVertexCount);
    glBindVertexArray(spherevao);
}

bool Orbiting = true; // 돌고 있는지
bool isPpressed = false; // p가 눌렸는가?
bool isOrtho = false; // 직교, 투영의 차이
bool isMpressed = false; // m이 눌렸는가?
bool isWire = false;
bool is1pressed = false; // 1이 눌렸는가
bool isScaling = false;// 반지름이 커지는 가?
bool isGrowing = false;
bool is2pressed = false; 
bool isCenterScale = false;
bool isCenter = true;
bool is3pressed = false;
bool isMoonTilt = false;



bool InitShaderProgram() {
    shaderProgramID = InitShader("vertex18.glsl", "fragment18.glsl");
    if (shaderProgramID == 0) return false;

    modelLocation = glGetUniformLocation(shaderProgramID, "model");
    viewLocation = glGetUniformLocation(shaderProgramID, "view");
    projectionLocation = glGetUniformLocation(shaderProgramID, "projection");
    colorLocation = glGetUniformLocation(shaderProgramID, "objectColor");
    return true;
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

void InitPlanets() {
    for (int i = 0; i < 3; i++) {
        planets[i].orbitAngle = 0.0f;
        planets[i].orbitRadius = radius(gen);
        planets[i].orbitSpeed = speed(gen);
        planets[i].scale = 0.5f;
        planets[i].PlanetColor = glm::vec3(color(gen), color(gen), color(gen));
        if (i == 1) {
            planets[i].orbitTile = 45.0f;
        }
        else if (i == 2) {
            planets[i].orbitTile = -45.0f;
        }
        else {
            planets[i].orbitTile = 0.0f;
        }
        planets[i].moon.orbitAngle = 0.0f;
        planets[i].moon.orbitRadius = 0.8f;
        planets[i].moon.orbitSpeed = speed(gen);
        planets[i].moon.scale = 0.2f;
        planets[i].moon.color = glm::vec3(color(gen), color(gen), color(gen));
        PlanetRadius[i] = planets[i].orbitRadius;
        MoonRadius[i] = planets[i].moon.orbitRadius;
    }
}


void InputProcess() {
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, GLFW_TRUE);
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        if (!isPpressed) {
            isOrtho = !isOrtho;
        }
        isPpressed = true;
    }
    else {
        isPpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        if (!isMpressed) {
            isWire = !isWire;
        }
        isMpressed = true;
    }
    else {
        isMpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        if (!is1pressed) {
            isGrowing = !isGrowing;
            isScaling = true;
        }
        is1pressed = true;
    }
    else {
        is1pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        if (!is2pressed) {
            isCenterScale = !isCenterScale;
        }
        is2pressed = true;
    }
    else {
        is2pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        if (!is3pressed) {
            isMoonTilt = !isMoonTilt;
        }
        is3pressed = true;
    }
    else {
        is3pressed = false;
    }
    float moveSpeed = 2.0f * deltaTime;
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        scenePosition.x -= moveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        scenePosition.x += moveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        scenePosition.y += moveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        scenePosition.y -= moveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_EQUAL) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
        scenePosition.z += moveSpeed;
    }
    if (glfwGetKey(window, GLFW_KEY_MINUS) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
        scenePosition.z -= moveSpeed;
    }
    scenePosition.x=glm::clamp(scenePosition.x, -1.0f, 1.0f);
    scenePosition.y=glm::clamp(scenePosition.y, -1.0f, 1.0f);
    scenePosition.z=glm::clamp(scenePosition.z, -10.0f, 10.0f);
}

void UpdateAnimation() {
    if (Orbiting) {
        for (int i = 0;i < 3;i++) {
            planets[i].orbitAngle += planets[i].orbitSpeed * deltaTime;
            planets[i].moon.orbitAngle += planets[i].moon.orbitSpeed * deltaTime;
        }
    }
    if (isScaling) {
        bool allFinished = true;
        float step = Scalespeed * deltaTime;

        for (int i = 0; i < 3; i++) {
            // 커질 때는 최댓값, 작아질 때는 초기 반지름
            float targetPlanet = isGrowing ? 3.5f : planets[i].orbitRadius;
            float targetMoon = isGrowing ? 1.5f : planets[i].moon.orbitRadius;

            if (isGrowing) {
                PlanetRadius[i] = glm::min(PlanetRadius[i] + step, targetPlanet);
                MoonRadius[i] = glm::min(MoonRadius[i] + step, targetMoon);
            }
            else {
                PlanetRadius[i] = glm::max(PlanetRadius[i] - step, targetPlanet);
                MoonRadius[i] = glm::max(MoonRadius[i] - step, targetMoon);
            }

            if (PlanetRadius[i] != targetPlanet ||
                MoonRadius[i] != targetMoon) {
                allFinished = false;
            }
        }

        if (allFinished) {
            isScaling = false;
        }
    }
    if (isCenterScale) {
        const float minScale = 1.0f;
        const float maxScale = 2.0f;
        const float maxColor = 1.0f;
        const float minColor = 0.0f;
        float step = Scalespeed * deltaTime;
        colorTimer += deltaTime;

        if (colorTimer >= 0.5f) {  // 0.5초마다 변경
            CenterR = color(gen);
            CenterG = color(gen);
            CenterB = color(gen);

            colorTimer = 0.0f;
        }

        if (isCenter) {
            CenterScale = glm::min(CenterScale + step, maxScale);

            if (CenterScale >= maxScale) {
                isCenter = false;  // 최대에 도달하면 축소
            }
        }
        else {
            CenterScale = glm::max(CenterScale - step, minScale);

            if (CenterScale <= minScale) {
                isCenter = true;   // 최소에 도달하면 확대
            }
        }
    }
    if (isMoonTilt) {
        moonOrbitTilt += 30.0f * deltaTime;
    }
}


void DrawScene() {
    int width, height;
    glfwGetFramebufferSize(window, &width, &height);
    if (width == 0 || height == 0) return;
    glViewport(0, 0, width, height);

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);

    // 좌표축, 모델 변환, 도형 그리기는 여기서부터 직접 작성한다.
    if (isWire) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }
    
    //구는 그대로 원점에 배치
    glm::mat4 model(1.0f);
    model = glm::translate(model, scenePosition);
    model = glm::scale(model, glm::vec3(CenterScale));
    //카메라 위치, 바라보는 지점, 위쪽 방향
    glm::mat4 view = glm::lookAt(
        glm::vec3(0.0f, 2.0f, 6.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 1.0f, 0.0f)
    );
    //보이는 범위와 원근감
    glm::mat4 projection(1.0f);
    if (isOrtho) {
        float aspect = static_cast<float>(width) / height; // 화면 비율 맞추기
        projection = glm::ortho(
            -4.0f * aspect, 4.0f * aspect, -4.0f, 4.0f, 0.1f, 100.0f
        );
    }
    else {
        projection = glm::perspective(
            glm::radians(45.0f), // 세로 시야각 45도
            static_cast<float>(width) / height,  // 화면 가로 / 세로 비율
            0.1f,              // 가까운 경계
            100.0f             // 먼 경계
        );
    }
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLocation, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projectionLocation, 1, GL_FALSE, glm::value_ptr(projection));

    glUniform3f(colorLocation, CenterR, CenterG, CenterB);

    glBindVertexArray(spherevao);
    glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, nullptr);

    
    for (int i = 0;i < 3;i++) {
        glm::mat4 planetmodel(1.0f);
        glm::mat4 moonmodel(1.0f);
        planetmodel = glm::translate(planetmodel, scenePosition);
        planetmodel = glm::rotate(planetmodel, glm::radians(planets[i].orbitTile), glm::vec3(0.0f, 0.0f, 1.0f));
        planetmodel = glm::rotate(planetmodel, glm::radians(planets[i].orbitAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        DrawOrbit(planetmodel, PlanetRadius[i]);
        planetmodel = glm::translate(planetmodel, glm::vec3(PlanetRadius[i], 0.0f, 0.0f));
        glm::vec3 planetPosition = glm::vec3(planetmodel[3]);
        // 달의 기준점을 행성 위치로 이동
        moonmodel = glm::translate(moonmodel, planetPosition);

        if (isMoonTilt) {
            moonmodel = glm::rotate(moonmodel,glm::radians(moonOrbitTilt),glm::vec3(0.0f, 0.0f, 1.0f));
        }

        // 행성의 공전 회전만 이어받기 (45도 기울기는 제외)
        moonmodel = glm::rotate(moonmodel,glm::radians(planets[i].orbitAngle),glm::vec3(0.0f, 1.0f, 0.0f));
        DrawOrbit(moonmodel, MoonRadius[i]);
        planetmodel = glm::scale(planetmodel, glm::vec3(planets[i].scale));

        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(planetmodel));

        glUniform3f(colorLocation, planets[i].PlanetColor.r, planets[i].PlanetColor.g, planets[i].PlanetColor.b);

        glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, nullptr);
        moonmodel = glm::rotate(moonmodel, glm::radians(planets[i].moon.orbitAngle), glm::vec3(0.0f, 1.0f, 0.0f));
        moonmodel = glm::translate(moonmodel, glm::vec3(MoonRadius[i], 0.0f, 0.0f));
        moonmodel = glm::scale(moonmodel, glm::vec3(planets[i].moon.scale));

        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(moonmodel));

        glUniform3f(colorLocation, planets[i].moon.color.r, planets[i].moon.color.g, planets[i].moon.color.b);

        glDrawElements(GL_TRIANGLES, sphereIndexCount, GL_UNSIGNED_INT, nullptr);
    }

    glBindVertexArray(0);

}

int main() {
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(1200, 1200, "Practice 18", nullptr, nullptr);
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
    glEnable(GL_DEPTH_TEST);

    // 실행 작업 폴더에 vertex.glsl, fragment.glsl, cube.obj가 있어야 한다.
    ObjData sphereobj;
    if (!InitShaderProgram() || !Loadobj("sphere.obj", sphereobj)) {
        if (shaderProgramID) glDeleteProgram(shaderProgramID);
        glfwDestroyWindow(window);
        glfwTerminate();
        return 1;
    }
    InitSphereBuffer(sphereobj);
    InitOrbitBuffer();
    InitPlanets();
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

    glDeleteBuffers(1, &orbitvbo);
    glDeleteVertexArrays(1, &orbitvao);
    glDeleteProgram(shaderProgramID);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}
