#version 330 core

layout(location = 0) in vec3 pos;
layout(location = 1) in vec2 uv_in;
layout(location = 2) in uint data;

out vec2 uv;
out float dist;

uniform mat4 view;
uniform mat4 projection;
uniform mat4 model;
uniform vec2 chunkPosition;
uniform ivec3 playerPos;

// 8x8 atlass (32pixel each block)
vec2 atlasv(ivec2 atlas, vec2 _uv) {
    return vec2((_uv.x + atlas.x) / 8.0f,
        (_uv.y + 7.0f - atlas.y) / 8.0f);
}

vec2 atlas(uint index, vec2 _uv) {
    return atlasv(ivec2(index % 8u, (index / 8u) % 8u), _uv);
}

vec2 block(uint _blockID, uint _face) {
    switch (_face) {
        case 0: // front
        break;
        case 1: // back
        break;
        case 2: // right
        break;
        case 3: // left
        break;
        case 4: // top
        break;
        case 5: // bottom
        break;
        default:
        return vec2(0);
    }
}

void main()
{
    mat4 projection_view_matrix = projection * view;
    mat4 transform_chunk = mat4(
            1, 0, 0, chunkPosition.x,
            0, 1, 0, 0,
            0, 0, 1, chunkPosition.y,
            0, 0, 0, 1
        );
    gl_Position = projection_view_matrix * model * transform_chunk * vec4(pos, 1.0f);
    uv = atlas(0u, uv_in);
    // dist = distance(vec3(playerPos.x, 0, playerPos.z), vec3(gl_Position.x, 0, gl_Position.z));
    dist = distance(playerPos, gl_Position.xyz);
    // uv = vec2(attribute_uv.x, 1.0 - attribute_uv.y);
    // vertexColor = vec4(1.0f,1.0f,1.0f,1.0f);
}
