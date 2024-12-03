#include "render_world.h"
#include "flat_shader.h"
#include "object.h"
#include "light.h"
#include "ray.h"


// Student Name: Denitca Grozeva
// Student ID: 862160754


extern bool enable_acceleration;

Render_World::~Render_World()
{
    for(auto a:all_objects) delete a;
    for(auto a:all_shaders) delete a;
    for(auto a:all_colors) delete a;
    for(auto a:lights) delete a;
}

// Find and return the Hit structure for the closest intersection.  Be careful
// to ensure that hit.dist>=small_t.
std::pair<Shaded_Object,Hit> Render_World::Closest_Intersection(const Ray& ray) const
{
    //TODO;
   double min_t = 1e30;
   Shaded_Object tempobject;
    Hit H = {-1,-1,{0,0}};
    Hit tempHit = {-1,-1,{0,0}};

   //for (long unsigned int i = 0; i<objects.size(); i++)
   for(auto o: objects)
   {
        //o.object->intersect();
        //std::pair(, Hit) ObjectHit
        H = o.object->Intersection(ray,0);
        /*if(i == 0 )
        {
            tempHit = H;
            min_t = H.dist;
        }*/
        if(H.dist<min_t && H.dist>= small_t)
        {
            tempHit = H;
            tempobject = o;
            min_t = H.dist;
        }
   }

   
   //scope.enable = true;
   Debug_Scope scope;
    Pixel_Print("Closest Intersection","; S; hit: ", "(dist:  ", tempHit.dist, "; triangle :  ", tempHit.triangle, "; uv: ", tempHit.uv, ")");

    return std::make_pair(tempobject,tempHit);
}

// set up the initial view ray and call
void Render_World::Render_Pixel(const ivec2& pixel_index)
{
    // TODO;// set up the initial view ray here
    Ray ray;
    ray.direction = (camera.World_Position(pixel_index) - camera.position).normalized();
    ray.endpoint = camera.position;
    vec3 color=Cast_Ray(ray,1);
    camera.Set_Pixel(pixel_index,Pixel_Color(color));


    Debug_Scope scope;
    Pixel_Print("Debug pixel: -x , -y ", pixel_index);

}

void Render_World::Render()
{
    for(int j=0;j<camera.number_pixels[1];j++)
        for(int i=0;i<camera.number_pixels[0];i++)
            Render_Pixel(ivec2(i,j));
}

// cast ray and return the color of the closest intersected surface point,
// or the background color if there is no object intersection
vec3 Render_World::Cast_Ray(const Ray& ray,int recursion_depth) const
{
    vec3 color;
    //TODO; // determine the color here

    Shaded_Object objecthit;
    Hit H;
    std::pair<Shaded_Object, Hit> ObjectHitPair = Closest_Intersection(ray);
    objecthit = ObjectHitPair.first;
    H = ObjectHitPair.second;
    if(recursion_depth>recursion_depth_limit)
    {
        return {0,0,0};
    }
    recursion_depth = recursion_depth+1;

    if(H.dist >= 0)
    {

        vec3 intersection = ray.Point(H.dist);
        vec3 normal = objecthit.object->Normal(ray,H);
        
        
        Debug_Scope scope1;
        Pixel_Print("Call Shade_Surface with location ", intersection, "; normal:  ", normal);
        
        //color = objecthit.shader.flat_shader->
        color = objecthit.shader->Shade_Surface(*this, ray, H, intersection, normal, recursion_depth);
        
        Debug_Scope scope2;
         Pixel_Print("flat shader; color: ", color);


    }
    else
    {
        // Debug_Scope scope;
        //Pixel_Print("Call Shade_Surface with location ", intersection, "; normal:  ", normal);
        if(background_shader == nullptr)
        {
            color = vec3(0,0,0);

            Debug_Scope scope3;
            Pixel_Print("flat shader; color: ", color);

             
        }
        else
        {
             vec3 emptyvec;

        color = background_shader->Shade_Surface(*this,ray,H,emptyvec,emptyvec, recursion_depth);
         
         
         Debug_Scope scope4;
        Pixel_Print("flat shader; color: ", color);
        }
       
    }

     Debug_Scope scope5;
    Pixel_Print("Cast ray( end: ", ray.endpoint, "; dir:  ", ray.direction, ")");


    return color;
}
