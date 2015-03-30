/**********************************************************************
 * Spheres class
 **********************************************************************/
#pragma once
#include "vector.hpp"
#include "object.h"
#include <vector>
#include <iostream>

class Sphere: public Object
{
  RGB mat_ambient;
  RGB mat_diffuse;
  RGB mat_specular;
  float mat_shineness;

  float mat_reflection;
  float mat_transparency; 
  float mat_transmission;
public: 
  Point center;
  float radius, radius2;

  // *********************************************************************
  // Sphere constructor
  // *********************************************************************
  Sphere(const RGB &amb, const RGB &dif, const RGB &spe, 
    const float &shine, const float &refl, const float &transp, 
    const float &transm, const Point &ctr, const float &rad) : 
    mat_ambient(amb), mat_diffuse(dif), mat_specular(spe), 
    mat_shineness(shine), mat_reflection(refl), mat_transparency(transp),
    mat_transmission(transm), center(ctr), radius(rad), radius2(rad * rad)
  {}


  ~Sphere() {}

  // *********************************************************************
  // Function for calculate sphere normal
  // *********************************************************************
  Vector normal(const Point &q) {
    return (this->center - q).normalize();
  }

  // *********************************************************************
  // Quadratic function solver 
  // *********************************************************************
  static bool solveQuadraticEquation(const float &a, const float &b, const float &c, float *x0, float *x1)
  {
    // discriminant for the equation
    float discr = b * b - 4 * a * c;
    if (discr < 0){
      // no solution to this case
      return false;
    }
    else if (discr == 0) {
      // single solution
      *x0 = *x1 = - 0.5 * b / a;
    }
    else {
      // dual solution
      // notice that the original root equation is replaced for more stable
      // floating point computation
      float q = (b > 0) ? -0.5 * (b + sqrt(discr)) : -0.5 * (b - sqrt(discr));
      *x0 = q / a;
      *x1 = c / q;
    }
    if (*x0 > *x1) std::swap(*x0, *x1);
    return true;
  }


  // *********************************************************************
  // Function for finding ray-sphere intersection point 
  // *********************************************************************
  bool intersect(const Point &origin, const Vector &direction, const float tmax, float *hit = NULL, Object * pObject = NULL, bool * flag = NULL)
  {
    float t0, t1;
    Vector L = origin - center;
    float a = direction.dot(direction);
    float b = 2 * direction.dot(L);
    float c = L.dot(L) - radius2;
    
    // if there is no solution to the qudratic solver, return false
    if (!solveQuadraticEquation(a, b, c, &t0, &t1)) 
      return false;

    // if nearest hit out of range or both hits are negative, return false
    if (t0 > tmax || (t0 < 0 && t1 < 0)){ 
      return false;
    }

    if(t0 > 0) *hit = t0;
    else *hit = t1;

    return true;
  }

  // *********************************************************************
  // Function for returning material property
  // *********************************************************************
  RGB ambient(const Point &q)         const { return mat_ambient; }
  RGB diffuse(const Point &q)         const { return mat_diffuse; }
  RGB specular(const Point &q)        const { return mat_specular; }
  float shineness(const Point &q)     const { return mat_shineness; }
  float reflection(const Point &q)    const { return mat_reflection; }
  float transparency(const Point &q)  const { return mat_transparency; }
  float transmission(const Point &q)  const { return mat_transmission; }
};
