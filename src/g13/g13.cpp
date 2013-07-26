#include "g13.h"
#include "res.h"
#include "stt/State.h"
#include "stt/Multiplayer.h"

#include <gfx/gfx.h>
#include <enet/enet.h>

namespace g13 {

#ifdef DEBUG
	Debugger *dbg = 0;
#endif

static stt::State *state = 0;

static Time time         = 0;
static Time accumulator  = 0;
static Time fps_time     = 0;
static int  fps_count    = 0;

static const Time dt        = sys::time<sys::Milliseconds>(30);
static const Time sec       = sys::time<sys::Seconds>(1);
static const Time frame_max = sys::time<sys::Milliseconds>(250);

// -----------------------------------------------------------------------------
// Initialize
// -----------------------------------------------------------------------------

void initialize()
{
	#ifdef DEBUG
		dbg = new Debugger();
	#endif

	sys::samples(4);
	sys::fullscreen(false);
	//sys::cursor_mode(sys::Disabled);
	sys::window_title("G13");
	sys::window_size(0.4f, 0.4f);
	sys::window_position(0.25f, 0.25f);
	sys::vsync(1);

	sys::initialize();
	gfx::initialize();
	res::initialize();

	int width;
	int height;

	sys::window_size(&width, &height);
	gfx::viewport(width, height, sys::window_rotation());

	if (enet_initialize() != 0)
		error_log("Failed to initialize enet.");

	state = new stt::Multiplayer();
	time = sys::time();
}

// -----------------------------------------------------------------------------
// Terminate
// -----------------------------------------------------------------------------

void terminate()
{
	#ifdef DEBUG
		delete dbg;
	#endif

	if (state != 0)
		delete state;

	res::terminate();
	gfx::terminate();
	enet_deinitialize();
}

// -----------------------------------------------------------------------------
// Display
// -----------------------------------------------------------------------------

void display()
{
	while (Event *event = sys::poll_events())
	{
		if (!state->event(event))
			continue;

		switch (event->type)
		{
			case Event::Resized:
				gfx::viewport(event->size.fboWidth, event->size.fboHeight, event->size.rotation);
				break;

			#ifdef DEBUG
				case Event::KeyPressed:
					dbg->onKeyPressed(event->key.code);
					break;
			#endif

			case Event::Closed:
				return;

			default: break;
		}
	}

	state->draw(accumulator / (double)dt);

	Time newTime = sys::time();
	Time frameTime = newTime - time;
	Time frozenTime = 0;

	fps_time += frameTime;

	if (fps_time >= sec)
	{
		#ifdef DEBUG
			if (dbg->showFPS)
				debug_log("FPS: " << fps_count << " - Frame time: " << frameTime);
		#endif

		fps_time -= sec;
		fps_count = 0;
	}

	if (frameTime > frame_max)
	{
		frozenTime = frameTime - frame_max;
		frameTime = frame_max;
	}

	time = newTime - frozenTime;
	accumulator += frameTime;

	while (accumulator >= dt)
	{
		state->update(dt);
		accumulator -= dt;
	}

	fps_count++;
}

} // g13
