#ifndef _WORLD_H
#define _WORLD_H
#define WORLD_SIZE 16
#include "chunk.h"
/*
    @attention: all functions in this header file take
    coordinates as in world (-WORLD_SIZE*8 to +WORLD_SIZE*8 exc) input
    for chunk coordinates (unseigned 0 to 16 exc) use chunk.h
*/

/*
    Used for easy access to loaded Chunks onto the memory
    Map size depends on how many chunks are allowed to leave on the memory

    @warning make sure unloaded chunks get written to disk before they get
    removed from the map.

*/
struct ChunkMap
{
    unsigned int size;
    struct Chunk **inner_array;
};

struct World
{
    struct Chunk *chunks;
    struct ChunkMap *chunkMap;
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