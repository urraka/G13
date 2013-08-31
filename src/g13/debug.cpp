#ifdef DEBUG

#include <g13/g13.h>
#include <gfx/gfx.h>
#include <vector>
#include <sstream>
#include <iomanip>

#include "res.h"
#include "Map.h"
#include "Collision.h"
#include "ent/Soldier.h"

namespace g13 {

Debugger::Debugger()
	:	map(0),
		soldier(0),
		showCollisionHulls(false),
		showFPS(false),
		stepMode(false),
		wireframe(false),
		interpolation(true),
		extrapolation(false),
		ticksBehind(4),
		showFontAtlas(false),
		consoleEnabled(false),
		collisionHulls(),
		consoleText_(0)
{
}

Debugger::~Debugger()
{
	if (collisionHulls[0])
	{
		delete collisionHulls[0]->ibo();
		delete collisionHulls[0];
	}

	if (collisionHulls[1])
	{
		delete collisionHulls[1]->ibo();
		delete collisionHulls[1];
	}
}

void Debugger::loadCollisionHulls()
{
	if (collisionHulls[0])
	{
		delete collisionHulls[0]->ibo();
		delete collisionHulls[0];
	}

	if (collisionHulls[1])
	{
		delete collisionHulls[1]->ibo();
		delete collisionHulls[1];
	}

	collisionHulls[0] = 0;
	collisionHulls[1] = 0;

	if (!soldier || !map)
		return;

	const std::vector<const Collision::Node*> &nodes = map->collisionMap()->retrieve(fixrect());

	std::vector<gfx::ColorVertex> vert;
	std::vector<uint16_t> indices;

	vert.reserve(nodes.size() * 4);
	indices.reserve(nodes.size() * 6);

	fixrect bboxes[] = { soldier->physics.bboxNormal, soldier->physics.bboxDucked };

	for (int k = 0; k < 2; k++)
	{
		fixrect bbox = bboxes[k];

		vert.resize(0);
		indices.resize(0);

		gfx::ColorVertex vertex;
		vertex.r = 255;
		vertex.g = 0;
		vertex.b = 0;
		vertex.a = 255;

		for (size_t i = 0; i < nodes.size(); i++)
		{
			Collision::Hull hull = Collision::createHull(nodes[i], bbox);

			uint16_t index = vert.size();

			vertex.x = hull.nodes[0].line.p1.x.to_float();
			vertex.y = hull.nodes[0].line.p1.y.to_float();
			vert.push_back(vertex);

			vertex.x = hull.nodes[0].line.p2.x.to_float();
			vertex.y = hull.nodes[0].line.p2.y.to_float();
			vert.push_back(vertex);

			indices.push_back(index);
			indices.push_back(index + 1);

			bool prev = hull.nodes[1].line.p1 != hull.nodes[1].line.p2;
			bool next = hull.nodes[2].line.p1 != hull.nodes[2].line.p2;

			if (prev)
			{
				vertex.x = hull.nodes[1].line.p1.x.to_float();
				vertex.y = hull.nodes[1].line.p1.y.to_float();
				vert.push_back(vertex);
				indices.push_back(index);
				indices.push_back(index + 2);
			}

			if (next)
			{
				vertex.x = hull.nodes[2].line.p2.x.to_float();
				vertex.y = hull.nodes[2].line.p2.y.to_float();
				vert.push_back(vertex);
				indices.push_back(index + 1);
				indices.push_back(index + 2 + (uint16_t)prev);
			}
		}

		gfx::IBO *ibo = new gfx::IBO(indices.size(), gfx::Static);
		ibo->set(indices.data(), 0, indices.size());

		gfx::VBO *vbo = new gfx::VBO(ibo);
		vbo->allocate<gfx::ColorVertex>(vert.size(), gfx::Static);
		vbo->set(vert.data(), 0, vert.size());
		vbo->mode(gfx::Lines);

		collisionHulls[k] = vbo;
	}
}

void Debugger::drawCollisionHulls()
{
	if (showCollisionHulls && soldier && collisionHulls[0] && collisionHulls[1])
		gfx::draw(collisionHulls[(int)soldier->physics.ducking()]);
}

void Debugger::showCollisionData()
{
	if (!map)
		return;

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

void Debugger::drawFontAtlas()
{
	if (!showFontAtlas)
		return;

	gfx::Font *font = res::font(res::Monospace);
	// gfx::Font *font = res::font(res::DefaultFont);

	if (font->texture(0) != 0)
	{
		gfx::Sprite sprite;
		sprite.texture = font->texture(0);
		sprite.width = sprite.texture->width();
		sprite.height = sprite.texture->height();
		// sprite.color.a = 127;

		gfx::matrix(mat4(1.0f));
		gfx::draw(sprite);
	}
}

void Debugger::drawConsole()
{
	if (!consoleEnabled)
		return;

	if (consoleText_ == 0)
	{
		std::string text;

		text += "C: Start client\n";
		text += "S: Start server\n";
		text += "F: Toggle font atlas\n";
		text += "W: Toggle wireframe\n";
		text += "H: Toggle collision hulls\n";
		text += "I: Toggle network interpolation\n";
		text += "O: Toggle network extrapolation\n";
		text += "E: Increment ticksBehind\n";
		text += "Q: Decrement ticksBehind\n";
		text += "M: Show collision data\n";

		consoleText_ = new gfx::Text();
		consoleText_->size(9);
		consoleText_->font(res::font(res::Monospace));
		consoleText_->value(text.c_str());
	}

	gfx::matrix(mat4(1.0f));
	gfx::translate(10.0f, 20.0f);
	gfx::draw(consoleText_);
}

bool Debugger::event(sys::Event *evt)
{
	if (evt->type == Event::KeyPressed)
		return onKeyPressed(evt->key.code);

	return true;
}

bool Debugger::onKeyPressed(int key)
{
	if (key == sys::F1 || (consoleEnabled && key == sys::Escape))
	{
		consoleEnabled = !consoleEnabled;
		return false;
	}

	if (!consoleEnabled)
		return true;

	bool result = false;

	switch (key)
	{
		case 'W':
			wireframe = !wireframe;
			break;

		case sys::F6:
			showFPS = !showFPS;
			break;

		case 'I':
			interpolation = !interpolation;
			debug_log("Interpolation " << (interpolation ? "enabled." : "disabled."));
			break;

		case 'O':
			extrapolation = !extrapolation;
			debug_log("Extrapolation " << (extrapolation ? "enabled." : "disabled."));
			break;

		case 'Q':
			ticksBehind--;
			debug_log("ticksBehind = " << ticksBehind);
			break;

		case 'E':
			ticksBehind++;
			debug_log("ticksBehind = " << ticksBehind);
			break;

		case 'H':
			showCollisionHulls = !showCollisionHulls;
			break;

		case 'M':
			dbg->showCollisionData();
			break;

		case 'F':
			showFontAtlas = !showFontAtlas;
			break;

		case 'S':
		case 'C':
			result = true;
			break;

		default:
			break;
	}

	return result;
}

} // g13

#endif
