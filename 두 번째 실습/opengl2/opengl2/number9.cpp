#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <vector>
#include <random>
#include "Shader.h"
#include <cmath>

using namespace std;

struct Triangle {
    float x, y;
    float size;
    float r, g, b;
    float vx, vy;
    float Orivx, Orivy;
    vector<float>path;
};

vector<Triangle> triangles;

int moveMode = 0;

random_device rd;
mt19937 gen(rd());

uniform_real_distribution<float>SizeDist(0.05f, 0.15f); // 사이즈 범위
uniform_real_distribution<float>ColorDist(0.0f, 1.0f); // 색상 범위
uniform_real_distribution<float>SpeedDist(-0.4f, 0.4f);
uniform_real_distribution<float>three(0.3f, 0.5f);
uniform_real_distribution<float>three1(-0.5f, -0.3f);


GLint width, height;
GLFWwindow* window = nullptr;
GLuint shaderProgramID;
GLuint vao;
GLuint vbo[2];

bool isLeftpressed = false;
bool isCpressed = false;
bool isOnepressed = false;
bool isTwopressed = false;
bool isThreepressed = false;
bool isFourpressed = false;

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
float lastTime = 0;// delta 시간 구하기 위해서

int main() {
    width = 1200;
    height =1200;

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
        float currentTime = (float)glfwGetTime();
        float deltaTime = currentTime - lastTime;
        lastTime = currentTime;
        if (moveMode == 1) {
            for (int k = triangles.size() - 1;k >= 0;k--) {
                triangles[k].x += triangles[k].vx * deltaTime;
                triangles[k].y += triangles[k].vy * deltaTime;

                if (triangles[k].x + triangles[k].size >= 1.0f || triangles[k].x - triangles[k].size <= -1.0f) {
                    triangles[k].vx = -triangles[k].vx;
                }
                if (triangles[k].y + 3*triangles[k].size >= 1.0f || triangles[k].y - triangles[k].size <= -1.0f) {
                    triangles[k].vy = -triangles[k].vy;
                }
            }
        }
        else if (moveMode == 2) {
            for (int k = triangles.size() - 1;k >= 0;k--) {
                triangles[k].x += triangles[k].vx * deltaTime;

                float stepY = (triangles[k].vy >= 0) ? 0.05f : -0.05f;

                if (triangles[k].x + 3 * triangles[k].size >= 1.0f) {
                    triangles[k].x = 1.0f - 3 * triangles[k].size; 
                    triangles[k].vx = -triangles[k].vx;            
                    triangles[k].y += stepY;                       
                }
                
                else if (triangles[k].x - 3 * triangles[k].size <= -1.0f) {
                    triangles[k].x = -1.0f + 3 * triangles[k].size; 
                    triangles[k].vx = -triangles[k].vx;             
                    triangles[k].y += stepY;                       
                }

                if (triangles[k].y + triangles[k].size >= 1.0f) {
                    triangles[k].vy = -abs(triangles[k].vy); // 무조건 아래(-)로
                }
                else if (triangles[k].y - triangles[k].size <= -1.0f) {
                    triangles[k].vy = abs(triangles[k].vy);  // 무조건 위(+)로
                }
            }
        }
        else if (moveMode == 3) {
            for (int k = triangles.size() - 1;k >= 0;k--) {
                // 1. 매 프레임 X축과 Y축을 동시에 계속 이동시킨다.
                triangles[k].x += triangles[k].vx * deltaTime;
                triangles[k].y += triangles[k].vy * deltaTime;

                if (triangles[k].y + 3 * triangles[k].size >= 1.0f) {
                    triangles[k].y = 1.0f - 3 * triangles[k].size;
                    triangles[k].vy = -abs(triangles[k].vy);       
                }
                else if (triangles[k].y - 3*triangles[k].size <= -1.0f) {
                    triangles[k].y = -1.0f + 3*triangles[k].size;   
                    triangles[k].vy = abs(triangles[k].vy);       
                }

                
                if (triangles[k].x + triangles[k].size >= 1.0f) {
                    triangles[k].x = 1.0f - triangles[k].size;     
                    triangles[k].vx = -abs(triangles[k].vx);      
                }
                else if (triangles[k].x - 3*triangles[k].size <= -1.0f) {
                    triangles[k].x = -1.0f + 3*triangles[k].size;    
                    triangles[k].vx = abs(triangles[k].vx);        
                }
            }
        }
        else if (moveMode == 4) {
            static int framecount = 0;
            framecount++;
            for (int k = triangles.size() - 1; k >= 0; k--) {
                
                float x = triangles[k].x;
                float y = triangles[k].y;

                // 현재 (x, y) 좌표에서 반지름 R과 각도 theta 구하기
                float R = sqrt(x * x + y * y);
                float theta = atan2(y, x);

                
                
                theta +=4.0f* deltaTime;         // 회전 속도

                R += triangles[k].vy * deltaTime;  // vy를 반지름 변화율로 활용

                
                if (R >= 0.3f) {
                    R = 0.3f;
                    triangles[k].vy = -abs(triangles[k].vy); 
                }
                else if (R <= 0.0f) {
                    R = 0.0f;
                    triangles[k].vy = abs(triangles[k].vy); 
                }

                // 변환된 극좌표를 다시 직교좌표 (x, y) 위치로 대입
                
                triangles[k].x = R * cos(theta);
                triangles[k].y = R * sin(theta);
                cout << "theta: " << theta << " R: " << "x: " << triangles[k].x << "y: " << triangles[k].y << endl;
                
                    triangles[k].path.push_back(triangles[k].x);
                    triangles[k].path.push_back(triangles[k].y);
                    triangles[k].path.push_back(0.0f);
                
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
    //[Q] 키 종료
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        if (!isLeftpressed) {
            float mx = (xpos / width) * 2.0f - 1.0f;
            float my = 1.0f - (ypos / height) * 2.0f;

            if (mx >= -0.85f && mx <= 0.85f && my >= -0.85f && my <= 0.85f) {
                Triangle newTri;
                newTri.x = mx;
                newTri.y = my;
                newTri.size = SizeDist(gen);
                newTri.r = ColorDist(gen);
                newTri.g = ColorDist(gen);
                newTri.b = ColorDist(gen);
                float vx = SpeedDist(gen);
                float vy = SpeedDist(gen);
                newTri.Orivx = vx;
                newTri.Orivy = vy;
                if (abs(vx) < 0.1f)vx = (vx < 0) ? -0.15f:0.15f;
                if (abs(vy) < 0.1f)vy = (vy < 0) ? -0.15f : 0.15f;
                newTri.vx = vx;
                if (moveMode == 4) {
                    newTri.vy = 0.02f;
                } else {
                    newTri.vy = vy;
                }
                triangles.push_back(newTri);
            }
        }
        isLeftpressed = true;
    }
    else {
        isLeftpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (!isCpressed) {
            for (auto& tri : triangles) {
                tri.path.clear();
            }
            triangles.clear();
        
        }
        isCpressed = true;
    }
    else {
        isCpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        if (!isOnepressed) {
            moveMode = 1;
            cout << "mode 변경 완료 1번" << endl;
            for (auto& tri : triangles) {
                tri.vx = (tri.vx >= 0) ? abs(tri.Orivx) : -abs(tri.Orivx);
                tri.vy = (tri.vy >= 0) ? abs(tri.Orivy) : -abs(tri.Orivy);
            }
        }
        isOnepressed = true;
    }
    else {
        isOnepressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        if (!isTwopressed) {
            moveMode = 2;
            cout << "mode 변경 완료 2번" << endl;
            for (auto& tri : triangles) {
                tri.vx = (tri.vx >= 0) ? abs(tri.Orivx) : -abs(tri.Orivx);
                tri.vy = (tri.vy >= 0) ? abs(tri.Orivy) : -abs(tri.Orivy);
            }
        }
        isTwopressed = true;
    }
    else {
        isTwopressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        if (!isThreepressed) {
            moveMode = 3;
            for (auto& tri : triangles) {
                tri.vx = (tri.vx >= 0) ? 0.05f : -0.05f; // 가로는 완만하게
                tri.vy = (tri.vy >= 0) ? three(gen) :three1(gen) ;   // 세로는 가파르고 빠르게!
            }
            cout << "mode 변경 완료 3번" << endl;
        }
        isThreepressed = true;
    }
    else {
        isThreepressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        if (!isFourpressed) {
            moveMode = 4;
            for (auto& tri : triangles) {
                tri.vy = 0.02f; // 반지름 팽창 속도 기본값 세팅
            }
            cout << "mode 변경 완료 4번" << endl;
        }
        isFourpressed = true;
    }
    else {
       
        isFourpressed = false;
    }
}

void DrawTri(float x, float y, float size, float r, float g, float b, float vx, float vy) {
    float posData[9];

    
    if (moveMode == 2) {
        if (vx > 0) {
            posData[0] = x - size;        posData[1] = y + size; posData[2] = 0.0f;
            posData[3] = x - size;        posData[4] = y - size; posData[5] = 0.0f;
            posData[6] = x + 3 * size;    posData[7] = y;        posData[8] = 0.0f;
        }
        else {
            posData[0] = x + size;        posData[1] = y - size; posData[2] = 0.0f;
            posData[3] = x + size;        posData[4] = y + size; posData[5] = 0.0f;
            posData[6] = x - 3 * size;    posData[7] = y;        posData[8] = 0.0f;
        }
    }
    
    else if (moveMode == 3) {
        if (vy > 0) { 
            posData[0] = x - size;        posData[1] = y - size; posData[2] = 0.0f;
            posData[3] = x + size;        posData[4] = y - size; posData[5] = 0.0f;
            posData[6] = x;               posData[7] = y + 3 * size; posData[8] = 0.0f;
        }
        else {
            posData[0] = x - size;        posData[1] = y + size; posData[2] = 0.0f;
            posData[3] = x + size;        posData[4] = y + size; posData[5] = 0.0f;
            posData[6] = x;               posData[7] = y - 3 * size; posData[8] = 0.0f;
        }
    }
    else {
        posData[0] = x - size;            posData[1] = y - size; posData[2] = 0.0f;
        posData[3] = x + size;            posData[4] = y - size; posData[5] = 0.0f;
        posData[6] = x;                   posData[7] = y + 3 * size; posData[8] = 0.0f;
    }

    float ColorData[]{
        r, g, b,
        r, g, b,
        r, g, b
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


    for (int k = triangles.size() - 1;k >= 0;k--) {
        if (moveMode == 4 && triangles[k].path.size() >= 6) {
            int vertexCount = triangles[k].path.size() / 3;

            // 1. 위치 VBO(vbo[0])에 누적된 path 좌표 배열 전송
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
            glBufferData(GL_ARRAY_BUFFER, triangles[k].path.size() * sizeof(float), triangles[k].path.data(), GL_DYNAMIC_DRAW);

            // 2. 색상 VBO(vbo[1])에 해당 삼각형의 색상을 정점 개수만큼 채워서 전송
            std::vector<float> lineColors;
            lineColors.reserve(triangles[k].path.size());
            for (int i = 0; i < vertexCount; i++) {
                lineColors.push_back(triangles[k].r);
                lineColors.push_back(triangles[k].g);
                lineColors.push_back(triangles[k].b);
            }
            glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
            glBufferData(GL_ARRAY_BUFFER, lineColors.size() * sizeof(float), lineColors.data(), GL_DYNAMIC_DRAW);

            // 3. 선 두께 설정 및 GL_LINE_STRIP으로 궤적 연결하기
            glPointSize(5.0f);
            glDrawArrays(GL_POINTS, 0, vertexCount);
        }
        DrawTri(triangles[k].x, triangles[k].y, triangles[k].size,
            triangles[k].r, triangles[k].g, triangles[k].b, triangles[k].vx, triangles[k].vy);
    }

}