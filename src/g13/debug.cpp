#ifdef DEBUG

#include <g13/g13.h>
#include <gfx/gfx.h>
#include <vector>
#include <sstream>
#include <iomanip>

#include "res.h"
#include "Map.h"
#include "ent/Soldier.h"
#include "net/Multiplayer.h"
#include "net/Player.h"

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
		showStateBuffers(false),
		collisionHulls(),
		consoleText_(0),
		playersState_(0)
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

	if (playersState_)
		delete playersState_;
}

void Debugger::loadCollisionHulls()
{
	// if (collisionHulls[0])
	// {
	// 	delete collisionHulls[0]->ibo();
	// 	delete collisionHulls[0];
	// }

	// if (collisionHulls[1])
	// {
	// 	delete collisionHulls[1]->ibo();
	// 	delete collisionHulls[1];
	// }

	// collisionHulls[0] = 0;
	// collisionHulls[1] = 0;

	// if (!soldier || !map)
	// 	return;

	// const std::vector<const coll::Segment*> &segments = map->world()->retrieve(fixrect());

	// std::vector<gfx::ColorVertex> vert;
	// std::vector<uint16_t> indices;

	// vert.reserve(nodes.size() * 4);
	// indices.reserve(nodes.size() * 6);

	// fixrect bboxes[] = { soldier->physics.bboxNormal, soldier->physics.bboxDucked };

	// for (int k = 0; k < 2; k++)
	// {
	// 	fixrect bbox = bboxes[k];

	// 	vert.resize(0);
	// 	indices.resize(0);

	// 	gfx::ColorVertex vertex;
	// 	vertex.r = 255;
	// 	vertex.g = 0;
	// 	vertex.b = 0;
	// 	vertex.a = 255;

	// 	for (size_t i = 0; i < nodes.size(); i++)
	// 	{
	// 		Collision::Hull hull = Collision::createHull(nodes[i], bbox);

	// 		uint16_t index = vert.size();

	// 		vertex.x = hull.nodes[0].line.p1.x.to_float();
	// 		vertex.y = hull.nodes[0].line.p1.y.to_float();
	// 		vert.push_back(vertex);

	// 		vertex.x = hull.nodes[0].line.p2.x.to_float();
	// 		vertex.y = hull.nodes[0].line.p2.y.to_float();
	// 		vert.push_back(vertex);

	// 		indices.push_back(index);
	// 		indices.push_back(index + 1);

	// 		bool prev = hull.nodes[1].line.p1 != hull.nodes[1].line.p2;
	// 		bool next = hull.nodes[2].line.p1 != hull.nodes[2].line.p2;

	// 		if (prev)
	// 		{
	// 			vertex.x = hull.nodes[1].line.p1.x.to_float();
	// 			vertex.y = hull.nodes[1].line.p1.y.to_float();
	// 			vert.push_back(vertex);
	// 			indices.push_back(index);
	// 			indices.push_back(index + 2);
	// 		}

	// 		if (next)
	// 		{
	// 			vertex.x = hull.nodes[2].line.p2.x.to_float();
	// 			vertex.y = hull.nodes[2].line.p2.y.to_float();
	// 			vert.push_back(vertex);
	// 			indices.push_back(index + 1);
	// 			indices.push_back(index + 2 + (uint16_t)prev);
	// 		}
	// 	}

	// 	gfx::IBO *ibo = new gfx::IBO(indices.size(), gfx::Static);
	// 	ibo->set(indices.data(), 0, indices.size());

	// 	gfx::VBO *vbo = new gfx::VBO(ibo);
	// 	vbo->allocate<gfx::ColorVertex>(vert.size(), gfx::Static);
	// 	vbo->set(vert.data(), 0, vert.size());
	// 	vbo->mode(gfx::Lines);

	// 	collisionHulls[k] = vbo;
	// }
}

