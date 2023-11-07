#ifndef MATERIAL_H
#define MATERIAL_H

#pragma once

#include <glm/glm.hpp>
#include <string>

namespace boop {
    class Material
    {
    public:
        Material();
        ~Material();

        std::string name;

        glm::vec3 ambient_color{1.0f};
        glm::vec3 diffuse_color{1.0f};
        glm::vec3 specular_color{1.0f};

        std::string diffuse_texture_path;
        std::string specular_texture_path;
    };
}

#endif