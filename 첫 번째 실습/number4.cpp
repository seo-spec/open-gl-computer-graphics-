#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <random>

using namespace std;

struct Rect {
    float x1, y1, x2, y2;
    float r, g, b;
};

// 1. window 변수를 전역 변수로 배치 (InputProcess에서도 접근 가능하게)
GLFWwindow* window = nullptr;
float r = 0.25f;
float g = 0.25f;
float b = 0.25f;

vector <Rect> rects;
bool Leftpressed = false;

random_device rd;
mt19937 gen(rd());


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
    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        double xpos, ypos;
        glfwGetCursorPos(window, &xpos, &ypos);
        if (!Leftpressed) {
			float mx = (float)((xpos / 1280.0f) * 2.0f - 1.0f);//NDC 좌표 변환
			float my = (float)(1.0f - (ypos / 900.0f) * 2.0f);//NDC 좌표 변환
			uniform_real_distribution<float> colorDist(0.0f, 1.0f);
			uniform_real_distribution<float> sizeDist(0.05f, 0.2f);
			float w = sizeDist(gen);
			float h = sizeDist(gen);

			//사각형이 화면 밖으로 나가지 않도록 위치 조정
			if (mx - w < -1.0f) mx = -1.0f + w;
            if (mx + w > 1.0f) mx = 1.0f - w;
            if (my - h < -1.0f) my = -1.0f + h; 
            if (my + h > 1.0f) my = 1.0f - h;

			Rect newRect ={mx-w,my-h,mx+w,my+h,colorDist(gen),colorDist(gen),colorDist(gen)};
			rects.push_back(newRect);
            if (rects.size() > 5) {
				rects.erase(rects.begin());
            }
            Leftpressed = true;
        }
   
    }
    else {
        Leftpressed = false;
    }
}


// 4. 렌더링 함수 정의
void DrawScene() {
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
    for (const auto& rect : rects) {
        glColor3f(rect.r, rect.g, rect.b);
        glRectf(rect.x1, rect.y1, rect.x2, rect.y2);
    }
}