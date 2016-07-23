dofile "interface.lua"
dofile "primitives.lua"

local verts = {}
local origin = Vec3(200, 58, 450)
local windowCount = 5
local cellWidth = 3
local thickness = 0.15

Cuboid(verts, origin + Vec3(thickness, 0, thickness), Vec3(windowCount*cellWidth - 2*thickness, 0.1, windowCount*cellWidth - 2*thickness), Colour(80, 80, 80, 255))

for i = 0, windowCount-1 do
    -- Front
    WindowWallCell(verts, origin + Vec3(cellWidth*i, 0, 0), Vec3(cellWidth, 3, thickness), Colour(120, 120, 120, 255), Vec2(2, 2.8), false)
    -- Back
    WindowWallCell(verts, origin + Vec3(cellWidth*i, 0, windowCount*cellWidth - thickness), Vec3(cellWidth, 3, thickness), Colour(120, 120, 120, 255), Vec2(2, 2.8), false)
    -- Left
    WindowWallCell(verts, origin + Vec3(0, 0, cellWidth*i), Vec3(cellWidth, 3, thickness), Colour(120, 120, 120, 255), Vec2(2, 2.8), true)
    -- Right
    WindowWallCell(verts, origin + Vec3(windowCount*cellWidth - thickness, 0, cellWidth*i), Vec3(cellWidth, 3, thickness), Colour(120, 120, 120, 255), Vec2(2, 2.8), true)
end

mesh.add(verts)