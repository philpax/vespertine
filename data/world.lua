dofile "interface.lua"
dofile "primitives.lua"

local verts = {}
local origin = Vec3(200, 58, 450)
local windowCount = 5
local wallHeight = 3
local cellWidth = 3
local thickness = 0.15
local levelCount = 10
local windowSize = Vec2(2, 2.2)

for level = 0, levelCount-1 do
    local levelOrigin = origin + Vec3(0, wallHeight * level, 0)
    Cuboid(verts, levelOrigin + Vec3(thickness, 0, thickness), Vec3(windowCount*cellWidth - 2*thickness, 0.1, windowCount*cellWidth - 2*thickness), Colour(80, 80, 80, 255))

    for i = 0, windowCount-1 do
        -- Front
        WindowWallCell(verts, levelOrigin + Vec3(cellWidth*i, 0, 0), Vec3(cellWidth, wallHeight, thickness), Colour(120, 120, 120, 255), windowSize, false)
        -- Back
        WindowWallCell(verts, levelOrigin + Vec3(cellWidth*i, 0, windowCount*cellWidth - thickness), Vec3(cellWidth, wallHeight, thickness), Colour(120, 120, 120, 255), windowSize, false)
        -- Left
        WindowWallCell(verts, levelOrigin + Vec3(0, 0, cellWidth*i), Vec3(cellWidth, wallHeight, thickness), Colour(120, 120, 120, 255), windowSize, true)
        -- Right
        WindowWallCell(verts, levelOrigin + Vec3(windowCount*cellWidth - thickness, 0, cellWidth*i), Vec3(cellWidth, wallHeight, thickness), Colour(120, 120, 120, 255), windowSize, true)
    end
end

-- Final ceiling
Cuboid(verts, origin + Vec3(0, levelCount*wallHeight, 0), Vec3(windowCount*cellWidth, 0.1, windowCount*cellWidth), Colour(80, 80, 80, 255))
-- Corner pillars
local pillarSize = 0.5
Cuboid(verts, origin + Vec3(thickness, 0, thickness), Vec3(pillarSize, levelCount*wallHeight, pillarSize), Colour(60, 60, 60, 255))
Cuboid(verts, origin + Vec3(windowCount*cellWidth - (pillarSize + thickness), 0, thickness), Vec3(pillarSize, levelCount*wallHeight, pillarSize), Colour(60, 60, 60, 255))
Cuboid(verts, origin + Vec3(thickness, 0, windowCount*cellWidth - (pillarSize + thickness)), Vec3(pillarSize, levelCount*wallHeight, pillarSize), Colour(60, 60, 60, 255))
Cuboid(verts, origin + Vec3(windowCount*cellWidth - (pillarSize + thickness), 0, windowCount*cellWidth - (pillarSize + thickness)), Vec3(pillarSize, levelCount*wallHeight, pillarSize), Colour(60, 60, 60, 255))

mesh.add(verts)