#include "world.h"
#include "cglm/cglm.h"
#include "shader.h"
#include "utils/timer.h"
#include <GLFW/glfw3.h>
#include <stdio.h>
#include <sys/stat.h>
#include <dirent.h>

int loadChunkFromDisk(struct World *world_obj, int x, int z, struct Chunk *chunk)
{
    char fileName[20];
    int id = hash(x, z);
    sprintf(fileName, "world/%d.bin", id);
    FILE *file = fopen(fileName, "r");
    int dataRed = sizeof(world_obj->loaded_chunks->blocks) / sizeof(char);
    if (file)
    {
        fread(chunk->blocks, sizeof(char), dataRed, file);
        return 1;
    }
    fprintf(stderr, "could not read chunk with x:%d z:%d\n", x, z);
    return 0;
}
int saveChunkToDisk(struct World *world_obj, struct Chunk *chunk, int x, int z)
{
    unsigned int blocks = chunk->block_count;
    unsigned char *blocksPtr = chunk->blocks;
    int id = hash(x, z);
    mkdir("world");
    if (opendir("world"))
    {
        char fileName[20];
        sprintf(fileName, "world/%d.bin", id);
        FILE *file = fopen(fileName, "w");
        if (file)
        {
            int dataWritten = sizeof(chunk->blocks) / sizeof(char);
            fwrite(chunk->blocks, sizeof(char), dataWritten, file);
            return 1;
        }
        fprintf(stderr, "could not create chunk file with x:%d z:%d\n", x, z);
        return 0;
    }
    fprintf(stderr, "could not create world directory\n");
    return 0;
}
// todo call this method everytime player passes a new chunk, never call it everytick
void loadChunksAroundPlayer(struct World *world_obj, int x_player, int z_player)
{
    for (size_t i = 0; i < sizeof(world_obj->loaded_chunks) / sizeof(struct Chunk); i++)
    {
        if (world_obj->loaded_chunks + i == NULL)
            continue;
        saveChunkToDisk(world_obj, world_obj->loaded_chunks + i, world_obj->loaded_chunks[i].x, world_obj->loaded_chunks[i].z);
    }
    world_obj->loaded_chunks_size = 0;
    // todo for now we just assume player is at 0,0
    for (int i = -4; i < 4; i++)
    {
        for (int j = -4; j < 4; j++)
        {
            loadChunkFromDisk(world_obj, i, j, world_obj->loaded_chunks + world_obj->loaded_chunks_size);
            struct Chunk *chunk = world_obj->loaded_chunks + world_obj->loaded_chunks_size;
            chunk->x = i;
            chunk->z = j;
            chunk->block_count = CHUNK_VOLUME;
            chunk->world = world_obj;
            world_obj->loaded_chunks_size++;
        }
    }
}
// todo make it to only generate around player
void world_generate(struct World *world_obj)
{
    unsigned int size = WORLD_SIZE * WORLD_SIZE;
    float timer = glfwGetTime();
    world_obj->world_size = size;
    // world_obj->chunks = calloc(size, sizeof(struct Chunk));
    for (char i = -4; i < 4; i++)
    {
        for (char j = -4; j < 4; j++)
        {
            struct Chunk *chunk = malloc(sizeof(struct Chunk));
            createNewChunk(i, j, world_obj, chunk);
            generateBlocksInChunk(chunk);
            saveChunkToDisk(world_obj, chunk, i, j);
            free(chunk);
        }
    }
    loadChunksAroundPlayer(world_obj, 0, 0);
    // for (size_t i = 0; i < size; i++)
    // {
    //     // createNewChunk(i % WORLD_SIZE - WORLD_SIZE / 2, (i / WORLD_SIZE) % WORLD_SIZE - WORLD_SIZE / 2, world_obj, world_obj->chunks + i);
    //     // initilizeChunk(chunk);
    // }
    printf("world gen took %f seconds", timer);
}
void world_render_tick(int x_player, int y_player, int z_player, struct World *world_obj, unsigned int chunk_shader)
{
    // if((int)(ceilf(glfwGetTime()))%60==0)
    for (size_t i = 0; i < WORLD_SIZE * WORLD_SIZE; i++)
    {
        // check if the chunk needs to be loaded
        glm_vec2_distance((vec2){x_player, z_player}, (vec2){});
        mat4 trans;
        glm_mat4_identity(trans);
        glm_translate(trans, (vec3){world_obj->loaded_chunks[i].x * 16, 0, world_obj->loaded_chunks[i].z * 16});
        // shader_set_vec2(chunk_shader, "chunkPosition", (vec2){world_obj->chunks[i].x, world_obj->chunks[i].z});
        shader_set_vec3i(chunk_shader, "playerPos", (ivec3){x_player, y_player, z_player});
        shader_set_mat4(chunk_shader, "model", trans);
        renderChunk(world_obj->loaded_chunks + i);
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
    _dest = world->loaded_chunks + ((x >> 4 + WORLD_SIZE / 2) + (z >> 4 + WORLD_SIZE / 2) * WORLD_SIZE);
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