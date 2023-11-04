#pragma once

#include <Albedo/Core/Math/vector.h>
#include <Albedo/Core/Math/matrix.h>
#include <Albedo/Core/Math/angle.h>
#include <Albedo/Core/Math/graphics.h>

#include <optional>

namespace Albedo
{
	
	class Model
	{
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