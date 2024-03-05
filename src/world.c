#include "world.h"
#include "cglm/cglm.h"
#include "shader.h"
#include "utils/hashmap.h"
#include "utils/timer.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

void world_generate(struct World *world_obj)
{
    world_obj = calloc(1, sizeof(struct World));
    unsigned int size = WORLD_SIZE * WORLD_SIZE;
    world_obj->size = size;
    world_obj->ChunkMap = calloc(size, sizeof(struct Chunk));
    float timer = glfwGetTime();
    for (size_t i = 0; i < size; i++)
    {
        createNewChunk(i % WORLD_SIZE - WORLD_SIZE / 2, (i / WORLD_SIZE) % WORLD_SIZE - WORLD_SIZE / 2, world_obj, world_obj->ChunkMap + i);
    }
    printf("world gen took %f seconds", timer);
    return world_obj;
}
void world_render_tick(int x_player, int y_player, int z_player, struct World *world_obj, unsigned int chunk_shader)
{
    for (size_t i = 0; i < WORLD_SIZE * WORLD_SIZE; i++)
    {
        mat4 trans;
        glm_mat4_identity(trans);
        glm_translate(trans, (vec3){world_obj->ChunkMap[i].x * 16, 0, world_obj->ChunkMap[i].z * 16});
        // shader_set_vec2(chunk_shader, "chunkPosition", (vec2){world_obj->ChunkMap[i].x, world_obj->ChunkMap[i].z});
        shader_set_vec3i(chunk_shader, "playerPos", (ivec3){x_player, y_player, z_player});
        shader_set_mat4(chunk_shader, "model", trans);
        renderChunk(world_obj->ChunkMap + i);
    }
}
int world_get_chunk(int x, int z, struct World *world, struct Chunk *_dest)
{
    int chunk_x = x >> 4;
    int chunk_z = z >> 4;
    if (chunk_x <= -WORLD_SIZE / 2 || chunk_x > WORLD_SIZE / 2)
    {
        sprintf(stderr, "Out of Bound X coordinates at (%d,%d): line:%d - at:%s"), x, z, __LINE__, __FILE__;
        sprintf(stderr, "\nTried to Access Invalid Chunk (%d,%d)\n", x >> 4, z >> 4);
        return 0;
    }
    if (chunk_z <= -WORLD_SIZE / 2 || chunk_z > WORLD_SIZE / 2)
    {
        sprintf(stderr, "Out of Bound Z coordinates at (%d,%d): line:%d - at:%s"), x, z, __LINE__, __FILE__;
        sprintf(stderr, "\nTried to Access Invalid Chunk (%d,%d)\n", x >> 4, z >> 4);
        return 0;
    }
    _dest = world->ChunkMap + ((x >> 4 + WORLD_SIZE / 2) + (z >> 4 + WORLD_SIZE / 2) * WORLD_SIZE);
    return 1;
}
unsigned char world_get_block(int x, int y, int z, struct World *world)
{
    struct Chunk *_chunk;
    if (world_get_chunk(x, z, world, _chunk))
    {
        return getBlockInChunk(_chunk, x >> 4, y, z >> 4);
    }
    else
    {
        sprintf(stderr, "Unable to Load Chunk for block (%d,%d,%d) line:%d file:%s\n", x, y, z, __LINE__, __FILE__);
        return 0;
    }
}
int world_set_block(int x, int y, int z, struct World *world, unsigned char block)
{
    struct Chunk *_chunk;
    if (world_get_chunk(x, z, world, _chunk))
    {
        /*
            todo set chunk to be scudeled for an update
            in the next tick (a few frames) in order to not
            preform expensive mesh updating every time we
            set our delete a block
        */
        return 1;
    }
    else
    {
        sprintf(stderr, "Unable to Load Chunk for block (%d,%d,%d) line:%d file:%s\n", x, y, z, __LINE__, __FILE__);
        return 0;
    }
}
int world_remove_block(int x, int y, int z, struct World *world)
{
    return world_set_block(x, y, z, world, 0);
}