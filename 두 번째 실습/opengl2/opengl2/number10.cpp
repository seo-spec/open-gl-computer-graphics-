#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <vector>
#include <random>
#include "Shader.h"


using namespace std;

bool isLeftPressed = false;
bool isRpressed = false;

int selectIndex = -1;
float prevX = 0.0f;
float prevY = 0.0f;

enum Shapetype {
    Square = 0,
    Eql_Tri,
    Right_Tri
};

struct Piece {
    float x, y;// 그려져야될 위치
    float size;
    float r, g, b;
    bool isDragging;
    bool isFixed;
    Shapetype type;
    int dir;
};

struct Slot {
    float x, y;
    float size;
    bool isfilled = false;
    int Matchindex = -1;
    Shapetype requiredType;
    int dir = 0; // 필요한 도형의 방향
};

struct Plate {
    vector<Slot>slots;
    bool isCompleted = false;
};

vector<Piece>pieces;
Plate plates;


random_device rd;
mt19937 gen(rd());

uniform_real_distribution <float> posDistX(-0.9f, -0.1f);
uniform_real_distribution <float> posDistY(-0.8f, 0.8f);
uniform_real_distribution <float> sizeDist(0.05f, 0.2f);
uniform_real_distribution <float> ColorDist(0.0f, 1.0f);
uniform_int_distribution <int> CountDist(1, 7);


GLint width, height;
GLFWwindow* window = nullptr;
GLuint shaderProgramID;
GLuint vao;
GLuint vbo[2];

