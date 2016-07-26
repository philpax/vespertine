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
	
	U32 AddMesh(graphics::Mesh&& mesh);
	void RemoveMesh(U32 meshId);
	void Draw();

	void Pulse();

private:
	void BindConsole();

	UniquePtr<script::Module> module_;

	UnorderedMap<U32, graphics::Mesh> meshes_;
	U32 nextMeshId_ = 0;
};

} }