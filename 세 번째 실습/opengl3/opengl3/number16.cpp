#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include "Shader.h"
#include "Obj.h"
#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/type_ptr.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>

using namespace std;

GLint width = 1280, height = 900;
GLFWwindow* window = nullptr;
GLuint shaderProgramID = 0;
GLint modelLocation = -1;
GLint colorLocation = -1;
GLuint axisvao;//좌표계
GLuint axisvbo;//좌표계
GLuint pyramidvao; // 사면체 정점 설정
GLuint pyramidvbo; // 큐브 정점 데이터
GLuint pyramidebo; // 사면체 인덱스 리스트 생성

float deltaTime = 0.0f;
float angleY = 30.0f;
float rotatespeed = 45.0f;
float OpenAngle = 0.0f;
float SwapAngle = 0.0f;
float OpClAngle = 0.0f;

bool Yrotate = false;
bool Drawpyramid = false;
bool isPpressed = false;
bool isHpressed = false;
bool is6pressed = false;
bool isDepth = true;
bool isYpressed = false;
bool isOpening = false;
bool faceOpening = false;
bool is7pressed = false;
bool is8pressed = false;
bool is9pressed = false;
bool isSwapping = false;
bool isOpCling = false;


int selectedface = -1;

float faceColor[6][3] = {
  {1.0f, 0.0f, 0.0f},  // 빨강
  {0.0f, 1.0f, 0.0f},  // 초록
  {0.0f, 0.0f, 1.0f},  // 파랑
  {1.0f, 1.0f, 0.0f},  // 노랑
  {0.0f, 1.0f, 1.0f},  // 청록
  {1.0f, 0.0f, 1.0f}   // 자홍
};

glm::vec3 hinge[4] = {
    {0.5f, 0.0f, 0.0f},
    {1.0f, 0.0f, 0.5f},
    {0.5f, 0.0f, 1.0f},
    {0.0f, 0.0f, 0.5f}
};

float Opendirection[4] = { -1.0f,-1.0f,1.0f,1.0f };
float otherAngle[4] = { 0.0f,0.0f,0.0f,0.0f };
float Swapdirection[4] = { -1.0f,1.0f,-1.0f,1.0f };


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
    window = glfwCreateWindow(width, height, "Practice 16", nullptr, nullptr);

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
    ObjData pyramidobj;
    if (!Loadobj("pyramid.obj", pyramidobj)) {
        return -1;
    }
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
    glDeleteBuffers(1, &axisvbo);
    glDeleteVertexArrays(1, &axisvao);
    glDeleteProgram(shaderProgramID);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void InitPyramidBuffer(const ObjData& obj) {
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
            Drawpyramid = true;

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
            Yrotate = !Yrotate;
        }
        isYpressed = true;
    }
    else {
        isYpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
        if (!is6pressed) {
            faceOpening = !faceOpening;
        }
        is6pressed = true;
    }
    else {
        is6pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
        if (!is7pressed) {
            isOpening = !isOpening;
            selectedface = 0;
            OpenAngle = 0.0f;
            faceOpening = false;
        }
        is7pressed = true;
    }
    else {
        is7pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) {
        if (!is8pressed) {
            isSwapping = true;
        }
        is8pressed = true;
    }
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {
        if (!is9pressed) {
            isOpCling = !isOpCling;
        }
        is9pressed = true;
    }
    else {
        is9pressed = false;
    }
}

