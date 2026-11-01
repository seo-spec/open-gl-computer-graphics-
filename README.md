# 🎨 OpenGL Computer Graphics

이 저장소는 OpenGL 및 컴퓨터 그래픽스 실습 과정을 정리한 개인 학습 저장소입니다.  
현재 `첫 번째 실습`, `두 번째 실습`, `세 번째 실습`이 포함되어 있습니다.

## 📚 Topics

- OpenGL 기본 렌더링
- Vertex Shader
- Fragment Shader
- GLSL
- Shader Compilation
- Shader Program Linking
- 외부 GLSL 파일 관리
- C++와 GLSL 코드 분리
- OpenGL Rendering Pipeline 기초
- OBJ 모델 로딩과 3D 변환·애니메이션
- Model / View / Projection 행렬과 원근·직교 투영
- 행성·위성 공전과 계층적 변환
- 탱크 계층적 모델링과 카메라 이동·회전

## 🗂️ Project Structure

```text
open-gl-computer-graphics-/
├── README.md
├── 첫 번째 실습/
│   ├── console11.vcxproj
│   ├── console11.vcxproj.filters
│   ├── console11.vcxproj.user
│   ├── number1.cpp
│   ├── number2.cpp
│   ├── number3.cpp
│   ├── number4.cpp
│   ├── number5.cpp
│   └── number6.cpp
├── 두 번째 실습/
│   ├── .gitignore
│   ├── opengl2.slnx
│   └── opengl2/
│       └── opengl2/
│           ├── Shader.h
│           ├── fragment.glsl
│           ├── number7.cpp
│           ├── number8.cpp
│           ├── number9.cpp
│           ├── number10.cpp
│           ├── number11.cpp
│           ├── number12.cpp
│           ├── opengl2.vcxproj
│           ├── opengl2.vcxproj.filters
│           └── vertex.glsl
└── 세 번째 실습/
    ├── .gitignore
    └── opengl3/
        ├── opengl3.slnx
        └── opengl3/
            ├── Obj.h
            ├── Shader.h
            ├── cone.obj
            ├── cube.obj
            ├── fragment.glsl
            ├── fragment14.glsl
            ├── fragment18.glsl
            ├── launcher.cpp
            ├── number13.cpp
            ├── number14.cpp
            ├── number15.cpp
            ├── number16.cpp
            ├── number17.cpp
            ├── number18.cpp
            ├── number19.cpp
            ├── opengl3.vcxproj
            ├── opengl3.vcxproj.filters
            ├── pyramid.obj
            ├── sphere.obj
            ├── vertex.glsl
            ├── vertex14.glsl
            └── vertex18.glsl
```

## 🧪 Practice 1

첫 번째 실습은 OpenGL 기초 상호작용과 2D 도형 렌더링 중심으로 구성되어 있습니다.

- `number1.cpp`: keyboard input으로 배경색을 변경하고 타이머 기반 랜덤 색상 전환을 실습합니다.
- `number2.cpp`: 사분면별 사각형 생성, 선택/크기 조절/색상 변경/초기화 기능을 통합한 확장 실습입니다.
- `number3.cpp`: 사각형 선택, 이동, 겹침 병합, 우클릭 분할 등 고급 2D 상호작용을 다룹니다.
- `number4.cpp`: 마우스 클릭 위치에 사각형 생성 후, 사각형들을 애니메이션으로 움직입니다.
- `number5.cpp`: 마우스 지우개 사각형 충돌 및 생성/리셋 기능 구현.
- `number6.cpp`: 퍼져나가는 사각형 애니메이션, 시간에 따른 크기/색상 변화를 구현합니다.

## 🧪 Practice 2 - OpenGL Shader

두 번째 실습은 C++ 프로그램과 GLSL Shader 코드를 분리하여 관리합니다.

