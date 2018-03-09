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
	object->draw(shaderprogram, modelview);
}

void Geometry::update()
{

}