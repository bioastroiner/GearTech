#include "world.h"
#include "cglm/cglm.h"
#include "shader.h"
#include "utils/timer.h"
#include <GLFW/glfw3.h>
#include <stdio.h>

/*
    @struct Chunk must be allocated by user
    @warning low level functions, Not designed for auto load and unloading, it won't insert chunks
    if there is a collision, handle those yourself
*/
int putIntoMap(struct ChunkMap *map, struct Chunk *chunk)
{
    // printf("Adding Chunk with hashID: %d to map", chunk->hash);
    if (chunk == NULL)
    {
        sprintf(stderr, "ERROR NULL chunk attempt to add to HashMap %s %d", __FILE__, __LINE__);
        return 0;
    }
    if (map->inner_array[chunk->hash % map->size] != NULL)
    {
        sprintf(stderr, "ERROR chunk collision detected (x:%d z:%d) place occupied with"
                        " (x:%d, z:%d)"
                        " %s %d"
                        " Failed to add chunk to Map",
                chunk->x, chunk->z,
                map->inner_array[chunk->hash % map->size]->x,
                map->inner_array[chunk->hash % map->size]->z,
                __FILE__, __LINE__);
        return 0;
    }
    map->inner_array[chunk->hash % map->size] = chunk;
    return 1;
}
/*
    Used for easy access to loaded Chunks onto the memory
    Map size depends on how many chunks are allowed to leave on the memory

    @warning make sure unloaded chunks get written to disk before they get
    removed from the map.

    @note functions handles memory of the hashmap dynamically
    but the chunks must be allocated somewhere since HashMap
    only stores the pointer to chunks and access them with their IDs.
    point is to have some chunks loaded on memory and unload the rest
    as player moves new chunks replace the old ones.
    increase the size of this map inorder to load more chunks at once.
*/
struct ChunkMap *initilizeChunkMap(unsigned int init_size)
{
    struct ChunkMap *map = calloc(init_size, sizeof(struct ChunkMap));
    if (map == NULL)
    {
        sprintf(stderr, "ERROR Intilizing HashMap %s %d", __FILE__, __LINE__);
    }
    return map;
}
int resizeChunkMap(struct ChunkMap *map, unsigned int newsize)
{
    struct ChunkMap *newmap = realloc(map, newsize);
    if (newmap == NULL)
    {
        sprintf("failed to reallocate HashMap %s %d", __FILE__, __LINE__);
    }
}

void world_generate(struct World *world_obj)
{
    unsigned int size = WORLD_SIZE * WORLD_SIZE;
    world_obj->size = size;
    world_obj->chunks = calloc(size, sizeof(struct Chunk));
    initilizeChunkMap(10);
    float timer = glfwGetTime();
    for (size_t i = 0; i < size; i++)
    {
        // createNewChunk(i % WORLD_SIZE - WORLD_SIZE / 2, (i / WORLD_SIZE) % WORLD_SIZE - WORLD_SIZE / 2, world_obj, world_obj->chunks + i);
        struct Chunk *chunk = malloc(sizeof(struct Chunk));
        createNewChunk(i % WORLD_SIZE - WORLD_SIZE / 2, (i / WORLD_SIZE) % WORLD_SIZE - WORLD_SIZE / 2, world_obj, chunk);
    }
    printf("world gen took %f seconds", timer);
}
void world_render_tick(int x_player, int y_player, int z_player, struct World *world_obj, unsigned int chunk_shader)
{
    for (size_t i = 0; i < WORLD_SIZE * WORLD_SIZE; i++)
    {
        mat4 trans;
        glm_mat4_identity(trans);
        glm_translate(trans, (vec3){world_obj->chunks[i].x * 16, 0, world_obj->chunks[i].z * 16});
        // shader_set_vec2(chunk_shader, "chunkPosition", (vec2){world_obj->chunks[i].x, world_obj->chunks[i].z});
        shader_set_vec3i(chunk_shader, "playerPos", (ivec3){x_player, y_player, z_player});
        shader_set_mat4(chunk_shader, "model", trans);
        renderChunk(world_obj->chunks + i);
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
    _dest = world->chunks + ((x >> 4 + WORLD_SIZE / 2) + (z >> 4 + WORLD_SIZE / 2) * WORLD_SIZE);
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