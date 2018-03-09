#include "Transform.h"
#include <vector>

Transform::Transform(glm::mat4 M, bool animated)
{
	this->M = M;
	A = glm::mat4(1.0f);
	this->animated = animated;
	angle = 0;
}

Transform::~Transform()
{
	for (Node* c : children)
	{
		removeChild(c);
		delete(c);
	}
}

void Transform::addChild(Node* child)
{
	children.push_back(child);
}

void Transform::removeChild(Node* child)
{
	children.remove(child);
}

void Transform::draw(glm::mat4 C)
{
	for (Node* c : children)
	{
		if (animated)
		{
			c->draw(C * M * A);
		}
		else
		{
			c->draw(C * M);
		}
	}
}

void Transform::update()
{
	if (animated)
	{
		A = glm::translate(glm::mat4(1.0f), glm::vec3(-26.75f, 0.0f, 45.0f)) * glm::rotate(glm::mat4(1.0f), glm::cos(glm::radians(angle)) * (glm::pi<float>() / 4.0f), glm::vec3(1.0f, 0.0f, 0.0f)) * glm::translate(glm::mat4(1.0f), glm::vec3(26.75f, 0.0f, -45.0f));
		angle += 0.5f;
		if (angle >= 360 || angle < 0) angle = 0;
	}

	for (Node* c : children)
	{
		c->update();
	}
}