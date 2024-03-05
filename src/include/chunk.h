#ifndef CHUNK_H_
#define CHUNK_H_
#define CHUNK_WIDTH 16
#define CHUNK_HEIGHT 16
#define CHUNK_VOLUME (CHUNK_WIDTH * CHUNK_WIDTH * CHUNK_HEIGHT)
/*
    @attention: all functions in this header file take
    coordinates as in chunk (0 to 16 exc) input
    for world coordinates (seigned) use world.h
*/
struct Chunk
{
    unsigned int vao;
    unsigned int vbo;
    unsigned int block_count;
    unsigned int mesh_size;
    unsigned char blocks[CHUNK_VOLUME];
    int hash;
    int x;
    int z;
    float *mesh;
    struct World *world;
};
void createNewChunk(int x, int z, struct World *world, struct Chunk *_dest);
void renderChunk(struct Chunk *chunk);
int hash(int x, int z);
void addBlockInChunk(struct Chunk *chunk, int x, int y, int z, unsigned char blockID);
unsigned char getBlockInChunk(struct Chunk *chunk, int x, int y, int z);
#endif