#ifdef DEBUG

#include "Debugger.h"
#include "../Graphics/Graphics.h"
#include "Game.h"
#include "Entities/Soldier.h"
#include "Map.h"
#include "Collision.h"

#include <vector>
#include <iostream>
#include <sstream>
#include <iomanip>


Debugger *dbg = 0;

Debugger::Debugger()
	:	map(0),
		soldier(0),
		graphics(0),
		showCollisionHulls(false),
		showFPS(false),
		stepMode(false),
		wireframe(false),
		interpolation(true),
		collisionHulls()
{
}

void Debugger::loadCollisionHulls()
{
	const std::vector<const Collision::Node*> &nodes = map->collisionMap()->retrieve(fixrect());

	std::vector<ColorVertex> vert;
	std::vector<uint16_t> indices;

	vert.reserve(nodes.size() * 4);
	indices.reserve(nodes.size() * 6);

	fixrect bboxes[] = { soldier->physics.bboxNormal, soldier->physics.bboxDucked };

	for (int k = 0; k < 2; k++)
	{
		fixrect bbox = bboxes[k];

		vert.resize(0);
		indices.resize(0);

		ColorVertex vertex;
		vertex.color = u8vec4(255, 255, 0, 255);

		for (size_t i = 0; i < nodes.size(); i++)
		{
			Collision::Hull hull = Collision::createHull(nodes[i], bbox);

			uint16_t index = vert.size();

			vertex.position.x = hull.nodes[0].line.p1.x.to_float();
			vertex.position.y = hull.nodes[0].line.p1.y.to_float();
			vert.push_back(vertex);

			vertex.position.x = hull.nodes[0].line.p2.x.to_float();
			vertex.position.y = hull.nodes[0].line.p2.y.to_float();
			vert.push_back(vertex);

			indices.push_back(index);
			indices.push_back(index + 1);

			bool prev = hull.nodes[1].line.p1 != hull.nodes[1].line.p2;
			bool next = hull.nodes[2].line.p1 != hull.nodes[2].line.p2;

			if (prev)
			{
				vertex.position.x = hull.nodes[1].line.p1.x.to_float();
				vertex.position.y = hull.nodes[1].line.p1.y.to_float();
				vert.push_back(vertex);
				indices.push_back(index);
				indices.push_back(index + 2);
			}

			if (next)
			{
				vertex.position.x = hull.nodes[2].line.p2.x.to_float();
				vertex.position.y = hull.nodes[2].line.p2.y.to_float();
				vert.push_back(vertex);
				indices.push_back(index + 1);
				indices.push_back(index + 2 + (uint16_t)prev);
			}
		}

		VBO<ColorVertex> *buffer = graphics->buffer<ColorVertex>(vbo_t::Lines, vbo_t::StaticDraw, vbo_t::StaticDraw, vert.size(), indices.size());
		buffer->set(vert.data(), 0, vert.size());
		buffer->set(indices.data(), 0, indices.size());

		collisionHulls[k] = buffer;
	}
}

void Debugger::drawCollisionHulls()
{
	if (showCollisionHulls)
		graphics->draw(collisionHulls[(int)soldier->physics.ducking()]);
}

void Debugger::showCollisionData()
{
	std::cout << std::endl;
	std::cout << "Collision map data: " << std::endl << std::endl;
	std::stringstream s;

	std::cout << std::setw(4)  << std::left << " ";
	std::cout << std::setw(12) << std::left << "P1";
	std::cout << std::setw(12) << std::left << "P2";
	std::cout << std::setw(16) << std::left << "Normal";
	std::cout << std::setw(7)  << std::left << "Floor";
	std::cout << std::setw(12) << std::left << "Hull[0].p1";
	std::cout << std::setw(12) << std::left << "Hull[0].p2";
	std::cout << std::endl;

	fixrect bbox = fixrect(fixed(-17), fixed(-66), fixed(17), fixed(0));

	const std::vector<const Collision::Node*> &nodes = map->collisionMap()->retrieve(fixrect());

	for (size_t i = 0; i < nodes.size(); i++)
	{
		Collision::Hull hull = Collision::createHull(nodes[i], bbox);
		fixvec2 normal = fpm::normal(nodes[i]->line);

		s.str(std::string());
		s.clear();
		s << "#" << i;
		std::cout << std::setw(4) << std::left << s.str();

		s.precision(0);

		s.str(std::string());
		s.clear();
		s << "(" << nodes[i]->line.p1.x << "," << nodes[i]->line.p1.y << ")";
		std::cout << std::setw(12) << std::left << s.str();

		s.str(std::string());
		s.clear();
		s << "(" << nodes[i]->line.p2.x << "," << nodes[i]->line.p2.y << ")";
		std::cout << std::setw(12) << std::left << s.str();

		s.str(std::string());
		s.clear();
		s.precision(3);
		s << "(" << normal.x << "," << normal.y << ")";
		std::cout << std::setw(16) << std::left << s.str();

		s.str(std::string());
		s.clear();
		s << (nodes[i]->floor ? "true" : "false");
		std::cout << std::setw(7) << std::left << s.str();

		s.precision(0);

		s.str(std::string());
		s.clear();
		s << "(" << hull.nodes[0].line.p1.x << "," << hull.nodes[0].line.p1.y << ")";
		std::cout << std::setw(12) << std::left << s.str();

		s.str(std::string());
		s.clear();
		s << "(" << hull.nodes[0].line.p2.x << "," << hull.nodes[0].line.p2.y << ")";
		std::cout << std::setw(12) << std::left << s.str();

		std::cout << std::endl;
	}

	std::cout << std::endl;
}

void Debugger::onKeyPressed(Keyboard::Key key)
{
	switch (key)
	{
		case Keyboard::W:
			wireframe = !wireframe;
			break;

		case Keyboard::F6:
			showFPS = !showFPS;
			break;

		case Keyboard::I:
			interpolation = !interpolation;
			break;

		default: break;
	}
}

#endif
