#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <vector>
#include <random>
#include "Shader.h"


using namespace std;

enum ShapeType {
    RECTANGLE,TRIANGLE,INVERTED_TRIANGLE
};

struct BoardCell {
    ShapeType type;
    float r;
    float g;
    float b;
    bool isPresent;
};

struct Player {
    ShapeType type;
    float x, y;
    float speed;    // 이동 속도 (예: 1.5f)
    float sizeW;    // 플레이어 너비 (CellW)
    float sizeH;    // 플레이어 높이 (CellH)
    float r, g, b;
};

int boardWidth, boardHeight;
float CellW, CellH;
float lastTime = 0;
float collisionTimer = 0.0f;
vector<vector<BoardCell>> board;
Player player;

random_device rd;
mt19937 gen(rd());

uniform_real_distribution<float> probDist(0.0f, 1.0f);   // 0.0 ~ 1.0 확률 체크용
uniform_int_distribution<int> shapeDist(0, 2);           // 0: RECTANGLE, 1: TRIANGLE, 2: INVERTED_TRIANGLE
uniform_real_distribution<float> colorDist(0.0f, 1.0f);  // R, G, B 색상 (0.0 ~ 1.0)
float obstacleProbability = 0.2f; // 장애물 생성 확률 (예: 0.2 = 20%)


GLint width, height;
GLFWwindow* window = nullptr;
GLuint shaderProgramID;
GLuint vao;
GLuint vbo[2];

void InitGame() {
    cout << "가로 및 세로 입력:" << endl;
    cin >> boardWidth >> boardHeight;

    CellW = 2.0f / boardWidth;
    CellH = 2.0f / boardHeight;

    board.resize(boardHeight, vector<BoardCell>(boardWidth));
    
    for (int i = 0;i < boardHeight;i++) {
        for (int j = 0; j < boardWidth;j++) {
            if (i == 0 && j == 0) {
                board[i][j].isPresent = false;
                continue;
            }
            if (probDist(gen) < obstacleProbability) {
                board[i][j].isPresent = true;
                board[i][j].type = static_cast<ShapeType>(shapeDist(gen)); // 도형 모양 랜덤 지정
                board[i][j].r = colorDist(gen);                             // R 색상 랜덤
                board[i][j].g = colorDist(gen);                             // G 색상 랜덤
                board[i][j].b = colorDist(gen);                             // B 색상 랜덤
            }
            else {
                board[i][j].isPresent = false; // 장애물 없음
            }
            
        }
    }

    player.x = -1.0f + 0.5f * CellW; // (0, 0) 셀의 NDC 중심 X
    player.y = 1.0f - 0.5f * CellH;  // (0, 0) 셀의 NDC 중심 Y
    player.type = RECTANGLE; // 문제 요구사항: 주인공 사각형
    player.speed = 0.5f;             // 이동 속도 (0 금지!)
    player.sizeW = CellW;            // 플레이어 가로 크기 (0 금지!)
    player.sizeH = CellH;            // 플레이어 세로 크기 (0 금지!)
    player.r = 0.0f;        // 주인공 기본 색상 (예: 검은색)
    player.g = 0.0f;
    player.b = 0.0f;
}
void CheckCollisionAndSwap() {
    // static 변수는 함수가 끝나도 이전 값을 유지함! (중복 스왑 방지)
    static int lastRow = -1;
    static int lastCol = -1;

    
    int col = (int)((player.x + 1.0f) / CellW);
    int row = (int)((1.0f - player.y) / CellH);

    
    if (row >= 0 && row < boardHeight && col >= 0 && col < boardWidth) {

        
        if (board[row][col].isPresent && (row != lastRow || col != lastCol)) {
            // ShapeType 스왑 연산 수행
            collisionTimer = 0.5f;
            ShapeType tempType = player.type;
            player.type = board[row][col].type;
            board[row][col].type = tempType;
            // 스왑했던 위치 기억
            lastRow = row;
            lastCol = col;
        }
        // 장애물이 없는 빈 칸으로 이동하면 중복 방지 기록 초기화
        else if (!board[row][col].isPresent) {
            lastRow = -1;
            lastCol = -1;
        }
    }
}

