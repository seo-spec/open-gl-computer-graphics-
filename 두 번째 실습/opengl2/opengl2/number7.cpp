#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include "Shader.h"
#include <vector>
#include <random>

using namespace std;

enum ShapeType {
    POINT,LINE,TRIANGLE,RECTANGLE
};

struct Shape {
    ShapeType type;
    float x, y;
    float r, g, b;
};

vector<Shape> shapes; // 도형 최대 50개 저장

random_device rd;
mt19937 gen(rd());

uniform_real_distribution<float>ColorDist(0.0f, 1.0f); // 색상 범위 [0, 1]
uniform_real_distribution<float>PosDist(-0.8f, 0.8f); // 위치 범위 [-0.8, 0.8]

GLint width, height;

GLFWwindow* window = nullptr;
GLuint shaderProgramID;
GLuint vao; //vertex array object
GLuint vbo[2]; //vertex buffer object 
GLuint ebo; //element buffer object

float speed = 0.03f; // 도형 이동 속도

int selectidx = -1; // 선택된 도형 인덱스 (-1이면 선택 없음)  

void InitBuffer() {
    // 1. VAO 생성 및 바인딩
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    // 2. VBO 2개와 EBO 1개 생성
    glGenBuffers(2, vbo);
    glGenBuffers(1, &ebo);

    // --- 0번 VBO: 위치 속성 규격 설정 (location = 0)
    glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
    // 3개(float) 기준, Stride = 0, Offset = 0 파이프라인 개설
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, (void*)0);
    glEnableVertexAttribArray(0);

    // --- 1번 VBO: 색상 속성 규격 설정 (location = 1)
    glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, (void*)0); // GL_FALSE : 정규화 안함, 내가 넣은 값 그대로 사용
    glEnableVertexAttribArray(1);

    // --- EBO: 인덱스 버퍼 타겟을 현재 VAO에 정식 바인딩해 둠
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
}

bool isPpressed = false; // P키 상태를 추적하는 변수
bool isEpressed = false; // E키 상태를 추적하는 변수
bool isTpressed = false; // T키 상태를 추적하는 변수
bool isRpressed = false; // R키 상태를 추적하는 변수
bool isCpressed = false; // C키 상태를 추적하는 변수
bool is1pressed = false; // 1키 상태를 추적하는 변수
bool is2pressed = false; // 2키 상태를 추적하는 변수
bool is3pressed = false; // 3키 상태를 추적하는 변수
bool is4pressed = false; // 4키 상태를 추적하는 변수
bool isMousePressed = false; // 마우스 상태를 추적하는 변수
bool isWpressed = false; // W키 상태를 추적하는 변수
bool isApressed = false; // A키 상태를 추적하는 변수
bool isSpressed = false; // S키 상태를 추적하는 변수
bool isDpressed = false; // D키 상태를 추적하는 변수
bool isIpressed = false; // I키 상태를 추적하는 변수
bool isJpressed = false; // J키 상태를 추적하는 변수
bool isKpressed = false; // K키 상태를 추적하는 변수
bool isLpressed = false; // L키 상태를 추적하는 변수



void InputProcess();
void DrawScene();

