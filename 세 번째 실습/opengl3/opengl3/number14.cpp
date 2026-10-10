#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include "Shader.h"
#include <vector>
#include "Obj.h"
#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/type_ptr.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>

using namespace std;

GLint width, height;
GLFWwindow* window = nullptr;

GLuint shaderProgramID;
GLuint vao;
GLuint vbo[2];
GLuint ebo;
GLuint axisvao;
GLuint axisvbo;
//사각뿔 전역 변수 선언
GLuint pyramidVAO; //사각뿔 정점 설정
GLuint pyramidVBO; // 정점 위치 데이터
GLuint pyramidColorVBO; // 삼각형 색깔 vbo 
GLuint pyramidEBO; // 삼각형 인덱스 데이터

bool isPyramid = false;
bool isCube = false;
bool isCpressed = false;
bool isPpressed = false;
bool isHpressed = false;
bool isXpressed = false;
bool isDepth = true;
bool isWpressed = false;
bool isWireframe = false;
bool isYpressed = false;
bool isSpressed = false;


float moveX=0.0f;
float moveY=0.0f;
float deltaTime = 0.0f;
float moveSpeed = 0.5f;//초당 이동거리

float angleX = 30.0f; // 현재 X축 각도
float rotateSpeed = 60.0f; // 초당 60도
int rotationXDirection = 0; // 0: 정지, 1 : 양의 방향, -1 : 음의 방향
int rotationYDirection = 0; // 0: 정지, 1 : 양의 방향, -1 : 음의 방향

float angleY = 30.0f;

GLsizei indexCount = 0;
GLsizei pyramidIndexCount = 0;

void InitBuffer(const ObjData& obj) {
    float vertexColor[] = {
        1.0f,0.0f,0.0f, //0
        0.0f,1.0f,0.0f, //1
        0.0f,0.0f,1.0f, //2 
        1.0f,1.0f,0.0f, //3
        0.0f,1.0f,1.0f, //4
        1.0f,0.0f,1.0f, //5
        1.0f,0.5f,0.0f, //6
        0.5f,0.0f,1.0f, //7
    };
    

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(2, vbo); // vbo[1]은 다음 단계에서 정점 색상용으로 연결
    glGenBuffers(1, &ebo);

    // 위치 VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, obj.vertices.size() * sizeof(float), obj.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);


    // 색상 vbo
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertexColor), vertexColor, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);
    // EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj.indices.size() * sizeof(unsigned int), obj.indices.data(), GL_STATIC_DRAW);
    indexCount = static_cast<GLsizei>(obj.indices.size());
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

void InitPyramidBuffer(const ObjData& obj) {
    pyramidIndexCount = static_cast<GLsizei>(obj.indices.size());
    float PyramidColor[] = {
        1.0f,0.0f,0.0f, //0
        0.0f,1.0f,0.0f, //1
        0.0f,0.0f,1.0f, //2 
        1.0f,1.0f,0.0f, //3
        0.0f,1.0f,1.0f, //4
    };

    glGenVertexArrays(1, &pyramidVAO);
    glBindVertexArray(pyramidVAO);
    glGenBuffers(1, &pyramidVBO);
    glGenBuffers(1, &pyramidEBO);
    glGenBuffers(1, &pyramidColorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
    glBufferData(GL_ARRAY_BUFFER, obj.vertices.size() * sizeof(float), obj.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, pyramidColorVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(PyramidColor), PyramidColor, GL_STATIC_DRAW);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE,0, (void*)0);
    glEnableVertexAttribArray(1);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramidEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj.indices.size() * sizeof(unsigned int), obj.indices.data(), GL_STATIC_DRAW);
}

bool InitShaderProgram() {
    shaderProgramID = InitShader("vertex14.glsl", "fragment14.glsl");

    if (shaderProgramID != 0) {
        cout << "셰이더 프로그램 초기화 성공" << endl;
        return true;
    }
    else {
        cout << "셰이더 프로그램 초기화 실패" << endl;
        return false;
    }
}

void InputProcess();
void DrawScene();

int main() {
    width = 1280;
    height = 900;
    ObjData obj;
    ObjData pyramidobj;

    if (!Loadobj("cube.obj", obj)) {
        return -1;
    }
    cout << obj.vertices.size() / 3 << endl;
    cout << obj.indices.size() << endl;

    if (!Loadobj("pyramid.obj", pyramidobj)) {
        return -1;
    }
    cout << pyramidobj.vertices.size() / 3 << endl;
    cout << pyramidobj.indices.size() << endl;

    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "OpenGL Exercise 14", nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, width, height);

    if (!InitShaderProgram()) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    InitBuffer(obj);
    InitaxisBuffer();
    InitPyramidBuffer(pyramidobj);
    double lastTime = glfwGetTime();
    // 메인 루프
    while (!glfwWindowShouldClose(window)) {
        double currentTime = glfwGetTime();
        deltaTime = static_cast<float>(currentTime - lastTime);
        lastTime = currentTime;
        InputProcess();
        DrawScene();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteBuffers(2, vbo);
    glDeleteBuffers(1, &ebo);
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &pyramidVBO);
    glDeleteBuffers(1, &pyramidEBO);
    glDeleteBuffers(1, &pyramidColorVBO);
    glDeleteBuffers(1, &axisvbo);
    glDeleteVertexArrays(1, &axisvao);
    glDeleteVertexArrays(1, &pyramidVAO);
    glDeleteProgram(shaderProgramID);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// 14번 키 입력은 다음 단계에서 추가한다.
