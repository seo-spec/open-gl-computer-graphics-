#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "Obj.h"
#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/type_ptr.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>
#include <random>
#include <utility>

using namespace std;

GLint width = 1280, height = 900;
GLFWwindow* window = nullptr;
GLuint shaderProgramID = 0;
GLint modelLocation = -1;
GLint colorLocation = -1;
GLuint axisvao;//좌표계
GLuint axisvbo;//좌표계
GLuint cubevao; // 큐브 정점 설정
GLuint cubevbo; // 큐브 정점 데이터
GLuint cubeebo; // 큐브 인덱스 리스트 생성
GLuint pyramidvao; // 사면체 정점 설정
GLuint pyramidvbo; // 큐브 정점 데이터
GLuint pyramidebo; // 사면체 인덱스 리스트 생성

float deltaTime = 0.0f;
float angleY = 30.0f;
float rotatespeed = 45.0f;
float isY = 0.0f;
float TopAngleZ = 0.0f;
float isZ = 0.0f;
float OpenAngleX = 0.0f;
float RotateAngleX = 0.0f;
float BackScale = 1.0f;
float Scalespeed = 0.2f;
float PairAngle = 0.0f;
float ChangeAngle1 = 0.0f;
float ChangeAngle2 = 0.0f;
float changeDirection = 1.0f;
float RotateAngle = 0.0f;
float XRotateAngle = 0.0f;


int selectedface1 = -1;//선택된 2면
int selectedface2 = -1;//선택된 2면
int orbitface1 = -1;
int orbitface2 = -1;
int faceAt[6] = { 0, 1, 2, 3, 4, 5 };

bool Bscaling = false;
bool Srotate = false;
bool isOpen = false;
bool Drawcube = false;
bool Drawpyramid = false;
bool isPpressed = false;
bool isHpressed = false;
bool isDepth = true;
bool isYpressed = false;
bool isRpressed = false;
bool isTpressed = false;
bool isFpressed = false;
bool isSpressed = false;
bool isBpressed = false;
bool is1pressed = false;
bool is2pressed = false;
bool is4pressed = false;
bool is5pressed = false;
bool isPair = false;
bool isChange = false;
bool isRotating = false;
bool isXRotating = false;

glm::vec3 edgeCenter(0.0f);

mt19937 gen{ random_device{}() };
uniform_int_distribution<int> Pairtwo(0, 3);


float faceColor[6][3] = {
  {1.0f, 0.0f, 0.0f},  // 빨강
  {0.0f, 1.0f, 0.0f},  // 초록
  {0.0f, 0.0f, 1.0f},  // 파랑
  {1.0f, 1.0f, 0.0f},  // 노랑
  {0.0f, 1.0f, 1.0f},  // 청록
  {1.0f, 0.0f, 1.0f}   // 자홍
};

int pairface[4][2] = {
    {0,1},
    {0,3},
    {5,1},
    {5,3}
};

void InitCubeBuffer(const ObjData& obj);
void InitPyramidBuffer(const ObjData& obj);
void InputProcess();
void DrawScene();
void UpdateAnimation();

