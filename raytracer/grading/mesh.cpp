#include "mesh.h"
#include <fstream>
#include <limits>
#include <string>
#include <algorithm>

// Consider a triangle to intersect a ray if the ray intersects the plane of the
// triangle with barycentric weights in [-weight_tolerance, 1+weight_tolerance]
static const double weight_tolerance = 1e-4;

Mesh::Mesh(const Parse* parse, std::istream& in)
{
    std::string file;
    in>>name>>file;
    Read_Obj(file.c_str());
}

// Read in a mesh from an obj file.  Populates the bounding box and registers
// one part per triangle (by setting number_parts).
void Mesh::Read_Obj(const char* file)
{
    std::ifstream fin(file);
    if(!fin)
    {
        exit(EXIT_FAILURE);
    }
    std::string line;
    ivec3 e, t;
    vec3 v;
    vec2 u;
    while(fin)
    {
        getline(fin,line);

        if(sscanf(line.c_str(), "v %lg %lg %lg", &v[0], &v[1], &v[2]) == 3)
        {
            vertices.push_back(v);
        }

        if(sscanf(line.c_str(), "f %d %d %d", &e[0], &e[1], &e[2]) == 3)
        {
            for(int i=0;i<3;i++) e[i]--;
            triangles.push_back(e);
        }

        if(sscanf(line.c_str(), "vt %lg %lg", &u[0], &u[1]) == 2)
        {
            uvs.push_back(u);
        }

        if(sscanf(line.c_str(), "f %d/%d %d/%d %d/%d", &e[0], &t[0], &e[1], &t[1], &e[2], &t[2]) == 6)
        {
            for(int i=0;i<3;i++) e[i]--;
            triangles.push_back(e);
            for(int i=0;i<3;i++) t[i]--;
            triangle_texture_index.push_back(t);
        }
    }
    num_parts=triangles.size();
}

// Check for an intersection against the ray.  See the base class for details.
Hit Mesh::Intersection(const Ray& ray, int part) const
{
    //TODO;
    int trsize = triangles.size();
    Hit retHit;
    if(part>=0)
    {
        /*Hit hit = Intersect_Triangle(ray, part);
        if((hit.dist>small_t) && (!std::isinf(hit.dist)))
        {
            Debug_Scope scope;
            Pixel_Print("Hit dist Empt: ", hit.dist, " Hit index: ", hit.triangle);
            return hit;
            
        }
        Debug_Scope scope1;
        Pixel_Print("Hit dist Empt2: ", hit.dist, " Hit index: ", hit.triangle);
        return retHit;*/
        for (int tidx = 0; tidx < part; tidx++)
        {
            Hit hit = Intersect_Triangle(ray, tidx);
            if ((hit.dist > small_t) && (!std::isinf(hit.dist)))
            {
                //Debug_Scope scope;
                //Pixel_Print("Hit dist Empt: ", hit.dist, " Hit index: ", hit.triangle);
                return hit;
            }
           // Debug_Scope scope1;
           // Pixel_Print("Hit dist Empt2: ", hit.dist, " Hit index: ", hit.triangle);
            return retHit;
        }


    }
    for(int tidx = 0; tidx < trsize; tidx++)
    {
        Hit hit = Intersect_Triangle(ray, tidx);

       // Debug_Scope scope2;
        //Pixel_Print("Hit dist: ", hit.dist, " Hit index: ", hit.triangle);
        if(retHit.dist<0)
        {
            retHit = hit;
        }

        else if((hit.dist<retHit.dist) && (hit.dist>small_t) && (!std::isinf(hit.dist)))
        {
            retHit = hit;
        }
    }

    return retHit;
}

// Compute the normal direction for the triangle with index part.
vec3 Mesh::Normal(const Ray& ray, const Hit& hit) const
{
    assert(hit.triangle>=0);
    //TODO;
    //vec3 p = ray.Point(hit.dist);

    vec3 a = vertices[triangles[hit.triangle][0]];
    
    vec3 b = vertices[triangles[hit.triangle][1]];
    
    vec3 c = vertices[triangles[hit.triangle][2]];

    vec3 crossed = cross((b-a),(c-a));

    return crossed.normalized();
}

