-- Vertex helper
function Vert(point, colour)
    return Vertex(point, {0, 0}, {0, 0}, colour)
end

-- Cuboid creation
function Cuboid(t, origin, size, colour)
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

-- Hollow cuboid creation
function HollowCuboid(verts, origin, size, thickness, colour, includeFloor, includeCeiling)
    if includeFloor ~= false then
        -- Floor
        Cuboid(verts, 
            origin + Vec3(0, 0, 0), 
            Vec3(size.x, thickness, size.z), 
            colour)
    end

    if includeCeiling ~= false then
        -- Ceiling
        Cuboid(verts, 
            origin + Vec3(0, size.y - thickness, 0), 
            Vec3(size.x, thickness, size.z), 
            colour)
    end

    -- Front
    Cuboid(verts, 
        origin + Vec3(0, thickness, 0), 
        Vec3(size.x, size.y - 2*thickness, thickness), 
        colour)

    -- Back
    Cuboid(verts, 
        origin + Vec3(0, thickness, size.z - thickness), 
        Vec3(size.x, size.y - 2*thickness, thickness), 
        colour)

    -- Left
    Cuboid(verts, 
        origin + Vec3(0, thickness, thickness), 
        Vec3(thickness, size.y - 2*thickness, size.z - 2*thickness), 
        colour)

    -- Right
    Cuboid(verts, 
        origin + Vec3(size.x - thickness, thickness, thickness), 
        Vec3(thickness, size.y - 2*thickness, size.z - 2*thickness), 
        colour)
end

function math.randomrange(low, high)
    return low + math.random() * (high - low)
end

function math.randommag(mag)
    return math.randomrange(-mag, mag)
end