- `number7.cpp`: GLFW/GLEW 초기화, VAO/VBO/EBO 구성, 입력 처리, 도형 렌더링 루프를 담당하며 `InitShader("vertex.glsl", "fragment.glsl")`를 호출해 셰이더 프로그램을 생성/사용합니다.
- `number8.cpp`: OpenGL + GLFW + GLEW 기반으로 `Shape` 구조체(`x, y, r, g, b, size`) 배열을 관리하며, `InitShader("vertex.glsl", "fragment.glsl")`로 셰이더를 초기화한 뒤 VAO/VBO를 사용해 기준선(GL_LINES)과 삼각형(GL_TRIANGLES)을 렌더링합니다. 마우스 좌클릭으로 사분면별 도형 생성, 우클릭으로 해당 사분면 도형 크기 재설정, `A`/`B` 키로 채움·와이어프레임 모드 전환, `C` 키로 전체 도형 초기화, `Q` 키로 종료 입력을 처리합니다.
- `number9.cpp`: OpenGL + GLFW + GLEW 환경에서 `Triangle` 구조체(`x, y, size, r, g, b, vx, vy`) 벡터를 관리하며, 좌클릭으로 랜덤 크기/색상/속도의 삼각형을 생성하고 VAO/VBO를 통해 동적으로 렌더링합니다. `C` 키로 전체 삭제, `Q` 키로 종료를 처리하며, `1`~`4` 키로 이동 모드를 전환해 (1) X/Y 경계 반사 이동, (2) X축 왕복 + 경계 충돌 시 Y 단계 이동, (3) 속도 재설정 후 X/Y 동시 반사 이동, (4) 극좌표 기반 회전·반지름 팽창/수축 이동을 적용합니다.
- `number10.cpp`: OpenGL + GLFW + GLEW 환경에서 퍼즐 조각(`Piece`)과 정답 슬롯(`Slot`) 목록을 구성해 드래그 앤 드롭 맞추기 게임을 구현합니다. 좌측에 랜덤 위치/색상으로 생성된 사각형·정삼각형·직각삼각형 조각을 마우스 좌클릭 드래그로 우측 윤곽 슬롯에 배치하며, 슬롯과 조각의 도형 종류·방향·크기·근접 조건이 모두 맞으면 해당 위치에 고정됩니다. `R` 키로 퍼즐을 초기화하고 `Q` 키로 프로그램을 종료합니다.
- `number11.cpp`: OpenGL + GLFW + GLEW 환경에서 격자 보드 기반 장애물 게임을 구현합니다. 시작 시 콘솔 입력으로 보드 크기를 받아 셀 크기를 계산하고, 각 칸에 확률적으로 장애물(사각형/정삼각형/역삼각형)과 랜덤 색상을 배치합니다. 플레이어는 초기 `(0,0)` 칸에서 사각형 형태로 시작해 `W`/`A`/`S`/`D` 키로 경계 안에서 이동하며, 장애물이 있는 칸에 진입하면 장애물과 플레이어의 도형 타입이 서로 스왑됩니다. 충돌 직후에는 짧은 시간 확대 + 무지개 색상 효과를 적용하고, `Q` 키로 프로그램을 종료합니다.
- `number12.cpp`: OpenGL + GLFW + GLEW 환경에서 두 개의 사각형이 각 통로를 따라 아래로 이동하는 타이밍 게임을 구현합니다. `Enter`를 눌렀을 때 두 사각형이 중앙 판정 영역 안에 동시에 들어오면 성공으로 처리되고, 성공한 쌍은 오른쪽 스택 영역으로 이동해 누적 표시됩니다. `R` 키로 전체 상태를 초기화하고 `Q` 키로 프로그램을 종료합니다.
- `Shader.h`: 외부 GLSL 파일을 읽어 Vertex Shader와 Fragment Shader를 생성·컴파일하고, 이를 하나의 Shader Program으로 링크하여 사용할 수 있도록 하는 재사용 가능한 OpenGL Shader 초기화 헤더입니다.
- `vertex.glsl`: 정점 위치(`layout(location = 0)`)와 색상(`layout(location = 1)`)을 입력받아 `gl_Position`과 색상 출력 변수(`out_Color`)를 설정합니다.
- `fragment.glsl`: Vertex Shader에서 전달된 `out_Color`를 받아 최종 픽셀 색상(`Frag_Color`)으로 출력합니다.

## 🧪 Practice 3 - OBJ 로딩과 3D 변환·애니메이션

