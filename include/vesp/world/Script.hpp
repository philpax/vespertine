#pragma once

#include "vesp/script/Module.hpp"

#include "vesp/graphics/Mesh.hpp"

#include "vesp/util/GlobalSystem.hpp"

namespace vesp { namespace world {

class Script : public util::GlobalSystem<Script>
{
public:
	Script();

	void Reload();
	
	void AddMesh(graphics::Mesh&& mesh);
	void Draw();

private:
	void BindConsole();

	Vector<graphics::Mesh> meshes_;
	UniquePtr<script::Module> module_;
};

} }