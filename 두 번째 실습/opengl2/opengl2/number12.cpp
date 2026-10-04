#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include "Shader.h"
#include <vector>

using namespace std;


GLint width, height;
GLFWwindow* window = nullptr;

GLuint shaderProgramID;
GLuint vao;
GLuint vbo[2];
GLuint ebo;

float lastTime = 0.0f;

// 두 사각형은 같은 높이에서 시작
float pos1y = 0.0f;
float pos2y = 0.0f;

// 서로 다른 속도
float speed1 = 0.30f;
float speed2 = 0.50f;

const float RECT_HALF_W = 0.10f;
const float RECT_HALF_H = 0.07f;

const float LEFT_X = -0.45f;
const float RIGHT_X = 0.15f;

// 같은 높이에서 시작
const float START_Y = 0.82f;


const float TARGET_TOP = 0.15f;
const float TARGET_BOTTOM = -0.15f;


// 화면 오른쪽
const float STACK_CENTER_X = 0.72f;

// 성공한 두 사각형은 가로로 나란히
const float STACK_LEFT_X = STACK_CENTER_X - 0.11f;
const float STACK_RIGHT_X = STACK_CENTER_X + 0.11f;

// 첫 성공은 화면 오른쪽 아래
const float STACK_START_Y = -0.82f;

// 다음 성공은 바로 위에
const float STACK_GAP = 0.15f;


// 0.0 -> 성공한 위치
// 1.0 -> 오른쪽 적재 위치
float moveProgress = 0.0f;

// 이동 속도
float moveSpeed = 0.75f;

// 성공한 순간 두 사각형의 Y 위치
float successY1 = 0.0f;
float successY2 = 0.0f;

// 현재 성공 쌍이 이동할 목표 Y
float targetStackY = 0.0f;


bool isMoving = true;
bool isStackMoving = false;

bool isEnterpressed = false;
bool isRpressed = false;


vector<float> stackY;



void InitBuffer();
void InputProcess();
void DrawScene();



void AddRectangle(
    vector<float>& pos,
    vector<float>& color,
    vector<unsigned int>& Index,
    float centerX,
    float centerY,
    float r,
    float g,
    float b
) {
    unsigned int base = static_cast<unsigned int>(pos.size() / 3);

    // 왼쪽 아래
    pos.push_back(centerX - RECT_HALF_W);
    pos.push_back(centerY - RECT_HALF_H);
    pos.push_back(0.0f);

    // 왼쪽 위
    pos.push_back(centerX - RECT_HALF_W);
    pos.push_back(centerY + RECT_HALF_H);
    pos.push_back(0.0f);

    // 오른쪽 위
    pos.push_back(centerX + RECT_HALF_W);
    pos.push_back(centerY + RECT_HALF_H);
    pos.push_back(0.0f);

    // 오른쪽 아래
    pos.push_back(centerX + RECT_HALF_W);
    pos.push_back(centerY - RECT_HALF_H);
    pos.push_back(0.0f);

    // 색상
    for (int i = 0; i < 4; i++) {
        color.push_back(r);
        color.push_back(g);
        color.push_back(b);
    }

    // 삼각형 1
    Index.push_back(base);
    Index.push_back(base + 1);
    Index.push_back(base + 2);

    // 삼각형 2
    Index.push_back(base);
    Index.push_back(base + 2);
    Index.push_back(base + 3);
}


void AddOutlineBox(
    vector<float>& pos,
    vector<float>& color,
    vector<unsigned int>& Index,
    float left,
    float right,
    float bottom,
    float top
) {
    float thickness = 0.008f;

    float r = 0.35f;
    float g = 0.50f;
    float b = 0.70f;

    auto AddLine = [&](float x1, float y1, float x2, float y2) {
        unsigned int base = static_cast<unsigned int>(pos.size() / 3);

        pos.push_back(x1);
        pos.push_back(y1);
        pos.push_back(0.0f);

        pos.push_back(x1);
        pos.push_back(y2);
        pos.push_back(0.0f);

        pos.push_back(x2);
        pos.push_back(y2);
        pos.push_back(0.0f);

        pos.push_back(x2);
        pos.push_back(y1);
        pos.push_back(0.0f);

        for (int i = 0; i < 4; i++) {
            color.push_back(r);
            color.push_back(g);
            color.push_back(b);
        }

        Index.push_back(base);
        Index.push_back(base + 1);
        Index.push_back(base + 2);

        Index.push_back(base);
        Index.push_back(base + 2);
        Index.push_back(base + 3);
        };

    // 왼쪽
    AddLine(left, bottom, left + thickness, top);

    // 오른쪽
    AddLine(right - thickness, bottom, right, top);

    // 위
    AddLine(left, top - thickness, right, top);

    // 아래
    AddLine(left, bottom, right, bottom + thickness);
}

