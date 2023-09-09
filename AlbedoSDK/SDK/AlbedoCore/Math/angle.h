#pragma once

#include "float.h"

namespace Albedo
{
	constexpr FloatType PI { 3.141592653589793 }; // 180 Degrees

	class Radian;
	class Degree
	{
	public:
		explicit Degree() = delete;
		explicit Degree(const Radian& radian);
		explicit Degree(FloatType degree) :m_degree{ degree } {};
		explicit Degree(const Degree& copy) :m_degree{ copy.m_degree } {};
		explicit Degree(Degree&& another) noexcept:m_degree{ another.m_degree } {};

		operator  FloatType() const { return m_degree; }	
		Degree operator*(const Degree& degree2) const { return Degree(m_degree * degree2); }
		Degree operator/(const Degree& degree2) const { return Degree(m_degree / degree2); }
		Degree operator/(FloatType num)		    const { return Degree(m_degree / num);	   }
		bool   operator==(const Degree& degree2)const { return FloatEqual(m_degree, degree2); }
		bool   operator!=(const Degree& degree2)const { return !FloatEqual(m_degree, degree2);}
		bool   operator<(const Degree& degree2) const { return FloatLess(m_degree, degree2);  }
		bool   operator>(const Degree& degree2) const { return FloatGreater(m_degree, degree2);}

	private:
		FloatType m_degree = 0.0;
	};
	
	class Radian
	{
	public:
		explicit Radian() = delete;
		explicit Radian(const Degree& degree);
		explicit Radian(FloatType radian):m_radian{ radian } {};
		explicit Radian(const Radian& copy) :m_radian{ copy.m_radian } {};
		explicit Radian(Radian&& another) noexcept:m_radian{ another.m_radian } {};

		operator  FloatType() const { return m_radian; }
		Radian operator*(const Radian& radian2) const { return Radian(m_radian * radian2); }
		Radian operator/(const Radian& radian2) const { return Radian(m_radian / radian2); }
		Radian operator/(FloatType num)		    const { return Radian(m_radian / num);	   }
		bool   operator==(const Radian& radian2)const { return FloatEqual(m_radian, radian2); }
		bool   operator!=(const Radian& radian2)const { return !FloatEqual(m_radian, radian2);}
		bool   operator<(const Radian& radian2) const { return FloatLess(m_radian, radian2);  }
		bool   operator>(const Radian& radian2) const { return FloatGreater(m_radian, radian2);}

	private:
		FloatType m_radian = 0.0;
	};

	Degree::Degree(const Radian& radian) :m_degree{ radian * (180.0 / PI) } {};
	Radian::Radian(const Degree& degree) :m_radian{ degree * (PI / 180.0) } {};

} // namespace Albedo