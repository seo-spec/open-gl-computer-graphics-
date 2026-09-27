#ifndef SHADER_H
#define SHADER_H

#include <GL/glew.h>
#include <string>
#include <iostream>
#include <fstream>

inline std::string filetobuf(const char* file) {
	std::ifstream shaderFile(file);
	if (!shaderFile.is_open()) {
		std::cerr << "ERROR: 파일을 열 수 없습니다: " << file << std::endl;
		return "";
	}
	std::string source((std::istreambuf_iterator<char>(shaderFile)), 
		std::istreambuf_iterator<char>()); // iterator를 사용하여 파일 내용을 문자열로 읽어오기
	shaderFile.close();
	return source;
}

// 2. 버텍스 / 프래그먼트 셰이더를 읽고 컴파일/링크하여 셰이더 프로그램 ID 반환
inline GLuint InitShader(const char* vertPath, const char* fragPath) {
    GLint result;
    GLchar errorLog[512];

    // --- Vertex Shader
    std::string vertSource = filetobuf(vertPath);
    const char* vSrc = vertSource.c_str();
    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vSrc, NULL);
    glCompileShader(vertexShader);

    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(vertexShader, 512, NULL, errorLog);
        std::cerr << "ERROR: Vertex Shader 컴파일 실패\n" << errorLog << std::endl;
        return 0;
    }

    // --- Fragment Shader
    std::string fragSource = filetobuf(fragPath);
    const char* fSrc = fragSource.c_str();
    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fSrc, NULL);
    glCompileShader(fragmentShader);

    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &result);
    if (!result) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, errorLog);
        std::cerr << "ERROR: Fragment Shader 컴파일 실패\n" << errorLog << std::endl;
        return 0;
    }

    // --- Shader Program Link
    GLuint shaderID = glCreateProgram();
    glAttachShader(shaderID, vertexShader);
    glAttachShader(shaderID, fragmentShader);
    glLinkProgram(shaderID);

    glGetProgramiv(shaderID, GL_LINK_STATUS, &result);
    if (!result) {
        glGetProgramInfoLog(shaderID, 512, NULL, errorLog);
        std::cerr << "ERROR: Shader Program 링크 실패\n" << errorLog << std::endl;
        return 0;
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return shaderID;
}


#endif