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
    {'1',{4,12,4,4,4,4,14}}, {'3',{30,1,1,14,1,1,30}},
    {'4',{2,6,10,18,31,2,2}}, {'5',{31,16,16,30,1,1,30}},
    {'6',{14,16,16,30,17,17,14}}
};

constexpr int menuWidth = 640, menuHeight = 480;
struct Button { int x, y, width, height, practice; const char* label; };
const Button buttons[] = {
    {80,140,220,90,13,"13"}, {340,140,220,90,14,"14"},
    {80,250,220,90,15,"15"}, {340,250,220,90,16,"16"},
    {240,390,160,48,0,"EXIT"}
};

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
    GLFWwindow* menu = glfwCreateWindow(menuWidth, menuHeight,
        "Practice Selector - click 13 / 14 / 15 / 16", nullptr, nullptr);
    if (!menu) { glfwTerminate(); return 1; }
    glfwMakeContextCurrent(menu);
    glewExperimental = GL_TRUE;
    if (glewInit() != GLEW_OK) {
        glfwDestroyWindow(menu); glfwTerminate(); return 1;
    }
    glfwSwapInterval(1);
    HANDLE process = nullptr;
    bool mouseWasDown = false;
    bool keyWasDown[4] = {};
    while (!glfwWindowShouldClose(menu)) {
        // Wait with event processing while the selected practice owns its window.
        if (process) {
            glfwWaitEventsTimeout(0.05);
            if (WaitForSingleObject(process, 0) == WAIT_OBJECT_0) {
                DWORD result = 0;
                GetExitCodeProcess(process, &result);
                CloseHandle(process);
                process = nullptr;
                glfwShowWindow(menu);
                glfwFocusWindow(menu);
                mouseWasDown = true;
                for (auto& key : keyWasDown) key = true;
                if (result != 0)
                    MessageBoxW(nullptr, L"The practice exited with an error.",
                                L"Practice Selector", MB_OK | MB_ICONERROR);
            }
            continue;
        }
        glfwPollEvents();
        if (glfwWindowShouldClose(menu)) break;
        if (glfwGetKey(menu, GLFW_KEY_ESCAPE) == GLFW_PRESS) break;
        int windowWidth, windowHeight, fbWidth, fbHeight;
        glfwGetWindowSize(menu, &windowWidth, &windowHeight);
        glfwGetFramebufferSize(menu, &fbWidth, &fbHeight);
        if (windowWidth <= 0 || windowHeight <= 0 || fbWidth <= 0 || fbHeight <= 0) {
            glfwWaitEventsTimeout(0.05); continue;
        }
        double mouseX, mouseY;
        glfwGetCursorPos(menu, &mouseX, &mouseY);
        mouseX *= static_cast<double>(menuWidth) / windowWidth;
        mouseY *= static_cast<double>(menuHeight) / windowHeight;
        bool mouseDown = glfwGetMouseButton(menu, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS;
        int selection = -1;
        glViewport(0, 0, fbWidth, fbHeight);
        glDisable(GL_SCISSOR_TEST);
        glClearColor(0.06f, 0.08f, 0.13f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_SCISSOR_TEST);
        Text("SELECT PRACTICE", 140, 45, 4, fbWidth, fbHeight);
        Text("CLOSE PRACTICE TO RETURN", 113, 100, 3, fbWidth, fbHeight);
        for (const auto& button : buttons) {
            bool hover = mouseX >= button.x && mouseX < button.x + button.width &&
                         mouseY >= button.y && mouseY < button.y + button.height;
            Rectangle(button.x, button.y, button.width, button.height,
                      hover ? 0.22f : 0.12f, hover ? 0.40f : 0.23f,
                      hover ? 0.68f : 0.39f, fbWidth, fbHeight);
            int size = button.practice ? 7 : 3;
            int textWidth = static_cast<int>(std::strlen(button.label)) * 6 * size - size;
            Text(button.label, button.x + (button.width - textWidth) / 2,
                 button.y + (button.height - 7 * size) / 2, size, fbWidth, fbHeight);
            if (hover && mouseDown && !mouseWasDown) selection = button.practice;
        }
        mouseWasDown = mouseDown;
        for (int i = 0; i < 4; ++i) {
            bool down = glfwGetKey(menu, GLFW_KEY_1 + i) == GLFW_PRESS;
            if (down && !keyWasDown[i]) selection = 13 + i;
            keyWasDown[i] = down;
        }
        glDisable(GL_SCISSOR_TEST);
        glfwSwapBuffers(menu);
        if (selection == 0) break;
        if (selection >= 13) {
            if (StartPractice(selection, process)) glfwHideWindow(menu);
            else MessageBoxW(nullptr, L"Could not start the selected practice.",
                             L"Practice Selector", MB_OK | MB_ICONERROR);
        }
    }
    if (process) CloseHandle(process);
    glfwDestroyWindow(menu);
    glfwTerminate();
    return 0;
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
        return 1;
    }
    return Launcher::RunMenu();
}
