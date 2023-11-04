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
            m_transform.GetModelMatrix(m_model_matrix.value());
        }
        return m_model_matrix->matrix();
    }
} // namespace Albedo