int main() {
    width = 1280;
	height = 900;

    // GLFW 초기화 및 윈도우 생성
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
    
    // 셰이더 프로그램 초기화
    shaderProgramID = InitShader("vertex.glsl", "fragment.glsl");

    InitBuffer();

	if (shaderProgramID != 0) {
		cout << "셰이더 프로그램 초기화 성공" << endl;
	}
	else {
		cout << "셰이더 프로그램 초기화 실패" << endl;
	}

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
    if (glfwGetKey(window, GLFW_KEY_P) == GLFW_PRESS) {
        if (!isPpressed) { // P키가 처음 눌렸을 때만 출력

            if (shapes.size() < 50) {
                Shape newRect;
                newRect.type = POINT;
                newRect.x = PosDist(gen);
                newRect.y = PosDist(gen);
                newRect.r = ColorDist(gen);
                newRect.g = ColorDist(gen);
                newRect.b = ColorDist(gen);

                shapes.push_back(newRect);
                std::cout << "[생성] 도형 종류: " << newRect.type << "/현재 개수: " << shapes.size() << std::endl;
            }
        }
        isPpressed = true;
    }
    else {
        isPpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        if (!isEpressed) { // E키가 처음 눌렸을 때만 출력
            if (shapes.size() < 50) {
                Shape newRect;
                newRect.type = LINE;
                newRect.x = PosDist(gen);
                newRect.y = PosDist(gen);
                newRect.r = ColorDist(gen);
                newRect.g = ColorDist(gen);
                newRect.b = ColorDist(gen);

                shapes.push_back(newRect);
                std::cout << "[생성] 도형 종류: " << newRect.type << "/현재 개수: " << shapes.size() << std::endl;
            }
        }
        isEpressed = true;
    }
    else {
        isEpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        if (!isTpressed) { // T키가 처음 눌렸을 때만 출력
            if (shapes.size() < 50) {
                Shape newRect;
                newRect.type = TRIANGLE;
                newRect.x = PosDist(gen);
                newRect.y = PosDist(gen);
                newRect.r = ColorDist(gen);
                newRect.g = ColorDist(gen);
                newRect.b = ColorDist(gen);

                shapes.push_back(newRect);
                std::cout << "[생성] 도형 종류: " << newRect.type << "/현재 개수: " << shapes.size() << std::endl;
            }
        }
        isTpressed = true;
    }
    else {
        isTpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        if (!isRpressed) { // R키가 처음 눌렸을 때만 출력
            if (shapes.size() < 50) {
                Shape newRect;
                newRect.type = RECTANGLE;
                newRect.x = PosDist(gen);
                newRect.y = PosDist(gen);
                newRect.r = ColorDist(gen);
                newRect.g = ColorDist(gen);
                newRect.b = ColorDist(gen);

                shapes.push_back(newRect);
                std::cout << "[생성] 도형 종류: " << newRect.type << "/현재 개수: " << shapes.size() << std::endl;
            }
        }
        isRpressed = true;
    }
    else {
        isRpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_C) == GLFW_PRESS) {
        if (!isCpressed) {
            shapes.clear();
            cout << "[삭제] 모든 도형 삭제" << endl;
        }
        isCpressed = true;
    }
    else {
        isCpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_1) == GLFW_PRESS) {
        if (!is1pressed) {
            for (int k = 0;k < shapes.size();k++) {
                shapes[k].x += speed;
            }
        }
        is1pressed = true;
    }
    else {
        is1pressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
        if (!is2pressed) {
            for (int k = 0;k < shapes.size();k++) {
                shapes[k].x -= speed;
            }
        }
        is2pressed = true;
    }
    else {
        is2pressed = false;
    }
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		if (!is3pressed) {
			for (int k = 0;k < shapes.size();k++) {
				shapes[k].y += speed;
			}
		}
		is3pressed = true;
	}
	else {
		is3pressed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
		if (!is4pressed) {
			for (int k = 0;k < shapes.size();k++) {
				shapes[k].y -= speed;
			}
		}
		is4pressed = true;
	}
	else {
		is4pressed = false;
	}
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        if (!isMousePressed) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
			float mx = (xpos / width) * 2.0f - 1.0f; // 마우스 좌표를 OpenGL 좌표로 변환
			float my = 1.0f - (ypos / height) * 2.0f; // 마우스 좌표를 OpenGL 좌표로 변환
			float size = 0.1f; // 도형의 크기
			for (int k = shapes.size() - 1; k >= 0; k--) {
				float X = shapes[k].x;
				float Y = shapes[k].y;
				if (mx >= X - size && mx <= X + size && my >= Y - size && my <= Y + size) {
					selectidx = k; // 선택된 도형 인덱스 저장
					std::cout << "[선택] 도형 인덱스: " << selectidx << std::endl;
					break; // 첫 번째로 선택된 도형만 처리
				}
			}

        }
        isMousePressed = true;

    }
    else {
		isMousePressed = false;
    }
	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
		if (!isWpressed && selectidx != -1) {
			shapes[selectidx].y += speed;
		}
		isWpressed = true;
	}
	else {
		isWpressed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
		if (!isApressed && selectidx != -1) {
			shapes[selectidx].x -= speed;
		}
		isApressed = true;
	}
	else {
		isApressed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
		if (!isSpressed && selectidx != -1) {
			shapes[selectidx].y -= speed;
		}
		isSpressed = true;
	}
	else {
		isSpressed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
		if (!isDpressed && selectidx != -1) {
			shapes[selectidx].x += speed;
		}
		isDpressed = true;
	}
	else {
		isDpressed = false;
	}
    if(glfwGetKey(window,GLFW_KEY_I)==GLFW_PRESS){
        if (!isIpressed && selectidx != -1) {
            shapes[selectidx].x += speed;
			shapes[selectidx].y += speed;
        }
		isIpressed = true;
    }
    else {
        isIpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        if (!isJpressed && selectidx != -1) {
			shapes[selectidx].x -= speed;
			shapes[selectidx].y += speed;
        }
		isJpressed = true;
    }
    else {
        isJpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        if (!isKpressed && selectidx != -1) {
			shapes[selectidx].x -= speed;
			shapes[selectidx].y -= speed;
        }
        isKpressed = true;
	}
    else {
        isKpressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        if (!isLpressed && selectidx != -1) {
			shapes[selectidx].x += speed;
			shapes[selectidx].y -= speed;
        }
        isLpressed = true;
    }
    else {
        isLpressed = false;
    }
}


