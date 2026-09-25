# 🎨 OpenGL Computer Graphics

이 저장소는 OpenGL 및 컴퓨터 그래픽스 실습 과정을 정리한 개인 학습 저장소입니다.  
현재 `첫 번째 실습`, `두 번째 실습`이 포함되어 있습니다.

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
└── 두 번째 실습/
    ├── .gitignore
    ├── opengl2.slnx
    └── opengl2/
        ├── Shader.h
        ├── fragment.glsl
        ├── number7.cpp
        ├── opengl2.vcxproj
        ├── opengl2.vcxproj.filters
        └── vertex.glsl
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
- `Shader.h`: 외부 GLSL 파일을 읽어 Vertex Shader와 Fragment Shader를 생성·컴파일하고, 이를 하나의 Shader Program으로 링크하여 사용할 수 있도록 하는 재사용 가능한 OpenGL Shader 초기화 헤더입니다.
- `vertex.glsl`: 정점 위치(`layout(location = 0)`)와 색상(`layout(location = 1)`)을 입력받아 `gl_Position`과 색상 출력 변수(`out_Color`)를 설정합니다.
- `fragment.glsl`: Vertex Shader에서 전달된 `out_Color`를 받아 최종 픽셀 색상(`Frag_Color`)으로 출력합니다.

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
