#include "SoldierGraphics.h"
#include "SoldierInput.h"
#include "SoldierPhysics.h"

SoldierGraphics::SoldierGraphics()
{
	vec2 tex0(1.0f, 1.0f);
	vec2 tex1 = tex0 + vec2(52.0f, 82.0f);
	vec2 texSize(375.0f, 82.0f);

	sprite.size = vec2(52.0f, 82.0f);
	sprite.center = vec2(26.0f, 78.0f);
	sprite.texcoords = vec4(vec2(tex0 / texSize), vec2(tex1 / texSize));
}

void SoldierGraphics::update(Time dt)
{
	position.update();
	position.current = math::from_fixed(physics->position);

	if (input->left)  sprite.scale.x = 1.f;
	if (input->right) sprite.scale.x = -1.f;
}

void SoldierGraphics::frame(float percent)
{
	sprite.position = position.value(percent);
}