세 번째 실습은 13~19번으로 구성되며, OBJ 모델의 면 선택에서 시작해 이동·회전·크기 변환, 면 애니메이션, 여러 객체의 공전과 계층적 변환, 탱크 모델링과 카메라 제어로 확장합니다.

- `number13.cpp`: GLFW/GLEW 초기화 후 `cube.obj`, `pyramid.obj`를 로드하고, VAO/VBO/EBO를 구성해 좌표축과 3D 도형을 렌더링합니다. `1`~`6` 키로 정육면체 면 선택, `7`~`0` 키로 사각뿔 옆면 선택, `C` 키로 정육면체 임의 2면 선택, `T` 키로 바닥+임의 옆면 선택, `ESC` 키로 종료를 처리합니다.
- `number14.cpp`: `cube.obj`, `pyramid.obj`를 모두 로드해 객체 토글/이동/회전/렌더링 모드 전환을 실습합니다. `C`/`P` 키로 정육면체·사각뿔 표시를 전환하고, 방향키로 위치 이동, `X`/`Y` 키로 각 축 회전 방향을 토글, `H` 키로 깊이 테스트 ON/OFF, `W` 키로 와이어프레임/채움 모드 전환, `S` 키로 위치·회전 상태 초기화, `ESC` 키로 종료합니다.
- `number15.cpp`: `cube.obj`, `pyramid.obj`를 로드해 좌표축과 객체를 렌더링하고, 키 입력으로 큐브 면 애니메이션/회전 상태를 제어합니다. `P` 키로 큐브·피라미드 표시 대상을 순환 전환하고, `H` 키로 깊이 테스트를 토글하며, `Y` 키로 Y축 공전 회전을 켜고 끕니다. `T`(윗면 Z축 회전), `F`(문 열기), `S`(측면 회전), `B`(뒷면 스케일), `1`(임의 인접 2면 페어 회전), `2`(임의 2면 위치 교체), `4`(Y축 기준 면 순환), `5`(X축 기준 면 순환), `R`(전체 상태 초기화), `Q`/`ESC`(종료) 키를 지원합니다.
- `number16.cpp`: `pyramid.obj`를 로드해 좌표축과 사각뿔을 렌더링하고 면 단위 애니메이션을 실습합니다. `P` 키로 사각뿔 표시를 시작하고, `H` 키로 깊이 테스트 ON/OFF, `Y` 키로 Y축 회전을 토글합니다. `6` 키로 사각뿔 옆면 전체 펼침/접힘, `7` 키로 옆면 순차 펼침/복귀, `8` 키로 옆면 회전 스왑 애니메이션, `9` 키로 꼭짓점 기준 열기/닫기 애니메이션을 제어하며 `Q`/`ESC` 키로 종료합니다.
- `number17.cpp`: 정육면체·구를 대상으로 개별/동시 선택, 회전·공전·이동·크기 변환과 자리 교환을 실습합니다. `C` 키로 사각뿔·원뿔 조합으로 전환할 수 있으며, 깊이 위치에 따른 크기 변화도 구현합니다. 상세 조작은 아래 표를 참고하세요.
- `number18.cpp`: `sphere.obj`를 재사용해 중심 구, 행성 3개와 각 행성의 위성을 렌더링합니다. 행성마다 임의의 공전 반지름·속도·색상을 설정하고, 0°/45°/-45°로 기울어진 행성 궤도와 위성 궤도를 선으로 표시합니다. Model/View/Projection 행렬을 분리해 원근·직교 투영 전환과 장면 이동을 지원합니다.
- `launcher.cpp`: `number13.cpp`~`number17.cpp`를 하나의 실행 진입점에서 선택 실행하는 런처입니다. OpenGL 메뉴 화면에서 버튼 클릭 또는 숫자 `1`~`5` 입력으로 Practice 13~17을 시작하고, `EXIT` 버튼/`ESC`로 종료할 수 있습니다. 선택한 실습은 별도 프로세스로 실행되어 종료 시 메뉴로 돌아옵니다. 18번은 현재 런처 메뉴에 포함되어 있지 않습니다.
- `Obj.h`: OBJ 파일의 `v`/`f` 라인을 파싱해 정점 좌표와 인덱스를 `ObjData` 구조체에 저장하는 로더를 제공합니다.
- `Shader.h`: 외부 GLSL 파일을 읽어 Vertex/Fragment Shader를 컴파일하고 Program으로 링크합니다.
- `vertex.glsl`: `modelTransform` 행렬을 적용해 3D 정점 좌표를 클립 공간으로 변환합니다.
- `fragment.glsl`: uniform `faceColor`를 사용해 면 단위 단색 렌더링을 수행합니다.
- `vertex14.glsl`, `fragment14.glsl`: 모델 변환 + 정점 색상 전달을 지원하며, uniform `useVertexColor`로 축 렌더링(고정색)과 객체 렌더링(정점색)을 분기합니다.
- `vertex18.glsl`: `projection * view * model` 순서로 정점 좌표를 변환합니다.
- `fragment18.glsl`: uniform `objectColor`를 최종 픽셀 색상으로 출력합니다.
- `cube.obj`, `pyramid.obj`, `sphere.obj`, `cone.obj`: 정육면체·사각뿔·구·원뿔 모델의 정점 및 면 인덱스 데이터입니다.

