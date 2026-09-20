#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <random>
#include <algorithm>

using namespace std;

// 1. window 변수를 전역 변수로 배치 (InputProcess에서도 접근 가능하게)
GLFWwindow* window = nullptr;
float r = 1.0f;
float g = 1.0f;
float b = 1.0f;


struct Rect {
	float x1, y1, x2, y2; // 사각형의 좌표
	float r, g, b; // 사각형의 색상
	float vx, vy; // 사각형의 속도
	bool isBrightening;// 사각형이 밝아지고 있는지 여부

	bool isAlive; // 사각형이 살아있는지 여부
};

bool isLeftPressed = false; // 왼쪽 마우스 버튼이 눌렸는지 여부를 추적하는 변수

random_device rd;
mt19937 gen(rd());
vector <Rect> rects; // 사각형들을 저장하는 벡터

void InitRects() {
	uniform_real_distribution<float> ColorDist(0.0f, 1.0f);
	uniform_int_distribution<int> countDist(5, 10);//사각형 개수
	uniform_real_distribution<float> posDist(-0.7f, 0.7f);//사각형 생성 위치
	uniform_real_distribution<float> sizeDist(0.1f, 0.3f);//사각형 크기)

	int count = countDist(gen);
    
    for (int i = 0;i < count;i++) {
		float x = posDist(gen);
        float y = posDist(gen);
		float size = sizeDist(gen);
        Rect newRect = {
            x,y,x+size,y+size,
            ColorDist(gen),ColorDist(gen),ColorDist(gen),
            0.0f,0.0f,
            false,
            true
        };
        rects.push_back(newRect);
    }
}

void InputProcess();
void DrawScene();

