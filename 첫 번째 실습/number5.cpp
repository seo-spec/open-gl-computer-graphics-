#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <random>
#include <vector>

using namespace std;

// 1. window 변수를 전역 변수로 배치 (InputProcess에서도 접근 가능하게)
GLFWwindow* window = nullptr;

float r = 1.0f;
float g = 1.0f;
float b = 1.0f;

random_device rd;
mt19937 gen(rd());

struct Rect {
    float x1, y1, x2, y2; // 사각형의 좌표
    float r, g, b; // 사각형의 색상
	bool isAlive; // 사각형이 살아있는지 여부
};

float prevX = 0.0f;
float prevY = 0.0f;

vector <Rect> rects; // 사각형들을 저장하는 벡터
Rect eraseRect; // 사각형을 삭제시키는 지우개 벡터

bool isLeftPressed = false; // 왼쪽 마우스 버튼이 눌렸는지 여부를 추적하는 변수
bool isRightPressed = false; // 오른쪽 마우스 버튼이 눌렸는지 여부를 추적하는 변수
bool isRPressed = false; // R 키가 눌렸는지 여부를 추적하는 변수

float eraserRadius = 0.1f;// 지우개 반지름
int addcount = 0; // 사각형 추가 횟수 추적 변수

void InputProcess();
void DrawScene();

void InitRects() {
    rects.clear();
	uniform_real_distribution <float> posDist(-0.9f, 0.9f);//사각형 생성 위치
	uniform_real_distribution <float> colorDist(0.0f, 1.0f);//사각형 색상
    uniform_int_distribution<int> countDist(20, 40);//사각형 개수
    float size = 0.1f;//사각형의 크기
	int count = countDist(gen);
    for (int i = 0;i < count;i++) {
        float x = posDist(gen);
		float y = posDist(gen);
		Rect newRect = { x, y, x + size, y + size, colorDist(gen), colorDist(gen), colorDist(gen), true };
		rects.push_back(newRect);
    }
}

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

	InitRects(); // 사각형 초기화
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

        if (!isLeftPressed){
         eraseRect = { mx - eraserRadius,my - eraserRadius,mx + eraserRadius,my + eraserRadius,0.0f,0.0f,0.0f,true };

        isLeftPressed = true;
        }
        else {
			float DeltaX = mx - prevX;
            float DeltaY = my - prevY;
			if (eraseRect.x1 + DeltaX < -1.0f) DeltaX = -1.0f - eraseRect.x1;
			if (eraseRect.y1 + DeltaY < -1.0f) DeltaY = -1.0f - eraseRect.y1;
			if (eraseRect.x2 + DeltaX > 1.0f) DeltaX = 1.0f - eraseRect.x2;
			if (eraseRect.y2 + DeltaY > 1.0f) DeltaY = 1.0f - eraseRect.y2;

            eraseRect.x1 += DeltaX;
            eraseRect.y1 += DeltaY;
            eraseRect.x2 += DeltaX;
            eraseRect.y2 += DeltaY;
        }
        for (int k = rects.size() - 1;k >= 0;k--) {
			if (rects[k].isAlive == false) continue;
            bool isOverlapped = (eraseRect.x1 <= rects[k].x2 &&
                eraseRect.x2 >= rects[k].x1 &&
                eraseRect.y1 <= rects[k].y2 &&
                eraseRect.y2 >= rects[k].y1);
            if (isOverlapped) {
				eraseRect.r = rects[k].r;
				eraseRect.g = rects[k].g;
				eraseRect.b = rects[k].b;
				rects[k].isAlive = false;
                eraseRect.x1 -= 0.01f;
				eraseRect.y1 -= 0.01f;
                eraseRect.x2 += 0.01f;
                eraseRect.y2 += 0.01f;
            }
        }
        prevX = mx;
        prevY = my;
    }
	else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE && isLeftPressed == true) {
        if (isLeftPressed) {
			isLeftPressed = false;

			for (int k = rects.size() - 1;k >= 0;k--) {
				rects[k].isAlive = true;
            }
        }
	}
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
        double xpos, ypos;
		glfwGetCursorPos(window, &xpos, &ypos);
		float mx = (float)((xpos / 1280.0f) * 2.0f - 1.0f);
		float my = (float)(1.0f - (ypos / 900.0f) * 2.0f);
		uniform_real_distribution<float> colorDist(0.0f, 1.0f);
        if (!isRightPressed) {
           
            if (addcount < 10) {
                Rect newRect = { mx - 0.05f,my - 0.05f,mx + 0.05f,my + 0.05f, colorDist(gen), colorDist(gen), colorDist(gen), true };
                rects.push_back(newRect);
                eraserRadius -= 0.01f; // 지우개 크기 감소
                if (eraserRadius < 0.03f) eraserRadius = 0.03f;
                addcount++;
            }
        }
		isRightPressed = true;
    }
    else {
		isRightPressed = false;
    }
	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		if (!isRPressed) {
			InitRects();
            eraserRadius=0.1f;
            addcount = 0;
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
    if (isLeftPressed == true) {
		glColor3f(eraseRect.r, eraseRect.g, eraseRect.b);
		glRectf(eraseRect.x1, eraseRect.y1, eraseRect.x2, eraseRect.y2);
    }
    for (int k = rects.size()-1;k >= 0;k--) {
		if (rects[k].isAlive) {
			glColor3f(rects[k].r, rects[k].g, rects[k].b);
			glRectf(rects[k].x1, rects[k].y1, rects[k].x2, rects[k].y2);
		}
    }
}