bool InitShaderProgram() {
    // 13번에서 사용한 셰이더
    shaderProgramID = InitShader("vertex.glsl", "fragment.glsl");
    if (shaderProgramID == 0) {
        cout << "셰이더 프로그램 초기화 실패" << endl;
        return false;
    }

    modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
    colorLocation = glGetUniformLocation(shaderProgramID, "faceColor");
    return true;
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

int main() {
    if (!glfwInit())
        return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    window = glfwCreateWindow(width, height, "Practice 15", nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    glViewport(0, 0, width, height);
    glEnable(GL_DEPTH_TEST);
    float lastTime = glfwGetTime();

    if (!InitShaderProgram()) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }
    ObjData cubeobj;
    ObjData pyramidobj;
    if (!Loadobj("cube.obj", cubeobj)) {
        return -1;
    }
    if (!Loadobj("pyramid.obj", pyramidobj)) {
        return -1;
    }
    InitCubeBuffer(cubeobj);
    InitPyramidBuffer(pyramidobj);
    InitaxisBuffer();

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

    // TODO: 만든 VAO, VBO, EBO도 여기서 삭제하기
    glDeleteBuffers(1, &pyramidvbo);
    glDeleteVertexArrays(1, &pyramidvao);
    glDeleteBuffers(1, &pyramidebo);
    glDeleteBuffers(1, &cubeebo);
    glDeleteBuffers(1, &cubevbo);
    glDeleteVertexArrays(1, &cubevao);
    glDeleteBuffers(1, &axisvbo);
    glDeleteVertexArrays(1, &axisvao);
    glDeleteProgram(shaderProgramID);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void InitCubeBuffer(const ObjData &obj) {
    // TODO: 육면체의 정점, 인덱스 및 VAO, VBO, EBO 설정
    glGenVertexArrays(1, &cubevao);
    glBindVertexArray(cubevao);

    glGenBuffers(1, &cubevbo);
    glGenBuffers(1, &cubeebo);

    //위치 VBO
    glBindBuffer(GL_ARRAY_BUFFER, cubevbo);
    glBufferData(GL_ARRAY_BUFFER, obj.vertices.size() * sizeof(float), obj.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0,(void*)0);
    glEnableVertexAttribArray(0);

   //ebo 처리
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cubeebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER,obj.indices.size() * sizeof(unsigned int), obj.indices.data(), GL_STATIC_DRAW);
}

void InitPyramidBuffer(const ObjData &obj) {
    // TODO: 사각뿔의 정점, 인덱스 및 VAO, VBO, EBO 설정
    glGenVertexArrays(1, &pyramidvao);
    glBindVertexArray(pyramidvao);

    glGenBuffers(1, &pyramidvbo);
    glGenBuffers(1, &pyramidebo);

    //위치 VBO
    glBindBuffer(GL_ARRAY_BUFFER, pyramidvbo);
    glBufferData(GL_ARRAY_BUFFER, obj.vertices.size() * sizeof(float), obj.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    //ebo처리
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramidebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj.indices.size() * sizeof(unsigned int), obj.indices.data(), GL_STATIC_DRAW);
    
}

void InputProcess() {
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS ||
        glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    // TODO: h, p, y, r 및 각 면의 애니메이션 키 처리
    // 켜기/끄기 명령은 키를 누른 순간에만 처리하도록 만들기
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        if (!isPpressed) {
            if (Drawcube == false && Drawpyramid == false) {
                Drawcube = true;
            }
            else if (Drawcube == true) {
                Drawcube = false;
                Drawpyramid = true;
            }
            else {
                Drawcube = true;
                Drawpyramid = false;
            }
            
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
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        if (!isYpressed) {
            if (isY == 0.0f) {
                isY = 1.0f;
            }
            else {
                isY = 0.0f;
            }
        }
        isYpressed = true;
    }
    else {
        isYpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        if (!isRpressed) {

        }
        isRpressed = true;
    }
    else {
        isRpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        if (!isTpressed) {
            if (isZ == 0.0f) {
                isZ = 1.0f;
            }
            else {
                isZ = 0.0f;
            }
        }
        isTpressed = true;
    }
    else {
        isTpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        if (!isFpressed) {
            isOpen = !isOpen;
        }
        isFpressed = true;
    }
    else {
        isFpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (!isSpressed) {
            Srotate = !Srotate;
        }
        isSpressed = true;
    }
    else {
        isSpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        if (!isBpressed) {
            Bscaling = !Bscaling;
        }
        isBpressed = true;
    }
    else {
        isBpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        if (!is1pressed) {
            PairAngle = 0.0f;
            isPair = true;
            int Pairindex = Pairtwo(gen);
            selectedface1 = pairface[Pairindex][0];
            selectedface2 = pairface[Pairindex][1];
            if (Pairindex == 0)
                edgeCenter = glm::vec3(0.0f, 0.5f, 0.0f);
            else if (Pairindex == 1)
                edgeCenter = glm::vec3(1.0f, 0.5f, 0.0f);
            else if (Pairindex == 2)
                edgeCenter = glm::vec3(0.0f, 0.5f, 1.0f);
            else
                edgeCenter = glm::vec3(1.0f, 0.5f, 1.0f);
        }
        is1pressed = true;
    }
    else {
        is1pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        if (!is2pressed&&!isChange) {
            ChangeAngle1 = 0.0f;
            ChangeAngle2 = 0.0f;
            isChange = true;
            int Changeindex = Pairtwo(gen);
            orbitface1 = pairface[Changeindex][0];
            orbitface2 = pairface[Changeindex][1];
            changeDirection = (Changeindex == 0 || Changeindex == 3) ? 1.0f : -1.0f;
        }
        is2pressed = true;
    }
    else {
        is2pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        if (!is4pressed) {
            isRotating = !isRotating;
        }
        is4pressed = true;
    }
    else {
        is4pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
        if (!is5pressed) {
            isXRotating = !isXRotating;
        }
        is5pressed = true;
    }
    else {
        is5pressed = false;
    }
    angleY += isY * rotatespeed * deltaTime;
    TopAngleZ += isZ * rotatespeed * deltaTime;
}

void UpdateAnimation() {
    // TODO: 회전 각도와 크기 등 애니메이션 값 갱신
    if (isOpen) {
        OpenAngleX = glm::max(
            OpenAngleX - rotatespeed * deltaTime, -90.0f);
    }
    else {
        OpenAngleX = glm::min(
            OpenAngleX + rotatespeed * deltaTime, 0.0f);
    }
    if (Srotate) {
        RotateAngleX += rotatespeed * deltaTime;
    }
    if (Bscaling) {
        BackScale = glm::max(
            BackScale - Scalespeed * deltaTime, 0.0f
        );
    }
    else {
        BackScale = glm::min(
            BackScale + Scalespeed * deltaTime, 1.0f
        );
    }
    if (isPair) {
        PairAngle = glm::min(PairAngle + rotatespeed * deltaTime, 360.0f);
        if (PairAngle >= 360.0f) {
            isPair = false;
        }
    }
    if (isChange) {
        ChangeAngle1 = glm::min(ChangeAngle1 + rotatespeed * deltaTime, 90.0f);
        ChangeAngle2 = glm::max(ChangeAngle2 - rotatespeed * deltaTime, -90.0f);
        if (ChangeAngle1 >= 90.0f && ChangeAngle2 <= -90.0f) {
            std::swap(faceAt[orbitface1], faceAt[orbitface2]);

            ChangeAngle1 = 0.0f;
            ChangeAngle2 = 0.0f;
            orbitface1 = -1;
            orbitface2 = -1;
            isChange = false;
        }
        
    }
    if (isRotating) {
        RotateAngle += rotatespeed * deltaTime;
        while (RotateAngle >= 90.0f) {
            int temp = faceAt[3];
            faceAt[3] = faceAt[5];
            faceAt[5] = faceAt[1];
            faceAt[1] = faceAt[0];
            faceAt[0] = temp;

            RotateAngle -= 90.0f;
        }
    }
    if (isXRotating) {
        XRotateAngle += rotatespeed * deltaTime;
        while (XRotateAngle >= 90.0f) {
            int temp = faceAt[2];
            faceAt[2] = faceAt[3];
            faceAt[3] = faceAt[4];
            faceAt[4] = faceAt[1];
            faceAt[1] = temp;

            XRotateAngle -= 90.0f;
        }
    }
}

void DrawScene() {
    if (isDepth) {
        glEnable(GL_DEPTH_TEST);
    }
    else {
        glDisable(GL_DEPTH_TEST);
    }

    glClearColor(0.9f, 0.9f, 0.9f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glUseProgram(shaderProgramID);
    // TODO: 각 면의 modelTransform과 faceColor를 보내고 면별로 그리기
    glm::mat4 projection = glm::ortho(-1.0f, 1.0f, -1.0f, 1.0f, -1.0f, 1.0f);//ortho 직교 투영 변환 행렬
    glm::mat4 axisModel(1.0f);
    axisModel=glm::rotate(axisModel, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    axisModel=glm::rotate(axisModel, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    glm::mat4 axisTran = projection * axisModel;
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, value_ptr(axisTran));
    glBindVertexArray(axisvao);
    glUniform3f(colorLocation, 1.0f, 0.0f, 0.0f);
    glDrawArrays(GL_LINES, 0, 2);
    glUniform3f(colorLocation, 0.0f, 1.0f, 0.0f);
    glDrawArrays(GL_LINES, 2, 2);
    glUniform3f(colorLocation, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_LINES, 4, 2);
    glm::mat4 cubemodel(1.0f);
    cubemodel = glm::scale(cubemodel, glm::vec3(0.5f));
    cubemodel = glm::rotate(cubemodel, glm::radians(30.0f),glm::vec3(1.0f, 0.0f, 0.0f));
    cubemodel = glm::rotate(cubemodel, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
    cubemodel = glm::translate(cubemodel, glm::vec3(-0.5f, -0.5f, -0.5f));
    glBindVertexArray(cubevao);
    if (Drawcube) {
        for (int i = 0; i < 6; ++i) {
            glm::mat4 faceAnimation(1.0f);

            if (i == 2) {
                // 윗면 애니메이션을 추가
                glm::vec3 center(0.5f, 1.0f, 0.5f);
                faceAnimation = glm::translate(faceAnimation, center);
                faceAnimation = glm::rotate(faceAnimation, glm::radians(TopAngleZ), glm::vec3(0.0f,0.0f,1.0f));
                faceAnimation = glm::translate(faceAnimation, -center);
            }
            if (i == 5) {
                //문이 열리는 애니메이션 추가
                glm::vec3 door(0.5f, 1.0f, 1.0f);
                faceAnimation = glm::translate(faceAnimation, door);
                faceAnimation = glm::rotate(faceAnimation, glm::radians(OpenAngleX), glm::vec3(1.0f, 0.0f, 0.0f));
                faceAnimation = glm::translate(faceAnimation, -door);
            }
            if (i ==1) {
                glm::vec3 center(0.0f, 0.5f, 0.5f);
                faceAnimation = glm::translate(faceAnimation, center);
                faceAnimation = glm::rotate(faceAnimation, glm::radians(RotateAngleX), glm::vec3(1.0f, 0.0f, 0.0f));
                faceAnimation = glm::translate(faceAnimation, -center);
            }
            if (i == 0) {
                glm::vec3 center(0.5f, 0.5f, 0.0f);
                faceAnimation = glm::translate(faceAnimation, center);
                faceAnimation = glm::scale(faceAnimation, glm::vec3(BackScale));
                faceAnimation = glm::translate(faceAnimation, -center);
            }
            if (i == selectedface1 || i == selectedface2) {
                faceAnimation = glm::translate(faceAnimation, edgeCenter);
                faceAnimation = glm::rotate(faceAnimation, glm::radians(PairAngle), glm::vec3(0.0f, 1.0f, 0.0f));
                faceAnimation = glm::translate(faceAnimation, -edgeCenter);
            }
            if (i == orbitface1 || i == orbitface2) {
                glm::vec3 center(0.5f, 0.5f, 0.5f);
                if (i == orbitface1) {
                    faceAnimation = glm::translate(faceAnimation, center);
                    faceAnimation = glm::rotate(faceAnimation, glm::radians(ChangeAngle1 * changeDirection), glm::vec3(0.0f, 1.0f, 0.0f));
                    faceAnimation = glm::translate(faceAnimation, -center);
                }
                if (i == orbitface2) {
                    faceAnimation = glm::translate(faceAnimation, center);
                    faceAnimation = glm::rotate(faceAnimation, glm::radians(ChangeAngle2 * changeDirection), glm::vec3(0.0f, 1.0f, 0.0f));
                    faceAnimation = glm::translate(faceAnimation, -center);
                }
            }
            if (i == 0 || i == 1 || i == 3 || i == 5) {
                glm::vec3 center(0.5f, 0.5f, 0.5f);
                faceAnimation = glm::translate(faceAnimation, center);
                faceAnimation = glm::rotate(faceAnimation, glm::radians(RotateAngle), glm::vec3(0.0f, 1.0f, 0.0f));
                faceAnimation = glm::translate(faceAnimation, -center);
            }
            if (i == 1 || i == 3 || i == 2 || i == 4) {
                glm::vec3 center(0.5f, 0.5f, 0.5f);
                faceAnimation = glm::translate(faceAnimation, center);
                faceAnimation = glm::rotate(faceAnimation, glm::radians(XRotateAngle), glm::vec3(0.0f, 0.0f, 1.0f));
                faceAnimation = glm::translate(faceAnimation, -center);
            }
            glm::mat4 faceTransform =
                projection * cubemodel * faceAnimation;

            glUniformMatrix4fv(modelLocation, 1, GL_FALSE,
                glm::value_ptr(faceTransform));
            int face = faceAt[i];
            glUniform3f(colorLocation,
                faceColor[face][0], faceColor[face][1], faceColor[face][2]);

            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT,
                (void*)(i * 6 * sizeof(unsigned int)));
        }
    }
    if (Drawpyramid) {
        glm::mat4 pyramidmodel(1.0f);
        pyramidmodel = glm::rotate(pyramidmodel, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        pyramidmodel = glm::rotate(pyramidmodel, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
        pyramidmodel = glm::translate(pyramidmodel, glm::vec3(-0.5f, -0.5f, -0.5f));
        glm::mat4 pyramidTran = projection * pyramidmodel;
        glBindVertexArray(pyramidvao);
        glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(pyramidTran));
        for (int i = 0; i < 5; ++i) {
            glUniform3f(colorLocation,
                faceColor[i][0], faceColor[i][1], faceColor[i][2]);

            int count = (i == 4) ? 6 : 3;
            int startIndex = (i == 4) ? 12 : i * 3;

            glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT,
                (void*)(startIndex * sizeof(unsigned int)));
        }
    }
}
