#ifndef MATERIAL_HPP
#define MATERIAL_HPP
#include <QColor>
#include "Color.hpp"
#include "Texture.hpp"

class Material{
private:
public:
    Color m_ambient;
    Color m_diffuse;
    Color m_specular;
    double m_beta;
    double m_phong;
    double m_refract_amount;
    double m_refract_coef;
    Texture* m_texture;
    Material() = default;
    Material(const Color & ambient, const Color & diffuse, const Color & specular, const double & beta, const double & phong,
             const double & refract_amount, const double & refract_coef, const std::string & texture_filename = "")
        : m_ambient(ambient), m_diffuse(diffuse), m_specular(specular), m_beta(beta), m_phong(phong),
          m_refract_amount(refract_amount), m_refract_coef(refract_coef), m_texture(NULL)
    {
        if (texture_filename.length() > 0)
            m_texture = new Texture(texture_filename);
    }
    Color get_color(const double & x, const double & y){
        if (!m_texture)
            return Color(1, 1, 1);
        return m_texture->pixel(x, y);
    }
    virtual ~Material(){
        if (!m_texture)
            delete m_texture;
    }
};

#endif // MATERIAL_HPP
