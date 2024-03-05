#ifndef _WORLD_H
#define _WORLD_H
#define WORLD_SIZE 16
#include "chunk.h"
/*
    @attention: all functions in this header file take
    coordinates as in world (-WORLD_SIZE*8 to +WORLD_SIZE*8 exc) input
    for chunk coordinates (unseigned 0 to 16 exc) use chunk.h
*/
struct World
{
    struct Chunk *ChunkMap;
    unsigned int size;
};
void world_generate(struct World *world_obj);
void world_render_tick(int x_player, int y_player, int z_player, struct World *world_obj, unsigned int chunk_shader);
void world_block_set(int x, int y, int z, unsigned char block);
int world_get_chunk(int x, int z, struct World *world, struct Chunk *_dest);
int world_remove_block(int x, int y, int z, struct World *world);
int world_set_block(int x, int y, int z, struct World *world, unsigned char block);
unsigned char world_get_block(int x, int y, int z, struct World *world);
#endif