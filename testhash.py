def hash(x: int,z: int):
    hashcode = 23
    hashcode = (hashcode * 37) + x
    hashcode = (hashcode * 37) + z
    return hashcode
def createChunkMap(diameter: int) -> dict[int, str]:
    map: dict[int,str] = {}
    for x in range(-diameter , diameter):
        for z in range(-diameter , diameter):
            map[hash(x,z)] = f"(x: {x},z: {z})"
    return map
def printChunk(map: dict[int,str],x: int,z: int):
    print(f"expected x:{x}, z:{z} ,got: {map[hash(2,-2)]}")

chunkMap = createChunkMap(4) # 8 * 8
# test cases
printChunk(chunkMap,2,-2);
printChunk(chunkMap,0,3);
printChunk(chunkMap,-4,-2);