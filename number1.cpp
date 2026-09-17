#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <random>

using namespace std;

// 1. window 변수를 전역 변수로 배치 (InputProcess에서도 접근 가능하게)
GLFWwindow* window = nullptr;
float r = 1.0f;
float g = 1.0f;
float b = 1.0f;
bool Timeron = false;
double lastTime = 0.0;
random_device rd;
mt19937 gen(rd());


void InputProcess();
void DrawScene();

int main() {
    // GLFW 초기화 및 윈도우 생성
    if (!glfwInit()) return -1;

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

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
		double currentTime = glfwGetTime(); // 현재 시간을 가져옴
		if (currentTime - lastTime >= 1.0) { // 1초마다
			if (Timeron) {
				uniform_real_distribution<float> dist(0.0f, 1.0f);
				r = dist(gen);
				g = dist(gen);
				b = dist(gen);
			}
			lastTime = currentTime; // 마지막 시간을 현재 시간으로 갱신
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
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) { // 청록색
        r = 0.0f;g = 1.0f;b = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) { // 자홍색
        r = 1.0f;g = 0.0f;b = 1.0f;
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) { // 노란색
        r = 1.0f;g = 1.0f;b = 0.0f;
    }
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) { // 랜덤색
		uniform_real_distribution<float> dist(0.0f, 1.0f);
		r = dist(gen);
		g = dist(gen);
		b = dist(gen);
    }
	if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) { // 회색
		r = 0.5f; g = 0.5f; b = 0.5f;
	}
	if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) { // 검정색
		r = 0.0f; g = 0.0f; b = 0.0f;
	}
    if (glfwGetKey(window,GLFW_KEY_T) == GLFW_PRESS) {
        Timeron = true;
    }
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		Timeron = false;
	}
}


// 4. 렌더링 함수 정의
void DrawScene() {
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}