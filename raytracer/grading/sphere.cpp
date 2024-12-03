#include "sphere.h"
#include "ray.h"


// Student Name: Denitca Grozeva
// Student ID: 862160754


Sphere::Sphere(const Parse* parse, std::istream& in)
{
    in>>name>>center>>radius;
}

// Determine if the ray intersects with the sphere
Hit Sphere::Intersection(const Ray& ray, int part) const
{
    //TODO;
    Hit empty ={-1,-1,{0,0}};
    vec3 minuscenter = ray.endpoint-center;

   

    double a = dot(ray.direction,ray.direction);
    double b = 2*dot(ray.direction, minuscenter);
    double c = dot(minuscenter,minuscenter) - pow(radius,2);

    double disc = pow(b,2) - 4*a*c;

    double t1;
    double t2;

    if(disc < 0)
    {
        //Hit h = {nullptr,0,0};

        //Debug_Scope scope;
        //Pixel_Print("Intersect test with S","; hit: (dist:  ", empty.triangle , "; triangle :  ", empty.triangle, "; uv: ", empty.triangle, ")");

        Debug_Scope scope;
        Pixel_Print("No intersection with ", this->name);

        return empty;
        
    }
    
    double sqrtdisc = sqrt(disc);
    t1 = (-b + sqrtdisc)/(2*a);
    t2 = (-b - sqrtdisc)/(2*a);

    if(disc == 0)
    {
        double t3 = -b/(2*a);
        if(t3 >0)
        {
            Debug_Scope scope;
            Pixel_Print("Intersect test with ", this->name,"; hit: (dist:  ", t3 , "; triangle :  ", empty.triangle, "; uv: ", empty.uv, ")");

            return {t3};
            
        }
    }
    if(disc > 0)
    {
        if(t1 < t2 && t1>= small_t)
        {
           // Hit hitt1 = {this*, t1, part};

           Debug_Scope scope;
            Pixel_Print("Intersect test with ", this->name, "; hit: (dist:  ", t1 , "; triangle :  ", empty.triangle, "; uv: ", empty.uv, ")");
            return {t1};
            

        }
        else if(t2 >= small_t)
        {

             Debug_Scope scope;
            Pixel_Print("Intersect test with ", this->name, "; hit: (dist:  ", t2 , "; triangle :  ", empty.triangle, "; uv: ", empty.uv, ")");
            return {t2};
           
        }
    }


   // Debug_Scope scope;
   // Pixel_Print("Calling intersection", "ray: ", ray, "Sphere center: ", center )
    // Debug_Scope scope;
    //Pixel_Print("Intersect test with S","; hit: (dist:  ", empty.dist , "; triangle :  ", empty.triangle, "; uv: ", empty.triangle, ")");
    Debug_Scope scope;
    Pixel_Print("No intersection with ", this->name);


    return empty;
}

vec3 Sphere::Normal(const Ray& ray, const Hit& hit) const
{
    vec3 normal;
    //TODO; // compute the normal direction
    if(hit.dist>0 && hit.dist > small_t)
    {
        normal = (ray.Point(hit.dist)-center).normalized();
    
    }
    
    //normal = (-center)/radius;
    return normal;
}

std::pair<Box,bool> Sphere::Bounding_Box(int part) const
{
    return {{center-radius,center+radius},false};
}