void AddTargetArea(
    vector<float>& pos,
    vector<float>& color,
    vector<unsigned int>& Index
) {
    float left = -0.72f;
    float right = 0.42f;

    float thickness = 0.008f;

    float r = 0.30f;
    float g = 0.45f;
    float b = 0.65f;

    auto AddLine = [&](float x1, float y1, float x2, float y2) {
        unsigned int base = static_cast<unsigned int>(pos.size() / 3);

        pos.push_back(x1);
        pos.push_back(y1);
        pos.push_back(0.0f);

        pos.push_back(x1);
        pos.push_back(y2);
        pos.push_back(0.0f);

        pos.push_back(x2);
        pos.push_back(y2);
        pos.push_back(0.0f);

        pos.push_back(x2);
        pos.push_back(y1);
        pos.push_back(0.0f);

        for (int i = 0; i < 4; i++) {
            color.push_back(r);
            color.push_back(g);
            color.push_back(b);
        }

        Index.push_back(base);
        Index.push_back(base + 1);
        Index.push_back(base + 2);

        Index.push_back(base);
        Index.push_back(base + 2);
        Index.push_back(base + 3);
        };

    // 위
    AddLine(left, TARGET_TOP, right, TARGET_TOP + thickness);

    // 아래
    AddLine(left, TARGET_BOTTOM - thickness, right, TARGET_BOTTOM);

    // 왼쪽
    AddLine(left, TARGET_BOTTOM, left + thickness, TARGET_TOP);

    // 오른쪽
    AddLine(right - thickness, TARGET_BOTTOM, right, TARGET_TOP);
}



void InitBuffer() {
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glGenBuffers(2, vbo);
    glGenBuffers(1, &ebo);

    // 위치 VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // 색상 VBO
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(1);

    // EBO
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}



int main() {
    width = 1280;
    height = 900;

    if (!glfwInit()) {
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    window = glfwCreateWindow(width, height, "Rectangle Matching", nullptr, nullptr);

    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        return -1;
    }

    glViewport(0, 0, width, height);

    shaderProgramID = InitShader("vertex.glsl", "fragment.glsl");

    InitBuffer();

    if (shaderProgramID != 0) {
        cout << "셰이더 프로그램 초기화 성공" << endl;
    }
    else {
        cout << "셰이더 프로그램 초기화 실패" << endl;
    }

    lastTime = (float)glfwGetTime();

    while (!glfwWindowShouldClose(window)) {
        float currentTime = (float)glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;


        if (isMoving) {
            pos1y += deltaTime * speed1;
            pos2y += deltaTime * speed2;

            float currentY1 = START_Y - pos1y;
            float currentY2 = START_Y - pos2y;

            // 첫 번째 사각형이 통로 아래로 완전히 나갔을 때
            if (currentY1 + RECT_HALF_H < -0.90f) {
                pos1y = 0.0f;
            }

            // 두 번째 사각형
            if (currentY2 + RECT_HALF_H < -0.90f) {
                pos2y = 0.0f;
            }
        }


        if (isStackMoving) {
            moveProgress += deltaTime * moveSpeed;

            if (moveProgress >= 1.0f) {
                moveProgress = 1.0f;

                // 최종 위치에 도착했으므로 이번 성공 결과를 저장
                stackY.push_back(targetStackY);

                // 다음 라운드
                pos1y = 0.0f;
                pos2y = 0.0f;

                successY1 = 0.0f;
                successY2 = 0.0f;

                moveProgress = 0.0f;

                isStackMoving = false;
                isMoving = true;
            }
        }

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

    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }


    if (glfwGetKey(window, GLFW_KEY_ENTER) == GLFW_PRESS) {
        if (!isEnterpressed && isMoving) {
            float currentY1 = START_Y - pos1y;
            float currentY2 = START_Y - pos2y;

            // 첫 번째 사각형
            float top1 = currentY1 + RECT_HALF_H;
            float bottom1 = currentY1 - RECT_HALF_H;

            // 두 번째 사각형
            float top2 = currentY2 + RECT_HALF_H;
            float bottom2 = currentY2 - RECT_HALF_H;

            bool rect1Inside = top1 <= TARGET_TOP && bottom1 >= TARGET_BOTTOM;
            bool rect2Inside = top2 <= TARGET_TOP && bottom2 >= TARGET_BOTTOM;

            if (rect1Inside && rect2Inside) {
                cout << "성공!" << endl;

                // 성공 순간 위치 기억
                successY1 = currentY1;
                successY2 = currentY2;

                targetStackY = STACK_START_Y + static_cast<float>(stackY.size()) * STACK_GAP;

                // 아래 이동 정지
                isMoving = false;

                // 오른쪽 적재 위치로 이동 시작
                isStackMoving = true;
                moveProgress = 0.0f;
            }
            else {
                cout << "실패" << endl;
            }

            isEnterpressed = true;
        }
    }
    else {
        isEnterpressed = false;
    }

    // R : 전체 리셋

    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        if (!isRpressed) {
            pos1y = 0.0f;
            pos2y = 0.0f;

            successY1 = 0.0f;
            successY2 = 0.0f;

            targetStackY = 0.0f;
            moveProgress = 0.0f;

            stackY.clear();

            isMoving = true;
            isStackMoving = false;

            lastTime = (float)glfwGetTime();

            isRpressed = true;
        }
    }
    else {
        isRpressed = false;
    }
}

