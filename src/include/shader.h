#ifndef Shader_h_
#define Shader_h_
void shader_use(unsigned int ID);
void shader_del(unsigned int ID);
unsigned int shader_make(const char *vertexPath, const char *fragmentPath);
unsigned int shader_makec(const char *vShaderCode, const char *fShaderCode);
unsigned int shader_set_bool(unsigned int ID, const char *name, int value);
unsigned int shader_set_int(unsigned int ID, const char *name, int value);
unsigned int shader_set_float(unsigned int ID, const char *name, float value);
unsigned int shader_set_mat4(unsigned int ID, const char *name, float *value);
unsigned int shader_set_vec4(unsigned int ID, const char *name, float *value);
unsigned int shader_set_vec3(unsigned int ID, const char *name, float *value);
unsigned int shader_set_vec3i(unsigned int ID, const char *name, int *value);
unsigned int shader_set_vec2(unsigned int ID, const char *name, float *value);
#endif