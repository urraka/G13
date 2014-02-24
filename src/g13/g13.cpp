#include "g13.h"
#include "res.h"
#include "vars.h"
#include "stt/State.h"
#include "stt/Multiplayer.h"
#include "stt/MainMenu.h"

#include <gfx/gfx.h>
#include <enet/enet.h>
#include <hlp/enet_callbacks.h>
#include <fstream>

namespace g13 {

static stt::State *state = 0;

static Time time        = 0;
static Time accumulator = 0;
static Time fps_time    = 0;
static int  fps_count   = 0;

static const Time dt        = sys::time<sys::Milliseconds>(30);
static const Time sec       = sys::time<sys::Seconds>(1);
static const Time frame_max = sys::time<sys::Milliseconds>(250);

// -----------------------------------------------------------------------------
// Initialize
// -----------------------------------------------------------------------------

void initialize()
{
	bool fullscreen = false;

	sys::samples(4);
	sys::fullscreen(fullscreen);
	sys::window_title("G13");

	if (!fullscreen)
	{
		#ifdef DEBUG
		{
			using std::fstream;
			uint8_t n = 0;

			fstream f("wndpos", fstream::in | fstream::out | fstream::binary);

			if (!f.is_open())
			{
				f.clear();
				f.open("wndpos", fstream::in | fstream::out | fstream::binary | fstream::trunc);
			}

			f.read((char*)&n, 1);
			f.clear();

			n = n % 4;

			float positions[4][2] = {
				{0.02f, 0.02f},
				{0.52f, 0.02f},
				{0.52f, 0.52f},
				{0.02f, 0.52f}
			};

			sys::window_size(0.42f, 0.42f);
			sys::window_position(positions[n][0], positions[n][1]);

			n = (n + 1) % 4;

			f.seekp(0);
			f.write((char*)&n, 1);
		}
		#else
		{
			sys::window_size(0.7f, 0.7f);
			sys::window_position(0.15f, 0.15f);
		}
		#endif
	}
	else
	{
		sys::window_size(1.0f, 1.0f);
	}

	sys::vsync(1);

	sys::initialize();
	gfx::initialize();
	res::initialize();

	int width;
	int height;

	sys::framebuffer_size(&width, &height);
	gfx::viewport(width, height, sys::window_rotation());

	if (enet_initialize_with_callbacks(ENET_VERSION, hlp::enet_callbacks) != 0)
		error_log("Failed to initialize enet.");

	state = new stt::MainMenu();
	time = sys::time();

	srand((unsigned int)time);
}

// -----------------------------------------------------------------------------
// Terminate
// -----------------------------------------------------------------------------

void terminate()
{
	#ifdef DEBUG
		hlp::enet_callbacks_counters();
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
	while (const sys::Event *event = sys::poll_events())
	{
		if (!state->onEvent(*event))
			continue;

		switch (event->type)
		{
			case sys::Resize:
				gfx::viewport(event->size.fboWidth, event->size.fboHeight, event->size.rotation);
				break;

			case sys::Close:
				return;

			default: break;
		}
	}

	Time newTime = sys::time();
	Time frameTime = newTime - time;
	Time frozenTime = 0;

	fps_time += frameTime;

	if (fps_time >= sec)
	{
		fps_time -= sec;
		fps_count = 0;
	}

	if (frameTime > frame_max)
	{
		frozenTime = frameTime - frame_max;
		frameTime = frame_max;
	}

	time = newTime - frozenTime;

	Frame frame = {time, frameTime, float(accumulator / (double)dt)};

	state->draw(frame);

	accumulator += frameTime;

	while (accumulator >= dt)
	{
		state->update(dt);
		accumulator -= dt;
	}

	fps_count++;
}

void set_state(stt::State *state)
{
	g13::state = state;
}

} // g13
