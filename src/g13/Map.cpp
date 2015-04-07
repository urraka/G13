#include "Map.h"

#include <g13/res.h>
#include <g13/Camera.h>
#include <gfx/gfx.h>
#include <json/json.h>
#include <vector>

namespace g13 {

void Map::load(const Json::Value &data)
{
	initTextures(data);
	initGround(data);
	initGrass(data);
	initSky(data);
	initParallax(data);
}

void Map::initTextures(const Json::Value &data)
{
	groundTexture_.load("data/rock.png");
	groundTexture_.wrap(gfx::Repeat);
	groundTexture_.generateMipmap();

	grassTexture_.load("data/grass.png");
	grassTexture_.wrap(gfx::Clamp);
	grassTexture_.generateMipmap();

	backgroundNear_.load("data/background1.png");
	backgroundNear_.wrapX(gfx::Repeat);
	backgroundNear_.wrapY(gfx::Clamp);
	backgroundNear_.generateMipmap();

	backgroundFar_.load("data/background2.png");
	backgroundFar_.wrapX(gfx::Repeat);
	backgroundFar_.wrapY(gfx::Clamp);
	backgroundFar_.generateMipmap();
}

void Map::initGround(const Json::Value &data)
{
	const Json::Value &ground_vbo = data["ground"]["vbo"];
	const Json::Value &ground_ibo = data["ground"]["ibo"];

	std::vector<gfx::SpriteVertex> vertices(ground_vbo.size());
	std::vector<uint16_t> indices(ground_ibo.size());

	for (Json::ArrayIndex i = 0; i < ground_vbo.size(); i++)
	{
		float x = ground_vbo[i]["x"].asFloat();
		float y = ground_vbo[i]["y"].asFloat();
		float u = ground_vbo[i]["u"].asFloat();
		float v = ground_vbo[i]["v"].asFloat();

		vertices[i] = gfx::sprite_vertex(x, y, u, v, gfx::Color(255));
	}

	for (Json::ArrayIndex i = 0; i < ground_ibo.size(); i++)
		indices[i] = ground_ibo[i].asInt();

	ground_.vbo.allocate<gfx::SpriteVertex>(vertices.size(), gfx::Static);
	ground_.vbo.set(&vertices[0], 0, vertices.size());
	ground_.vbo.mode(gfx::Triangles);

	ground_.ibo.allocate(indices.size(), gfx::Static);
	ground_.ibo.set(&indices[0], 0, indices.size());
}

void Map::initGrass(const Json::Value &data)
{
	const Json::Value &grass_data = data["grass"];
	std::vector<gfx::Sprite> grassSprites(grass_data.size());

	for (Json::ArrayIndex i = 0; i < grass_data.size(); i++)
	{
		const Json::Value &sprite_data = grass_data[i];
		gfx::Sprite &sprite = grassSprites[i];

		sprite.position.x     = sprite_data["x"].asFloat();
		sprite.position.y     = sprite_data["y"].asFloat();
		sprite.width          = sprite_data["w"].asFloat();
		sprite.height         = sprite_data["h"].asFloat();
		sprite.rotation       = sprite_data["rotation"].asFloat();
		sprite.center.x       = sprite_data["cx"].asFloat();
		sprite.center.y       = sprite_data["cy"].asFloat();
		sprite.scale.x        = sprite_data["sx"].asFloat();
		sprite.scale.y        = sprite_data["sy"].asFloat();
		sprite.tx0.x          = sprite_data["u0"].asFloat();
		sprite.tx0.y          = sprite_data["v0"].asFloat();
		sprite.tx1.x          = sprite_data["u1"].asFloat();
		sprite.tx1.y          = sprite_data["v1"].asFloat();
		sprite.textureRotated = sprite_data["uvrot"].asBool();
	}

	grass_.resize(grassSprites.size(), gfx::Static);
	grass_.texture(&grassTexture_);
	grass_.add(&grassSprites[0], grassSprites.size());
}

void Map::initSky(const Json::Value &data)
{
	gfx::ColorVertex v[4];

	v[0] = v[1] = gfx::color_vertex(0.0f, 0.0f, gfx::Color(204, 238, 251));
	v[2] = v[3] = gfx::color_vertex(0.0f, 1.0f, gfx::Color(0x00, 0xAB, 0xEB));

	v[1].x = v[3].x = 1.0f;

	uint16_t indices[] = {0, 1, 2, 3};

	sky_.vbo.allocate<gfx::ColorVertex>(countof(v), gfx::Static);
	sky_.vbo.set(v, 0, countof(v));
	sky_.vbo.mode(gfx::TriangleStrip);

	sky_.ibo.allocate(countof(indices), gfx::Static);
	sky_.ibo.set(indices, 0, countof(indices));
}

void Map::initParallax(const Json::Value &data)
{
	gfx::Sprite sprites[2];

	const float W = data["width"].asFloat();
	const float H = data["height"].asFloat();
	const float L = -W / 2.0f;
	const float T = -H / 2.0f;
	const float R =  W / 2.0f;
	// const float B =  H / 2.0f;

	sprites[0].width = 2.0f * W;
	sprites[0].height = 2.0f * H;
	sprites[0].position.x = (L + R) / 2.0f - sprites[0].width / 2.0f;
	sprites[0].position.y = T + H * 0.35f;
	sprites[0].tx1.x = 0.5f * (sprites[0].width  / (float)backgroundNear_.width());
	sprites[0].tx1.y = 0.5f * (sprites[0].height / (float)backgroundNear_.height());

	sprites[1].width = 4.0f * W;
	sprites[1].height = 4.0f * H;
	sprites[1].position.x = (L + R) / 2.0f - sprites[1].width / 2.0f;
	sprites[1].position.y = T + H * 0.25f;
	sprites[1].tx1.x = 0.25f * (sprites[1].width  / (float)backgroundFar_.width());
	sprites[1].tx1.y = 0.25f * (sprites[1].height / (float)backgroundFar_.height());

	parallax_.resize(2, gfx::Static);
	parallax_.add(sprites[0]);
	parallax_.add(sprites[1]);
}

void Map::drawBackground(const Frame &frame, const Camera &camera)
{
	const mat2d &matrix = camera.matrix();
	const vec2 viewport = camera.viewport();

	gfx::matrix(mat2d::scale(viewport.x, viewport.y));
	gfx::draw(sky_.vbo);

	mat2d m0 = mat2d::translate(viewport.x / 2.0f, viewport.y / 2.0f) *
	           mat2d::scale(0.5f, 0.5f) *
	           mat2d::translate(-viewport.x / 2.0f, -viewport.y / 2.0f);

	mat2d m1 = mat2d::translate(viewport.x / 2.0f, viewport.y / 2.0f) *
	           mat2d::scale(0.25f, 0.25f) *
	           mat2d::translate(-viewport.x / 2.0f, -viewport.y / 2.0f);

	gfx::matrix(m1 * matrix);
	parallax_.texture(&backgroundFar_);
	gfx::draw(parallax_, 1, 1);

	gfx::matrix(m0 * matrix);
	parallax_.texture(&backgroundNear_);
	gfx::draw(parallax_, 0, 1);
}

void Map::drawForeground(const Frame &frame, const Camera &camera)
{
	gfx::matrix(camera.matrix());
	gfx::bind(groundTexture_);
	gfx::draw(ground_.vbo);
	gfx::draw(grass_);
}

} // g13
