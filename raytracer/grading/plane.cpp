#include "plane.h"
#include "hit.h"
#include "ray.h"
#include <cfloat>
#include <limits>


// Student Name: Denitca Grozeva
// Student ID: 862160754


Plane::Plane(const Parse* parse,std::istream& in)
{
    in>>name>>x>>normal;
    normal=normal.normalized();
}

// Intersect with the plane.  The plane's normal points outside.
Hit Plane::Intersection(const Ray& ray, int part) const
{
    //TODO;
    Hit empty = {-1,-1,{0,0}};
    double udotn = dot(ray.direction, normal);
    double t;
    double TS;

    if(udotn ==0 )
    {
        return empty;
    }
    else
    {
        t = dot((x - ray.endpoint), normal);
        TS = t/udotn;
        if(TS < small_t)
        {
            return empty;
        }
    
    }

    return {TS};
}

vec3 Plane::Normal(const Ray& ray, const Hit& hit) const
{
    return normal;
}

std::pair<Box,bool> Plane::Bounding_Box(int part) const
{
    Box b;
    b.Make_Full();
    return {b,true};
}
