#include "Geometry.h"

Geometry::Geometry(GLint shaderprogram)
{
	this->shaderprogram = shaderprogram;
}

Geometry::~Geometry()
{
	delete(object);
}

void Geometry::init(char* filename)
{
	object = new OBJObject(filename);
}

void Geometry::draw(glm::mat4 C)
{
	modelview = C;
	//object->draw(shaderprogram, glm::vec3(0.0f, 1.0f, 0.0f), glm::vec3(1.0f, 1.0f, 1.0f), glm::vec3(0.0f, 0.0f, 1.0f), Window::cam_pos, glm::vec4(0.1f, 1.0f, 1.0f, 64.0f), modelview);
}

void Geometry::update()
{

}