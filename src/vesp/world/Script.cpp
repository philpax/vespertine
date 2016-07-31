#include "vesp/world/Script.hpp"

#include "vesp/graphics/ShaderManager.hpp"
#include "vesp/graphics/imgui.h"

#include "vesp/EventManager.hpp"
#include "vesp/FileSystem.hpp"
#include "vesp/Console.hpp"

namespace vesp { namespace world {

extern "C" __declspec(dllexport) U32 MeshAdd(graphics::Vertex* vertices, U32 verticesCount)
{
	auto shaderManager = graphics::ShaderManager::Get();

	graphics::Mesh mesh;
	mesh.Create(ArrayView<graphics::Vertex>(vertices, verticesCount));
	mesh.SetVertexShader(shaderManager->GetVertexShader("default"));
	mesh.SetPixelShader(shaderManager->GetPixelShader("default"));

	return Script::Get()->AddMesh(std::move(mesh));
}

extern "C" __declspec(dllexport) void MeshRemove(U32 meshId)
{
	Script::Get()->RemoveMesh(meshId);
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

	auto imgui = state.create_named_table("imgui");
	imgui["window"] = [](char const* title, sol::protected_function fn)
	{
		ImGui::Begin(title);
		fn();
		ImGui::End();
	};
	imgui["button"] = [](char const* title, sol::protected_function fn)
	{
		if (ImGui::Button(title))
			fn();
	};
	
	auto file = FileSystem::Get()->Open("data/world.lua", FileSystem::Mode::ReadBinary);
	auto fileContents = file.Read<StringByte>();
	this->module_->RunString(fileContents);
}

U32 Script::AddMesh(graphics::Mesh&& mesh)
{
	this->meshes_[this->nextMeshId_] = std::move(mesh);
	return this->nextMeshId_++;
}

void Script::RemoveMesh(U32 meshId)
{
	VESP_ASSERT(this->meshes_.find(meshId) != this->meshes_.end());
	this->meshes_.erase(meshId);
}

void Script::Draw()
{
	for (auto& meshPair : this->meshes_)
		meshPair.second.Draw();
}

void Script::Pulse()
{
	auto& state = this->module_->GetState();

	sol::object pulse = state["pulse"];
	if (!pulse.is<sol::protected_function>())
		return;

	auto runResult = pulse.as<sol::protected_function>()();

	if (runResult.status() != sol::call_status::ok)
	{
		auto errorStr = runResult.get<std::string>();
		LogError("Failed to run world tick: %s", errorStr.c_str());
	}
}

void Script::BindConsole()
{
	Console::Get()->AddCommand("world.reload", [&] { this->Reload(); });
}

} }