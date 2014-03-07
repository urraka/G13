#pragma once

#include <g13/g13.h>
#include <gfx/VBO.h>
#include <gfx/IBO.h>
#include <gfx/SpriteBatch.h>
#include <gfx/Texture.h>

namespace g13 {

class Map
{
public:
	void load(const Json::Value &data);
	void drawBackground(const Frame &frame, const Camera &camera);
	void drawForeground(const Frame &frame, const Camera &camera);

protected:
	struct Mesh
	{
		Mesh() : vbo(&ibo) {}
		gfx::VBO vbo;
		gfx::IBO ibo;
	};

	Mesh sky_;
	Mesh ground_;

	gfx::SpriteBatch parallax_;
	gfx::SpriteBatch grass_;

	gfx::Texture groundTexture_;
	gfx::Texture grassTexture_;
	gfx::Texture backgroundNear_;
	gfx::Texture backgroundFar_;

	void initTextures(const Json::Value &data);
	void initGround(const Json::Value &data);
	void initGrass(const Json::Value &data);
	void initSky(const Json::Value &data);
	void initParallax(const Json::Value &data);
};

} // g13
