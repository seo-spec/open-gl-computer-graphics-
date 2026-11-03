// Include shared headers first so their declarations stay in the global namespace.
#define NOMINMAX
#include <windows.h>
#include <GL/glew.h>
#include <GL/glfw3.h>
#include <iostream>
#include <vector>
#include <random>
#include <utility>
#include <filesystem>
#include <cstring>
#include <cmath>
#include <cstdio>
#include <GL/glm/glm.hpp>
#include <GL/glm/gtc/type_ptr.hpp>
#include <GL/glm/gtc/matrix_transform.hpp>
#include "Shader.h"
#include "Obj.h"

// Keep the original sources unchanged and isolate their variables and functions.
namespace Practice13 {
#include "number13.cpp"
}
namespace Practice14 {
#include "number14.cpp"
}
namespace Practice15 {
#include "number15.cpp"
}
namespace Practice16 {
#include "number16.cpp"
}
namespace Practice17 {
#include "number17.cpp"
}

namespace Practice18 {
#include "number18.cpp"
}
namespace Practice19 {
#include "number19.cpp"
}
namespace Practice20 {
#include "number20.cpp"
}
namespace Practice21 {
#include "number21.cpp"
}
namespace Practice22 {
#include "number22.cpp"
}

namespace Launcher {
// Small bitmap font rendered with scissor rectangles; no extra font assets needed.
struct Glyph { char character; unsigned char rows[7]; };
const Glyph font[] = {
    {'A',{14,17,17,31,17,17,17}}, {'B',{30,17,17,30,17,17,30}},
    {'C',{14,17,16,16,16,17,14}}, {'E',{31,16,16,30,16,16,31}},
    {'I',{31,4,4,4,4,4,31}}, {'L',{16,16,16,16,16,16,31}},
    {'N',{17,25,25,21,19,19,17}}, {'O',{14,17,17,17,17,17,14}},
    {'P',{30,17,17,30,16,16,16}}, {'R',{30,17,17,30,20,18,17}},
    {'S',{15,16,16,14,1,1,30}}, {'T',{31,4,4,4,4,4,4}},
    {'U',{17,17,17,17,17,17,14}}, {'X',{17,17,10,4,10,17,17}},
    {'0',{14,17,19,21,25,17,14}}, {'2',{14,17,1,2,4,8,31}},
    {'8',{14,17,17,14,17,17,14}}, {'9',{14,17,17,15,1,1,14}},
    {'D',{30,17,17,17,17,17,30}}, {'F',{31,16,16,30,16,16,16}},
    {'G',{14,17,16,23,17,17,14}},
    {'H',{17,17,17,31,17,17,17}}, {'K',{17,18,20,24,20,18,17}},
    {'M',{17,27,21,21,17,17,17}}, {'V',{17,17,17,17,17,10,4}},
    {'W',{17,17,17,21,21,21,10}}, {'Y',{17,17,10,4,4,4,4}},
    {'1',{4,12,4,4,4,4,14}}, {'3',{30,1,1,14,1,1,30}},
    {'4',{2,6,10,18,31,2,2}}, {'5',{31,16,16,30,1,1,30}},
    {'6',{14,16,16,30,17,17,14}}, {'7',{31,1,2,4,8,8,8}}
};

constexpr int menuWidth = 640, menuHeight = 480;
void Rectangle(int x, int y, int width, int height,
               float r, float g, float b, int fbWidth, int fbHeight) {
    const int left = x * fbWidth / menuWidth;
    const int right = (x + width) * fbWidth / menuWidth;
    const int bottom = (menuHeight - y - height) * fbHeight / menuHeight;
    const int top = (menuHeight - y) * fbHeight / menuHeight;
    glScissor(left, bottom, right - left, top - bottom);
    glClearColor(r, g, b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);
}

void Text(const char* text, int x, int y, int size, int fbWidth, int fbHeight) {
    for (; *text; ++text, x += 6 * size) {
        for (const auto& glyph : font) {
            if (glyph.character != *text) continue;
            for (int row = 0; row < 7; ++row)
                for (int column = 0; column < 5; ++column)
                    if (glyph.rows[row] & (1 << (4 - column)))
                        Rectangle(x + column * size, y + row * size, size, size,
                                  0.93f, 0.95f, 1.0f, fbWidth, fbHeight);
            break;
        }
    }
}

bool StartPractice(int practice, HANDLE& process) {
    wchar_t executable[32768];
    DWORD length = GetModuleFileNameW(nullptr, executable, 32768);
    if (length == 0 || length >= 32768) return false;
    std::wstring command = L"\"" + std::wstring(executable) +
        L"\" --practice " + std::to_wstring(practice);
    STARTUPINFOW startup{};
    startup.cb = sizeof(startup);
    PROCESS_INFORMATION info{};
    // A new process resets all original globals without editing the exercises.
    if (!CreateProcessW(executable, command.data(), nullptr, nullptr, FALSE,
                        CREATE_NO_WINDOW, nullptr, nullptr, &startup, &info))
        return false;
    CloseHandle(info.hThread);
    process = info.hProcess;
    return true;
}

int RunMenu() {
    if (!glfwInit()) return 1;
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* menu = glfwCreateWindow(1100, 825, "3D Practice Gallery | 13 - 22", nullptr, nullptr);
    if (!menu) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(menu);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) { glfwDestroyWindow(menu); glfwTerminate(); return 1; }
    glfwSwapInterval(1);
    GLuint shader = InitShader("vertex18.glsl", "fragment18.glsl");
    ObjData cube;
    if (!shader || !Loadobj("cube.obj", cube)) {
        glDeleteProgram(shader); glfwDestroyWindow(menu); glfwTerminate(); return 1;
    }
    for (float& v : cube.vertices) v -= 0.5f;
    GLuint vao, vbo, ebo;
    glGenVertexArrays(1, &vao); glBindVertexArray(vao);
    glGenBuffers(1, &vbo); glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, cube.vertices.size()*sizeof(float), cube.vertices.data(), GL_STATIC_DRAW);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr); glEnableVertexAttribArray(0);
    glGenBuffers(1, &ebo); glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, cube.indices.size()*sizeof(unsigned int), cube.indices.data(), GL_STATIC_DRAW);
    GLint ml=glGetUniformLocation(shader,"model"), vl=glGetUniformLocation(shader,"view");
    GLint pl=glGetUniformLocation(shader,"projection"), cl=glGetUniformLocation(shader,"objectColor");
    const glm::vec3 colors[] = {{0.18f,0.65f,0.95f},{0.36f,0.45f,1.0f},{0.65f,0.35f,0.95f},
        {0.95f,0.35f,0.55f},{1.0f,0.55f,0.22f},{0.95f,0.78f,0.25f},
        {0.2f,0.8f,0.65f},{0.2f,0.65f,0.8f},{0.5f,0.85f,0.35f},{0.95f,0.65f,0.4f}};
    HANDLE process=nullptr;
    bool mouseWasDown=false, keys[15]={};
    int active=0;
    double lastTime=glfwGetTime();
    float animation=0.0f;
    while (!glfwWindowShouldClose(menu)) {
        if (process) {
            glfwWaitEventsTimeout(0.05);
            if (WaitForSingleObject(process,0)==WAIT_OBJECT_0) {
                DWORD result=0; GetExitCodeProcess(process,&result);
                CloseHandle(process); process=nullptr;
                glfwShowWindow(menu); glfwFocusWindow(menu);
                mouseWasDown=true; for (bool& key:keys) key=true;
                lastTime=glfwGetTime();
                if (result) MessageBoxW(nullptr,L"The practice exited with an error.",L"Practice Gallery",MB_OK|MB_ICONERROR);
            }
            continue;
        }
        glfwPollEvents();
        if (glfwGetKey(menu,GLFW_KEY_ESCAPE)==GLFW_PRESS) break;
        double now=glfwGetTime(); animation+=static_cast<float>(glm::min(now-lastTime,0.05)); lastTime=now;
        int ww,wh,fw,fh; glfwGetWindowSize(menu,&ww,&wh); glfwGetFramebufferSize(menu,&fw,&fh);
        if (ww<=0 || wh<=0 || fw<=0 || fh<=0) { glfwWaitEventsTimeout(0.05); continue; }
        double mx,my; glfwGetCursorPos(menu,&mx,&my);
        mx=mx*menuWidth/ww; my=my*menuHeight/wh;
        bool down=glfwGetMouseButton(menu,GLFW_MOUSE_BUTTON_LEFT)==GLFW_PRESS;
        int selection=-1, hover=-1;
        // Project each gallery cell to the same coordinates used by the text HUD.
        glm::mat4 view=glm::lookAt(glm::vec3(0,0,11),glm::vec3(0),glm::vec3(0,1,0));
        glm::mat4 projection=glm::perspective(glm::radians(42.0f),float(fw)/fh,0.1f,50.0f);
        glm::vec2 centers[10];
        for (int i=0;i<10;++i) {
            glm::vec4 q=projection*view*glm::vec4((i%5-2)*1.8f,(0.5f-i/5)*2.6f,0,1);
            centers[i]=glm::vec2((q.x/q.w+1)*menuWidth/2,(1-q.y/q.w)*menuHeight/2);
            if (std::abs(mx-centers[i].x)<38 && std::abs(my-centers[i].y)<47) hover=i;
        }
        if (hover>=0) { active=hover; if(down&&!mouseWasDown) selection=13+active; }
        mouseWasDown=down;
        const int codes[15]={GLFW_KEY_1,GLFW_KEY_2,GLFW_KEY_3,GLFW_KEY_4,GLFW_KEY_5,
            GLFW_KEY_6,GLFW_KEY_7,GLFW_KEY_8,GLFW_KEY_9,GLFW_KEY_0,GLFW_KEY_LEFT,GLFW_KEY_RIGHT,GLFW_KEY_UP,GLFW_KEY_DOWN,GLFW_KEY_ENTER};
        for(int k=0;k<15;++k) {
            bool pressed=glfwGetKey(menu,codes[k])==GLFW_PRESS;
            if(pressed&&!keys[k]) {
                if(k<10) { active=k; selection=13+k; }
                else if(k==10) active=(active+9)%10;
                else if(k==11) active=(active+1)%10;
                else if(k==12) active=(active+5)%10;
                else if(k==13) active=(active+5)%10;
                else selection=13+active;
            }
            keys[k]=pressed;
        }
        glDisable(GL_SCISSOR_TEST); glEnable(GL_DEPTH_TEST); glDisable(GL_CULL_FACE);
        glViewport(0,0,fw,fh); glClearColor(0.025f,0.04f,0.075f,1); glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
        glUseProgram(shader); glBindVertexArray(vao);
        glUniformMatrix4fv(vl,1,GL_FALSE,glm::value_ptr(view)); glUniformMatrix4fv(pl,1,GL_FALSE,glm::value_ptr(projection));
        for(int i=0;i<10;++i) {
            glm::vec3 position((i%5-2)*1.8f,(0.5f-i/5)*2.6f,0);
            position.y+=0.06f*std::sin(animation*1.4f+i);
            glm::mat4 model=glm::translate(glm::mat4(1),position);
            model=glm::rotate(model,glm::radians(22.0f),glm::vec3(1,0,0));
            model=glm::rotate(model,animation*(i==active?0.65f:0.16f)+i*0.4f,glm::vec3(0,1,0));
            model=glm::scale(model,glm::vec3(i==active?0.82f:0.65f));
            glUniformMatrix4fv(ml,1,GL_FALSE,glm::value_ptr(model));
            // Face tones make the volume readable without lighting assets.
            for(size_t face=0;face<cube.indices.size()/3;++face) {
                glm::vec3 color=colors[i]*(0.55f+0.09f*float((face/2)%6));
                glUniform3fv(cl,1,glm::value_ptr(color));
                glDrawElements(GL_TRIANGLES,3,GL_UNSIGNED_INT,reinterpret_cast<void*>(face*3*sizeof(unsigned int)));
            }
        }
        glDisable(GL_DEPTH_TEST); glEnable(GL_SCISSOR_TEST);
        Text("PRACTICE GALLERY",140,16,4,fw,fh);
        Text("3D LAB   13 TO 22",225,52,2,fw,fh);
        for(int i=0;i<10;++i) {
            char label[8]; std::snprintf(label,sizeof(label),"%d",13+i);
            int x=int(centers[i].x), y=int(centers[i].y);
            Rectangle(x-23,y+33,46,23,i==active?0.18f:0.045f,i==active?0.38f:0.08f,i==active?0.52f:0.13f,fw,fh);
            Text(label,x-16,y+35,3,fw,fh);
        }
        char selected[40]; std::snprintf(selected,sizeof(selected),"SELECTED %d   ENTER TO OPEN",13+active);
        Text(selected,158,421,2,fw,fh);
        Text("CLICK CUBE OR KEYS 1 TO 9  0   ESC EXIT",98,451,2,fw,fh);
        glDisable(GL_SCISSOR_TEST); glfwSwapBuffers(menu);
        if(selection>=13) {
            if(StartPractice(selection,process)) glfwHideWindow(menu);
            else MessageBoxW(nullptr,L"Could not start the selected practice.",L"Practice Gallery",MB_OK|MB_ICONERROR);
        }
    }
    if(process) CloseHandle(process);
    glDeleteBuffers(1,&ebo); glDeleteBuffers(1,&vbo); glDeleteVertexArrays(1,&vao); glDeleteProgram(shader);
    glfwDestroyWindow(menu); glfwTerminate(); return 0;
}

}

