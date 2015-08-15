#pragma once

#include "vesp/graphics/Buffer.hpp"

#include "vesp/math/Matrix.hpp"
#include "vesp/math/Quaternion.hpp"

namespace vesp { namespace graphics {

	class VertexShader;
	class PixelShader;

	class Mesh
	{
	public:
		Mesh();

		bool Create(ArrayView<Vertex> vertices, 
			D3D11_PRIMITIVE_TOPOLOGY topology = D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

		Vec3 GetPosition();
		void SetPosition(Vec3 const& position);

		Quat GetAngle();
		void SetAngle(Quat const& angle);

		Vec3 GetScale();
		void SetScale(Vec3 const& scale);
		void SetScale(F32 scale);

		void SetPositionAngle(Vec3 const& position, Quat const& angle);

		Colour GetColour();
		void SetColour(Colour colour);

		void SetTopology(D3D11_PRIMITIVE_TOPOLOGY topology);
		D3D11_PRIMITIVE_TOPOLOGY GetTopology();

		void SetVertexShader(VertexShader* shader);
		void SetPixelShader(PixelShader* shader);

		void Draw();

	private:
		void UpdateMatrix();

		struct PerMeshConstants
		{
			Mat4 world;
			Mat4 worldView;
			Mat4 worldViewInverseTranspose;
			Vec4 colour;
		};
		
		VertexShader* vertexShader_ = nullptr;
		PixelShader* pixelShader_ = nullptr;

		VertexBuffer vertexBuffer_;
		ConstantBuffer<PerMeshConstants> perMeshConstantBuffer_;
		D3D11_PRIMITIVE_TOPOLOGY topology_;

		Mat4 world_;
		Vec3 position_;
		Quat angle_;
		Vec3 scale_;
		Colour colour_ = Colour::White;
	};

} }