void DrawScene() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 배경색
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgramID); // 셰이더 프로그램 활성화
    glBindVertexArray(vao);

    for (size_t k = 0; k < shapes.size(); k++) {
        float size = 0.1f; // 도형의 크기

        float X = shapes[k].x;
        float Y = shapes[k].y;
        float R = shapes[k].r;
        float G = shapes[k].g;
        float B = shapes[k].b;
        // 1. POINT (점)
        if (shapes[k].type == POINT) {
			auto DrawPoint = [&](float offsetX, float offsetY) {    //auto [&] : 람다 캡처, auto안에 있는 내용을 람다 안에서 사용 가능
                float posData[] = { X + offsetX, Y + offsetY, 0.0f };   //offsetX, offsetY는 경계 복사본을 그릴 때 사용
                float colorData[] = { R, G, B };

                glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(posData), posData, GL_DYNAMIC_DRAW);

                glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_DYNAMIC_DRAW);

                glPointSize(15.0f);
                glDrawArrays(GL_POINTS, 0, 1);
                };

            // 본체 그리기
            DrawPoint(0.0f, 0.0f);

            // 경계 복사본 그리기 (점 좌표 기준)
            if (X > 1.0f)   DrawPoint(-2.0f, 0.0f);
            if (X < -1.0f)  DrawPoint(2.0f, 0.0f);
            if (Y > 1.0f)   DrawPoint(0.0f, -2.0f);
            if (Y < -1.0f)  DrawPoint(0.0f, 2.0f);
            if (k == selectidx) {
                glPointSize(25.0f);

                DrawPoint(0.0f, 0.0f);
                if (X > 1.0f)   DrawPoint(-2.0f, 0.0f);
                if (X < -1.0f)  DrawPoint(2.0f, 0.0f);
                if (Y > 1.0f)   DrawPoint(0.0f, -2.0f);
                if (Y < -1.0f)  DrawPoint(0.0f, 2.0f);

                glPointSize(15.0f);
            }
        }

        // 2. LINE (선)
        else if (shapes[k].type == LINE) {
            auto DrawLine = [&](float offsetX, float offsetY) {
                float posData[] = {
                    X + offsetX,        Y + offsetY,        0.0f,
                    X + offsetX + size, Y + offsetY + size, 0.0f
                };
                float colorData[] = {
                    R, G, B,
                    R, G, B
                };

                glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(posData), posData, GL_DYNAMIC_DRAW);

                glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_DYNAMIC_DRAW);

                glLineWidth(2.0f);
                glDrawArrays(GL_LINES, 0, 2);
                };

            DrawLine(0.0f, 0.0f);

            // 선의 두 정점 범위를 고려한 경계 복사본
            if (X + size > 1.0f)  DrawLine(-2.0f, 0.0f);
            if (X < -1.0f)         DrawLine(2.0f, 0.0f);
            if (Y + size > 1.0f)  DrawLine(0.0f, -2.0f);
            if (Y < -1.0f)         DrawLine(0.0f, 2.0f);

            if (k == selectidx) {
                glLineWidth(6.0f);

                DrawLine(0.0f, 0.0f);
                if (X + size > 1.0f)  DrawLine(-2.0f, 0.0f);
                if (X < -1.0f)         DrawLine(2.0f, 0.0f);
                if (Y + size > 1.0f)  DrawLine(0.0f, -2.0f);
                if (Y < -1.0f)         DrawLine(0.0f, 2.0f);

                glLineWidth(2.0f); // 기본 두께 복구
            }
        }
        // 3. TRIANGLE (삼각형)
        else if (shapes[k].type == TRIANGLE) {
            auto DrawTri = [&](float offsetX, float offsetY) {
                float posData[] = {
                    X + offsetX - size, Y + offsetY - size, 0.0f,
                    X + offsetX + size, Y + offsetY - size, 0.0f,
                    X + offsetX,        Y + offsetY + size, 0.0f
                };
                float colorData[] = {
                    R, G, B,
                    R, G, B,
                    R, G, B
                };

                glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(posData), posData, GL_DYNAMIC_DRAW);

                glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_DYNAMIC_DRAW);

                glDrawArrays(GL_TRIANGLES, 0, 3);
                };

            DrawTri(0.0f, 0.0f);

            // 삼각형 외곽 범위 고려 경계 복사본
            if (X + size > 1.0f)   DrawTri(-2.0f, 0.0f);
            if (X - size < -1.0f)  DrawTri(2.0f, 0.0f);
            if (Y + size > 1.0f)   DrawTri(0.0f, -2.0f);
            if (Y - size < -1.0f)  DrawTri(0.0f, 2.0f);
            if (k == selectidx) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
                glLineWidth(3.0f);

                DrawTri(0.0f, 0.0f);
                if (X + size > 1.0f)   DrawTri(-2.0f, 0.0f);
                if (X - size < -1.0f)  DrawTri(2.0f, 0.0f);
                if (Y + size > 1.0f)   DrawTri(0.0f, -2.0f);
                if (Y - size < -1.0f)  DrawTri(0.0f, 2.0f);

                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
        // 4. RECTANGLE (사각형)
        else if (shapes[k].type == RECTANGLE) {
            auto DrawRect = [&](float offsetX, float offsetY) {
                float posData[] = {
                    X + offsetX - size, Y + offsetY - size, 0.0f,
                    X + offsetX + size, Y + offsetY - size, 0.0f,
                    X + offsetX + size, Y + offsetY + size, 0.0f,
                    X + offsetX - size, Y + offsetY + size, 0.0f
                };
                unsigned int index[] = {
                    0, 1, 2,
                    2, 3, 0
                };
                float colorData[] = {
                    R, G, B,
                    R, G, B,
                    R, G, B,
                    R, G, B
                };

                glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(posData), posData, GL_DYNAMIC_DRAW);

                glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
                glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_DYNAMIC_DRAW);

                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
                glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_DYNAMIC_DRAW);

                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
                };

            DrawRect(0.0f, 0.0f);

            // 사각형 모서리 범위 고려 경계 복사본
            if (X + size > 1.0f)   DrawRect(-2.0f, 0.0f);
            if (X - size < -1.0f)  DrawRect(2.0f, 0.0f);
            if (Y + size > 1.0f)   DrawRect(0.0f, -2.0f);
            if (Y - size < -1.0f)  DrawRect(0.0f, 2.0f);
            if (k == selectidx) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE); 
                glLineWidth(5.0f);                         // 테두리 두께

                
                DrawRect(0.0f, 0.0f);
                if (X + size > 1.0f)   DrawRect(-2.0f, 0.0f);
                if (X - size < -1.0f)  DrawRect(2.0f, 0.0f);
                if (Y + size > 1.0f)   DrawRect(0.0f, -2.0f);
                if (Y - size < -1.0f)  DrawRect(0.0f, 2.0f);

                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }

    }
}