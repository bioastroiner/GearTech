#define VERTEX_SIZE (3 + 2) /* x y z u v*/
#include "chunk.h"
#include "models.h"
#include "shader.h"
#include "utils/timer.h"
#include <cglm/cglm.h>
#include <glad/glad.h>
#include <malloc.h>
#include <memory.h>
#include <stdbool.h>
#include <stdio.h>

enum Side
{
    FRONT,
    BACK,
    RIGHT,
    LEFT,
    TOP,
    BOTTOM
};
void addBlockInChunk(struct Chunk *chunk, int x, int y, int z, unsigned char blockID)
{
    chunk->blocks[x + z * CHUNK_WIDTH + y * CHUNK_WIDTH * CHUNK_WIDTH] = blockID;
    chunk->block_count++;
}
void generateBlocksInChunk(struct Chunk *chunk)
{
    for (int x = 0; x < CHUNK_WIDTH; x++)
    {
        for (int z = 0; z < CHUNK_WIDTH; z++)
        {
            for (int y = 0; y < CHUNK_HEIGHT; y++)
            {
                addBlockInChunk(chunk, x, y, z, 1);
            }
        }
    }
}
unsigned char getBlockInChunk(struct Chunk *chunk, int x, int y, int z)
{
    return chunk->blocks[(x) + (y)*CHUNK_WIDTH + (z)*CHUNK_WIDTH * CHUNK_WIDTH];
}
void createChunkMesh(unsigned char *blocks, unsigned int size, float **_dest_mesh, unsigned int *_dest_size)
{
    float *vert = (float *)malloc(sizeof(float) * 5 * size * 6 * 6);
    unsigned int verts_added = 0;
    for (int i = 0; i < CHUNK_VOLUME; i++)
    {
        if (blocks[i] == 0)
            continue;
        float x = i % CHUNK_WIDTH;
        float z = (i / CHUNK_WIDTH) % CHUNK_WIDTH;
        float y = (i / CHUNK_WIDTH / CHUNK_WIDTH) % CHUNK_HEIGHT;
        bool sides[6] = {true, true, true, true, true, true};
        bool remove_nonvisible_faces = true;
        // calculate renderable faces
        if (remove_nonvisible_faces)
            for (size_t k = 0; k < 6; k++)
            {
                int j = 0;
                switch (k)
                {
                case FRONT:
                    if (!(/*z == 0 ||*/ z == CHUNK_WIDTH - 1))
                        sides[k] = (blocks[i + CHUNK_WIDTH] == 0);
                    break;
                case BACK:
                    if (!(z == 0 /*|| z == CHUNK_WIDTH - 1*/))
                        sides[k] = (blocks[i - CHUNK_WIDTH] == 0);
                    break;
                case RIGHT:
                    if (!(/*x == 0 ||*/ x == CHUNK_WIDTH - 1))
                        sides[k] = (blocks[i + 1] == 0);
                    break;
                case LEFT:
                    if (!(x == 0 /*|| x == CHUNK_WIDTH - 1*/))
                        sides[k] = (blocks[i - 1] == 0);
                    break;
                case TOP:
                    if (!(/*y == 0 ||*/ y == CHUNK_HEIGHT - 1))
                        sides[k] = (blocks[i + CHUNK_WIDTH * CHUNK_WIDTH] == 0);
                    break;
                case BOTTOM:
                    if (!(y == 0 /*|| y == CHUNK_HEIGHT - 1*/))
                        sides[k] = (blocks[i - CHUNK_WIDTH * CHUNK_WIDTH] == 0);
                    break;
                }
            }
        // construct mesh
        for (size_t k = 0; k < 6; k++)
        {
            if (sides[k])
            {
                for (unsigned int ii = 0; ii < 30; ii++)
                {
                    if (ii % 5 == 0)
                        vert[verts_added + ii] = buffer[k][ii] + x;
                    else if (ii % 5 == 1)
                        vert[verts_added + ii] = buffer[k][ii] + y;
                    else if (ii % 5 == 2)
                        vert[verts_added + ii] = buffer[k][ii] + z;
                    else
                        vert[verts_added + ii] = buffer[k][ii];
                }
                verts_added += 30;
            }
        }
    }
    *_dest_mesh = (float *)malloc(sizeof(float) * verts_added);
    *_dest_size = verts_added;
    memcpy(*_dest_mesh, vert, sizeof(float) * verts_added);
    free(vert);
}
void renderMesh(struct Chunk *chunk)
{
    GLuint vao = chunk->vao;
    GLuint vbo = chunk->vbo;
    if (!glIsVertexArray(vao))
        glGenVertexArrays(1, &vao);
    chunk->vao = vao;
    size_t size = chunk->mesh_size;
    if (!glIsBuffer(vbo))
        glGenBuffers(1, &vbo);
    chunk->vbo = vbo;
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * size, chunk->mesh, GL_STATIC_DRAW);
    glBindVertexArray(vao);
    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(GLfloat), (void *)(0));
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, 5 * sizeof(GLfloat), (void *)(3 * sizeof(GLfloat)));
    glEnableVertexAttribArray(0);
    glEnableVertexAttribArray(1);
    int size_quad;
    glGetBufferParameteriv(GL_ARRAY_BUFFER, GL_BUFFER_SIZE, &size_quad);
    glDrawArrays(GL_TRIANGLES, 0, size_quad);
    // glDeleteBuffers(1, &chunk->vbo);
}
void renderChunk(struct Chunk *chunk)
{
    renderMesh(chunk);
}
void createNewChunk(int x, int z, struct World *world, struct Chunk *_dest)
{
    _dest->world = world;
    _dest->block_count = 0;
    _dest->mesh_size = 0;
    _dest->vao = 0;
    _dest->vbo = 0;
    _dest->mesh = NULL;
    _dest->x = x;
    _dest->z = z;
    generateBlocksInChunk(_dest);
    createChunkMesh(_dest->blocks, CHUNK_VOLUME, &_dest->mesh, &_dest->mesh_size);
}