void Debugger::drawCollisionHulls()
{
	if (showCollisionHulls && soldier && collisionHulls[0] && collisionHulls[1])
		gfx::draw(collisionHulls[(int)soldier->physics.ducking()]);
}

void Debugger::showCollisionData()
{
	// if (!map)
	// 	return;

	// std::cout << std::endl;
	// std::cout << "Collision map data: " << std::endl << std::endl;
	// std::stringstream s;

	// std::cout << std::setw(4)  << std::left << " ";
	// std::cout << std::setw(12) << std::left << "P1";
	// std::cout << std::setw(12) << std::left << "P2";
	// std::cout << std::setw(16) << std::left << "Normal";
	// std::cout << std::setw(7)  << std::left << "Floor";
	// std::cout << std::setw(12) << std::left << "Hull[0].p1";
	// std::cout << std::setw(12) << std::left << "Hull[0].p2";
	// std::cout << std::endl;

	// fixrect bbox = fixrect(fixed(-17), fixed(-66), fixed(17), fixed(0));

	// const std::vector<const Collision::Node*> &nodes = map->collisionMap()->retrieve(fixrect());

	// for (size_t i = 0; i < nodes.size(); i++)
	// {
	// 	Collision::Hull hull = Collision::createHull(nodes[i], bbox);
	// 	fixvec2 normal = fpm::normal(nodes[i]->line);

	// 	s.str(std::string());
	// 	s.clear();
	// 	s << "#" << i;
	// 	std::cout << std::setw(4) << std::left << s.str();

	// 	s.precision(0);

	// 	s.str(std::string());
	// 	s.clear();
	// 	s << "(" << nodes[i]->line.p1.x << "," << nodes[i]->line.p1.y << ")";
	// 	std::cout << std::setw(12) << std::left << s.str();

	// 	s.str(std::string());
	// 	s.clear();
	// 	s << "(" << nodes[i]->line.p2.x << "," << nodes[i]->line.p2.y << ")";
	// 	std::cout << std::setw(12) << std::left << s.str();

	// 	s.str(std::string());
	// 	s.clear();
	// 	s.precision(3);
	// 	s << "(" << normal.x << "," << normal.y << ")";
	// 	std::cout << std::setw(16) << std::left << s.str();

	// 	s.str(std::string());
	// 	s.clear();
	// 	s << (nodes[i]->floor ? "true" : "false");
	// 	std::cout << std::setw(7) << std::left << s.str();

	// 	s.precision(0);

	// 	s.str(std::string());
	// 	s.clear();
	// 	s << "(" << hull.nodes[0].line.p1.x << "," << hull.nodes[0].line.p1.y << ")";
	// 	std::cout << std::setw(12) << std::left << s.str();

	// 	s.str(std::string());
	// 	s.clear();
	// 	s << "(" << hull.nodes[0].line.p2.x << "," << hull.nodes[0].line.p2.y << ")";
	// 	std::cout << std::setw(12) << std::left << s.str();

	// 	std::cout << std::endl;
	// }

	// std::cout << std::endl;
}

void Debugger::drawFontAtlas()
{
	if (!showFontAtlas)
		return;

	// gfx::Font *font = res::font(res::Monospace);
	gfx::Font *font = res::font(res::DefaultFont);

	if (font->texture(0) != 0)
	{
		gfx::Sprite sprite;
		sprite.texture = font->texture(0);
		sprite.width = sprite.texture->width();
		sprite.height = sprite.texture->height();
		// sprite.color.a = 127;

		gfx::matrix(mat2d());
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

		text += "CTRL+C: Start client\n";
		text += "CTRL+S: Start server\n";
		text += "     W: Toggle wireframe\n";
		text += "     H: Toggle collision hulls\n";
		text += "     L: Toggle game state bars\n";
		text += "    F6: Toggle FPS\n";

		consoleText_ = new gfx::Text();
		consoleText_->size(9);
		consoleText_->font(res::font(res::Monospace));
		consoleText_->value(text.c_str());
	}

	gfx::matrix(mat2d());
	gfx::translate(10.0f, 20.0f);
	gfx::draw(consoleText_);
}

