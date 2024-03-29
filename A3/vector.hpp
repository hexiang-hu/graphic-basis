#pragma once 
#include <stdint.h>
#include <math.h>
#include <ostream>

template<typename T>
class Vec3
{
public:
	T x, y, z;
	Vec3() : x(T(0)), y(T(0)), z(T(0)) {}
	Vec3(T xx) : x(xx), y(xx), z(xx) {}
	Vec3(T xx, T yy, T zz) : x(xx), y(yy), z(zz) {}
	Vec3& normalize()
	{
		T nor2 = length2();
		if (nor2 > 0) {
			T invNor = 1 / sqrt(nor2);
			x *= invNor, y *= invNor, z *= invNor;
		}
		return *this;
	}
	Vec3 & rerange()
	{
		if(this->x > 1.0) this->x = 1.0;
		if(this->y > 1.0) this->y = 1.0;
		if(this->z > 1.0) this->z = 1.0;
		if(this->x < 0.0) this->x = 0.0;
		if(this->y < 0.0) this->y = 0.0;
		if(this->z < 0.0) this->z = 0.0;
		return *this;
	}
	Vec3<T> operator * (const T &f) const { return Vec3<T>(x * f, y * f, z * f); }
	Vec3<T> operator * (const Vec3<T> &v) const { return Vec3<T>(x * v.x, y * v.y, z * v.z); }
	friend Vec3<T> operator * ( const T &s, const Vec3<T>& v ) { return v * s; }
	T dot(const Vec3<T> &v) const { return x * v.x + y * v.y + z * v.z; }
	// minus operation
	Vec3<T> operator - (const Vec3<T> &v) const { return Vec3<T>(x - v.x, y - v.y, z - v.z); }
	Vec3<T> operator - (const T &s) const { return Vec3<T>(x - s, y - s, z - s); }
	friend Vec3<T> operator - (const T &s, const Vec3<T>& v) { return Vec3<T>(s - v.x, s - v.y, s - v.z); }
	// plus operation
	Vec3<T> operator + (const Vec3<T> &v) const { return Vec3<T>(x + v.x, y + v.y, z + v.z); }
	Vec3<T> operator + (const T &s) const { return Vec3<T>(x + s, y + s, z + s); }
	friend Vec3<T> operator + (const T &s, const Vec3<T>& v) { return v + s; }
	Vec3<T>& operator += (const Vec3<T> &v) { x += v.x, y += v.y, z += v.z; return *this; }
	Vec3<T>& operator *= (const Vec3<T> &v) { x *= v.x, y *= v.y, z *= v.z; return *this; }
	Vec3<T> operator - () const { return Vec3<T>(-x, -y, -z); }
	bool operator == (const Vec3<T> &v) { return x == v.x && y == v.y && z == v.z; }
	bool operator != (const Vec3<T> &v) { return !(*this == v); }
	T length2() const { return x * x + y * y + z * z; }
	T length() const { return sqrt(length2()); }
	friend std::ostream & operator << (std::ostream &os, const Vec3<T> &v)
	{
		os << "[" << v.x << " " << v.y << " " << v.z << "]";
		return os;
	}
	Vec3<T> cross(const Vec3<T>& v ) const
	{
	    return Vec3<T>( y * v.z - z * v.y, z * v.x - x * v.z, x * v.y - y * v.x );
	}
	Vec3<T> operator / (const Vec3<T> &v) { return Vec3<T>( x / v.x, y / v.y, z / v.z); } 
	friend Vec3<T> operator / (const T &s, const Vec3<T> &v) { return Vec3<T>( s / v.x, s / v.y, s / v.z); } 
	T & operator [] (const uint32_t i) { 
		if( i == 0 ) return x;
		else if( i == 1) return y;
		else return z;
	}
	const T & operator [] (const uint32_t i) const { 
		if( i == 0 ) return x;
		else if( i == 1) return y;
		else return z;
	}

};

typedef Vec3<float> Point;  			// geometric 3D point
typedef Vec3<float> Vector; 			// geometric 3D vector
typedef Vec3<float> RGB;  				// 3D RBG color