// This is a helper routine whose purpose is to simplify the implementation
// of the Intersection routine.  It should test for an intersection between
// the ray and the triangle with index tri.  If an intersection exists,
// record the distance and return true.  Otherwise, return false.
// This intersection should be computed by determining the intersection of
// the ray and the plane of the triangle.  From this, determine (1) where
// along the ray the intersection point occurs (dist) and (2) the barycentric
// coordinates within the triangle where the intersection occurs.  The
// triangle intersects the ray if dist>small_t and the barycentric weights are
// larger than -weight_tolerance.  The use of small_t avoid the self-shadowing
// bug, and the use of weight_tolerance prevents rays from passing in between
// two triangles.
Hit Mesh::Intersect_Triangle(const Ray& ray, int tri) const
{
    //TODO;
    
   
    ivec3 triangle = triangles[tri];
    vec3 a = vertices[triangle[0]];
    vec3 b = vertices[triangle[1]];
    vec3 c = vertices[triangle[2]];

    ivec3 uv_triangle = triangle_texture_index[tri];

    vec2 a_uv = uvs[uv_triangle[0]];
    vec2 b_uv = uvs[uv_triangle[1]];
    vec2 c_uv = uvs[uv_triangle[2]];


    vec3 AB = b-a;
    vec3 AC = c-a;

    vec3 planenormal = cross(AB,AC).normalized();

    Hit empty = {-1,-1,{0,0}};
    //return empty;
    double udotn = dot(ray.direction, planenormal);
    double t;
    double hitdist;

    if(udotn ==0 )
    {
        return empty;
    }
    else
    {
        t = dot((a-ray.endpoint), planenormal);
        hitdist = t/udotn;
        
        if(hitdist < small_t)
        {
            return empty;
        }
    }
    vec3 P = ray.Point(hitdist);// intersection point ray-plane
    vec3 AP = P-a;
    vec3 BP = P-b;
    vec3 BC = c-b;
    //areas by 1/2
    double ABC =  cross(AB,AC).magnitude();
    double APB =  cross(AB,AP).magnitude();
    double BPC = cross(BP, BC).magnitude();
    double APC = cross(AP, AC).magnitude();

    double alpha = BPC/ABC;
    double beta = APC/ABC;
    double gamma = APB/ABC;

    if((alpha< -weight_tol) || (beta< -weight_tol) || (gamma< -weight_tol) || (alpha >1) || (beta >1) || (gamma > 1) || ((alpha + beta + gamma) <(1-weight_tol)) ||  ((alpha + beta + gamma) >(1+weight_tol)))
    {
        return empty;
    }

    double u_p = alpha*a_uv[0] + beta*b_uv[0] + gamma*c_uv[0];
    double v_p = alpha*a_uv[1] + beta*b_uv[1] + gamma*c_uv[1];
    
    Hit trihit = {hitdist,tri,{u_p,v_p}};

    //mesh M triangle 5 intersected; weights: (0.381336 0.202289 0.416375); dist 2.27676

    Debug_Scope scope6;
    Pixel_Print("Mesh : ", name,": triangle : ", tri, "; weights: ", "(", alpha, " ", beta, " ", gamma, "); dist ", hitdist );

    return trihit;

}

std::pair<Box,bool> Mesh::Bounding_Box(int part) const
{
    if(part<0)
    {
        Box box;
        box.Make_Empty();
        for(const auto& v:vertices)
            box.Include_Point(v);
        return {box,false};
    }

    ivec3 e=triangles[part];
    vec3 A=vertices[e[0]];
    Box b={A,A};
    b.Include_Point(vertices[e[1]]);
    b.Include_Point(vertices[e[2]]);
    return {b,false};
}
