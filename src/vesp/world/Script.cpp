#include "vesp/world/Script.hpp"

#include "vesp/graphics/ShaderManager.hpp"

#include "vesp/EventManager.hpp"
#include "vesp/FileSystem.hpp"
#include "vesp/Console.hpp"

namespace vesp { namespace world {

extern "C" __declspec(dllexport) void MeshAdd(graphics::Vertex* vertices, U32 verticesCount)
{
	auto shaderManager = graphics::ShaderManager::Get();

	graphics::Mesh mesh;
	mesh.Create(ArrayView<graphics::Vertex>(vertices, verticesCount));
	mesh.SetVertexShader(shaderManager->GetVertexShader("default"));
	mesh.SetPixelShader(shaderManager->GetPixelShader("default"));

	Script::Get()->AddMesh(std::move(mesh));
}

Script::Script()
{
	this->Reload();
	this->BindConsole();
}

void Script::Reload()
{
	this->meshes_.clear();
	this->module_.reset(new script::Module("World"));

	auto& state = this->module_->GetState();
	
	state["dofile"] = [&](RawStringPtr filename)
	{
		auto path = Concat("data/", filename);
		if (!FileSystem::Get()->Exists(path))
			return;

		auto file = FileSystem::Get()->Open(path, FileSystem::Mode::ReadBinary);
		auto fileContents = file.Read<StringByte>();
		this->module_->RunString(fileContents);
	};

	auto file = FileSystem::Get()->Open("data/world.lua", FileSystem::Mode::ReadBinary);
	auto fileContents = file.Read<StringByte>();
	this->module_->RunString(fileContents);
}

void Script::AddMesh(graphics::Mesh&& mesh)
{
	this->meshes_.push_back(std::move(mesh));
}

void Script::Draw()
{
	for (auto& mesh : this->meshes_)
		mesh.Draw();
}

void Script::BindConsole()
{
	auto& state = Console::Get()->GetModule()->GetState();
	state["world"] = state.create_table();
	state["world"]["reload"] = [&] { this->Reload(); };
}

} }