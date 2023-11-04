#include "model.h"

#include <Albedo/Core/World/world.h>
#include <ktx.h>

namespace Albedo
{
    Matrix4x4
    Model::
    GetModelMatrix()
    {
        if (!m_model_matrix.has_value())
        {
            m_model_matrix = Affine3D::Identity();

            // Euler Extrinsic Rotation
            AngleAxis roll(static_cast<Radian>(m_transform.rotate.roll),     World::Axis.Roll);
            AngleAxis pitch(static_cast<Radian>(m_transform.rotate.pitch),   World::Axis.Pitch);
            AngleAxis yaw(static_cast<Radian>(m_transform.rotate.yaw),       World::Axis.Yaw);
            
            m_model_matrix->rotate(yaw * pitch * roll)
                           .scale(m_transform.scale)
                           .translate(m_transform.translate);
        }

        return m_model_matrix->matrix();
    }
} // namespace Albedo