void Debugger::drawStateBuffers(int tick, int interp, int local, const net::Player *players)
{
	if (!showStateBuffers)
		return;

	const int nTicks = 40;
	const int vpp = nTicks * 2 + 2; // vertices per player

	if (!playersState_)
	{
		playersState_ = new gfx::VBO();
		playersState_->allocate<gfx::ColorVertex>(net::Multiplayer::MaxPlayers * vpp, gfx::Dynamic);
		playersState_->mode(gfx::Lines);
	}

	gfx::ColorVertex vertices[vpp];

	const float padding = 20.0f;
	const float L = padding;
	const float R = sys::framebuffer_width() - padding;

	int count = 0;

	for (int i = 0; i < net::Multiplayer::MaxPlayers; i++)
	{
		if (players[i].state() != net::Player::Playing)
			continue;

		const int N = players[i].stateBuffer_.size();
		const float y = padding + count * padding;
		const gfx::Color color = (i == local) ? gfx::Color(0, 200) : gfx::Color(0xCC, 200);

		int mintick = N > 0 ? players[i].stateBuffer_[0].tick : 0;
		int maxtick = N > 0 ? players[i].stateBuffer_[0].tick : 0;

		for (int j = 1; j < (int)N; j++)
		{
			mintick = std::min(mintick, players[i].stateBuffer_[j].tick);
			maxtick = std::max(maxtick, players[i].stateBuffer_[j].tick);
		}

		vertices[0] = gfx::color_vertex(L, y, color);
		vertices[1] = gfx::color_vertex(R, y, color);

		for (int j = 0; j < nTicks; j++)
		{
			const int t = tick - (nTicks - j - 1);
			const float hh = (t == mintick || t == maxtick) ? 6.0f : 3.0f;
			const float x = L + (R - L) * j / float(nTicks - 1);

			vertices[2 + 2*j + 0] = gfx::color_vertex(x, y - hh, color);
			vertices[2 + 2*j + 1] = gfx::color_vertex(x, y + hh, color);
		}

		playersState_->set(vertices, count * vpp, vpp);

		count++;
	}

	if (count == 0)
		return;

	gfx::Sprite bg;
	bg.texture = res::texture(res::Soldier);
	bg.tx0 = bg.tx1 = vec2(100.0f / bg.texture->width(), 100.0f / bg.texture->height());
	bg.position = vec2(L - 10.0f, padding - 10.0f);
	bg.width = R - L + padding;
	bg.height = count * padding;
	bg.color = gfx::Color(255, 200);

	gfx::Sprite mark = bg;
	mark.position.x = L + (R - L) * (nTicks - 1 - interp) / float(nTicks - 1) - 4.0f;
	mark.position.y = padding - 13.0f;
	mark.width = 8.0f;
	mark.height = count * padding + 6;
	mark.color = gfx::Color(255, 0, 0, 128);

	const float lineWidth = gfx::line_width();

	gfx::line_width(2.0f);
	gfx::matrix(mat2d());
	gfx::draw(bg);
	gfx::draw(playersState_, count * vpp);
	gfx::draw(mark);
	gfx::line_width(lineWidth);
}

bool Debugger::event(sys::Event *evt)
{
	if (evt->type == Event::KeyPressed)
		return onKeyPressed(evt->key.code);

	return true;
}

bool Debugger::onKeyPressed(int key)
{
	switch (key)
	{
		case sys::F1:
			showFPS = !showFPS;
			break;

		case sys::F2:
			wireframe = !wireframe;
			break;

		case sys::F3:
			showStateBuffers = !showStateBuffers;
			break;

		case sys::F4:
			showCollisionHulls = !showCollisionHulls;
			break;

		default:
			break;
	}

	return true;
}

} // g13

#endif
