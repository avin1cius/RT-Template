#include "raytracer.h"

RayTracer::RayTracer( Camera &camera,
                      const Scene &scene,
                      const glm::vec3 background_color,
                      const size_t samples,
                      const size_t maximum_depth,
                      Buffer &buffer ) :
        camera_( camera ),
        scene_( scene ),
        background_color_{ background_color },
        samples_{ samples},
        maximum_depth_{ maximum_depth },
        buffer_( buffer )
{}

glm::vec3 RayTracer::L( Ray ray, size_t curr_depth ) //Rendering equation
{
    IntersectionRecord intersection_record;
    glm::vec3 Lo{ 0.0f, 0.0f, 0.0f };
    Ray refl_ray;
    


    intersection_record.t_ = std::numeric_limits< double >::max();

    if ( curr_depth < maximum_depth_ )
    {
        if ( scene_.intersect ( ray, intersection_record ))
        {


            if(intersection_record.material == 0){
            
            IntersectionPoint intersection_point{{0.0f, 0.0f, 0.0f}, intersection_record.color_, intersection_record.normal_};

           // intersection_point.fr = intersection_record.color_;
           // intersection_point.Le = {0.0f, 0.0f, 0.0f};
           // intersection_point.normal_ = intersection_record.normal_;

            refl_ray = intersection_point.get_new_ray( intersection_record );

            Lo = intersection_point.Le + 2.0f * ((float)M_PI) * intersection_point.fr *
            L( refl_ray, ++curr_depth ) * glm::dot( intersection_point.normal_, refl_ray.direction_ - refl_ray.origin_);
            
            }

            if(intersection_record.material == 1){

            IntersectionPoint intersection_point{{20.0f, 20.0f, 20.0f}, {1.0f, 1.0f, 1.0f}, intersection_record.normal_};

            //intersection_point.fr = {0.0f, 0.0f, 0.0f};
            //intersection_point.Le = {20.0f, 20.0f, 20.f};
            //intersection_point.normal_ = intersection_record.normal_;

            refl_ray = intersection_point.get_new_ray( intersection_record );

            Lo = intersection_point.Le + 2.0f * ((float) M_PI) * intersection_point.fr *
            L( refl_ray, ++curr_depth ) * glm::dot( intersection_point.normal_, (refl_ray.direction_ - refl_ray.origin_) );
            
            }

        }
    }
    return Lo;
}

void RayTracer::integrate( void )
{
    //IntersectionRecord intersection_record;

    //glm::vec3 final_color{ 0.0f, 0.0f, 0.0f };

    // Image space origin (i.e. x = 0 and y = 0) at the top left corner.

    // Loops over image rows
    for ( std::size_t y = 0; y < buffer_.v_resolution_; y++ )
    {
        std::stringstream progress_stream;
        progress_stream << "\r  progress .........................: "
                        << std::fixed << std::setw( 6 )
                        << std::setprecision( 2 )
                        << 100.0 * y / ( buffer_.v_resolution_ - 1 )
                        << "%";

        std::clog << progress_stream.str();

        srand(std::time(0));

        // Loops over image columns
        for ( std::size_t x = 0; x < buffer_.h_resolution_; x++ )
        {
            for(std::size_t i = 0; i < samples_; i++ )
            {   
                //intersection_record.t_ = std::numeric_limits< double >::max();

                float rand1 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);
                float rand2 = static_cast <float> (rand()) / static_cast <float> (RAND_MAX);

                //Ray ray{ camera_.getWorldSpaceRay( glm::vec2{ x + 0.5f, y + 0.5f } ) };
                Ray ray{ camera_.getWorldSpaceRay( glm::vec2{ x + rand1, y + rand2 } ) };

                //if ( scene_.intersect( ray, intersection_record ) )
                    //buffer_.buffer_data_[x][y] = glm::vec3{ intersection_record.t_ * 0.2f };
                    //final_color = final_color + intersection_record.color_;
                    buffer_.buffer_data_[x][y] += L( ray, 0 );            
            }

            //buffer_.buffer_data_[x][y] = glm::vec3{ final_color / static_cast <float> (samples_) };
            buffer_.buffer_data_[x][y] /= static_cast <float> (samples_);
            //final_color = { 0.0f, 0.0f, 0.0f };
        }
    }

    std::clog << std::endl;
}