void InitGame() {
    pieces.clear();
    plates.slots.clear();
    plates.isCompleted = false;

    // 1. 모양판 1: 2x2 사각형 4개 (우측 상단)
    plates.slots.push_back({ 0.42f, 0.73f, 0.08f, false, -1, Square, 0 });
    plates.slots.push_back({ 0.58f, 0.73f, 0.08f, false, -1, Square, 0 });
    plates.slots.push_back({ 0.42f, 0.57f, 0.08f, false, -1, Square, 0 });
    plates.slots.push_back({ 0.58f, 0.57f, 0.08f, false, -1, Square, 0 });

    // 2. 모양판 2: 바람개비/나비 모양 정삼각형 4개 (우측 중앙)
    // 중심: (0.50, 0.25), 각 방향(0: 아래쪽 향함, 1: 위쪽 향함, 2: 왼쪽 향함, 3: 오른쪽 향함)
    plates.slots.push_back({ 0.50f, 0.35f, 0.08f, false, -1, Eql_Tri, 0 }); // 위쪽 슬롯 (꼭짓점 아래로)
    plates.slots.push_back({ 0.50f, 0.15f, 0.08f, false, -1, Eql_Tri, 1 }); // 아래쪽 슬롯 (꼭짓점 위로)
    plates.slots.push_back({ 0.40f, 0.25f, 0.08f, false, -1, Eql_Tri, 2 }); // 왼쪽 슬롯 (꼭짓점 오른쪽으로)
    plates.slots.push_back({ 0.60f, 0.25f, 0.08f, false, -1, Eql_Tri, 3 }); // 오른쪽 슬롯 (꼭짓점 왼쪽으로)

    // 3. 모양판 3: 직각삼각형 2개로 만든 세로 직사각형 (우측 하단)
    // 중심: (0.50, -0.30), sizeX = 0.10f, sizeY = 0.20f
    plates.slots.push_back({ 0.50f, -0.30f, 0.10f, false, -1, Right_Tri, 0 }); // 대각선 상단/좌측 삼각
    plates.slots.push_back({ 0.50f, -0.30f, 0.10f, false, -1, Right_Tri, 1 }); // 대각선 하단/우측 삼각

    //4. 모양판 4
    plates.slots.push_back({ 0.80f, 0.57f, 0.08f, false, -1, Square,  0 }); // 몸통 사각형 (Y: 0.50f ~ 0.64f)
    plates.slots.push_back({ 0.80f, 0.73f, 0.08f, false, -1, Eql_Tri, 1 }); // 지붕 정삼각형 (Y: 0.64f ~ 0.78f)
    plates.slots.push_back({ 0.80f, 0.41f, 0.08f, false, -1, Eql_Tri, 0 }); // 위쪽 슬롯 (꼭짓점 아래로)\

    //5. 모양판 
    // 상단 정삼각형 (지붕, Y: 0.03f ~ 0.17f)
    plates.slots.push_back({ 0.80f,  0.10f, 0.07f, false, -1, Eql_Tri, 1 });

    // 중단 사각형 (몸통, Y: -0.11f ~ 0.03f)
    plates.slots.push_back({ 0.80f, -0.04f, 0.07f, false, -1, Square,  0 });

    // 하단 직각삼각형 2개 (받침대, Y: -0.25f)
    // 직각삼각형 2개가 합쳐져 세로 높이 0.28f(Y: -0.39f ~ -0.11f)의 받침대가 됨
    plates.slots.push_back({ 0.80f, -0.25f, 0.07f, false, -1, Right_Tri, 0 });
    plates.slots.push_back({ 0.80f, -0.25f, 0.07f, false, -1, Right_Tri, 1 });

    for (size_t i = 0; i < plates.slots.size();i++) {
        Piece piece;
        piece.x = posDistX(gen);
        piece.y = posDistY(gen);
        
        piece.size = plates.slots[i].size;
        piece.type = plates.slots[i].requiredType;
        piece.dir = plates.slots[i].dir;

        piece.r = ColorDist(gen);
        piece.g = ColorDist(gen);
        piece.b = ColorDist(gen);
        piece.isDragging = false;
        piece.isFixed = false;

        pieces.push_back(piece);
    }
}


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
    InitGame();

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
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        float mx = (xpos / width) * 2.0f - 1.0f;
        float my = 1.0f - (ypos / height) * 2.0f;
       
        if (!isLeftPressed) {
            for (int k = pieces.size() - 1;k >= 0;k--) {
                if (pieces[k].isFixed)continue;
                float s = pieces[k].size;
                float px = pieces[k].x;
                float py = pieces[k].y;
                
                float halfW = s;
                float halfH = s;

                if (pieces[k].type == Right_Tri) {
                    halfH = s * 2.0f;
                }

                
                if (mx >= px - halfW && mx <= px + halfW &&
                    my >= py - halfH && my <= py + halfH)
                {
                    pieces[k].isDragging = true;
                    selectIndex = k;
                    break; // 맨 위 1개만 잡고 루프 탈출
                }
            }

            prevX = mx;
            prevY = my;
            isLeftPressed = true;
        }
        else {
            float deltaX = mx - prevX;
            float deltaY = my - prevY;

            if (selectIndex != -1) {
                pieces[selectIndex].x += deltaX;
                pieces[selectIndex].y += deltaY;
            }

            prevX = mx;
            prevY = my;
        }
    }
    else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && isLeftPressed) {
        if (selectIndex != -1) {
            for (int i = 0; i < plates.slots.size();i++) {
                auto& slots = plates.slots[i];
                bool isSameSize = (abs(pieces[selectIndex].size - slots.size) < 0.001f);

                if (!slots.isfilled && pieces[selectIndex].type == slots.requiredType
                    && pieces[selectIndex].dir == slots.dir&&isSameSize) {

                    float dx = pieces[selectIndex].x - slots.x;
                    float dy = pieces[selectIndex].y - slots.y;

                    float dist = dx * dx + dy * dy;
                    if (dist < 0.0064f) {
                        pieces[selectIndex].x = slots.x;
                        pieces[selectIndex].y = slots.y;
                        pieces[selectIndex].isFixed = true;
                        slots.isfilled = true;

                        break;
                    }
                    
                }
            }
            pieces[selectIndex].isDragging = false;
            selectIndex = -1;
        }
        
        isLeftPressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        if (!isRpressed) {
            InitGame();
            isRpressed = true;
        }
    }
    else {
        isRpressed = false;
    }
}

