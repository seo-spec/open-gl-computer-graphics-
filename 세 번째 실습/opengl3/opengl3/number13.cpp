#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include "Shader.h"
#include <vector>
#include "Obj.h"
#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/type_ptr.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>
#include <random>

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
GLuint pyramidEBO; // 삼각형 인덱스 데이터

GLsizei indexCount = 0; // opengl에서 사용하는 정수 타입
constexpr GLsizei indicies = 6;


vector<int> selectedFaces;
vector<int> PyramidFaces;

bool is1Pressed = false;
bool is2Pressed = false;
bool is3Pressed = false;
bool is4Pressed = false;
bool is5Pressed = false;
bool is6Pressed = false;
bool isCpressed = false;
bool is7Pressed = false;
bool is8Pressed = false;
bool is9Pressed = false;
bool is0Pressed = false;
bool isTpressed = false;

float faceColor[6][3] = {
  {1.0f, 0.0f, 0.0f},  // 빨강
  {0.0f, 1.0f, 0.0f},  // 초록
  {0.0f, 0.0f, 1.0f},  // 파랑
  {1.0f, 1.0f, 0.0f},  // 노랑
  {0.0f, 1.0f, 1.0f},  // 청록
  {1.0f, 0.0f, 1.0f}   // 자홍
};


random_device rd;
mt19937 gen(rd());

uniform_real_distribution<float> colorDist(0.0f, 1.0f);
uniform_int_distribution <int> FaceDist(0, 5);
uniform_int_distribution <int>PyramidDist(0, 3);

void InitBuffer(const ObjData& obj) {
    

    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(2, vbo);
    glGenBuffers(1, &ebo);

    // 위치 VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, obj.vertices.size() * sizeof(float), obj.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

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

    glGenVertexArrays(1, &pyramidVAO);
    glBindVertexArray(pyramidVAO);
    glGenBuffers(1, &pyramidVBO);
    glGenBuffers(1, &pyramidEBO);
    glBindBuffer(GL_ARRAY_BUFFER, pyramidVBO);
    glBufferData(GL_ARRAY_BUFFER, obj.vertices.size() * sizeof(float), obj.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, pyramidEBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, obj.indices.size() * sizeof(unsigned int), obj.indices.data(), GL_STATIC_DRAW);
}

bool InitShaderProgram() {
    shaderProgramID = InitShader("vertex.glsl", "fragment.glsl");

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

    window = glfwCreateWindow(width, height, "OpenGL Window", nullptr, nullptr);

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
    glEnable(GL_DEPTH_TEST);

    if (!InitShaderProgram()) {
        glfwDestroyWindow(window);
        glfwTerminate();
        return -1;
    }

    InitBuffer(obj);
    InitaxisBuffer();
    InitPyramidBuffer(pyramidobj);

    // 메인 루프
    while (!glfwWindowShouldClose(window)) {
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
    glDeleteBuffers(1, &axisvbo);
    glDeleteVertexArrays(1, &axisvao);
    glDeleteVertexArrays(1, &pyramidVAO);
    glDeleteProgram(shaderProgramID);
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

// 키보드 입력 처리
void InputProcess() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        if (!is1Pressed) {
            PyramidFaces.clear();
            selectedFaces.clear();
            selectedFaces.push_back(0);
        }
        is1Pressed = true;
    }
    else {
        is1Pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        if (!is2Pressed) {
            PyramidFaces.clear();
            selectedFaces.clear();
            selectedFaces.push_back(1);
        }
        is2Pressed = true;
    }
    else {
        is2Pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        if (!is3Pressed) {
            PyramidFaces.clear();
            selectedFaces.clear();
            selectedFaces.push_back(2);
        }
        is3Pressed = true;
    }
    else {
        is3Pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        if (!is4Pressed) {
            PyramidFaces.clear();
            selectedFaces.clear();
            selectedFaces.push_back(3);
        }
        is4Pressed = true;
    }
    else {
        is4Pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
        if (!is5Pressed) {
            PyramidFaces.clear();
            selectedFaces.clear();
            selectedFaces.push_back(4);
        }
        is5Pressed = true;
    }
    else {
        is5Pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_6) == GLFW_PRESS) {
        if (!is6Pressed) {
            PyramidFaces.clear();
            selectedFaces.clear();
            selectedFaces.push_back(5);
        }
        is6Pressed = true;
    }
    else {
        is6Pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_7) == GLFW_PRESS) {
        if (!is7Pressed) {
            selectedFaces.clear();
            PyramidFaces.clear();
            PyramidFaces.push_back(0);
        }
        is7Pressed = true;
    }
    else {
        is7Pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_8) == GLFW_PRESS) {
        if (!is8Pressed) {
            selectedFaces.clear();
            PyramidFaces.clear();
            PyramidFaces.push_back(1);
        }
        is8Pressed = true;
    }
    else {
        is8Pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_9) == GLFW_PRESS) {
        if (!is9Pressed) {
            selectedFaces.clear();
            PyramidFaces.clear();
            PyramidFaces.push_back(2);
        }
        is9Pressed = true;
    }
    else {
        is9Pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_0) == GLFW_PRESS) {
        if (!is0Pressed) {
            selectedFaces.clear();
            PyramidFaces.clear();
            PyramidFaces.push_back(3);
        }
        is0Pressed = true;
    }
    else {
        is0Pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (!isCpressed) {
            PyramidFaces.clear();
            int first = FaceDist(gen);
            int second = FaceDist(gen);
            while (second == first) {
                second = FaceDist(gen);
            }
            selectedFaces.clear();
            selectedFaces.push_back(first);
            selectedFaces.push_back(second);
        }
        isCpressed = true;
    }
    else {
        isCpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        if (!isTpressed) {
            selectedFaces.clear();
            PyramidFaces.clear();
            PyramidFaces.push_back(4);
            PyramidFaces.push_back(PyramidDist(gen));
        }
        isTpressed = true;
    }
    else {
        isTpressed = false;
    }
}

