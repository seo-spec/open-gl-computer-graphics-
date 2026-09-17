    #include <GL/glew.h>
    #include <GL/glfw3.h>
    #include <iostream>
    #include <vector>
    #include <random>
    #include<algorithm>


    using namespace std;

    bool isAPressed = false;  // A 키가 눌렸는지 여부를 추적하는 변수
	bool leftMousePressed = false; // 왼쪽 마우스 버튼이 눌렸는지 여부를 추적하는 변수
	bool RightMousePressed = false; // 오른쪽 마우스 버튼이 눌렸는지 여부를 추적하는 변수

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

        // A 키: 새로운 랜덤 사각형 생성
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            if (!isAPressed) {
                uniform_real_distribution<float> colorDist(0.0f, 1.0f);
                uniform_real_distribution<float> posDistx(-0.8f, 0.8f);
                float x1 = posDistx(gen);
                float y1 = posDistx(gen);
                float x2 = x1 + 0.2f;
                float y2 = y1 + 0.2f;

                Rect newRect = {
                    x1, y1, x2, y2,
                    colorDist(gen), colorDist(gen), colorDist(gen),
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

        // =========================================================
        // 마우스 폴링 처리 (PRESS / RELEASE 상태 분리)
        // =========================================================
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            double xpos, ypos;
            glfwGetCursorPos(window, &xpos, &ypos);

            // 윈도우 좌표 -> OpenGL NDC 좌표 (-1.0 ~ 1.0) 변환
            float mx = (float)((xpos / 1280.0f) * 2.0f - 1.0f);
            float my = (float)(1.0f - (ypos / 900.0f) * 2.0f);

            // 커서 위치 화면 내부로 고정 (Clamp)
            if (mx < -1.0f) mx = -1.0f; if (mx > 1.0f) mx = 1.0f;
            if (my < -1.0f) my = -1.0f; if (my > 1.0f) my = 1.0f;

            // [상태 1] 마우스를 처음 누른 순간 (PRESS Initial)
            if (!leftMousePressed) {
                // 맨 위에 그려진(vector의 뒤쪽에 있는) 사각형부터 클릭 검사
                for (int k = (int)rect.size() - 1; k >= 0; k--) {
                    if (mx >= rect[k].x1 && mx <= rect[k].x2 && my >= rect[k].y1 && my <= rect[k].y2) {
                        rect[k].isSelected = true;
                        break; // 하나만 선택
                    }
                }
                leftMousePressed = true; // 드래그 상태 진입
            }
            // [상태 2] 마우스를 누른 채 드래그 중인 동안 (Holding / Dragging)
            else {
                float deltaX = mx - prevX;
                float deltaY = my - prevY;

                for (size_t k = 0; k < rect.size(); k++) {
                    if (rect[k].isSelected) {
                        // 화면 밖으로 나가지 않도록 이동량(delta) 제한
                        if (rect[k].x1 + deltaX < -1.0f) deltaX = -1.0f - rect[k].x1;
                        if (rect[k].x2 + deltaX > 1.0f)  deltaX = 1.0f - rect[k].x2;
                        if (rect[k].y1 + deltaY < -1.0f) deltaY = -1.0f - rect[k].y1;
                        if (rect[k].y2 + deltaY > 1.0f)  deltaY = 1.0f - rect[k].y2;

                        // 이동 적용
                        rect[k].x1 += deltaX;
                        rect[k].x2 += deltaX;
                        rect[k].y1 += deltaY;
                        rect[k].y2 += deltaY;
                    }
                }
            }
            prevX = mx;
            prevY = my;
        }
        // [상태 3] 마우스 버튼을 뗐을 때
        else if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE&&leftMousePressed) {
            if (leftMousePressed) { // 이전 프레임까지 누르고 있었다면
                int selectedIdx = -1;
                for (size_t k = 0; k < rect.size(); k++) {
                    if (rect[k].isSelected) {
                        selectedIdx = (int)k;
                        break;
                    }
                }

                // 선택된 사각형이 존재할 때 겹침 판정 및 합치기 수행
                if (selectedIdx != -1) {
                    for (int k = 0; k < (int)rect.size(); k++) {
                        if (k == selectedIdx) continue;

                        Rect A = rect[selectedIdx];
                        Rect B = rect[k];

                       
                        bool isOverlapped = !(A.x2 < B.x1 || A.x1 > B.x2 || A.y2 < B.y1 || A.y1 > B.y2);

                        if (isOverlapped) {
                            // 1. min/max 연산으로 두 도형을 모두 포함하는 큰 사각형 좌표 계산
                            uniform_real_distribution<float> colorDist(0.0f, 1.0f);
                            Rect mergedRect = {
                                min(A.x1, B.x1),
                                min(A.y1, B.y1),
                                max(A.x2, B.x2),
                                max(A.y2, B.y2),
                                colorDist(gen), colorDist(gen), colorDist(gen), // 새로운 랜덤 색상
                                false
                            };

                            // 2. 기존 두 사각형 제거 (인덱스 큰 쪽부터 지워야 배열 파손 방지)
                            int firstErase = max(selectedIdx, k);
                            int secondErase = min(selectedIdx, k);

                            rect.erase(rect.begin() + firstErase);
                            rect.erase(rect.begin() + secondErase);

                            // 3. 합쳐진 큰 사각형 추가
                            rect.push_back(mergedRect);
                            break; // 한 번에 한 쌍만 합치기
                        }
                    }

                    // 선택 상태 해제
                    for (auto& r : rect) {
                        r.isSelected = false;
                    }
                }

                leftMousePressed = false; // 마우스 스위치 해제
            }
        }
        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_RIGHT) == GLFW_PRESS) {
            double xpos, ypos;
			glfwGetCursorPos(window, &xpos, &ypos);
			float mx = (float)((xpos / 1280.0f) * 2.0f - 1.0f);
			float my = (float)(1.0f - (ypos / 900.0f) * 2.0f);
			// mx 구하는 공식: ((xpos / window_width) * 2.0f - 1.0f)
			// my 구하는 공식: (1.0f - (ypos / window_height) * 2.0f)
            if (!RightMousePressed) {
                if (rect.size() < 20) {
                    for (int k = (int)rect.size() - 1;k >= 0;k--) {
                        if (mx >= rect[k].x1 && mx <= rect[k].x2 && my >= rect[k].y1 && my <= rect[k].y2) {
                            Rect target = rect[k];
                            float midX = (target.x1 + target.x2) / 2.0f;
                            float midY = (target.y1 + target.y2) / 2.0f;
                            rect.erase(rect.begin() + k);
                            uniform_real_distribution<float>posx1(target.x1, midX);
                            uniform_real_distribution<float>posy1(target.y1, midY);
                            uniform_real_distribution<float>posx2(midX, target.x2);
                            uniform_real_distribution<float>posy2(midY, target.y2);
                            uniform_real_distribution<float> size(0.05f, 0.3f);
							uniform_real_distribution<float> colorDist(0.0f, 1.0f);

                            float x1 = posx1(gen);
                            float y1 = posy1(gen);
                            float x2 = x1+size(gen);
                            float y2 = y1+size(gen);
                            Rect newRect1 = {
                                x1, y1, x2, y2,
                                colorDist(gen), colorDist(gen), colorDist(gen),
                                false
                            };
                            float x3 = posx2(gen);
                            float y3 = posy2(gen);
                            float x4 = x3+size(gen);
                            float y4 = y3+size(gen);
                            Rect newRect2 = {
                                x3, y3, x4, y4,
								colorDist(gen), colorDist(gen), colorDist(gen),
                                false
                            };
                            rect.push_back(newRect1);
                            rect.push_back(newRect2);
                            break;
                        }
                    }
                }
            }

            RightMousePressed = true;
        }
        else {
            RightMousePressed = false;
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