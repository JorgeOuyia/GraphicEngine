#include "PhongLight.h"

PhongLight::PhongLight(DirectionalLight dirLight, SpecularLight specLight, SpotLight spotLight) : dirLight(dirLight), specLight(specLight), spotLight(spotLight)
{

}

PhongLight::~PhongLight()
{
}