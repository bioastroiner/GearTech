#include "shader.h"
#include "glad/glad.h"
#include <stdbool.h>
#include <stdio.h>
#define MAX_SHADER_SIZE 4000

unsigned int shader_makec(const char *vShaderCode, const char *fShaderCode)
{
    int success = true;
    char infoLog[512];
    GLuint shaderVObj = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(shaderVObj, 1, &vShaderCode, NULL);
    glCompileShader(shaderVObj);
    glGetShaderiv(shaderVObj, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderVObj, 512, NULL, infoLog);
        fprintf(stderr, "%s", infoLog);
        success = true;
    }
    GLuint shaderFObj = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(shaderFObj, 1, &fShaderCode, NULL);
    glCompileShader(shaderFObj);
    glGetShaderiv(shaderFObj, GL_COMPILE_STATUS, &success);
    if (!success)
    {
        glGetShaderInfoLog(shaderFObj, 512, NULL, infoLog);
        fprintf(stderr, "%s", infoLog);
        success = true;
    }
    GLuint shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, shaderVObj);
    glAttachShader(shaderProgram, shaderFObj);
    glLinkProgram(shaderProgram);
    glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
    if (!success)
    {
        glGetProgramInfoLog(shaderProgram, 512, NULL, infoLog);
        fprintf(stderr, "%s", infoLog);
        success = true;
    }
    glDeleteShader(shaderVObj);
    glDeleteShader(shaderFObj);
    return shaderProgram;
}

unsigned int shader_make(const char *vertexPath, const char *fragmentPath)
{
    FILE *vShaderFile = fopen(vertexPath, "r");
    FILE *fShaderFile = fopen(fragmentPath, "r");
    char vShaderCode[MAX_SHADER_SIZE] = "";
    char fShaderCode[MAX_SHADER_SIZE] = "";
    bool errored = false;
    if (vShaderFile == NULL)
    {
        fprintf(stderr, "ERROR: Reading %s\n", vertexPath);
        errored = true;
    }
    else
    {
        int red = fread(vShaderCode, sizeof(char), MAX_SHADER_SIZE, vShaderFile);
        printf("INFO: Loaded Vertex Shader \"%s\" size:%d \n", vertexPath, red);
    }
    if (fShaderFile == NULL)
    {
        fprintf(stderr, "ERROR: Reading %s\n", fragmentPath);
        errored = true;
    }
    else
    {
        int red = fread(fShaderCode, sizeof(char), MAX_SHADER_SIZE, fShaderFile);
        printf("INFO: Loaded Fragment Shader \"%s\" size:%d \n", fragmentPath, red);
    }
    if (errored)
        return 0;
    return shader_makec(vShaderCode, fShaderCode);
}

void shader_use(unsigned int ID)
{
    if (ID == 0)
    {
        fprintf(stderr, "ERROR: Using %d Shader Program", ID);
        return;
    }
    glUseProgram(ID);
    // printf("INFO: Using %d Shader Program",ID);
}

void shader_del(unsigned int ID)
{
    if (ID == 0)
    {
        fprintf(stderr, "ERROR: Deleting %d Shader Program", ID);
        return;
    }
    glDeleteProgram(ID);
    printf("INFO: Deleting %d Shader Program", ID);
}

unsigned int shader_set_bool(unsigned int ID, const char *name, int value)
{
    if (ID == 0)
    {
        fprintf(stderr, "ERROR: Using %d Shader Program", ID);
        return 0;
    }
    glUniform1i(glGetUniformLocation(ID, name), (int)value);
    return ID;
}
unsigned int shader_set_int(unsigned int ID, const char *name, int value)
{
    if (ID == 0)
    {
        fprintf(stderr, "ERROR: Using %d Shader Program", ID);
        return 0;
    }
    glUniform1i(glGetUniformLocation(ID, name), value);
    return ID;
}
unsigned int shader_set_float(unsigned int ID, const char *name, float value)
{
    if (ID == 0)
    {
        fprintf(stderr, "ERROR: Using %d Shader Program", ID);
        return 0;
    }
    glUniform1f(glGetUniformLocation(ID, name), value);
    return ID;
}

unsigned int shader_set_mat4(unsigned int ID, const char *name, float *value)
{
    if (ID == 0)
    {
        fprintf(stderr, "ERROR: Using %d Shader Program", ID);
        return 0;
    }
    glUniformMatrix4fv(glGetUniformLocation(ID, name), 1, GL_FALSE, value);
    return ID;
}
unsigned int shader_set_vec4(unsigned int ID, const char *name, float *value)
{
    if (ID == 0)
    {
        fprintf(stderr, "ERROR: Using %d Shader Program", ID);
        return 0;
    }
    glUniform4fv(glGetUniformLocation(ID, name), GL_FALSE, value);
    return ID;
}
unsigned int shader_set_vec3(unsigned int ID, const char *name, float *value)
{
    if (ID == 0)
    {
        fprintf(stderr, "ERROR: Using %d Shader Program", ID);
        return 0;
    }
    glUniform3fv(glGetUniformLocation(ID, name), GL_FALSE, value);
    return ID;
}
unsigned int shader_set_vec2(unsigned int ID, const char *name, float *value)
{
    if (ID == 0)
    {
        fprintf(stderr, "ERROR: Using %d Shader Program", ID);
        return 0;
    }
    glUniform2fv(glGetUniformLocation(ID, name), GL_FALSE, value);
    return ID;
}
unsigned int shader_set_vec3i(unsigned int ID, const char *name, int *value)
{
    if (ID == 0)
    {
        fprintf(stderr, "ERROR: Using %d Shader Program", ID);
        return 0;
    }
    glUniform3iv(glGetUniformLocation(ID, name), GL_FALSE, value);
    return ID;
}