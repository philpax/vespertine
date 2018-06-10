#pragma warning(disable: 4005)
#include "vesp/graphics/Mesh.hpp"
#include "vesp/graphics/Engine.hpp"
#include "vesp/graphics/Camera.hpp"
#include "vesp/graphics/Shader.hpp"
#include "vesp/graphics/ShaderManager.hpp"

#include "vesp/Assert.hpp"

namespace vesp { namespace graphics {

	Mesh::Mesh()
	{
		this->scale_ = Vec3(1, 1, 1);
	}

	bool Mesh::Create(ArrayView<Vertex> vertices, ArrayView<U32> indices, D3D11_PRIMITIVE_TOPOLOGY topology)
	{
		auto ret = this->Create(vertices, topology);
		if (!ret)
			return this->exists_;

		if (!this->indexBuffer_.Create(indices))
			this->exists_ = false;

		return this->exists_;
	}

	bool Mesh::Create(ArrayView<Vertex> vertices, D3D11_PRIMITIVE_TOPOLOGY topology)
	{
		if (!this->vertexBuffer_.Create(vertices))
			return this->exists_;

		PerMeshConstants constants;

		if (!this->perMeshConstantBuffer_.Create(constants))
			return this->exists_;

		this->topology_ = topology;
		this->exists_ = true;

		return this->exists_;
	}

	Vec3 Mesh::GetPosition()
	{
		return this->position_;
	}

	void Mesh::SetPosition(Vec3 const& position)
	{
		this->SetPositionAngle(position, this->GetAngle());
	}

	Quat Mesh::GetAngle()
	{
		return this->angle_;
	}

	void Mesh::SetAngle(Quat const& angle)
	{
		this->SetPositionAngle(this->GetPosition(), angle);
	}

	void Mesh::SetPositionAngle(Vec3 const& position, Quat const& angle)
	{
		this->position_ = position;
		this->angle_ = angle;
	}

	Colour Mesh::GetColour()
	{
		return this->colour_;
	}

	void Mesh::SetColour(Colour colour)
	{
		this->colour_ = colour;
	}

	Vec3 Mesh::GetScale()
	{
		return this->scale_;
	}

	void Mesh::SetScale(Vec3 const& scale)
	{
		this->scale_ = scale;
	}

	void Mesh::SetScale(F32 scale)
	{
		this->SetScale(Vec3(scale, scale, scale));
	}

	void Mesh::SetTopology(D3D11_PRIMITIVE_TOPOLOGY topology)
	{
		this->topology_ = topology;
	}

	D3D11_PRIMITIVE_TOPOLOGY Mesh::GetTopology()
	{
		return this->topology_;
	}

	void Mesh::SetVertexShader(StringView const shaderId)
	{
		this->vertexShader_ = shaderId.CopyToVector();
	}

	void Mesh::SetPixelShader(StringView const shaderId)
	{
		this->pixelShader_ = shaderId.CopyToVector();
	}

	bool Mesh::Exists() const
	{
		return this->exists_;
	}

	void Mesh::Draw()
	{
		VESP_ASSERT(this->Exists());
		VESP_ASSERT(this->vertexShader_.size() != 0);
		VESP_ASSERT(this->pixelShader_.size() != 0);

		ShaderManager::Get()->GetVertexShader(this->vertexShader_)->Activate();
		ShaderManager::Get()->GetPixelShader(this->pixelShader_)->Activate();

		this->UpdateMatrix();
		this->vertexBuffer_.Use(0);

		this->perMeshConstantBuffer_.UseVS(1);
		Engine::ImmediateContext->IASetPrimitiveTopology(this->topology_);

		if (this->indexBuffer_.Initialized())
		{
			this->indexBuffer_.Use();
			Engine::ImmediateContext->DrawIndexed(
				this->indexBuffer_.GetCount(), 0, 0);
		}
		else
		{
			Engine::ImmediateContext->Draw(this->vertexBuffer_.GetCount(), 0);
		}
	}

	void Mesh::UpdateMatrix()
	{
		this->world_ = math::Transform(this->position_, this->angle_, this->scale_);

		PerMeshConstants constants;
		constants.world = this->world_;
		constants.worldView = 
			this->world_ * Engine::Get()->GetCamera()->GetView();
		constants.worldViewInverseTranspose = 
			glm::transpose(glm::inverse(constants.worldView));
		constants.colour = this->colour_;

		this->perMeshConstantBuffer_.Load(constants);
	}

} }