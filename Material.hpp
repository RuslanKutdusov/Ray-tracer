#ifndef MATERIAL_HPP
#define MATERIAL_HPP
#include "cuda.hpp"
#include "Color.hpp"
//#include "Texture.hpp"

class Material{
private:
public:
    Color m_ambient;
    Color m_diffuse;
    Color m_specular;
    float m_beta;
    float m_phong;
    float m_refract_amount;
    float m_refract_coef;
    //Texture* m_texture;
    HOST_DEVICE Material()
    {}
    HOST_DEVICE Material(const Color & ambient, const Color & diffuse, const Color & specular, const float & beta, const float & phong,
             const float & refract_amount, const float & refract_coef )//, const std::string & texture_filename = "")
        : m_ambient(ambient), m_diffuse(diffuse), m_specular(specular), m_beta(beta), m_phong(phong),
          m_refract_amount(refract_amount), m_refract_coef(refract_coef)//, m_texture(NULL)
    {
        //if (texture_filename.length() > 0)
        //    m_texture = new Texture(texture_filename);
    }
    HOST_DEVICE Color get_color(const float & x, const float & y) const{
        //if (!m_texture)
            return Color(1, 1, 1);
        //return m_texture->pixel(x, y);
    }
    HOST_DEVICE ~Material(){
        //if (!m_texture)
        //    delete m_texture;
    }
};

#endif // MATERIAL_HPP