int main(int argc, char* argv[]) {
    // /FC supplies an absolute source path; assets are beside this file.
    const auto sourceDirectory = std::filesystem::path(__FILE__).parent_path();
    if (sourceDirectory.is_absolute()) {
        std::error_code error;
        std::filesystem::current_path(sourceDirectory, error);
        if (error) {
            std::cerr << "Cannot open asset directory: " << error.message() << '\n';
            return 1;
        }
    }

    if (argc == 3 && std::strcmp(argv[1], "--practice") == 0) {
        if (std::strcmp(argv[2], "13") == 0) return Practice13::main();
        if (std::strcmp(argv[2], "14") == 0) return Practice14::main();
        if (std::strcmp(argv[2], "15") == 0) return Practice15::main();
        if (std::strcmp(argv[2], "16") == 0) return Practice16::main();
        if (std::strcmp(argv[2], "17") == 0) return Practice17::main();
        if (std::strcmp(argv[2], "18") == 0) return Practice18::main();
        if (std::strcmp(argv[2], "19") == 0) return Practice19::main();
        if (std::strcmp(argv[2], "20") == 0) return Practice20::main();
        if (std::strcmp(argv[2], "21") == 0) return Practice21::main();
        if (std::strcmp(argv[2], "22") == 0) return Practice22::main();
        return 1;
    }
    return Launcher::RunMenu();
}
