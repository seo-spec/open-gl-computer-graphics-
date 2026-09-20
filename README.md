# 🎨 Computer Graphics Lab & Practice Solutions

![Language](https://img.shields.io/badge/Language-C%2FC%2B%2B-blue)
![Graphics](https://img.shields.io/badge/Graphics-OpenGL-orange)
![Status](https://img.shields.io/badge/Status-Practice%20Repository-success)

A collection of solution codes, lab assignments, and implementation exercises for fundamental Computer Graphics topics.  
This repository is organized for hands-on learning, experimentation, and reference while studying core graphics concepts.

## 📚 Topics Covered

- 2D/3D Transformations & Matrix Operations
- Rendering Pipeline & Shaders (GLSL)
- Lighting & Shading Models (Phong, Blinn-Phong)
- Texturing & Mapping
- Camera Systems & View/Projection Matrices

## 🗂️ Project Structure

```text
open-gl-computer-graphics-/
├── README.md
└── 첫 번째 실습/
    ├── number1.cpp
    ├── number2.cpp
    ├── number3.cpp
    ├── number4.cpp
    ├── number5.cpp
    ├── number6.cpp
    ├── console11.vcxproj
    ├── console11.vcxproj.filters
    └── console11.vcxproj.user
```

## 🧩 Practice File Overview

- `number1.cpp`: keyboard input으로 배경색을 변경하고 타이머 기반 랜덤 색상 전환을 실습합니다.
- `number2.cpp`: 사분면별 사각형 생성, 선택/크기 조절/색상 변경/초기화 기능을 통합한 확장 실습입니다.
- `number3.cpp`: 사각형 선택, 이동, 겹침 병합, 우클릭 분할 등 고급 2D 상호작용을 다룹니다.
- `number4.cpp`: 마우스 클릭 위치에 사각형 생성 후, 사각형들을 애니메이션으로 움직입니다
- `number5.cpp`: 마우스 지우개 사각형 충돌 및 생성/리셋 기능 구현
- `number6.cpp`: 퍼져나가는 사각형 애니메이션, 시간에 따른 크기 색상 다르게 구현

## ⚙️ Environment & Prerequisites

- C/C++ compiler (MSVC, GCC, or Clang)
- OpenGL development support
- GLFW
- GLAD
- GLM
- Build tooling: Visual Studio (project file included) or CMake-based workflow

## 🚀 Quick Start

1. Clone the repository:
   ```bash
   git clone https://github.com/seo-spec/open-gl-computer-graphics-.git
   cd open-gl-computer-graphics-
   ```
2. Open `첫 번째 실습/console11.vcxproj` in Visual Studio  
   **or** configure your own CMake/build setup with GLFW/GLAD/GLM linked.
3. Build and run the selected practice source file (e.g., `number1.cpp`).
