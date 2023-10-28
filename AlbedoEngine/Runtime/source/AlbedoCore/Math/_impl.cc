#include "angle.h"

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

} // namespace Albedo