void DrawGrid() {
    vector<float> vertices;
    vector<float> colors;
    
    for (int j = 0; j <= boardWidth; j++) {
        float x = -1.0f + (j * CellW); //ndc 좌표 계산

        // 정점 1 (위)
        vertices.push_back(x);     vertices.push_back(1.0f);    vertices.push_back(0.0f);
        // 정점 2 (아래)
        vertices.push_back(x);     vertices.push_back(-1.0f);   vertices.push_back(0.0f);
        // 검은색 (0,0,0) 지정
        for (int k = 0; k < 2; k++) {
            colors.push_back(0.0f); colors.push_back(0.0f); colors.push_back(0.0f);
        }
    }
   
    for (int i = 0; i <= boardHeight; i++) {
        float y = 1.0f - (i * CellH);
        // 정점 1 (왼쪽)
        vertices.push_back(-1.0f);  vertices.push_back(y);       vertices.push_back(0.0f);
        // 정점 2 (오른쪽)
        vertices.push_back(1.0f);   vertices.push_back(y);       vertices.push_back(0.0f);
        // 검은색 (0,0,0) 지정
        for (int k = 0; k < 2; k++) {
            colors.push_back(0.0f); colors.push_back(0.0f); colors.push_back(0.0f);
        }
    }
    
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertices.size() * sizeof(float), vertices.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_LINES, 0, vertices.size() / 3);
}

void DrawObstacle() {
    vector <float> vertics;
    vector <float> colors;

    for (int i = 0;i < boardHeight;i++) {
        for (int j = 0; j < boardWidth; j++) {
            float x1 = -1.0f + (j * CellW);
            float x2 = -1.0f + ((j + 1) * CellW);
            float y1 = 1.0f - (i * CellH);
            float y2 = 1.0f - ((i + 1) * CellH);
            if (board[i][j].isPresent) {
                if (board[i][j].type == TRIANGLE) {
                    vertics.push_back((x1 + x2) / 2.0f);vertics.push_back(y1);vertics.push_back(0.0f); // 정점1
                    vertics.push_back(x1);vertics.push_back(y2);vertics.push_back(0.0f);//정점2
                    vertics.push_back(x2);vertics.push_back(y2);vertics.push_back(0.0f);//정점3

                    for (int k = 0; k < 3; k++) {
                        colors.push_back(board[i][j].r);
                        colors.push_back(board[i][j].g);
                        colors.push_back(board[i][j].b);
                    }

                }
                else if (board[i][j].type == INVERTED_TRIANGLE){
                    vertics.push_back(x1);vertics.push_back(y1);vertics.push_back(0.0f);//정점1
                    vertics.push_back(x2);vertics.push_back(y1);vertics.push_back(0.0f);//정점2
                    vertics.push_back((x1 + x2) / 2.0f);vertics.push_back(y2);vertics.push_back(0.0f); // 정점3
                    for (int k = 0; k < 3; k++) {
                        colors.push_back(board[i][j].r);
                        colors.push_back(board[i][j].g);
                        colors.push_back(board[i][j].b);
                    }
                }
                else {
                    vertics.push_back(x1);vertics.push_back(y1);vertics.push_back(0.0f);//정점1
                    vertics.push_back(x1);vertics.push_back(y2);vertics.push_back(0.0f);//정점2
                    vertics.push_back(x2);vertics.push_back(y2);vertics.push_back(0.0f);//정점3
                    vertics.push_back(x1);vertics.push_back(y1);vertics.push_back(0.0f);//정점4
                    vertics.push_back(x2);vertics.push_back(y2);vertics.push_back(0.0f);//정점5
                    vertics.push_back(x2);vertics.push_back(y1);vertics.push_back(0.0f);//정점6

                    for (int k = 0; k < 6; k++) {
                        colors.push_back(board[i][j].r);
                        colors.push_back(board[i][j].g);
                        colors.push_back(board[i][j].b);
                    }
                }
            }
        }
    }
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertics.size() * sizeof(float), vertics.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, vertics.size() / 3);
}

