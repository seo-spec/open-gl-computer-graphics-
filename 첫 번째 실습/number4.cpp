#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <random>
#include <cmath>
#include <algorithm> // std::min

using namespace std;

struct Rect {
    float x1, y1, x2, y2;// 사각형의 좌표
	float Orgx1, Orgy1, Orgx2, Orgy2; // 사각형의 원래 좌표
	float vx, vy; // 사각형의 속도
    int edgestate; // 사각형의 이동방향(시계 방향)
	float scaleDir; // 사각형의 크기 변화 방향
    float r, g, b;
};

// 1. window 변수를 전역 변수로 배치 (InputProcess에서도 접근 가능하게)
GLFWwindow* window = nullptr;
float r = 0.25f;
float g = 0.25f;
float b = 0.25f;

vector <Rect> rects;
bool Leftpressed = false;
bool isMoved1 = false;// 사각형들은 각각 대각선으로 이동하고 벽에 닿으면 튕겨 다른 방향으로 이동한다/멈춘다.
bool isMoved2 = false;// 사각형들이 가로 지그재그로 이동한다/멈춘다.
bool isMoved3 = false;// 모든 사각형들이 윈도우의 가장자리를 따라 시계방향으로 순서대로 이동한다/멈춘다.
bool isMoved4 = false;// 사각형들이 크기가 커졌다 작아졌다를 반복한다.
bool isMoved5 = false;// 사각형들이 랜덤한 위치로 순간이동한다/멈춘다.

