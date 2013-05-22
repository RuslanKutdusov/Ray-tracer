#ifndef MATERIAL_HPP
#define MATERIAL_HPP
#include <QColor>
#include "Color.hpp"

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
    Material() = default;
    Material(const Color & ambient, const Color & diffuse, const Color & specular, const double & beta, const double & phong,
              const double & refract_amount, const double & refract_coef  )
        : m_ambient(ambient), m_diffuse(diffuse), m_specular(specular), m_beta(beta), m_phong(phong),
          m_refract_amount(refract_amount), m_refract_coef(refract_coef)
    {}
};

#endif // MATERIAL_HPP
