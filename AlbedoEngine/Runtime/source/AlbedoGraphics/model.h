#pragma once

#include "../AlbedoCore/Math/vector.h"
#include "../AlbedoCore/Math/matrix.h"
#include "../AlbedoCore/Math/angle.h"
#include "../AlbedoCore/Math/graphics.h"

#include <optional>

namespace Albedo
{
	
	class Model
	{
	public:
		struct Transform
		{
			Vector3D	scale{ Vector3D::Ones() };
			Vector3D	translate{ Vector3D::Zero() };
			union { struct { Degree roll, pitch, yaw; }; }rotate;
		};

	public:
		auto GetTransform() const -> const Transform& { return m_transform; }
		auto GetModelMatrix() -> Matrix4x4;

		auto SetTransform() -> Transform& { m_model_matrix.reset(); return m_transform; }

	public:
		struct Vertex
		{
			alignas(16) Vector3D position;
			alignas(16) Vector3D normal;
			alignas(16) Vector2D uv;
			alignas(16) Vector4D color;
		};

	private:
		// Space State
		Transform				m_transform;
		std::optional<Affine3D> m_model_matrix;
		// Geometric State
		std::vector<Vertex>		m_vertices;
	};

} // namespace Albedo