bool Onepressed = false;//1번 버튼이 눌렸나 여부
bool Twopressed = false;//2번 버튼이 눌렸나 여부
bool Threepressed = false;//3번 버튼이 눌렸나 여부
bool Fourpressed = false;//4번 버튼이 눌렸나 여부
bool Fivepressed = false; //5번 버튼이 눌렸나 여부
bool Spressed = false; //S버튼이 눌렸나 여부
bool Rpressed = false; //R버튼이 눌렸나 여부
bool Mpressed = false; //M버튼이 눌렸나 여부



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

    double lastTime = 0.0; // 마지막 시간을 저장할 변수 초기화
	float ColorTime = 0.0f; // 색상 변경을 위한 시간 변수 초기화

    // 메인 루프
    while (!glfwWindowShouldClose(window)) {
		double currentTime = glfwGetTime(); // 현재 시간을 가져옴
		double DeltaTime = currentTime - lastTime; // 지난 프레임과의 시간 차이 계산
        lastTime = currentTime;
        if (isMoved1 == true) {
			for (int k = rects.size() - 1;k >= 0;k--) {

				rects[k].x1 += rects[k].vx * (float)DeltaTime;
				rects[k].y1 += rects[k].vy * (float)DeltaTime;
				rects[k].x2 += rects[k].vx * (float)DeltaTime;
				rects[k].y2 += rects[k].vy * (float)DeltaTime;
                if (rects[k].x2 >= 1.0f || rects[k].x1 <= -1.0f)  {
                    rects[k].vx = -rects[k].vx;
                }
				if (rects[k].y2 >= 1.0f || rects[k].y1 <= -1.0f) {
					rects[k].vy = -rects[k].vy;
				}
			}
        }
        if (isMoved2 == true) {
            for (int k = rects.size() - 1;k >= 0;k--) {
                
                rects[k].x1 += rects[k].vx * (float)DeltaTime;
                rects[k].x2 += rects[k].vx * (float)DeltaTime;
                rects[k].vy = copysign(0.05f, rects[k].vy); // 값을 그대로 두면서 부호는 유지

				float width = rects[k].x2 - rects[k].x1;
				float height = rects[k].y2 - rects[k].y1;
                if (rects[k].x2 >= 1.0f) {
					rects[k].y1 += rects[k].vy;
					rects[k].y2 += rects[k].vy;

                    rects[k].x2 = 1.0f;
					rects[k].x1 = 1.0f - width;

					rects[k].vx = -rects[k].vx;
                }
                if (rects[k].x1 <= -1.0f) {
                    rects[k].y1 += rects[k].vy;
                    rects[k].y2 += rects[k].vy;

					rects[k].x1 = -1.0f;
					rects[k].x2 = -1.0f + width;

					rects[k].vx = -rects[k].vx;
                }
				if (rects[k].y2 >= 1.0f) {
					rects[k].y2 = 1.0f;
					rects[k].y1 = 1.0f - height;

                    rects[k].vy = -rects[k].vy;
				}
				if (rects[k].y1 <= -1.0f) {
					rects[k].y1 = -1.0f;
					rects[k].y2 = -1.0f + height;

                    rects[k].vy = -rects[k].vy;
				}
            }
        }
        if (isMoved3 == true) {
            for (int k = rects.size() - 1; k >= 0; k--) {
                float width = rects[k].x2 - rects[k].x1;
                float height = rects[k].y2 - rects[k].y1;


                // 0: 상단 레일 (X축 +방향 이동) -> 오른쪽 코너 도달 시 edgestate = 1
                if (rects[k].edgestate == 0) {
                    rects[k].x1 += rects[k].vx * (float)DeltaTime;
                    rects[k].x2 += rects[k].vx * (float)DeltaTime;

                    if (rects[k].x2 >= 1.0f) {
                        rects[k].x2 = 1.0f;
                        rects[k].x1 = 1.0f - width;
                        rects[k].edgestate = 1; // 우측 레일로 전환 (아래로 이동)
                    }
                }
                // 1: 우측 레일 (Y축 -방향 이동) -> 아래쪽 코너 도달 시 edgestate = 2
                else if (rects[k].edgestate == 1) {
                    rects[k].y1 -= rects[k].vy * (float)DeltaTime;
                    rects[k].y2 -= rects[k].vy * (float)DeltaTime;

                    if (rects[k].y1 <= -1.0f) {
                        rects[k].y1 = -1.0f;
                        rects[k].y2 = -1.0f + height;
                        rects[k].edgestate = 2; // 하단 레일로 전환 (왼쪽으로 이동)
                    }
                }
                // 2: 하단 레일 (X축 -방향 이동) -> 왼쪽 코너 도달 시 edgestate = 3
                else if (rects[k].edgestate == 2) {
                    rects[k].x1 -= rects[k].vx * (float)DeltaTime;
                    rects[k].x2 -= rects[k].vx * (float)DeltaTime;

                    if (rects[k].x1 <= -1.0f) {
                        rects[k].x1 = -1.0f;
                        rects[k].x2 = -1.0f + width;
                        rects[k].edgestate = 3; // 좌측 레일로 전환 (위로 이동)
                    }
                }
                // 3: 좌측 레일 (Y축 +방향 이동) -> 위쪽 코너 도달 시 edgestate = 0
                else if (rects[k].edgestate == 3) {
                    rects[k].y1 += rects[k].vy * (float)DeltaTime;
                    rects[k].y2 += rects[k].vy * (float)DeltaTime;

                    if (rects[k].y2 >= 1.0f) {
                        rects[k].y2 = 1.0f;
                        rects[k].y1 = 1.0f - height;
                        rects[k].edgestate = 0; 
                    }
                }
            }
        }
        if (isMoved4 == true) {
              for(int k=rects.size()-1;k>=0;k--) {
				float width = rects[k].x2 - rects[k].x1;
				   
				float speed = 0.05f; // 크기 변화 속도
				if (width >= 0.4f) {
						rects[k].scaleDir = -1.0f; // 크기 감소 방향
				}
				else if (width <= 0.05f) {
						rects[k].scaleDir = 1.0f; // 크기 증가 방향
				}

				float change = rects[k].scaleDir * speed * (float)DeltaTime;
				rects[k].x1 -= change;
				rects[k].y1 -= change;
				rects[k].x2 += change;
				rects[k].y2 += change;
			
              }
        }
        if (isMoved5) {
            for (int k = rects.size() - 1;k >= 0;k--) {
				uniform_real_distribution<float>Colordist(0.0f, 1.0f);
				ColorTime += DeltaTime;
                if (ColorTime >= 1.0f) {
                    ColorTime = 0.0f;
                    rects[k].r = Colordist(gen);
                    rects[k].g = Colordist(gen);
                    rects[k].b = Colordist(gen);
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

			Rect newRect =
			{ mx - w,my - h,mx + w,my + h,mx - w,my - h,mx + w,my + h,
                0.3f,0.3f,0,1.0f,colorDist(gen),colorDist(gen),colorDist(gen)};
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

    if(glfwGetKey(window,GLFW_KEY_1) == GLFW_PRESS) {
        if (!Onepressed) {
            isMoved1 = !isMoved1;
            isMoved2 = false;
            isMoved3 = false;
            if (isMoved1) {
                for (auto& rect : rects) {
                    // 부호(방향)는 유지하고 크기만 기본 속도 0.3f로 원상복구
                    rect.vx = (rect.vx >= 0.0f) ? 0.3f : -0.3f;
                    rect.vy = (rect.vy >= 0.0f) ? 0.3f : -0.3f;
                }
            }
        }
        Onepressed = true;
    } else {
        
        Onepressed = false;
    }
	if (glfwGetKey(window, GLFW_KEY_2) == GLFW_PRESS) {
		if (!Twopressed) {
			isMoved2 = !isMoved2;
			isMoved1 = false;
            isMoved3 = false;
            if (isMoved2) {
                for (auto& rect : rects) {
                    rect.vx = (rect.vx >= 0.0f) ? 0.3f : -0.3f;
                    rect.vy = (rect.vy >= 0.0f) ? 0.05f : -0.05f;
                }
            }
		}
		Twopressed = true;
	} else {
		Twopressed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_3) == GLFW_PRESS) {
		if (!Threepressed) {
			isMoved3 = !isMoved3;
			isMoved2 = false;
			isMoved1 = false;

            for (int k = rects.size() - 1;k >= 0;k--) {
                rects[k].vx = 0.3f;
                rects[k].vy = 0.3f;
				float centerX = (rects[k].x1 + rects[k].x2) / 2.0f;
				float centerY = (rects[k].y1 + rects[k].y2) / 2.0f;
				float width = rects[k].x2 - rects[k].x1;
				float height = rects[k].y2 - rects[k].y1;
				float disTop = 1.0f - centerY;
				float disBottom = centerY -(-1.0f);
				float disRight = 1.0f - centerX;
				float disLeft = centerX -(-1.0f);
				float minDis = min({ disTop, disBottom, disRight, disLeft });
				if (minDis == disTop) {
                    rects[k].y2 = 1.0f;
                    rects[k].y1 = 1.0f - height;

					rects[k].edgestate = 0; // 위쪽
				} else if (minDis == disBottom) {
                    rects[k].y1 = -1.0f;
					rects[k].y2 = -1.0f + height;

					rects[k].edgestate = 2; // 아래쪽
				} else if (minDis == disRight) {
					rects[k].x2 = 1.0f;
					rects[k].x1 = 1.0f - width;

					rects[k].edgestate = 1; // 오른쪽
				} else if (minDis == disLeft) {
					rects[k].x1 = -1.0f;
					rects[k].x2 = -1.0f + width;

					rects[k].edgestate = 3; // 왼쪽
				}   
            }
		}
		Threepressed = true;
	} else {
		Threepressed = false;
	}
	if (glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
		if (!Fourpressed) {
			isMoved4 = !isMoved4;
		}
		Fourpressed = true;
	}
	else {
		Fourpressed = false;
	}
    if (glfwGetKey(window, GLFW_KEY_5) == GLFW_PRESS) {
        if (!Fivepressed) {
			isMoved5 = !isMoved5;
        }
		Fivepressed = true;
    }
    else {
		Fivepressed = false;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        if (!Spressed) {
            isMoved1 = false;
            isMoved2 = false;
            isMoved3 = false;
            isMoved4 = false;
            isMoved5 = false;
        }
        Spressed = true;
    }
    else {
        Spressed = false;
    }

	if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
		if (!Rpressed) {
			rects.clear();
		}
		Rpressed = true;
	}
	else {
		Rpressed = false;
	}

    if (glfwGetKey(window, GLFW_KEY_M) == GLFW_PRESS) {
        if (!Mpressed) {
            isMoved1 = false;
            isMoved2 = false;
			isMoved3 = false;
			isMoved4 = false;
			isMoved5 = false;
            for (int k = rects.size() - 1;k >= 0;k--) {
				rects[k].x1 = rects[k].Orgx1;
				rects[k].y1 = rects[k].Orgy1;
				rects[k].x2 = rects[k].Orgx2;
                rects[k].y2 = rects[k].Orgy2;

				rects[k].vx = 0.3f;
				rects[k].vy = 0.3f;
            }
        }
        Mpressed = true;
    }
    else {
		Mpressed = false;
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