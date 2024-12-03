#include "light.h"
#include "parse.h"
#include "object.h"
#include "phong_shader.h"
#include "ray.h"
#include "render_world.h"

Phong_Shader::Phong_Shader(const Parse* parse,std::istream& in)
{
    in>>name;
    color_ambient=parse->Get_Color(in);
    color_diffuse=parse->Get_Color(in);
    color_specular=parse->Get_Color(in);
    in>>specular_power;
}


/*double max(double v, double l)
{
    if(v>l)
    {
        return v;
    }
    if(l>v)
    {
        return l;
    }

    return v;
}*/

vec3 Phong_Shader::
Shade_Surface(const Render_World& render_world,const Ray& ray,const Hit& hit,
    const vec3& intersection_point,const vec3& normal,int recursion_depth) const
{
    vec3 color = vec3(0,0,0);
    //TODO; //determine the color

    //Phong reflection model: Ambient + Diffuse + Specular
    // I I = I_a + I_d + I_s = R_a*L_a + R_d*L_d*max(ndotl, 0) + R_s*L_s* max(cosphi, 0)
    vec3 amb = vec3(0,0,0);
    if(render_world.ambient_color != nullptr)
    {
         amb = render_world.ambient_color->Get_Color(hit.uv)*render_world.ambient_intensity*this->color_ambient->Get_Color(hit.uv);
    }
    
    vec3 diff = vec3(0,0,0);
    vec3 spec = vec3(0,0,0);


    color += amb;

    Debug_Scope scope5;
    Pixel_Print("ambient: ", amb);





    for(auto l: render_world.lights)
    {
        //l*r is the same as cosphi I think

        vec3 v = l->position-intersection_point;
       
        //vec3 v = (intersection_point-l->position).normalized();


        vec3 refl = (-v+(2.0*dot(v, normal)*normal)).normalized();
        
        //vec3 view_dir = (render_world.camera.position- intersection_point).normalized();
        //vec3 half_vec = (v + view_dir).normalized();
        
        /*
        Ray Lightray(intersection_point, normal);
       std::pair<Shaded_Object, Hit> ObjectHitPair = render_world.Closest_Intersection(Lightray);
       Hit hitLight = ObjectHitPair.second;
        */


       
            


            if(hit.dist>small_t)
            {
                 if(render_world.enable_shadows== true )
                {

                    Ray shadowray(intersection_point, v);

                    std::pair<Shaded_Object, Hit> ObjectHitPair = render_world.Closest_Intersection(shadowray);
                    Shaded_Object objecthitshadow = ObjectHitPair.first;
                    Hit shadowhit = ObjectHitPair.second;





                    if(objecthitshadow.object== nullptr|| shadowhit.dist < small_t || shadowhit.dist> v.magnitude())
                    {

                        double diffp = dot(normal, v.normalized());


                        



                        if(diffp>0)
                        {
                             
                              
                             
                             
                              diff = this->color_diffuse->Get_Color(hit.uv)* l->Emitted_Light(v) * diffp;

                           

                         }
                         else
                         {
                             diff = this->color_diffuse->Get_Color(hit.uv) * l->Emitted_Light(v) * 0.0;
                         }



                        vec3 cam = (render_world.camera.position-intersection_point).normalized();

                         double maxspe = dot(cam,refl);
                         if(maxspe>0)
                         {
                             spec = this->color_specular->Get_Color(hit.uv)* l->Emitted_Light(v)*pow(maxspe, specular_power);
                         }
                        else
                        {
                             spec = this->color_specular->Get_Color(hit.uv)* l->Emitted_Light(v)*pow(0.0, specular_power);
                         }


                    color = color+ spec + diff;





                    }
                 }
                else{

                         double diffp = dot(normal, v.normalized());
                        if(diffp>0)
                        {
                            //diff = (this->color_diffuse->Get_Color(hit.uv)* l->Emitted_Light(v)).normalized() * diffp;
                            diff =  diffp*l->Emitted_Light(v);
                            diff =(this->color_diffuse->Get_Color(hit.uv)*diff);
                            //diff = (this->color_diffuse->Get_Color(hit.uv)* l->Emitted_Light(v)).normalized() * diffp;

                               // diff = (this->color_diffuse->Get_Color(hit.uv)).normalized()* (l->Emitted_Light(v)).normalized() * diffp;

                               Debug_Scope scope7;
                            Pixel_Print("diffp: ", diffp,": diff: ", diff,"color_diff :" , this->color_diffuse->Get_Color(hit.uv), "; emmited light: ", l->Emitted_Light(v.normalized()), "; light position: ", l->position );
                         }
                         else
                         {
                             diff = this->color_diffuse->Get_Color(hit.uv) * l->Emitted_Light(v) * 0.0;
                         }



                        vec3 cam = (render_world.camera.position-intersection_point).normalized();

                         double maxspe = dot(cam,refl);
                         if(maxspe>0)
                         {
                             spec = this->color_specular->Get_Color(hit.uv)* l->Emitted_Light(v)*pow(maxspe, specular_power);
                         }
                        else
                        {
                             spec = this->color_specular->Get_Color(hit.uv)* l->Emitted_Light(v)*0.0;
                         }


                    color = color+ spec + diff;






                 }







            }
           






            /*
            double diffp = dot(normal, v);
            if(diffp>0)
                {
                    diff = this->color_diffuse->Get_Color(hit.uv)* l->Emitted_Light(v) * diffp;
                }
            else
                {
                    diff = this->color_diffuse->Get_Color(hit.uv) * l->Emitted_Light(v) * 0.0;
                }



            vec3 cam = (render_world.camera.position-intersection_point).normalized();

            double maxspe = dot(cam,refl);
                if(maxspe>0)
                {
                    spec = this->color_specular->Get_Color(hit.uv)* l->Emitted_Light(v)*pow(maxspe, specular_power);
                }
                else
                {
                    spec = this->color_specular->Get_Color(hit.uv)* l->Emitted_Light(v)*pow(0.0, specular_power);
                }


                color = color+ spec + diff;

            */
                
        

        
                Debug_Scope scope6;
                Pixel_Print("Shading for light: ", l->name,": diffuse: ", diff, "; specular: ", spec );


        /*else
        {
            diff = vec3(0.0,0.0,0.0);
            spec =  vec3(0.0,0.0,0.0);
        }*/
        
        
        
        
        
      
        
        
        
        
        /*
                
                double diffp = dot(normal, v);
                
                if(diffp>0)
                {
                    diff = this->color_diffuse->Get_Color(hit.uv)* l->Emitted_Light(v) * diffp;
                }
                else
                {
                    diff = this->color_diffuse->Get_Color(hit.uv) * l->Emitted_Light(v) * 0.0;
                }

                 color += diff;
               
               
               
               
                double maxspe = dot(-ray.direction,refl.normalized());
                if(maxspe>0)
                {
                    spec = this->color_specular->Get_Color(hit.uv)* l->Emitted_Light(v)*pow(maxspe, specular_power);
                }
                else
                {
                    spec = this->color_specular->Get_Color(hit.uv)* l->Emitted_Light(v)*pow(0.0, specular_power);
                }

                
                
                //spec *= pow(dot(-ray.direction, refl.normalized()), specular_power);
                color += spec;
            }
        }
        
        else
        {


            
            double diffp = dot(normal, v);
                
                if(diffp>0)
                {
                    diff = this->color_diffuse->Get_Color(hit.uv)* l->Emitted_Light(v) * diffp;
                }
                else
                {
                    diff = this->color_diffuse->Get_Color(hit.uv) * l->Emitted_Light(v) * 0.0;
                }

                 color += diff;
               
               
               
               
                double maxspe = dot(-ray.direction,refl.normalized());
                if(maxspe>0)
                {
                    spec = this->color_specular->Get_Color(hit.uv)* l->Emitted_Light(v)*pow(maxspe, specular_power);
                }
                else
                {
                    spec = this->color_specular->Get_Color(hit.uv)* l->Emitted_Light(v)*pow(0.0, specular_power);
                }

                
                
                //spec *= pow(dot(-ray.direction, refl.normalized()), specular_power);
                color += spec;

            
             double diffp = dot(normal, v);
                
                if(diffp>0)
                {
                    diff = this->color_diffuse->Get_Color(hit.uv) * l->Emitted_Light(v) * diffp;
                }
                else
                {
                    diff = this->color_diffuse->Get_Color(hit.uv) * l->Emitted_Light(v) * 0.0;
                }
                
                //diff *= max(dot(normal, v.normalized()), 0);
                color += diff;
               
               
               
               
                double maxspe = dot(half_vec,normal);
                if(maxspe>0)
                {
                    spec = this->color_specular->Get_Color(hit.uv)* l->Emitted_Light(v)*pow(maxspe, specular_power);
                }
                else
                {
                    spec = this->color_specular->Get_Color(hit.uv)* l->Emitted_Light(v)*pow(0.0, specular_power);
                }

                
                
                //spec *= pow(dot(-ray.direction, refl.normalized()), specular_power);
                color += spec;

                */


    }
            
    Debug_Scope scope7;
    Pixel_Print("final color: ", color); 
       
    return color;
 

}





