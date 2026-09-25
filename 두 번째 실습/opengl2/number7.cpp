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
                std::cout << "[생성] 도형 종류: " <<newRect.type<<"/현재 개수: "<<shapes.size() << std::endl;
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
		isEpressed  = true;
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
}


void DrawScene() {
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f); // 배경색
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(shaderProgramID); // 셰이더 프로그램 활성화

    glBindVertexArray(vao);
    for(int k=0;k<shapes.size();k++) {
        float size = 0.1f; // 도형의 크기

		float X = shapes[k].x;
	    float Y = shapes[k].y;
		float R = shapes[k].r;
		float G = shapes[k].g;
		float B = shapes[k].b;

        // 각 도형을 그리는 코드
        if (shapes[k].type == POINT) {
			float posData[] = {
				X, Y, 0.0f // Vertex position
			};
            float colorData[] ={
				R, G, B // Color for Vertex 1
			};
            glBindBuffer(GL_ARRAY_BUFFER, vbo[0]); // 바인딩
			glBufferData(GL_ARRAY_BUFFER, sizeof(posData), posData, GL_DYNAMIC_DRAW);// 위치 데이터 업데이트

			glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);//바인딩, layout location = 1 -> vbo[1]에 색상 데이터 업데이트
			glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_DYNAMIC_DRAW); //색상 데이터 업데이트

            glPointSize(15.0f);
			glDrawArrays(GL_POINTS, 0, 1); // 점 그리기

        }
        else if(shapes[k].type == LINE) {
            float posData[] = {
                X, Y, 0.0f,
                X + size, Y + size, 0.0f
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
			glDrawArrays(GL_LINES, 0, 2); // 선 그리기
        }
        else if(shapes[k].type == TRIANGLE) {
			float posData[] = {
				X - size, Y - size, 0.0f, // Vertex 1
				X + size, Y - size, 0.0f, // Vertex 2
				X, Y + size, 0.0f  // Vertex 3
			};
			float colorData[] = {
				R, G, B, // Color for Vertex 1
				R, G, B, // Color for Vertex 2
				R, G, B  // Color for Vertex 3
			};
            
			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(posData), posData, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_DYNAMIC_DRAW);

			glDrawArrays(GL_TRIANGLES, 0, 3); // 삼각형 그리기)

        }
        else if(shapes[k].type == RECTANGLE) {
			float posData[] = {
				X - size, Y - size, 0.0f, // Vertex 1
				X + size, Y - size, 0.0f, // Vertex 2
				X + size, Y + size, 0.0f, // Vertex 3
				X - size, Y + size, 0.0f  // Vertex 4
			};
			unsigned int index[] = {
				0, 1, 2, // First triangle
				2, 3, 0  // Second triangle
			};
            float colorData[] = {
				R, G, B, // Color for Vertex 1
				R, G, B, // Color for Vertex 2
				R, G, B, // Color for Vertex 3
				R, G, B  // Color for Vertex 4
            };

			glBindBuffer(GL_ARRAY_BUFFER, vbo[0]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(posData), posData, GL_DYNAMIC_DRAW);

			glBindBuffer(GL_ARRAY_BUFFER, vbo[1]);
			glBufferData(GL_ARRAY_BUFFER, sizeof(colorData), colorData, GL_DYNAMIC_DRAW);

			//ebo 데이터 바인딩 및 업데이트
			glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
			glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(index), index, GL_DYNAMIC_DRAW);

			glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0); // 사각형 그리기
        }
    }
}
