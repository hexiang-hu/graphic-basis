/**********************************************************************
 * Object class
 * An abstract class for polymorphism
 **********************************************************************/
#pragma once
#include "ray.hpp"
#include "vector.hpp"
#include "matrix.hpp"
#include "stdint.h"
 
class Object 
{
public: 
  static uint64_t intersect_cnt;

  // material property used in Phong model
  Object(){}

  virtual ~Object() {}

  virtual bool intersect(const Ray &, Intersection &) = 0;
  virtual Vector normal(const Point &)          const = 0;

  // getter function for derived class variable 
  virtual RGB   ambient(const Point &q)      const { return RGB();           }
  virtual RGB   diffuse(const Point &q)      const { return RGB();           }
  virtual RGB   specular(const Point &q)     const { return RGB();           }
  virtual float shineness(const Point &q)    const { return float(0.0);      }
  virtual float reflection(const Point &q)   const { return float(0.0);      }
  virtual float transparency(const Point &q) const { return float(0.0);      }
  virtual float transmission(const Point &q) const { return float(0.0);      }

  static void count_intersection(){ 
    intersect_cnt ++; 
  }
};
