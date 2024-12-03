#include "transparent_shader.h"
#include "parse.h"
#include "ray.h"
#include "render_world.h"

Transparent_Shader::
Transparent_Shader(const Parse* parse,std::istream& in)
{
    in>>name>>index_of_refraction>>opacity;
    shader=parse->Get_Shader(in);
    assert(index_of_refraction>=1.0);
}

// Use opacity to determine the contribution of this->shader and the Schlick
// approximation to compute the reflectivity.  This routine shades transparent
// objects such as glass.  Note that the incoming and outgoing indices of
// refraction depend on whether the ray is entering the object or leaving it.
// You may assume that the object is surrounded by air with index of refraction
// 1.
vec3 Transparent_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    //TODO;
    //vec3 color;

    //opacity
    //index_of_refraction

  //  index_of_refraction = 1.05;
//

   bool inside = false;

    
   // double airindex = 1.0;

    vec3 v = ray.direction.normalized();
    //vec3 l_color = l->Emitted_Light(v);
    double n1 = 1, n2 = this->index_of_refraction;
    
    if(dot(v,normal)>0)
    {
        n1 = this->index_of_refraction;
        n2 = 1;
        inside = true;
    }

    Debug_Scope scope;
    Pixel_Print("Index of Refraction ", this->index_of_refraction, "; Opacity: ", opacity);

    vec3 shaderColor = {};
    if((this->shader != nullptr) && (!inside))
    {
        shaderColor = this->shader->Shade_Surface(render_world,ray,hit,intersection_point,normal,recursion_depth);
    }

    v = v.normalized();



   /*
    if (recursion_depth > 1)
    {
        n1 = n2;
        n2 = 1;
    }*/

    double a = (n1 / n2);
    double a_squared = pow(a, 2);
    double sin_i = cross(v, normal).magnitude();
    double cos_i = -dot(v, normal);
    
    if (inside)
    {
        cos_i = -cos_i;
    }

    // Schlick approximation
    double R0 = pow((n1-n2)/(n1+n2),2);
    double Rof0 = R0 + (1-R0)*pow((1-cos_i),5);//cos sign is reversed as the cos_i = cos(180-theta)
    

    if (Rof0 > 1) 
    {
        Rof0 = 1;
    }

    if (n1 == n2)
    {
        Rof0 = 0;
    }

    Debug_Scope scope1;
    Pixel_Print("R of o: ", Rof0);

    vec3 refl =(v-(2.0*dot(v, normal)*normal)).normalized();
    Ray reflectray(intersection_point, refl);
    vec3 worldColorRefracted;
    vec3 worldColorReflected;
    vec3 worldColor;

    if (hit.dist > small_t)
    {


        if (1 >=( a_squared*(sin_i*sin_i)))
        {

            vec3 w = a * v + (a * cos_i - pow(1 - (a_squared * sin_i*sin_i), 0.5)) * normal;
            Ray refractray(intersection_point, w);
            
            Debug_Scope scope2;
            Pixel_Print("V : ", v, "; W: ", w, "; a : ", a, "; Cos: ", cos_i, "; Sin: ", sin_i, "; N1 and N2: ", n1, " ", n2, " Inside: ", inside);
            if (inside)
            {
                worldColorRefracted = render_world.Cast_Ray(refractray, recursion_depth);
                worldColorReflected = render_world.Cast_Ray(reflectray, recursion_depth)*(1-opacity);
            }
            else
            {
                worldColorRefracted = render_world.Cast_Ray(refractray, recursion_depth)*(1-opacity);
                worldColorReflected = render_world.Cast_Ray(reflectray, recursion_depth);
            }

            // mix the reflected and refracted based on 
            worldColor = (worldColorRefracted*(1-Rof0)) + (worldColorReflected*Rof0); 


            

            /*std::pair<Shaded_Object, Hit> ObjectHitPairRefract = render_world.Closest_Intersection(refractray);
            Shaded_Object refractob = ObjectHitPairRefract.first;
            Hit refracthit = ObjectHitPairRefract.second;
            vec3 intersectRef = refractray.Point(refracthit.dist);

            color += this->shader->Shade_Surface(render_world, refractray)*/
        }
        else
        {
            
            vec3 worldColorRefracted;
            if (inside)
            {
                worldColorReflected = render_world.Cast_Ray(reflectray, recursion_depth)*(1-opacity);
            }
            else
            {
               worldColorReflected = render_world.Cast_Ray(reflectray, recursion_depth);
            }

            // mix the reflected and refracted based on 
            worldColor = worldColorReflected; 

        }




        /*if (render_world.enable_shadows == true)
        {
            Ray shadowray(intersection_point, v);

            std::pair<Shaded_Object, Hit> ObjectHitPair = render_world.Closest_Intersection(shadowray);
            Shaded_Object objecthitshadow = ObjectHitPair.first;
            Hit shadowhit = ObjectHitPair.second;

            if (objecthitshadow == nullptr || shadowhit.dist < small_t || shadowhit.dist > v.magnitude())
            {

                if (a_squared > sin_i)
                {

                    vec3 w = a * v + (a * cos_i - pow(1 - a_squared * sin_i, 0.5));
                    Ray refractray(intersection_point, w);
                    std::pair<Shaded_Object, Hit> ObjectHitPairRefract = render_world.Closest_Intersection(refractray);
                    Shaded_Object refractob = ObjectHitPairRefract.first;
                    Hit refracthit = ObjectHitPairRefract.second;
                    vec3 intersectRef = refractray.Point(refracthit.dist);
                    if (recursion_depth == 0)
                    {
                        recursion_depth++;
                    }
                    else
                    {
                        recursion_depth--;
                    }
                    color = this->shader->Shade_Surface(render_world, refractray)
                }
            }
        }*/
    }
    




    //return worldColor+(shaderColor*(1-Rof0)*opacity);
    return worldColor+(shaderColor*opacity);
}
