#include "reflective_shader.h"
#include "parse.h"
#include "ray.h"
#include "render_world.h"

Reflective_Shader::Reflective_Shader(const Parse* parse,std::istream& in)
{
    in>>name;
    shader=parse->Get_Shader(in);
    in>>reflectivity;
    reflectivity=std::max(0.0,std::min(1.0,reflectivity));
}

vec3 Reflective_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    //TODO; // determine the color
    if(hit.dist<=small_t)
    {
        return {};
    }
    // surface light
    vec3 shaderColor = {};
    if(this->shader != nullptr)
    {
        shaderColor = this->shader->Shade_Surface(render_world,ray,hit,intersection_point,normal,recursion_depth)*(1-reflectivity);
    }
    // reflected light
    vec3 reflectedColor = {};
   
    /*double a = (n1 / n2);
    double a_squared = pow(a, 2);
    double sin_i = cross(v, normal).magnitude();
    double cos_i = dot(v, normal);
    double R0 = pow((n1-n2)/(n1+n2),2);
    double Rof0 = R0 + (1-R0)*pow((1-cos_i),5);
    if (Rof0 > 1) 
    {
        Rof0 = 1;
    }*/
    vec3 v = ray.direction.normalized();
    
    vec3 refl =(v-(2.0*dot(v, normal)*normal)).normalized();
    Ray reflectray(intersection_point, refl);

    reflectedColor = render_world.Cast_Ray(reflectray, recursion_depth)*reflectivity*1;
    

    return (shaderColor+reflectedColor);
}
