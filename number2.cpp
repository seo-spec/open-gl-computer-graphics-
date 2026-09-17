#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <vector>
#include <random>

using namespace std;

struct Rect {
	float x1, y1, x2, y2;
	float r, g, b;
	bool isSelected; // 선택 여부를 나타내는 변수
};

random_device rd;
mt19937 gen(rd());

vector<Rect> rect1;
vector<Rect> rect2;
vector<Rect> rect3; 
vector<Rect> rect4;

bool isKeypressed1 = false;  // 1번 키가 눌렸는지 여부를 추적하는 변수
bool isKeypressed2 = false;  // 2번 키가 눌렸는지 여부를 추적하는 변수
bool isKeypressed3 = false;  // 3번 키가 눌렸는지 여부를 추적하는 변수
bool isKeypressed4 = false;  // 4번 키가 눌렸는지 여부를 추적하는 변수

bool isMousedPressed = false; // 마우스 버튼이 눌렸는지 여부를 추적하는 변수
bool isPlusPressed = false; // '+' 키가 눌렸는지 여부를 추적하는 변수
bool isMinusPressed = false; // '-' 키가 눌렸는지 여부를 추적하는 변수 
bool isCPressed = false; // 'C' 키가 눌렸는지 여부를 추적하는 변수
bool isRPressed = false; // 'R' 키가 눌렸는지 여부를 추적하는 변수

// 1. window 변수를 전역 변수로 배치 (InputProcess에서도 접근 가능하게)
GLFWwindow* window = nullptr;

float r = 1.0f;
float g = 1.0f;
float b = 1.0f;

float bg1[3] = { 1.0f, 0.0f, 0.0f }; // 1사분면 (빨강)
float bg2[3] = { 1.0f, 1.0f, 0.0f }; // 2사분면 (노랑)
float bg3[3] = { 0.0f, 1.0f, 0.0f }; // 3사분면 (초록)
float bg4[3] = { 0.0f, 0.0f, 1.0f }; // 4사분면 (파랑)

void InputProcess();
void DrawScene();

