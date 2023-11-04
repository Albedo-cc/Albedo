#include "angle.h"
#include "graphics.h"

#include <Albedo/Core/World/world.h>

namespace Albedo
{
	Degree::
	Degree(const Radian& radian) :
		m_degree{ (double(radian) / PI) * 180.0 }
	{

	}

	Degree::
	operator Radian() const
	{ 
		return Radian{ (m_degree / 180.0) * PI };
	}

	Radian::
	Radian(const Degree& degree) :
		m_radian{ (double(degree) / 180.0) * PI }
	{

	}

	Radian::operator Degree() const
	{ 
		return Degree{ (m_radian / PI) * 180.0 };
	}

		void
	Transform::
	GetModelMatrix(Affine3D& out) const
	{
		out = Affine3D::Identity();

        // Euler Extrinsic Rotation
        AngleAxis roll	(static_cast<Radian>(rotate.roll),  World::Axis.Roll);
        AngleAxis pitch	(static_cast<Radian>(rotate.pitch), World::Axis.Pitch);
        AngleAxis yaw	(static_cast<Radian>(rotate.yaw),   World::Axis.Yaw);
            
        out .rotate(yaw * pitch * roll)
			.scale(scale)
			.translate(translate);
	}

	Matrix4x4
	Transform::
	GetModelMatrix() const
	{
		Affine3D affine;
		GetModelMatrix(affine);

		return affine.matrix();
	}

} // namespace Albedo