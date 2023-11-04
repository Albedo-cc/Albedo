#pragma once

#include "float.h"

namespace Albedo
{
	constexpr double PI { 3.141592653589793 }; // 180 Degrees

	class Radian;
	class Degree
	{
	public:
		constexpr Degree(double degree) :m_degree{ degree } {};
		explicit  Degree() = default;
		explicit  Degree(const Radian& radian);
		explicit  Degree(const Degree& copy) :m_degree{ copy.m_degree } {};
		explicit  Degree(Degree&& another) noexcept:m_degree{ another.m_degree } {};

		operator double() const { return m_degree; }
		operator Radian() const;
		Degree operator*(const Degree& degree2) const { return Degree(m_degree * degree2); }
		Degree operator/(const Degree& degree2) const { return Degree(m_degree / degree2); }
		Degree operator/(double num)		    const { return Degree(m_degree / num);	   }
		Degree operator/(float num)				const { return Degree(m_degree / num);	   }
		bool   operator==(const Degree& degree2)const { return FloatEqual(  m_degree, double(degree2));}
		bool   operator!=(const Degree& degree2)const { return !FloatEqual( m_degree, double(degree2));}
		bool   operator<(const Degree& degree2) const { return FloatLess(   m_degree, double(degree2));}
		bool   operator>(const Degree& degree2) const { return FloatGreater(m_degree, double(degree2));}

	private:
		double m_degree = 0.0;
	};
	
	class Radian
	{
	public:
		constexpr Radian(double radian):m_radian{ radian } {};
		explicit  Radian() = default;
		explicit  Radian(const Degree& degree);
		explicit  Radian(const Radian& copy) :m_radian{ copy.m_radian } {};
		explicit  Radian(Radian&& another) noexcept:m_radian{ another.m_radian } {};

		operator double() const { return m_radian; }
		operator Degree() const;
		Radian operator*(const Radian& radian2) const { return Radian(m_radian * radian2); }
		Radian operator/(const Radian& radian2) const { return Radian(m_radian / radian2); }
		Radian operator/(double num)		    const { return Radian(m_radian / num);	   }
		Radian operator/(float num)				const { return Radian(m_radian / num);	   }
		bool   operator==(const Radian& radian2)const { return FloatEqual(  m_radian, double(radian2));}
		bool   operator!=(const Radian& radian2)const { return !FloatEqual( m_radian, double(radian2));}
		bool   operator<(const Radian& radian2) const { return FloatLess(   m_radian, double(radian2));}
		bool   operator>(const Radian& radian2) const { return FloatGreater(m_radian, double(radian2));}

	private:
		double m_radian = 0.0;
	};

} // namespace Albedo