int main() {
    // GLFW 초기화 및 윈도우 생성
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);

    window = glfwCreateWindow(800, 600, "OpenGL Window", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) return -1;

    glViewport(0, 0, 800, 600);

    // 메인 루프
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
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }

    // 1번 키를 '누르고 있을 때'
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        if (!isKeypressed1) { // 처음 누른 순간 딱 1번만 실행!
            uniform_real_distribution<float> colorDist(0.0f, 1.0f);
            uniform_real_distribution<float> posDist(0.0f, 0.7f);

            float x1 = posDist(gen);
            float y1 = posDist(gen);
            float x2 = x1 + 0.2f;
            float y2 = y1 + 0.2f;

            Rect newRect = {
                x1, y1, x2, y2,
                colorDist(gen), colorDist(gen), colorDist(gen),
                false
            };
            if (rect1.size() >= 5) {
                rect1.erase(rect1.begin());
            }
            rect1.push_back(newRect);

            isKeypressed1 = true; // 눌림 상태 잠금 (키를 떼기 전까지 연타 방지)
        }
    }
    else {
        // 1번 키에서 '손을 떼었을 때' 비로소 플래그를 해제!
        isKeypressed1 = false;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        if (!isKeypressed2) { // 처음 누른 순간 딱 1번만 실행!
            uniform_real_distribution<float> colorDist(0.0f, 1.0f);
            uniform_real_distribution<float> posDistx(-1.0f, -0.2f);
            uniform_real_distribution<float> posDisty(0.0f, 0.7f);

            float x1 = posDistx(gen);
            float y1 = posDisty(gen);
            float x2 = x1 + 0.2f;
            float y2 = y1 + 0.2f;

            Rect newRect = {
                x1, y1, x2, y2,
                colorDist(gen), colorDist(gen), colorDist(gen),
                false
            };
            if (rect2.size() >= 5) {
                rect2.erase(rect2.begin());
            }
            rect2.push_back(newRect);

            isKeypressed2 = true; // 눌림 상태 잠금 (키를 떼기 전까지 연타 방지)
        }
    }
    else {
        // 2번 키에서 '손을 떼었을 때' 비로소 플래그를 해제!
        isKeypressed2 = false;
    }
    if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
        if (!isKeypressed3) { // 처음 누른 순간 딱 1번만 실행!
            uniform_real_distribution<float> colorDist(0.0f, 1.0f);
            uniform_real_distribution<float> posDistx(-1.0f, -0.2f);
            uniform_real_distribution<float> posDisty(-1.0f, -0.2f);
            float x1 = posDistx(gen);
            float y1 = posDisty(gen);
            float x2 = x1 + 0.2f;
            float y2 = y1 + 0.2f;
            Rect newRect = {
                x1, y1, x2, y2,
                colorDist(gen), colorDist(gen), colorDist(gen),
                false
            };
            if (rect3.size() >= 5) {
                rect3.erase(rect3.begin());
            }
            rect3.push_back(newRect);
            isKeypressed3 = true; // 눌림 상태 잠금 (키를 떼기 전까지 연타 방지)
        }
    }
    else {
        isKeypressed3 = false;
    }
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
		if (!isKeypressed4) { // 처음 누른 순간 딱 1번만 실행!
			uniform_real_distribution<float> colorDist(0.0f, 1.0f);
			uniform_real_distribution<float> posDistx(0.0f, 0.7f);
			uniform_real_distribution<float> posDisty(-1.0f, -0.2f);
			float x1 = posDistx(gen);
			float y1 = posDisty(gen);
			float x2 = x1 + 0.2f;
			float y2 = y1 + 0.2f;
			Rect newRect = {
				x1, y1, x2, y2,
				colorDist(gen), colorDist(gen), colorDist(gen),
				false
			};
			if (rect4.size() >= 5) {
				rect4.erase(rect4.begin());
			}
			rect4.push_back(newRect);
			isKeypressed4 = true; // 눌림 상태 잠금 (키를 떼기 전까지 연타 방지)
		}
	}
	else {
		isKeypressed4 = false;
	}
	if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!isMousedPressed) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            float mx = (float)((xpos / 800.0f) * 2.0f-1.0f);
            float my = (float)(1.0f-(ypos / 600.0f) * 2.0f);
            vector<Rect>* allRect[] = {&rect1,&rect2,&rect3,&rect4};
            for (int k = 0;k < 4;k++) {
                for (auto& r : *allRect[k]) {
                    if (mx >= r.x1 && mx <= r.x2 && my >= r.y1 && my <= r.y2) {
                        r.isSelected = !r.isSelected;
                    }
                }
            }
        }
		isMousedPressed = true;
	}
	else {
		isMousedPressed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_ADD) == GLFW_PRESS) {
		if (!isPlusPressed) {
			vector<Rect>* allRect[] = { &rect1, &rect2, &rect3, &rect4 };
            for (int k = 0;k < 4;k++) {
                for (auto& r : *allRect[k]) {
                    if (r.isSelected) {
                        float scale = 0.02f;
                        r.x1 -= scale; r.x2 += scale;
                        r.y1 -= scale; r.y2 += scale;
                    }
                }
            }
			isPlusPressed = true;
		}
	}
	else {
		isPlusPressed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_KP_SUBTRACT) == GLFW_PRESS) {
		if (!isMinusPressed) {
			vector<Rect>* allRect[] = { &rect1, &rect2, &rect3, &rect4 };
            for (int k = 0;k < 4;k++) {
                for (auto& r : *allRect[k]) {
                    if (r.isSelected) {
                        float scale = 0.02f;
                        if ((r.x2 - r.x1) > 0.05f&& (r.y2 - r.y1) > 0.05f) {
                            r.x1 += scale; r.x2 -= scale;
                            r.y1 += scale; r.y2 -= scale;
                        }
                    }
                }
            }

			isMinusPressed = true;
		}
	}
	else {
		isMinusPressed = false;
	}
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (!isCPressed) {
            vector<Rect>* allRect[] = { &rect1,&rect2,&rect3,&rect4 };
            for (int k = 0;k < 4;k++) {
				for (auto& r : *allRect[k]) {
					if (r.isSelected) {
						r.r = static_cast<float>(rand()) / RAND_MAX;
						r.g = static_cast<float>(rand()) / RAND_MAX;
						r.b = static_cast<float>(rand()) / RAND_MAX;
					}
				}
            }
			isCPressed = true;
        }
    }
    else {
        isCPressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        if (!isRPressed) {
            rect1.clear();
            rect2.clear();
			rect3.clear();
			rect4.clear();
			uniform_real_distribution<float> colors(0.0f, 1.0f);
			bg1[0] = colors(gen); bg1[1] = colors(gen); bg1[2] = colors(gen);
			bg2[0] = colors(gen); bg2[1] = colors(gen); bg2[2] = colors(gen);
			bg3[0] = colors(gen); bg3[1] = colors(gen); bg3[2] = colors(gen);
			bg4[0] = colors(gen); bg4[1] = colors(gen); bg4[2] = colors(gen);

            isRPressed = true;
        }
    }
    else {
		isRPressed = false;
    }

}


// 4. 렌더링 함수 정의
void DrawScene() {
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    // 사분면 배경 그리기
    glColor3fv(bg1); glRectf(0.0f, 0.0f, 1.0f, 1.0f);
    glColor3fv(bg2); glRectf(-1.0f, 0.0f, 0.0f, 1.0f);
    glColor3fv(bg3); glRectf(-1.0f, -1.0f, 0.0f, 0.0f);
    glColor3fv(bg4); glRectf(0.0f, -1.0f, 1.0f, 0.0f);

    // 4개 벡터를 포인터 배열로 묶어서 하나의 루프로 그려주기
    vector<Rect>* allRect[] = { &rect1, &rect2, &rect3, &rect4 };

    for (int k = 0; k < 4; k++) {
        for (const auto& rect : *allRect[k]) {
            // 1. 기본 사각형 채우기
            glColor3f(rect.r, rect.g, rect.b);
            glRectf(rect.x1, rect.y1, rect.x2, rect.y2);

            // 2. 선택된 사각형(isSelected == true)이면 흰색 외곽선 덧그리기
            if (rect.isSelected) {
                glLineWidth(5.0f);           // 테두리 선 두께 설정
                glColor3f(0.0f, 0.0f, 0.0f); // 검정 선 지정
                glBegin(GL_LINE_LOOP);
                glVertex2f(rect.x1, rect.y1);
                glVertex2f(rect.x2, rect.y1);
                glVertex2f(rect.x2, rect.y2);
                glVertex2f(rect.x1, rect.y2);
                glEnd();
            }
        }
    }
}