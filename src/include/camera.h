#include <cglm/cglm.h>
#ifndef CAMERA_H_
#define CAMERA_H_

typedef enum
{
    FORWARD,
    BACKWARD,
    LEFT,
    RIGHT
} movement_t;

enum ProjectionMode
{
    ORTHO,
    PRESPECTIVE
};

// there may be only ONE instance of this struct
struct camera
{
    float yaw;
    float pitch;
    float speed;
    float sensetivity;
    float fov;
    float aspect;
    float nearZ;
    float farZ;
    float scroll_rate;
    vec3 position;
    vec3 up;
    vec3 front;
    vec3 right;
    vec3 world_up;
    enum ProjectionMode mode;
};

#define YAW -90.0f
#define PITCH 0.0f
#define SPEED 2.5f
#define SENSITIVITY 0.1f
#define ZOOM 60.0f
#define SCROLL_RATE 1.5f

// fairly easy to use 3D look with controls camera
// first initilize using camera_set
// bind it to your chosing view matrix using camera_view_matrix
// and hook to proc methods for controls
void camera_proc_movement(movement_t direction, float deltaTime);
void camera_proc_mouse(float xoffset, float yoffset, int constrainPitch);
void camera_proc_scroll(float yoffset);
void camera_view_matrix(float *dest);
void camera_update_vectors();
void camera_set_x(float posX, float posY, float posZ, float upX, float upY, float upZ, float yaw, float pitch);
void camera_set_v(float *pos, float *up, float yaw, float pitch);
void camera_set_pos(float *pos);
void camera_get_pos(float *pos);
int camera_initilized(void);
// void camera_ortho(float left,    float right,
//           float bottom,  float top,
//           float nearZ,   float farZ,
//           float *dest);
void camera_perspective_matrix(float aspect, float nearZ, float farZ, float *dest);
void camera_perspective_view_matrix(float aspect, float nearZ, float farZ, float *dest);
void camera_set_fov(float fovIn);
float camera_fov(void);
#endif