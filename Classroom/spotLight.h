//
//  spotLight.h
//  3D Classroom
//
//  Written in the same style as the course's pointLight.h so it plugs into
//  the same Shader / uniform pattern, just for a cone-shaped spotlight.
//

#ifndef spotLight_h
#define spotLight_h

#include <glad/glad.h>
#include <glm/glm.hpp>
#include "shader.h"

class SpotLight {
public:
    glm::vec3 position;
    glm::vec3 direction;
    glm::vec3 ambient;
    glm::vec3 diffuse;
    glm::vec3 specular;
    float k_c;
    float k_l;
    float k_q;
    float cutOff;       // cosine of the inner cone angle
    float outerCutOff;  // cosine of the outer cone angle (soft edge)

    SpotLight(float posX, float posY, float posZ, float dirX, float dirY, float dirZ,
        float ambR, float ambG, float ambB,
        float diffR, float diffG, float diffB,
        float specR, float specG, float specB,
        float constant, float linear, float quadratic,
        float innerAngleDeg, float outerAngleDeg)
    {
        position = glm::vec3(posX, posY, posZ);
        direction = glm::vec3(dirX, dirY, dirZ);
        ambient = glm::vec3(ambR, ambG, ambB);
        diffuse = glm::vec3(diffR, diffG, diffB);
        specular = glm::vec3(specR, specG, specB);
        k_c = constant;
        k_l = linear;
        k_q = quadratic;
        cutOff = cos(glm::radians(innerAngleDeg));
        outerCutOff = cos(glm::radians(outerAngleDeg));
    }

    void setUpSpotLight(Shader& lightingShader)
    {
        lightingShader.use();

        lightingShader.setVec3("spotLight.position", position);
        lightingShader.setVec3("spotLight.direction", direction);
        lightingShader.setVec3("spotLight.ambient", onOff * ambient);
        lightingShader.setVec3("spotLight.diffuse", onOff * diffuse);
        lightingShader.setVec3("spotLight.specular", onOff * specular);
        lightingShader.setFloat("spotLight.k_c", k_c);
        lightingShader.setFloat("spotLight.k_l", k_l);
        lightingShader.setFloat("spotLight.k_q", k_q);
        lightingShader.setFloat("spotLight.cutOff", cutOff);
        lightingShader.setFloat("spotLight.outerCutOff", outerCutOff);
    }

    void turnOn()
    {
        onOff = 1.0f;
    }
    void turnOff()
    {
        onOff = 0.0f;
    }

private:
    float onOff = 1.0f;
};

#endif /* spotLight_h */
