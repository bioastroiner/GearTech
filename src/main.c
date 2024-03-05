#define STB_IMAGE_IMPLEMENTATION
#define GLFW_INCLUDE_NONE
#define WIDTH 800
#define HEIGHT 600
#define GLT_IMPLEMENTATION
#include "camera.h"
#include "chunk.h"
#include "shader.h"
#include "stb_image.h"
#include "utils/timer.h"
#include "world.h"
#include <GLFW/glfw3.h>
#include <glad/glad.h>
#include "gltext.h"
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <windows.h>

static GLFWwindow *window;
static bool render_wireframe = false;
static bool firstMouse = true;
static float lastX = WIDTH / 2;
static float lastY = HEIGHT / 2;
static GLuint screen_width = WIDTH;
static GLuint screen_height = HEIGHT;
GLuint texture_atlas;
GLuint texture_grass;
GLuint chunk_shader;
GLuint loadTexture(const char *file_path, bool alpha)
{
    stbi_set_flip_vertically_on_load(true);
    GLuint texture = 0;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    int width, height, nrChannels;
    unsigned char *data = stbi_load(file_path, &width, &height, &nrChannels, 0);
    if (data)
    {
        if (!alpha)
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
        else
            glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
    }
    else
    {
        fprintf(stderr, "ERROR: Loading Texture %s", file_path);
        return 0;
    }
    stbi_image_free(data);
    glBindTexture(GL_TEXTURE_2D, 0);
    return texture;
}
void mouse_callback(GLFWwindow *window, double xpos, double ypos)
{
    if (firstMouse)
    {
        lastX = xpos;
        lastY = ypos;
        firstMouse = false;
    }
    float xoffset = xpos - lastX;
    float yoffset = lastY - ypos; // reversed since y-coordinates go from bottom to top
    lastX = xpos;
    lastY = ypos;
    camera_proc_mouse(xoffset, yoffset, true);
}
void framebuffer_size_callback(GLFWwindow *window, int width, int height)
{
    glViewport(0, 0, width, height);
    screen_width = width;
    screen_height = height;
}
int gl_init()
{
    if (!glfwInit())
    {
        fprintf(stderr, "%s\n", "Failed to Initlize GLFW Context");
        return false;
    }
    window = glfwCreateWindow(WIDTH, HEIGHT, "GAME", NULL, NULL);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
    glfwSetCursorPosCallback(window, mouse_callback);
    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
    if (!window)
    {
        glfwTerminate();
        fprintf(stderr, "%s\n", "Failed to Initilize Window");
        return false;
    }
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        fprintf(stderr, "%s\n", "Failed to initialize GLAD");
        return false;
    }
    glViewport(0, 0, WIDTH, HEIGHT);
    return true;
}
int main(void)
{
    gl_init();
    chunk_shader = shader_make(RESOURCES_PATH "shaders/chunk.vs", RESOURCES_PATH "shaders/chunk.fs");
    texture_atlas = loadTexture(RESOURCES_PATH "/textures/blocks.png", false);
    mat4 view;
    mat4 proj;
    mat4 model;
    vec3 player_pos;
    glm_mat4_identity(proj);
    glm_mat4_identity(view);
    glm_mat4_identity(model);
    world_generate();
    camera_set_x(0.0f, CHUNK_HEIGHT + 2, 0.0f, 0.0f, 1.0f, 0.0f, -90.0f, 0.0f);
    camera_set_fov(70);
    gltInit();
    GLTtext *text = gltCreateText();
    while (!glfwWindowShouldClose(window))
    {
        tick();
        deltaTime = getDeltaTime();
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
        {
            camera_proc_movement(0, deltaTime * 10);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
        {
            camera_proc_movement(1, deltaTime * 10);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
        {
            camera_proc_movement(2, deltaTime * 10);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
        {
            camera_proc_movement(3, deltaTime * 10);
        }
        if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
        {
            if (MessageBox(NULL, "Are you sure you want to close the program?", "Minecraft Dialouge", MB_YESNO) == IDYES)
            {
                glfwSetWindowShouldClose(window, GL_TRUE);
                return 0;
            }
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS)
        {
            if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            else
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        }
        camera_view_matrix(view);
        camera_perspective_matrix(WIDTH / HEIGHT, 0.1f, 800.0f, proj);
        camera_get_pos(player_pos);
        // printf("player pos: %.0f,%.0f,%.0f\n", player_pos[0], player_pos[1], player_pos[2]);
        shader_use(chunk_shader);
        shader_set_mat4(chunk_shader, "view", view);
        shader_set_mat4(chunk_shader, "projection", proj);
        glClearColor(0.0f, 0.0f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);
        glEnable(GL_DEPTH_TEST);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glBindTexture(GL_TEXTURE_2D, texture_atlas);
        shader_set_mat4(chunk_shader, "model", model);
        shader_set_float(chunk_shader, "aspect", WIDTH / HEIGHT);
        world_render_tick(player_pos[0], player_pos[1], player_pos[2], chunk_shader);
        char pos_text[128];
        sprintf(pos_text, "Player: x:%.1f y:%.1f z:%.1f"
                          "\nFPS: %.1f"
                          "\nwidth: %d, height: %d"
                /*"\nChunk (X:%d Z:%d)"*/,
                player_pos[0], player_pos[1], player_pos[2],
                1 / deltaTime,
                screen_width, screen_height
                /*,_chunk->x, _chunk->z*/);
        gltSetText(text, pos_text);
        gltBeginDraw();
        gltColor(1.0f, 1.0f, 1.0f, 1.0f);
        gltDrawText2D(text, 0.0f, 0.0f, 1);
        gltEndDraw();
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    gltDeleteText(text);
    gltTerminate();
    glfwTerminate();
    return 0;
}