void UpdateAnimation() {
    // TODO: 회전 각도와 크기 등 애니메이션 값 갱신
    if (Yrotate) {
        angleY += rotatespeed * deltaTime;
    }
    if (faceOpening) {
        OpenAngle=glm::max(OpenAngle - rotatespeed * deltaTime, -233.13f);
    }
    else {
        OpenAngle=glm::min(OpenAngle + rotatespeed * deltaTime, 0.0f);
    }
    if (selectedface >= 0 && selectedface < 4) {
        if (isOpening) {
            otherAngle[selectedface] = max(otherAngle[selectedface] - rotatespeed * deltaTime, -90.0f);
            if (otherAngle[selectedface]==-90.0f) {
                selectedface += 1;
                if (selectedface == 4) {
                    selectedface = -1;
                }
            }
        }
        else {
            otherAngle[selectedface] = min(otherAngle[selectedface] + rotatespeed * deltaTime, 0.0f);
            if (otherAngle[selectedface] == 0.0f) {
                selectedface += 1;
                if (selectedface == 4) {
                    selectedface = -1;
                }
            }
        }
    }
    if (isSwapping) {
        SwapAngle += rotatespeed * deltaTime;
    }
    if (isOpCling) {
        OpClAngle = glm::min(OpClAngle + rotatespeed * deltaTime, 60.0f);
    }
    else {
        OpClAngle = glm::max(OpClAngle - rotatespeed * deltaTime, 0.0f);
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
    axisModel = glm::rotate(axisModel, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
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
    if (Drawpyramid) {
        glm::mat4 pyramidmodel(1.0f);
        pyramidmodel = glm::scale(pyramidmodel, glm::vec3(0.5f));
        pyramidmodel = glm::rotate(pyramidmodel, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
        pyramidmodel = glm::rotate(pyramidmodel, glm::radians(angleY), glm::vec3(0.0f, 1.0f, 0.0f));
        pyramidmodel = glm::translate(pyramidmodel, glm::vec3(-0.5f, -0.5f, -0.5f));
        glBindVertexArray(pyramidvao);
        for (int i = 0; i < 5; ++i) {
            glm::mat4 faceAnimation(1.0f);
            if (i < 4) {
                float faceAngle = -OpenAngle * Opendirection[i];
                faceAnimation = glm::translate(faceAnimation, hinge[i]);
                if (i == 0 || i == 2) {
                    faceAnimation = glm::rotate(faceAnimation, glm::radians(faceAngle), glm::vec3(1.0f, 0.0f, 0.0f));
                }
                else {
                    faceAnimation = glm::rotate(faceAnimation, glm::radians(faceAngle), glm::vec3(0.0f, 0.0f, 1.0f));
                }
                faceAnimation = glm::translate(faceAnimation, -hinge[i]);
            }
            if (i < 4) {
                float faceAngle = -otherAngle[i] * Opendirection[i];
                faceAnimation = glm::translate(faceAnimation, hinge[i]);
                if (i == 0 || i == 2) {
                    faceAnimation = glm::rotate(faceAnimation, glm::radians(faceAngle), glm::vec3(1.0f, 0.0f, 0.0f));
                }
                else {
                    faceAnimation = glm::rotate(faceAnimation, glm::radians(faceAngle), glm::vec3(0.0f, 0.0f, 1.0f));
                }
                faceAnimation = glm::translate(faceAnimation, -hinge[i]);
            }
            if (i < 4) {
                float SpAngle = SwapAngle * Swapdirection[i];
                glm::vec3 apex(0.5f, 1.0f, 0.5f);
                faceAnimation = glm::translate(faceAnimation, apex);
                faceAnimation = glm::rotate(faceAnimation, glm::radians(SpAngle), glm::vec3(0.0f, 1.0f, 0.0f));
                faceAnimation = glm::translate(faceAnimation, -apex);
            }
            if (i < 4) {
                float faceAngle = -OpClAngle * Opendirection[i];
                glm::vec3 apex(0.5f, 1.0f, 0.5f);
                faceAnimation = glm::translate(faceAnimation, apex);
                if (i == 0 || i == 2) {
                    faceAnimation = glm::rotate(faceAnimation, glm::radians(faceAngle), glm::vec3(1.0f, 0.0f, 0.0f));
                }
                else {
                    faceAnimation = glm::rotate(faceAnimation, glm::radians(faceAngle), glm::vec3(0.0f, 0.0f, 1.0f));
                }
                faceAnimation = glm::translate(faceAnimation, -apex);
            }
            glm::mat4 pyramidTran = projection * pyramidmodel * faceAnimation;
            glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(pyramidTran));
            
            glUniform3f(colorLocation,
                faceColor[i][0], faceColor[i][1], faceColor[i][2]);

            int count = (i == 4) ? 6 : 3;
            int startIndex = (i == 4) ? 12 : i * 3;

            glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT,
                (void*)(startIndex * sizeof(unsigned int)));
        }
    }
}