### 17번 조작 방법

| 키 | 동작 |
| --- | --- |
| `1` / `2` / `3` | 왼쪽 객체 / 오른쪽 객체 / 두 객체 선택 |
| `C` | 정육면체·구 ↔ 사각뿔·원뿔 전환 |
| `X` / `Y` | 선택 객체의 X/Y축 회전 시작, 다시 누르면 방향 반전 |
| `R` | 선택 객체의 Y축 공전 시작, 다시 누르면 방향 반전 |
| `A` | 선택 객체 자체 크기 확대/축소 실행 |
| `B` | 원점 기준 확대/축소 실행(위치와 크기 함께 변화) |
| `D` / `E` | 선택 객체의 X/Y축 이동 시작, 다시 누르면 방향 반전(경계에서 정지) |
| `T` / `U` | 두 객체를 원점 경유 / 위·아래 우회 경로로 자리 교환 |
| `V` | 두 객체 모두 Y축 회전·공전과 깊이에 따른 반복 크기 변화 시작 |
| `S` | 객체 변환·애니메이션과 도형 조합 초기화 |
| `Q` | 종료 |

### 18번 조작 방법

| 키 | 동작 |
| --- | --- |
| `P` | 원근 투영 ↔ 직교 투영 전환 |
| `M` | 채움 ↔ 와이어프레임 전환 |
| `1` | 행성·위성 궤도 반지름 확대 / 초기 반지름으로 복귀 |
| `2` | 중심 구의 반복 확대·축소와 0.5초 간격 임의 색상 변경 켜기/끄기 |
| `3` | 위성 궤도의 기울기 애니메이션 켜기/끄기 |
| `W` / `A` / `S` / `D` | 장면 전체를 위 / 왼쪽 / 아래 / 오른쪽으로 이동 |
| `+` / `-` | 장면 전체를 Z축 양/음의 방향으로 이동(`=` 및 숫자패드 덧셈·뺄셈 지원) |
| `Q` | 종료 |

### 19번 - 탱크 모델링과 카메라 제어

`number19.cpp`는 `cube.obj`로 하부 2개, 중앙 몸체 1개, 상부 2개, 포신 2개, 깃대 2개로 구성된 탱크와 바닥을 렌더링합니다. 부모 변환을 상속하는 계층 구조로 탱크와 부품을 제어하며, 카메라 이동·회전·공전과 색상 변경을 지원합니다.

| 키 | 동작 |
| --- | --- |
| 방향키 | 탱크를 XZ 평면에서 이동 |
| `T` | 중앙 몸체와 그 위 부품을 Y축으로 회전 |
| `G` | 두 포신을 서로 반대 방향으로 Y축 회전 |
| `P` | 두 깃대를 서로 반대 방향으로 X축 회전 |
| `D` | 하부 높이를 기본 → 확대 → 기본 → 축소 순서로 변경 |
| `L` | 두 상부와 부속 부품의 좌우 위치 교환 / 복귀 |
| `X` / `Shift+X` | 카메라를 X축 양 / 음의 방향으로 이동 |
| `Z` / `Shift+Z` | 카메라를 Z축 양 / 음의 방향으로 이동 |
| `Y` / `Shift+Y` | 카메라 시선의 Y축 회전 시작(서로 반대 방향), 같은 입력으로 정지 |
| `R` / `Shift+R` | 카메라의 원점 중심 공전 시작(서로 반대 방향), 같은 입력으로 정지 |
| `O` | 탱크 부품의 색상을 임의로 변경 |
| `C` | 탱크·카메라·애니메이션과 색상을 초기 상태로 복구 |
| `Q` | 종료 |

