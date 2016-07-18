local ffi = require "ffi"

-- Struct definitions
ffi.cdef [[
typedef unsigned char U8;
typedef unsigned short U16;

typedef struct
{ 
    float x;
    float y;
    float z; 
} Vec3;

typedef union {
    struct
    {
        U8 r;
        U8 g;
        U8 b;
        U8 a;
    };
    U8 data[4];
} Colour;

typedef struct
{
    Vec3 position;
    U16 normal[2];
    U16 texcoord[2];
    Colour colour;
} Vertex;
]]

Vec3 = ffi.metatype("Vec3", {
    __add = function(a, b)
        return Vec3(a.x + b.x, a.y + b.y, a.z + b.z)
    end
})

Colour = ffi.metatype("Colour", {})
Vertex = ffi.metatype("Vertex", {})

-- Vertex helper
local function Vert(point, colour)
    return Vertex(point, {0, 0}, {0, 0}, colour)
end

-- Cuboid creation
local function Cuboid(t, origin, size, colour)
    -- Front
    table.insert(t, Vert(origin + Vec3(0,       0,       0      ), colour))
    table.insert(t, Vert(origin + Vec3(0,       size.y,  0      ), colour))
    table.insert(t, Vert(origin + Vec3(size.x,  0,       0      ), colour))

    table.insert(t, Vert(origin + Vec3(0,       size.y,  0      ), colour))
    table.insert(t, Vert(origin + Vec3(size.x,  size.y,  0      ), colour))
    table.insert(t, Vert(origin + Vec3(size.x,  0,       0      ), colour))

    -- Left
    table.insert(t, Vert(origin + Vec3(0,       0,       size.z ), colour))
    table.insert(t, Vert(origin + Vec3(0,       size.y,  0      ), colour))
    table.insert(t, Vert(origin + Vec3(0,       0,       0      ), colour))

    table.insert(t, Vert(origin + Vec3(0,       0,       size.z ), colour))
    table.insert(t, Vert(origin + Vec3(0,       size.y,  size.z ), colour))
    table.insert(t, Vert(origin + Vec3(0,       size.y,  0      ), colour))

    -- Right
    table.insert(t, Vert(origin + Vec3(size.x,  0,       0      ), colour))
    table.insert(t, Vert(origin + Vec3(size.x,  size.y,  0      ), colour))
    table.insert(t, Vert(origin + Vec3(size.x,  0,       size.z ), colour))

    table.insert(t, Vert(origin + Vec3(size.x,  size.y,  0      ), colour))
    table.insert(t, Vert(origin + Vec3(size.x,  size.y,  size.z ), colour))
    table.insert(t, Vert(origin + Vec3(size.x,  0,       size.z ), colour))

    -- Back
    table.insert(t, Vert(origin + Vec3(size.x,  0,        size.z), colour))
    table.insert(t, Vert(origin + Vec3(0,       size.y,   size.z), colour))
    table.insert(t, Vert(origin + Vec3(0,       0,        size.z), colour))

    table.insert(t, Vert(origin + Vec3(size.x,  0,        size.z), colour))
    table.insert(t, Vert(origin + Vec3(size.x,  size.y,   size.z), colour))
    table.insert(t, Vert(origin + Vec3(0,       size.y,   size.z), colour))

    -- Top
    table.insert(t, Vert(origin + Vec3(0,       size.y,  size.z ), colour))
    table.insert(t, Vert(origin + Vec3(size.x,  size.y,  0      ), colour))
    table.insert(t, Vert(origin + Vec3(0,       size.y,  0      ), colour))

    table.insert(t, Vert(origin + Vec3(0,       size.y,  size.z ), colour))
    table.insert(t, Vert(origin + Vec3(size.x,  size.y,  size.z ), colour))
    table.insert(t, Vert(origin + Vec3(size.x,  size.y,  0      ), colour))

    -- Bottom
    table.insert(t, Vert(origin + Vec3(0,       0,       0      ), colour))
    table.insert(t, Vert(origin + Vec3(size.x,  0,       0      ), colour))
    table.insert(t, Vert(origin + Vec3(0,       0,       size.z ), colour))

    table.insert(t, Vert(origin + Vec3(size.x,  0,       0      ), colour))
    table.insert(t, Vert(origin + Vec3(size.x,  0,       size.z ), colour))
    table.insert(t, Vert(origin + Vec3(0,       0,       size.z ), colour))
end

-- Create a building floor with walls
function Floor(verts, origin, thickness, height, width, depth, includeFloor, includeCeiling, floorPadding)
    if not floorPadding then
        floorPadding = 0
    end

    if includeFloor then
        -- Floor
        Cuboid(verts, origin + Vec3(-floorPadding, 0, -floorPadding),       Vec3(width + 2 * floorPadding, thickness, depth + 2 * floorPadding),    Colour(100, 100, 100, 255))
    end

    if includeCeiling then
        -- Ceiling
        Cuboid(verts, origin + Vec3(floorPadding, height, floorPadding),    Vec3(width + 2 * floorPadding, thickness, depth + 2 * floorPadding),    Colour(100, 100, 100, 255))
    end

    -- Front
    Cuboid(verts, origin + Vec3(0, thickness, 0),                           Vec3(width, height - thickness, thickness),                 Colour(80, 80, 80, 255))
    -- Back
    Cuboid(verts, origin + Vec3(0, thickness, depth - thickness),           Vec3(width, height - thickness, thickness),                 Colour(80, 80, 80, 255))
    -- Left
    Cuboid(verts, origin + Vec3(0, thickness, thickness),                   Vec3(thickness, height - thickness, depth - 2*thickness),   Colour(80, 80, 80, 255))
    -- Right
    Cuboid(verts, origin + Vec3(width - thickness, thickness, thickness),   Vec3(thickness, height - thickness, depth - 2*thickness),   Colour(80, 80, 80, 255))
end

-- Create a building from floors
function Building(verts, origin, floorCount, width, depth, floorPadding)
    if not floorPadding then
        floorPadding = 0
    end

    local thickness = 0.15
    local height = 3
    for i=1, floorCount-1 do
        Floor(verts, origin, thickness, height, width, depth, true, false, floorPadding)
        origin = origin + Vec3(0, height, 0)
    end
    Floor(verts, origin, thickness, height, width, depth, true, true, floorPadding)
end

-- Create a city from buildings
local verts = {}

math.randomseed(0)

local width = 18
local depth = 24
local origin = Vec3(200, 56, 450)

local xCount = 8
local yCount = 8

for y=1, xCount do
    local rowOrigin = origin
    local maxDepth = 0

    for x=1, yCount do
        local ourWidth = width + ((math.random() - 0.5) * 10)
        local ourDepth = depth + ((math.random() - 0.5) * 10)
        maxDepth = math.max(maxDepth, ourDepth)

        local centeredX = x - xCount/2
        local centeredY = y - yCount/2
        local distFromCenter = math.sqrt(centeredX * centeredX + centeredY * centeredY)

        local height = math.floor(65 - 15 * (distFromCenter))
        height = height + (4 * math.random())

        Building(verts, rowOrigin, height, ourWidth, ourDepth, math.random() * 0.5)
        rowOrigin = rowOrigin + Vec3(ourWidth + 5, 0, 0)
    end
    origin = origin + Vec3(0, 0, maxDepth + 5)
end

-- Pass vertices to C++
local vertices = ffi.new("Vertex[?]", #verts)
for i,v in ipairs(verts) do
    vertices[i-1] = v
end

ffi.cdef [[
void MeshAdd(Vertex* vertices, unsigned int count);
]]

ffi.C.MeshAdd(vertices, #verts)