void DrawPlayer() {
    vector <float> vertics;
    vector <float> colors;

    float halfW = player.sizeW / 2.0f;     // 또는 CellW / 2.0f
    float halfH = player.sizeH / 2.0f;     // 또는 CellH / 2.0f
    float r = player.r;
    float g = player.g;
    float b = player.b;

    if (collisionTimer > 0.0f) {
        // [크기 쿵!] 스케일 적용
        float scale = 1.0f + (collisionTimer * 1.5f);
        halfW *= scale;
        halfH *= scale;

        // [무지개 사이키] 색상 연산
        float t = (float)glfwGetTime() * 30.0f;
        r = sin(t) * 0.5f + 0.5f;
        g = sin(t + 2.0f) * 0.5f + 0.5f;
        b = sin(t + 4.0f) * 0.5f + 0.5f;
    }

    float x1 = player.x - halfW;
    float x2 = player.x + halfW;
    float y1 = player.y + halfH;
    float y2 = player.y - halfH;
   
    if (player.type == TRIANGLE) {
        vertics.push_back((x1 + x2) / 2.0f);vertics.push_back(y1);vertics.push_back(0.0f); // 정점1
        vertics.push_back(x1);vertics.push_back(y2);vertics.push_back(0.0f);//정점2
        vertics.push_back(x2);vertics.push_back(y2);vertics.push_back(0.0f);//정점3

        for (int i = 0; i < 3;i++) {
            colors.push_back(r);
            colors.push_back(g);
            colors.push_back(b);
        }
    }
    else if (player.type == INVERTED_TRIANGLE) {
        vertics.push_back(x1);vertics.push_back(y1);vertics.push_back(0.0f);//정점1
        vertics.push_back(x2);vertics.push_back(y1);vertics.push_back(0.0f);//정점2
        vertics.push_back((x1 + x2) / 2.0f);vertics.push_back(y2);vertics.push_back(0.0f); // 정점3

        for (int i = 0; i < 3;i++) {
            colors.push_back(r);
            colors.push_back(g);
            colors.push_back(b);
        }
    }
    else { // RECTANGLE
        vertics.push_back(x1);vertics.push_back(y1);vertics.push_back(0.0f);//정점1
        vertics.push_back(x1);vertics.push_back(y2);vertics.push_back(0.0f);//정점2
        vertics.push_back(x2);vertics.push_back(y2);vertics.push_back(0.0f);//정점3
        vertics.push_back(x1);vertics.push_back(y1);vertics.push_back(0.0f);//정점4
        vertics.push_back(x2);vertics.push_back(y2);vertics.push_back(0.0f);//정점5
        vertics.push_back(x2);vertics.push_back(y1);vertics.push_back(0.0f);//정점6
        for (int i = 0; i < 6;i++) {
            colors.push_back(r);
            colors.push_back(g);
            colors.push_back(b);
        }
    }


    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    glBufferData(GL_ARRAY_BUFFER, vertics.size() * sizeof(float), vertics.data(), GL_DYNAMIC_DRAW);
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
    glBufferData(GL_ARRAY_BUFFER, colors.size() * sizeof(float), colors.data(), GL_DYNAMIC_DRAW);
    glDrawArrays(GL_TRIANGLES, 0, vertics.size() / 3);
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

void InputProcess(float deltaTime);
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

        float currentTime =(float)glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        if (collisionTimer > 0.0f) {
            collisionTimer -= deltaTime;
        }

        InputProcess(deltaTime);
        CheckCollisionAndSwap();
        DrawScene();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
}

void InputProcess(float deltaTime) {
    //[Q] 키 종료
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    float halfW = player.sizeW / 2.0f;
    float halfH = player.sizeH / 2.0f;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        if (player.y + halfH < 1.0f) { // 상단 화면 경계 체크
            player.y += player.speed * deltaTime;
        }
    }
   

    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (player.y - halfH > -1.0f) { // 하단 화면 경계 체크
            player.y -= player.speed * deltaTime;
        }
    }
    

    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        if (player.x - halfW > -1.0f) { // 좌측 화면 경계 체크
            player.x -= player.speed * deltaTime;
        }
    }
    

    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        if (player.x + halfW < 1.0f) { // 우측 화면 경계 체크
            player.x += player.speed * deltaTime;
        }
    }
}



void DrawScene() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgramID);
    glBindVertexArray(vao);

    DrawGrid();
    DrawObstacle();
    DrawPlayer();
}