현재 프로젝트의 `Debug/x64` 구성은 `number19.cpp`를 직접 빌드하도록 설정되어 있습니다. 19번 실행 작업 폴더에 `cube.obj`, `vertex18.glsl`, `fragment18.glsl`을 준비해야 합니다. 18번은 `sphere.obj`와 동일한 셰이더 파일을 사용합니다. 18~19번은 현재 런처 메뉴에 포함되어 있지 않습니다. 17번은 `cube.obj`, `sphere.obj`, `pyramid.obj`, `cone.obj`와 `vertex.glsl`, `fragment.glsl`을 사용합니다.

전체 흐름:

```text
C++ 프로그램
      ↓
   Shader.h
      ↓
GLSL 파일 읽기
      ↓
Vertex Shader / Fragment Shader
      ↓
Shader Compile
      ↓
Shader Program Link
      ↓
OpenGL에서 사용
```

## 🧩 Shader.h

### 1. `filetobuf()`

`filetobuf(const char* file)`는 GLSL 파일 내용을 문자열로 읽어오는 함수입니다.

1. `std::ifstream`으로 파일을 엽니다.
2. 파일을 열지 못하면 오류 메시지를 출력하고 빈 문자열을 반환합니다.
3. `std::istreambuf_iterator<char>`를 이용해 파일 전체를 `std::string`으로 읽습니다.
4. 읽은 GLSL 소스 문자열을 반환합니다.

즉, `GLSL 파일 → 파일 열기 → 파일 내용 읽기 → std::string 반환` 역할을 수행합니다.

### 2. `InitShader()`

`InitShader(const char* vertPath, const char* fragPath)`는 Vertex/Fragment Shader를 컴파일하고 하나의 Program으로 링크한 뒤 Program ID를 반환합니다.

1. Vertex Shader 파일을 `filetobuf()`으로 읽습니다.
2. `glCreateShader(GL_VERTEX_SHADER)`로 Vertex Shader 객체를 생성합니다.
3. `glShaderSource()`로 소스를 전달합니다.
4. `glCompileShader()`로 컴파일합니다.
5. `glGetShaderiv(..., GL_COMPILE_STATUS, ...)`로 컴파일 성공 여부를 확인합니다.
6. 실패 시 `glGetShaderInfoLog()`로 오류 로그를 출력하고 `0`을 반환합니다.
7. Fragment Shader 파일을 `filetobuf()`으로 읽습니다.
8. `glCreateShader(GL_FRAGMENT_SHADER)`로 Fragment Shader 객체를 생성합니다.
9. `glShaderSource()`로 소스를 전달합니다.
10. `glCompileShader()`로 컴파일합니다.
11. `glGetShaderiv(..., GL_COMPILE_STATUS, ...)`로 컴파일 상태를 확인합니다.
12. 실패 시 `glGetShaderInfoLog()`로 오류 로그를 출력하고 `0`을 반환합니다.
13. `glCreateProgram()`으로 Shader Program을 생성합니다.
14. `glAttachShader()`로 Vertex/Fragment Shader를 Program에 연결합니다.
15. `glLinkProgram()`으로 Program을 링크합니다.
16. `glGetProgramiv(..., GL_LINK_STATUS, ...)`로 링크 성공 여부를 확인합니다.
17. 실패 시 `glGetProgramInfoLog()`로 오류 로그를 출력하고 `0`을 반환합니다.
18. 링크 성공 후 `glDeleteShader()`로 개별 Vertex/Fragment Shader 객체를 삭제합니다.
19. 최종 Shader Program ID를 반환합니다.

## 🤖 AI 활용

- AI 도구 사용 비율: **25%**