// 화면

void DrawScene() {

    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgramID);
    glBindVertexArray(vao);

    vector<float> pos;
    vector<float> color;
    vector<unsigned int> Index;

    // 두 개의 세로 통로


    AddOutlineBox(
        pos, color, Index,
        LEFT_X - 0.16f, LEFT_X + 0.16f,
        -0.90f, 0.95f
    );

    AddOutlineBox(
        pos, color, Index,
        RIGHT_X - 0.16f, RIGHT_X + 0.16f,
        -0.90f, 0.95f
    );

    // 가운데 판정 영역


    AddTargetArea(pos, color, Index);

    // 이미 성공해서 쌓여 있는 사각형


    for (size_t k = 0; k < stackY.size(); k++) {
        float y = stackY[k];

        // 성공한 첫 번째 사각형
        AddRectangle(
            pos, color, Index,
            STACK_LEFT_X, y,
            0.15f, 0.45f, 0.80f
        );

        // 성공한 두 번째 사각형 (같은 높이에 나란히 배치)
        AddRectangle(
            pos, color, Index,
            STACK_RIGHT_X, y,
            0.90f, 0.85f, 0.10f
        );
    }

    // 현재 사각형


    if (isMoving) {
        float currentY1 = START_Y - pos1y;
        float currentY2 = START_Y - pos2y;

        // 왼쪽 통로
        AddRectangle(
            pos, color, Index,
            LEFT_X, currentY1,
            0.15f, 0.45f, 0.80f
        );

        // 오른쪽 통로
        AddRectangle(
            pos, color, Index,
            RIGHT_X, currentY2,
            0.90f, 0.85f, 0.10f
        );
    }

    // 성공 후 이동 애니메이션


    if (isStackMoving) {
        // 선형 보간: 현재 위치 = 시작 위치 + (목표 - 시작) * 진행률
        float movingX1 = LEFT_X + (STACK_LEFT_X - LEFT_X) * moveProgress;
        float movingY1 = successY1 + (targetStackY - successY1) * moveProgress;

        float movingX2 = RIGHT_X + (STACK_RIGHT_X - RIGHT_X) * moveProgress;
        float movingY2 = successY2 + (targetStackY - successY2) * moveProgress;

        // 첫 번째
        AddRectangle(
            pos, color, Index,
            movingX1, movingY1,
            0.15f, 0.45f, 0.80f
        );

        // 두 번째
        AddRectangle(
            pos, color, Index,
            movingX2, movingY2,
            0.90f, 0.85f, 0.10f
        );
    }

    // GPU 데이터 전송

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(
        GL_ARRAY_BUFFER,
        pos.size() * sizeof(float),
        pos.data(),
        GL_DYNAMIC_DRAW
    );

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(
        GL_ARRAY_BUFFER,
        color.size() * sizeof(float),
        color.data(),
        GL_DYNAMIC_DRAW
    );

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(
        GL_ELEMENT_ARRAY_BUFFER,
        Index.size() * sizeof(unsigned int),
        Index.data(),
        GL_DYNAMIC_DRAW
    );

    glDrawElements(
        GL_TRIANGLES,
        static_cast<GLsizei>(Index.size()),
        GL_UNSIGNED_INT,
        0
    );
}