void InputProcess() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (!isCpressed)
        {   
            isPyramid = false;
            isCube = !isCube;
           
        }
        isCpressed = true;
    }
    else {
        isCpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        if (!isPpressed)
        {
            isCube = false;
            isPyramid = !isPyramid;
        }
        isPpressed = true;
    }
    else {
        isPpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        if (!isHpressed) {
            isDepth = !isDepth;
        }
        isHpressed = true;
    }
    else {
        isHpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (!isWpressed) {
            isWireframe = !isWireframe;
        }
        isWpressed = true;
    }
    else {
        isWpressed=false;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        moveX -= moveSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        moveX += moveSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        moveY += moveSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        moveY -= moveSpeed * deltaTime;
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        if (!isXpressed) {
            if (rotationXDirection == 0) {
                rotationXDirection = 1;
            }
            else {
                rotationXDirection = -rotationXDirection;
            }
        }
        isXpressed = true;
    }
    else {
        isXpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        if (!isYpressed) {
            if (rotationYDirection == 0) {
                rotationYDirection = 1;
            }
            else {
                rotationYDirection = -rotationYDirection;
            }
        }
        isYpressed = true;
    }
    else {
        isYpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (!isSpressed) {
            moveX = 0.0f;
            moveY = 0.0f;
            angleX = 30.0f;
            angleY = 30.0f;
            rotationXDirection = 0;
            rotationYDirection = 0;
        }
        isSpressed = true;
    }
    else {
        isSpressed = false;
    }
    angleX += rotationXDirection * rotateSpeed * deltaTime;
    angleY += rotationYDirection * rotateSpeed * deltaTime;
    moveX = glm::clamp(moveX, -0.3f, 0.3f);
    moveY = glm::clamp(moveY, -0.3f, 0.5f);
}

void DrawScene() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    if (isDepth) {
        glEnable(GL_DEPTH_TEST);
    }
    else {
        glDisable(GL_DEPTH_TEST);
    }
    glUseProgram(shaderProgramID);

    GLint modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
    GLint colorLocation = glGetUniformLocation(shaderProgramID, "faceColor");
    GLint useVertexColorLocation = glGetUniformLocation(shaderProgramID, "useVertexColor");
    glUniform1i(useVertexColorLocation, GL_FALSE); // 좌표축은 uniform 색상 사용

    // 좌표축은 객체의 이동이나 회전과 독립적으로 고정한다.
    glm::mat4 axisModel(1.0f);
    axisModel = glm::rotate(axisModel, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    axisModel = glm::rotate(axisModel, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(axisModel));
    glBindVertexArray(axisvao);
    glUniform3f(colorLocation, 1.0f, 0.0f, 0.0f);
    glDrawArrays(GL_LINES, 0, 2);
    glUniform3f(colorLocation, 0.0f, 1.0f, 0.0f);
    glDrawArrays(GL_LINES, 2, 2);
    glUniform3f(colorLocation, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_LINES, 4, 2);

    if (isWireframe) {
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
    }
    else {
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    }

    // 다음 단계: 정점 색상과 객체 전체 출력, 14번 명령어를 추가한다.
    if (isCube) {
        glm::mat4 cubeModel(1.0f);
        cubeModel = glm::translate(cubeModel, glm::vec3(moveX, moveY, 0.0f));
        cubeModel = glm::rotate(cubeModel, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
        cubeModel = glm::rotate(cubeModel, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
        cubeModel = glm::translate(cubeModel, glm::vec3(-0.5f, -0.5f, -0.5f));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(cubeModel));
        glUniform1i(useVertexColorLocation, GL_TRUE); // 1은 값 한개라는 뜻
        glBindVertexArray(vao);
        
        glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void*)0);
    }
    if (isPyramid) {
        glm::mat4 PyramidModel(1.0f);
        PyramidModel = glm::translate(PyramidModel, glm::vec3(moveX, moveY, 0.0f));
        PyramidModel = glm::rotate(PyramidModel, glm::radians(angleX), glm::vec3(1.0f, 0.0f, 0.0f));
        PyramidModel = glm::rotate(PyramidModel, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
        PyramidModel = glm::translate(PyramidModel, glm::vec3(-0.5f, -0.5f, -0.5f));
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(PyramidModel));
        glUniform1i(useVertexColorLocation, GL_TRUE); // 1은 값 한개라는 뜻
        glBindVertexArray(pyramidVAO);
        glDrawElements(GL_TRIANGLES, pyramidIndexCount, GL_UNSIGNED_INT, (void*)0);
    }
}