void DrawRect(float x,float y, float size, float r, float g, float b) {
    float posData[] = {
        x - size,y - size,0.0f,
        x + size,y - size,0.0f,
        x - size,y + size,0.0f,

        x + size,y - size,0.0f,
        x + size,y + size,0.0f,
        x - size,y + size,0.0f
    };

    float ColorData[] = {
        r,g,b, r,g,b, r,g,b,
        r,g,b, r,g,b, r,g,b
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(posData), posData, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ColorData), ColorData, GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, 6);
}

void DrawequalTri(float x, float y, float size, float r, float g, float b, int dir = 0) {
    float posData[9];

    if (dir == 0) { // 꼭짓점 아래로
        posData[0] = x - size; posData[1] = y + size; posData[2] = 0.0f;
        posData[3] = x + size; posData[4] = y + size; posData[5] = 0.0f;
        posData[6] = x;        posData[7] = y - size; posData[8] = 0.0f;
    }
    else if (dir == 1) { // 꼭짓점 위로 (기본)
        posData[0] = x - size; posData[1] = y - size; posData[2] = 0.0f;
        posData[3] = x + size; posData[4] = y - size; posData[5] = 0.0f;
        posData[6] = x;        posData[7] = y + size; posData[8] = 0.0f;
    }
    else if (dir == 2) { // 꼭짓점 오른쪽으로
        posData[0] = x - size; posData[1] = y + size; posData[2] = 0.0f;
        posData[3] = x - size; posData[4] = y - size; posData[5] = 0.0f;
        posData[6] = x + size; posData[7] = y;        posData[8] = 0.0f;
    }
    else if (dir == 3) { // 꼭짓점 왼쪽으로
        posData[0] = x + size; posData[1] = y - size; posData[2] = 0.0f;
        posData[3] = x + size; posData[4] = y + size; posData[5] = 0.0f;
        posData[6] = x - size; posData[7] = y;        posData[8] = 0.0f;
    }

    float ColorData[] = {
        r,g,b, r,g,b, r,g,b
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(posData), posData, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ColorData), ColorData, GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void DrawRightTriangle(float x, float y, float size, float r, float g, float b, int dir = 0) {
    float posData[9];
    float h = size * 2.0f; // 세로 길이 2*size

    if (dir == 0) { // 직사각형의 좌상단 반쪽 직각삼각형 (대각선 좌하->우상)
        posData[0] = x - size; posData[1] = y - h; posData[2] = 0.0f;
        posData[3] = x + size; posData[4] = y + h; posData[5] = 0.0f;
        posData[6] = x - size; posData[7] = y + h; posData[8] = 0.0f;
    }
    else { // 직사각형의 우하단 반쪽 직각삼각형
        posData[0] = x - size; posData[1] = y - h; posData[2] = 0.0f;
        posData[3] = x + size; posData[4] = y - h; posData[5] = 0.0f;
        posData[6] = x + size; posData[7] = y + h; posData[8] = 0.0f;
    }

    float ColorData[] = {
        r,g,b, r,g,b, r,g,b
    };

    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(posData), posData, GL_DYNAMIC_DRAW);

    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, sizeof(ColorData), ColorData, GL_DYNAMIC_DRAW);

    glDrawArrays(GL_TRIANGLES, 0, 3);
}

