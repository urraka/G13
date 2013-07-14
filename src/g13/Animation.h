#pragma once

#include "g13.h"

namespace g13 {

struct Frame
{
	Frame() : x(0), y(0), width(0), height(0), cx(0), cy(0), duration(0) {}

	Frame(int X, int Y, int W, int H, int CX, int CY, Time dur)
		:	x(X),
			y(Y),
			width(W),
			height(H),
			cx(CX),
			cy(CY),
			duration(dur)
	{}

	int x, y, width, height;
	int cx, cy; // center
	Time duration;
};

struct AnimationInfo
{
	AnimationInfo() : first(0), last(0) {}
	AnimationInfo(int First, int Last) : first(First), last(Last) {}

	int first;
	int last;
};

class Animation
{
public:
	Animation()
		:	id_(-1),
			frame_(-1),
			time_(0),
			frames_(0),
			animations_(0)
	{}

	int id() const
	{
		return id_;
	}

	const Frame *frame() const
	{
		return frame_ != -1 ? &frames_[frame_] : 0;
	}

	int frameIndex() const
	{
		return frame_;
	}

	void set(int id, int frame = -1)
	{
		id_ = id;
		frame_ = frame;

		if (frame_ == -1)
			frame_ = animations_[id].first;
	}

	void data(const Frame *frames, const AnimationInfo *animations)
	{
		frames_ = frames;
		animations_ = animations;
	}

	void update(Time dt)
	{
		assert(frames_ != 0);
		assert(animations_ != 0);

		Time duration = frames_[frame_].duration;

		if (duration > 0)
		{
			time_ += dt;

			while (time_ > duration)
			{
				time_ -= duration;
				frame_++;

				if (frame_ > animations_[id_].last)
					frame_ = animations_[id_].first;

				duration = frames_[frame_].duration;

				if (duration == 0)
					break;
			}
		}
	}

private:
	int id_;
	int frame_;
	Time time_;
	const Frame *frames_;
	const AnimationInfo *animations_;
};

} // g13
