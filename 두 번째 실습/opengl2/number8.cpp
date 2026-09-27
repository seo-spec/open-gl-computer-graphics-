#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <vector>
#include <random>
#include "Shader.h"

using namespace std;

struct Shape {
	float x, y; // 도형의 중심 좌표
	float r, g, b; // 도형의 색상
	float size; // 도형의 크기
};

Shape fourShapes[4]; // 4개의 도형 저장
bool hasShapes[4] = {false}; // 도형 존재 여부

random_device rd;
mt19937 gen(rd());

uniform_real_distribution <float> ColorDist(0.0f, 1.0f); // 색상 범위
uniform_real_distribution <float> SizeDist(0.05f, 0.1f);

GLint width, height;
GLFWwindow* window = nullptr;
GLuint shaderProgramID;
GLuint vao;
GLuint vbo[2];

bool isLeftMousePressed = false; // 마우스 왼쪽 버튼 상태
bool isRightMousePressed = false; // 마우스 오른쪽 버튼 상태
bool isApressed = false; // a버튼 상태
bool isBpressed = false; // b버튼 상태
bool isCpressed = false; // c버튼 상태


void InitBuffer() {
    // 1. VAO 생성 및 바인딩
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // 2. VBO 2개 생성
    glGenBuffers(2, vbo);

    // --- 0번 VBO: 위치 속성 (location = 0)
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // --- 1번 VBO: 색상 속성 (location = 1)
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);
}

void InputProcess();
void DrawScene();

int main() {
    width = 1280;
    height = 900;

    if (!glfwInit()) return -1;

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
    if (glewInit() != GLEW_OK) return -1;

    glViewport(0, 0, width, height);

    shaderProgramID = InitShader("vertex.glsl", "fragment.glsl");
    InitBuffer();

    if (shaderProgramID != 0) {
        cout << "셰이더 프로그램 초기화 성공" << endl;
    }
    else {
        cout << "셰이더 프로그램 초기화 실패" << endl;
    }

    while (!glfwWindowShouldClose(window)) {
        InputProcess();
        DrawScene();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void InputProcess() {
    //[Q] 키 종료
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if(glfwGetMouseButton(window,GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!isLeftMousePressed) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            float mx = (xpos / width) * 2.0f - 1.0f;
            float my = 1.0f - (ypos / height) * 2.0f;
            if (mx >= 0.0f && my >= 0.0f) {
                fourShapes[0].x = mx;
                fourShapes[0].y = my;
                fourShapes[0].size = SizeDist(gen);
                fourShapes[0].r = ColorDist(gen);
                fourShapes[0].g = ColorDist(gen);
                fourShapes[0].b = ColorDist(gen);
                hasShapes[0] = true;
            }
            else if (mx < 0.0f && my>=0.0f) {
                fourShapes[1].x = mx;
                fourShapes[1].y = my;
                fourShapes[1].size = SizeDist(gen);
                fourShapes[1].r = ColorDist(gen);
                fourShapes[1].g = ColorDist(gen);
                fourShapes[1].b = ColorDist(gen);
                hasShapes[1] = true;
            }
            else if (mx<0.0f&&my<0.0f){
                fourShapes[2].x = mx;
                fourShapes[2].y = my;
                fourShapes[2].size = SizeDist(gen);
                fourShapes[2].r = ColorDist(gen);
                fourShapes[2].g = ColorDist(gen);
                fourShapes[2].b = ColorDist(gen);
                hasShapes[2] = true;
            }
            else {
                fourShapes[3].x = mx;
                fourShapes[3].y = my;
                fourShapes[3].size = SizeDist(gen);
                fourShapes[3].r = ColorDist(gen);
                fourShapes[3].g = ColorDist(gen);
                fourShapes[3].b = ColorDist(gen);
                hasShapes[3] = true;
            }
        }
        isLeftMousePressed = true;
    }
    else {
        isLeftMousePressed = false;
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        int index = -1;
        double xpos, ypos;
        if (!isRightMousePressed) {
            glfwGetCursorPos(window, &xpos, &ypos);
            float mx = (xpos / width) * 2.0f - 1.0f;
            float my = 1.0f - (ypos / height) * 2.0f;
            if (mx >= 0.0f && my >= 0.0f) {
                index = 0;
            }
            else if (mx < 0.0f && my >= 0.0f) {
                index = 1;
            }
            else if (mx < 0.0f && my < 0.0f) {
                index = 2;
            }
            else {
                index = 3;
            }
            if (hasShapes[index] == true) {
                fourShapes[index].size = SizeDist(gen);
            }
        }
        isRightMousePressed = true;
    }
    else {
        isRightMousePressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        if (!isApressed) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        isApressed = true;
    }
    else {
        isApressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        if (!isBpressed) {
            glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        }
        isBpressed = true;
    }
    else {
        isBpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (!isCpressed) {
            for (int i = 0;i < 4;i++) {
                hasShapes[i] = false;
            }
        }
        isCpressed = true;
    }
    else {
        isCpressed = false;
    }
}

void DrawLine() {
    float posData[] = {
      -1.0f,0.0f,0.0f,
      1.0f,0.0f,0.0f,
      0.0f,-1.0f,0.0f,
      0.0f,1.0f,0.0f
    };
    float colorData[] = {
        0.0f,0.0f,0.0f,
        0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f,
		0.0f,0.0f,0.0f
    };
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(posData), posData, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_DYNAMIC_DRAW);
    glLineWidth(2.0f);
    glDrawArrays(GL_LINES, 0, 4);
}

void DrawTriangle(float x, float y, float size, float r, float g, float b) {
    float posData[]{
        x - size,y - size,0.0f,
        x + size,y - size,0.0f,
        x,y + 3*size,0.0f
    };
    float ColorData[]{
        r,g,b,
        r,g,b,
        r,g,b
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(posData), posData, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ColorData), ColorData, GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void DrawScene() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgramID);
    glBindVertexArray(vao);
    DrawLine();
    for (int i = 0;i < 4;i++) {
        if (hasShapes[i]) {
            DrawTriangle(fourShapes[i].x, fourShapes[i].y, fourShapes[i].size,
                fourShapes[i].r, fourShapes[i].g, fourShapes[i].b);
        }
    }
}