// 화면 출력
void DrawScene() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgramID);

    glm::mat4 axisModel(1.0f); // 좌표축 
   

    GLint colorLocation = glGetUniformLocation(shaderProgramID, "faceColor");

    glm::mat4 model(1.0f); //mat숫자 자료형의 이름에 따라 행렬의 크기가 달라짐을 확인, 행렬 만든 거
    GLint modelLocation = glGetUniformLocation(shaderProgramID, "modelTransform");
    
    axisModel=glm::rotate(axisModel, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));//좌표축 x축 회전
    axisModel=glm::rotate(axisModel, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));//좌표축 y축 회전
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(axisModel));//cpu에서 만든 4x4행렬을 셰이더의 uniform변수에 전달
    glUniform3f(colorLocation, 1.0f, 0.0f, 0.0f);
    glBindVertexArray(axisvao);//축용정점설정을 선택, 선택
    glDrawArrays(GL_LINES, 0, 2);//x축
    glUniform3f(colorLocation, 0.0f, 1.0f, 0.0f);
    glDrawArrays(GL_LINES, 2, 2);//y축
    glUniform3f(colorLocation, 0.0f, 0.0f, 1.0f);
    glDrawArrays(GL_LINES, 4, 2);//z축
    //여기서부터 육면체 그리기 코드
    model= glm::rotate(model, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model=glm::rotate(model, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    // rotate 회전 행렬 생성,인자 - 기존행렬, 회전 각도, 회전축
    model = glm::translate(model,glm::vec3(-0.5f, -0.5f, -0.5f));
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(model)); // 이미 만든 행렬을 셰이더로 보내는 함수
    // 만든 행렬을 uniformmatrix4fv를 통해 보낸다. 위치, 개수, 전치 여부, 데이터 주소
    glBindVertexArray(vao);



    glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
    for(int face: selectedFaces){
        glUniform3f(colorLocation, faceColor[face][0], faceColor[face][1], faceColor[face][2]);

        glDrawElements(
            GL_TRIANGLES,
            indicies,
            GL_UNSIGNED_INT,
            reinterpret_cast<void*>(
                face * indicies * sizeof(unsigned int)) //drawelement의 마지막 인자는 const void* 값으로 받아야된다.
                // 여기서는 reinterpret_cast 연산자를 사용해서 타입을 재해석 (i-1)*indicies*sizeof(unsigned int) 로 시작할 위치 받기
        );
        
    }
    // 사각뿔의 행렬과 VAO를 선택한 다음 옆면을 그린다.
    glm::mat4 pyramidmodel(1.0f);
    pyramidmodel = glm::rotate(pyramidmodel, glm::radians(30.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    pyramidmodel = glm::rotate(pyramidmodel, glm::radians(30.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    pyramidmodel = glm::translate(pyramidmodel, glm::vec3(-0.5f, -0.5f, -0.5f));
    glUniformMatrix4fv(modelLocation, 1, GL_FALSE, glm::value_ptr(pyramidmodel));
    glBindVertexArray(pyramidVAO);

    for (int face : PyramidFaces) {
        GLsizei count;
        unsigned int startIndex;
        if (face == 4) {
            count = 6;
            startIndex = 12;
        }
        else {
            count = 3;
            startIndex = face * 3;
        }

        glUniform3f(colorLocation, faceColor[face][0], faceColor[face][1], faceColor[face][2]);

        glDrawElements(
            GL_TRIANGLES,
            count,
            GL_UNSIGNED_INT,
            reinterpret_cast<void*>(
                startIndex * sizeof(unsigned int)) //drawelement의 마지막 인자는 const void* 값으로 받아야된다.
            // 여기서는 reinterpret_cast 연산자를 사용해서 타입을 재해석 (i-1)*indicies*sizeof(unsigned int) 로 시작할 위치 받기
        );

    }
    
}
