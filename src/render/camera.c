#include "camera.h"
#include <stdbool.h>
struct camera ACTIVE_CAMERA = {
    .yaw = YAW,
    .pitch = PITCH,

    .speed = SPEED,
    .sensetivity = SENSITIVITY,

    .fov = ZOOM,
    .scroll_rate = SCROLL_RATE,

    .mode = PRESPECTIVE,
    // vectors:
    .position = GLM_VEC3_ZERO_INIT,
    .front = {0.0f, 0.0f, -1.0f},
    .up = {0.0f, 1.0f, 0.0f},
    .world_up = {0.0f, 1.0f, 0.0f},
};
void camera_proc_movement(movement_t direction, float deltaTime)
{
    float velocity = ACTIVE_CAMERA.speed * deltaTime;
    if (direction == FORWARD)
        glm_vec3_add(ACTIVE_CAMERA.position, (vec3){ACTIVE_CAMERA.front[0] * velocity, ACTIVE_CAMERA.front[1] * velocity, ACTIVE_CAMERA.front[2] * velocity}, ACTIVE_CAMERA.position);
    if (direction == BACKWARD)
        glm_vec3_sub(ACTIVE_CAMERA.position, (vec3){ACTIVE_CAMERA.front[0] * velocity, ACTIVE_CAMERA.front[1] * velocity, ACTIVE_CAMERA.front[2] * velocity}, ACTIVE_CAMERA.position);
    if (direction == LEFT)
        glm_vec3_sub(ACTIVE_CAMERA.position, (vec3){ACTIVE_CAMERA.right[0] * velocity, ACTIVE_CAMERA.right[1] * velocity, ACTIVE_CAMERA.right[2] * velocity}, ACTIVE_CAMERA.position);
    if (direction == RIGHT)
        glm_vec3_add(ACTIVE_CAMERA.position, (vec3){ACTIVE_CAMERA.right[0] * velocity, ACTIVE_CAMERA.right[1] * velocity, ACTIVE_CAMERA.right[2] * velocity}, ACTIVE_CAMERA.position);
}
void camera_proc_scroll(float yoffset)
{
    ACTIVE_CAMERA.scroll_rate -= (float)yoffset * ACTIVE_CAMERA.scroll_rate;
    if (ACTIVE_CAMERA.scroll_rate < 1.0f)
        ACTIVE_CAMERA.scroll_rate = 1.0f;
    if (ACTIVE_CAMERA.scroll_rate > ACTIVE_CAMERA.scroll_rate)
        ACTIVE_CAMERA.scroll_rate = ACTIVE_CAMERA.scroll_rate;
}
void camera_proc_mouse(float xoffset, float yoffset, int constrainPitch)
{
    xoffset *= ACTIVE_CAMERA.sensetivity;
    yoffset *= ACTIVE_CAMERA.sensetivity;
    ACTIVE_CAMERA.yaw += xoffset;
    ACTIVE_CAMERA.pitch += yoffset;
    if (constrainPitch)
    {
        if (ACTIVE_CAMERA.pitch > 89.0f)
            ACTIVE_CAMERA.pitch = 89.0f;
        if (ACTIVE_CAMERA.pitch < -89.0f)
            ACTIVE_CAMERA.pitch = -89.0f;
    }
    camera_update_vectors();
}
void camera_update_vectors_p(const float x, const float y, const float z)
{
    ACTIVE_CAMERA.position[0] = x;
    ACTIVE_CAMERA.position[0] = y;
    ACTIVE_CAMERA.position[0] = z;
    camera_update_vectors();
}
void camera_update_vectors()
{
    // calculate the new Front vector
    vec3 front;
    front[0] = cos(glm_rad(ACTIVE_CAMERA.yaw)) * cos(glm_rad(ACTIVE_CAMERA.pitch));
    front[1] = sin(glm_rad(ACTIVE_CAMERA.pitch));
    front[2] = sin(glm_rad(ACTIVE_CAMERA.yaw)) * cos(glm_rad(ACTIVE_CAMERA.pitch));
    glm_normalize_to(front, ACTIVE_CAMERA.front);
    glm_cross(ACTIVE_CAMERA.front, ACTIVE_CAMERA.world_up, ACTIVE_CAMERA.right);
    glm_normalize(ACTIVE_CAMERA.right);
    glm_cross(ACTIVE_CAMERA.right, ACTIVE_CAMERA.front, ACTIVE_CAMERA.up);
    glm_normalize(ACTIVE_CAMERA.up);
}
void camera_view_matrix(float *dest)
{
    vec3 pos_front;
    glm_vec3_add(ACTIVE_CAMERA.position, ACTIVE_CAMERA.front, pos_front);
    glm_lookat(ACTIVE_CAMERA.position, pos_front, ACTIVE_CAMERA.up, dest);
}
void camera_ortho(float left, float right, float bottom, float top, float nearZ, float farZ, float *dest)
{
    // TODO: implement
}
void camera_perspective_matrix(float aspect, float nearZ, float farZ, float *dest)
{
    ACTIVE_CAMERA.aspect = aspect;
    ACTIVE_CAMERA.nearZ = nearZ;
    ACTIVE_CAMERA.farZ = farZ;
    glm_perspective(glm_rad(ACTIVE_CAMERA.fov), aspect, nearZ, farZ, dest);
}
void camera_perspective_view_matrix(float aspect, float nearZ, float farZ, float *dest)
{
    // mat4 view;
    // glm_mat4_identity(view);
    // camera_view_matrix(view);
    // mat4 proj;
    // glm_mat4_identity(proj);
    // glm_perspective(80,aspect,0,100,proj);
    // glm_mat4_mul(view,proj,dest);
}
void camera_set_fov(float fovIn)
{
    ACTIVE_CAMERA.fov = fovIn;
}
void camera_set_x(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch)
{
    camera_set_v(
        (vec3){posX, posY, posZ},
        (vec3){upX, upY, upZ},
        yaw,
        pitch);
}
void camera_set_v(float *pos, float *up, float yaw, float pitch)
{
    glm_vec3(pos, ACTIVE_CAMERA.position);
    glm_vec3(up, ACTIVE_CAMERA.world_up);
    ACTIVE_CAMERA.yaw = yaw;
    ACTIVE_CAMERA.pitch = pitch;
    camera_update_vectors();
}
void camera_set_pos(float *pos)
{
    glm_vec3(pos, ACTIVE_CAMERA.position);
    camera_update_vectors();
}
void camera_get_pos(float *pos)
{
    glm_vec3_copy(ACTIVE_CAMERA.position, pos);
}
float camera_get_pitch()
{
    return ACTIVE_CAMERA.pitch;
}
float camera_get_yaw()
{
    return ACTIVE_CAMERA.yaw;
}