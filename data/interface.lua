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

typedef struct
{
    float x;
    float y;
} Vec2;

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

void MeshAdd(Vertex* vertices, unsigned int count);
]]

-- Generate constructors + metatables for types
Vec3 = ffi.metatype("Vec3", {
    __add = function(a, b)
        return Vec3(a.x + b.x, a.y + b.y, a.z + b.z)
    end
})

Vec2 = ffi.metatype("Vec2", {
    __add = function(a, b)
        return Vec2(a.x + b.x, a.y + b.y)
    end
})

Colour = ffi.metatype("Colour", {
    __add = function(a, b)
        return Colour(a.r + b.r, a.g + b.g, a.b + b.b, a.a + b.a)
    end,

    __mul = function(a, b)
        return Colour(a.r * b, a.g * b, a.b * b, a.a * b)
    end
})
Vertex = ffi.metatype("Vertex", {})

function math.lerp(a, b, t)
    return (a * (1.0-t)) + (b * t)
end

-- Define a Lua-friendly interface for mesh creation
mesh = {
    add = function(verts)     
        -- Pass vertices to C++
        local vertices = ffi.new("Vertex[?]", #verts)
        for i,v in ipairs(verts) do
            vertices[i-1] = v
        end
        ffi.C.MeshAdd(vertices, #verts)
    end
}