int main() {
    // GLFW 초기화 및 윈도우 생성
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    window = glfwCreateWindow(1280, 900, "OpenGL Window", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return -1;

    glViewport(0, 0, 1280, 900);

    InitRects();

	float lastTime = 0.0f; // 마지막 시간을 저장할 변수 초기화
    // 메인 루프
    while (!glfwWindowShouldClose(window)) {
		float currentTime = (float)glfwGetTime(); // 현재 시간을 가져옴
		float deltaTime = currentTime - lastTime; // 지난 프레임과의 시간 차이 계산
		lastTime = currentTime;

		for (int k = rects.size() - 1;k >= 0;k--) {
            if (!rects[k].isAlive) continue;

			rects[k].x1 += rects[k].vx * deltaTime;
			rects[k].y1 += rects[k].vy * deltaTime;
			rects[k].x2 += rects[k].vx * deltaTime;
			rects[k].y2 += rects[k].vy * deltaTime;
			float Colorrate = 0.8f * deltaTime; // 색상 변화 속도

            // 2. 클릭되어 움직이기 시작한 조각 사각형들만 크기를 줄임 (vx, vy가 0이 아닌 경우)
            if (rects[k].vx != 0.0f || rects[k].vy != 0.0f) {
                float Sizerate = 0.05f * deltaTime; // 크기 변화 속도

                rects[k].x1 += Sizerate; // 왼쪽 변은 오른쪽으로
                rects[k].y1 += Sizerate; // 아래 변은 위쪽으로
                rects[k].x2 -= Sizerate; // 오른쪽 변은 왼쪽으로
                rects[k].y2 -= Sizerate; // 위쪽 변은 아래쪽으로

                if (rects[k].isBrightening) {
					// 밝아짐: 1.0f를 넘지 않도록 min() 처리
                    rects[k].r = min(1.0f, rects[k].r + Colorrate);
                    rects[k].g = min(1.0f, rects[k].g + Colorrate);
                    rects[k].b = min(1.0f, rects[k].b + Colorrate);
                }
                else {
                    // 어두워짐: 0.0f 밑으로 내려가지 않도록 max() 처리
                    rects[k].r = max(0.0f, rects[k].r - Colorrate);
                    rects[k].g = max(0.0f, rects[k].g - Colorrate);
                    rects[k].b = max(0.0f, rects[k].b - Colorrate);
                }

                // 특정 크기 이하가 되면 완전 소멸 (사망 처리)
                if ((rects[k].x2 - rects[k].x1) <= 0.005f || (rects[k].y2 - rects[k].y1) <= 0.005f) {
                    rects[k].isAlive = false;
                }
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

// 3. 키보드 입력 처리 함수 정의
void InputProcess() {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		float mx = (float)((xpos / 1280.0f) * 2.0f - 1.0f);
		float my = (float)(1.0f - (ypos / 900.0f) * 2.0f);
		float speed = 0.3f; // 이동 속도
        
        if (!isLeftPressed) {
            for (int k = rects.size() - 1;k >= 0;k--) {
                if (rects[k].isAlive && mx >= rects[k].x1 && mx <= rects[k].x2 && my >= rects[k].y1 && my <= rects[k].y2) {
                    
                    Rect target = rects[k];
                    rects[k].isAlive = false;

                    uniform_int_distribution<int> brightDist(0, 1);
                    uniform_int_distribution<int> typeDist(1, 4);

                    int intype = typeDist(gen);    // 1~4 패턴 중 무작위 선택
                    bool bright = brightDist(gen); // 밝기 플래그 무작위 지정

                    
                    float midX = (target.x1 + target.x2) / 2.0f;
                    float midY = (target.y1 + target.y2) / 2.0f;

                    
                    switch (intype) {
                        // 1. 좌우상하 이동 (4개 조각이 상, 하, 좌, 우 직선 방향으로 진행)
                    case 1: {
                        Rect Top = { target.x1, midY, midX, target.y2, target.r, target.g, target.b,  0.0f,   speed, bright, true }; // 위쪽
                        Rect Right = { midX, midY, target.x2, target.y2, target.r, target.g, target.b,  speed,  0.0f,  bright, true }; // 오른쪽
                        Rect Left = { target.x1, target.y1, midX, midY, target.r, target.g, target.b, -speed,  0.0f,  bright, true }; // 왼쪽
                        Rect Bottom = { midX, target.y1, target.x2, midY, target.r, target.g, target.b,  0.0f,  -speed, bright, true }; // 아래쪽

                        rects.push_back(Top);
                        rects.push_back(Right);
                        rects.push_back(Left);
                        rects.push_back(Bottom);
                        break;
                    }

                          // 2. 대각선 이동 (4개 조각이 대각선 사방으로 퍼짐)
                    case 2: {
                        Rect Topleft = { target.x1, midY, midX, target.y2, target.r, target.g, target.b, -speed,  speed, bright, true };
                        Rect Topright = { midX, midY, target.x2, target.y2, target.r, target.g, target.b,  speed,  speed, bright, true };
                        Rect Bottomleft = { target.x1, target.y1, midX, midY, target.r, target.g, target.b, -speed, -speed, bright, true };
                        Rect Bottomright = { midX, target.y1, target.x2, midY, target.r, target.g, target.b,  speed, -speed, bright, true };

                        rects.push_back(Topleft);
                        rects.push_back(Topright);
                        rects.push_back(Bottomleft);
                        rects.push_back(Bottomright);
                        break;
                    }

                          // 3. 한쪽 방향으로 같이 이동 (4개 조각이 한쪽 대각선 방향으로 함께 퍼짐)
                    case 3: {
                        Rect Topleft = { target.x1, midY, midX, target.y2, target.r, target.g, target.b, speed, speed, bright, true };
                        Rect Topright = { midX, midY, target.x2, target.y2, target.r, target.g, target.b, speed, speed, bright, true };
                        Rect Bottomleft = { target.x1, target.y1, midX, midY, target.r, target.g, target.b, speed, speed, bright, true };
                        Rect Bottomright = { midX, target.y1, target.x2, midY, target.r, target.g, target.b, speed, speed, bright, true };

                        rects.push_back(Topleft);
                        rects.push_back(Topright);
                        rects.push_back(Bottomleft);
                        rects.push_back(Bottomright);
                        break;
                    }

                          // 4. 좌우상하와 대각선 이동 (8방향 이동으로 축소된 사각형 8개 생성)
                    case 4: {
                        float quarterW = (target.x2 - target.x1) / 4.0f;
                        float quarterH = (target.y2 - target.y1) / 4.0f;

                        
                        rects.push_back({ midX - quarterW, target.y2 - quarterH * 2, midX + quarterW, target.y2, target.r, target.g, target.b, 0.0f, speed, bright, true });
                        rects.push_back({ midX - quarterW, target.y1, midX + quarterW, target.y1 + quarterH * 2, target.r, target.g, target.b, 0.0f, -speed, bright, true });
                        rects.push_back({ target.x1, midY - quarterH, target.x1 + quarterW * 2, midY + quarterH, target.r, target.g, target.b, -speed, 0.0f, bright, true });
                        rects.push_back({ target.x2 - quarterW * 2, midY - quarterH, target.x2, midY + quarterH, target.r, target.g, target.b, speed, 0.0f, bright, true });

                       
                        rects.push_back({ target.x1, target.y2 - quarterH * 2, target.x1 + quarterW * 2, target.y2, target.r, target.g, target.b, -speed, speed, bright, true });
                        rects.push_back({ target.x2 - quarterW * 2, target.y2 - quarterH * 2, target.x2, target.y2, target.r, target.g, target.b, speed, speed, bright, true });
                        rects.push_back({ target.x1, target.y1, target.x1 + quarterW * 2, target.y1 + quarterH * 2, target.r, target.g, target.b, -speed, -speed, bright, true });
                        rects.push_back({ target.x2 - quarterW * 2, target.y1, target.x2, target.y1 + quarterH * 2, target.r, target.g, target.b, speed, -speed, bright, true });
                        break;
                    }
                    }

                    break; 
                }
            }
            isLeftPressed = true;
        }
    } 
    else {
        isLeftPressed = false;
    }

}


// 4. 렌더링 함수 정의
void DrawScene() {
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
	for (int k = rects.size() - 1;k >= 0;k--) {
		if (rects[k].isAlive) {
			glColor3f(rects[k].r, rects[k].g, rects[k].b);
			glRectf(rects[k].x1, rects[k].y1, rects[k].x2, rects[k].y2);
		}
	}
}