void DrawPlate() {
    for (size_t i = 0; i < plates.slots.size(); i++) {
        const auto& slot = plates.slots[i];

        if (slot.requiredType == Square) {
            float x = slot.x, y = slot.y, size = slot.size;
            // 정점 4개 (좌하 -> 우하 -> 우상 -> 좌상)
            float posData[] = {
                x - size, y - size, 0.0f,
                x + size, y - size, 0.0f,
                x + size, y + size, 0.0f,
                x - size, y + size, 0.0f
            };
            float ColorData[] = {
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f
            };
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(posData), posData, GL_DYNAMIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(ColorData), ColorData, GL_DYNAMIC_DRAW);

            // GL_LINE_LOOP로 외곽선 4개 선분만 연결 (대각선 완전히 사라짐)
            glDrawArrays(GL_LINE_LOOP, 0, 4);
        }
        else if (slot.requiredType == Eql_Tri) {
            float x = slot.x, y = slot.y, size = slot.size;
            int dir = slot.dir;
            float posData[9];

            if (dir == 0) {
                posData[0] = x - size; posData[1] = y + size; posData[2] = 0.0f;
                posData[3] = x + size; posData[4] = y + size; posData[5] = 0.0f;
                posData[6] = x;        posData[7] = y - size; posData[8] = 0.0f;
            }
            else if (dir == 1) {
                posData[0] = x - size; posData[1] = y - size; posData[2] = 0.0f;
                posData[3] = x + size; posData[4] = y - size; posData[5] = 0.0f;
                posData[6] = x;        posData[7] = y + size; posData[8] = 0.0f;
            }
            else if (dir == 2) {
                posData[0] = x - size; posData[1] = y + size; posData[2] = 0.0f;
                posData[3] = x - size; posData[4] = y - size; posData[5] = 0.0f;
                posData[6] = x + size; posData[7] = y;        posData[8] = 0.0f;
            }
            else if (dir == 3) {
                posData[0] = x + size; posData[1] = y - size; posData[2] = 0.0f;
                posData[3] = x + size; posData[4] = y + size; posData[5] = 0.0f;
                posData[6] = x - size; posData[7] = y;        posData[8] = 0.0f;
            }

            float ColorData[] = {
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f
            };

            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(posData), posData, GL_DYNAMIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(ColorData), ColorData, GL_DYNAMIC_DRAW);

            glDrawArrays(GL_LINE_LOOP, 0, 3);
        }
        else if (slot.requiredType == Right_Tri) {
            float x = slot.x, y = slot.y, size = slot.size;
            int dir = slot.dir;
            float h = size * 2.0f;
            float posData[9];

            if (dir == 0) {
                posData[0] = x - size; posData[1] = y - h; posData[2] = 0.0f;
                posData[3] = x + size; posData[4] = y + h; posData[5] = 0.0f;
                posData[6] = x - size; posData[7] = y + h; posData[8] = 0.0f;
            }
            else {
                posData[0] = x - size; posData[1] = y - h; posData[2] = 0.0f;
                posData[3] = x + size; posData[4] = y - h; posData[5] = 0.0f;
                posData[6] = x + size; posData[7] = y + h; posData[8] = 0.0f;
            }

            float ColorData[] = {
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f,
                0.0f, 0.0f, 1.0f
            };

            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(posData), posData, GL_DYNAMIC_DRAW);

            glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
            glBufferData(GL_ARRAY_BUFFER, sizeof(ColorData), ColorData, GL_DYNAMIC_DRAW);

            glDrawArrays(GL_LINE_LOOP, 0, 3);
        }
    }
}



void DrawScene() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgramID);
    glBindVertexArray(vao);

    DrawPlate();

    for (size_t i = 0; i < pieces.size(); i++) {
        const auto& piece = pieces[i];

        if (piece.type == Square) {
            DrawRect(piece.x, piece.y, piece.size, piece.r, piece.g, piece.b);
        }
        else if (piece.type == Eql_Tri) {
            DrawequalTri(piece.x, piece.y, piece.size, piece.r, piece.g, piece.b, piece.dir);
        }
        else if (piece.type == Right_Tri) {
            DrawRightTriangle(piece.x, piece.y, piece.size, piece.r, piece.g, piece.b, piece.dir);
        }
    }
}