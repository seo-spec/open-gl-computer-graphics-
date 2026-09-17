    #include <GL/glew.h>
    #include <GL/glfw3.h>
    #include <iostream>
    #include <vector>
    #include <random>

    using namespace std;

    bool isAPressed = false;  // A 키가 눌렸는지 여부를 추적하는 변수
	bool leftMousePressed = false; // 마우스 버튼이 눌렸는지 여부를 추적하는 변수

    random_device rd;
    mt19937 gen(rd());

    struct Rect {
	    float x1, y1, x2, y2;
	    float r, g, b;
	    bool isSelected; // 선택 여부를 나타내는 변수
    };

    vector<Rect> rect;

    // 1. window 변수를 전역 변수로 배치 (InputProcess에서도 접근 가능하게)
    GLFWwindow* window = nullptr;

    float r = 1.0f;
    float g = 1.0f;
    float b = 1.0f;

    float prevX = 0.0f;
    float prevY = 0.0f;


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

    void InputProcess() {
        if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
            glfwSetWindowShouldClose(window, true);
        }
	    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            if (!isAPressed) {
              uniform_real_distribution<float> colorDist(0.0f, 1.0f);
		      uniform_real_distribution<float> posDistx(-0.8f, 0.8f);
		      float x1 = posDistx(gen);
		      float y1 = posDistx(gen);
		      float x2 = x1 + 0.2f;
		      float y2 = y1 + 0.2f;
		      float r = colorDist(gen);
		      float g = colorDist(gen);
		      float b = colorDist(gen); 
		      Rect newRect = {
			      x1, y1, x2, y2,
			      r,g,b,
			      false
		      };
		      rect.push_back(newRect);

              if (rect.size() > 10) {
			      rect.erase(rect.begin());
              }
              isAPressed = true;
            }
	    }
        else {
            isAPressed = false;
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);
            float mx = (float)((xpos / 1280.0f) * 2.0f - 1.0f);
            float my = (float)(1.0f - (ypos / 900.0f) * 2.0f);
            if (mx < -1.0f) mx = -1.0f;
            if (mx > 1.0f)  mx = 1.0f;
            if (my < -1.0f) my = -1.0f;
            if (my > 1.0f)  my = 1.0f;
            if (!leftMousePressed) {
				for (int k = 0;k < rect.size();k++) {
					if (mx >= rect[k].x1 && mx <= rect[k].x2 && my >= rect[k].y1 && my <= rect[k].y2) {
                        rect[k].isSelected = true;
                        break;
					}
				}
                leftMousePressed = true;
            }
            else {
				
				float DeltaX = mx - prevX;
				float DeltaY = my - prevY;
                
				for (int k = 0;k < rect.size();k++) {
					if (rect[k].isSelected) {
						float width = rect[k].x2 - rect[k].x1;
						float height = rect[k].y2 - rect[k].y1; 

                      
                       
                        if (rect[k].x1 + DeltaX < -1.0f) {
                            DeltaX = -1.0f - rect[k].x1;
                        }
                        if (rect[k].x2 + DeltaX > 1.0f) {
                            DeltaX = 1.0f - rect[k].x2;
                        }
                        if (rect[k].y1 + DeltaY < -1.0f) {
                            DeltaY = -1.0f - rect[k].y1;
                        }
                        if (rect[k].y2 + DeltaY > 1.0f) {
                            DeltaY = 1.0f - rect[k].y2;
                        }
						
                        rect[k].x1 += DeltaX;
                        rect[k].x2 += DeltaX;
                        rect[k].y1 += DeltaY;
                        rect[k].y2 += DeltaY;
                        cout << "Selected Rect Moved: (" << rect[k].x1 << ", " << rect[k].y1 << ") to (" << rect[k].x2 << ", " << rect[k].y2 << ")" << endl;
					}
				}

            }
            prevX = mx;
            prevY = my;
		}
        else {
			leftMousePressed = false;
			for (int k = 0;k < rect.size();k++) {
				rect[k].isSelected = false;
			}
        }

    }


    // 4. 렌더링 함수 정의
    void DrawScene() {
        glClearColor(r, g, b, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
	    for (const auto& rect : rect) {
		    glColor3f(rect.r, rect.g, rect.b);
		    glRectf(rect.x1, rect.y1, rect.x2, rect.y2);
	    }
    }