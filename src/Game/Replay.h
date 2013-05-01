#pragma once

#include "../Math/math.h"
#include "Components/SoldierInput.h"

#include <stdint.h>
#include <cstdio>
#include <vector>
#include <string>

class Soldier;

class Replay
{
public:
	class Log
	{
	public:
		void update(Replay *replay, Soldier *soldier);
		void save(const char *filename);
	protected:
		std::string data_;
		fixvec2 prevPosition_;
	};

	enum State { Idle, Playing, Recording };

	Replay();

	void play(const char *filename, Soldier *soldier);
	void stop();
	void startRecording(Soldier *soldier);
	void stopRecording(const char *filename);
	void input(const SoldierInput *input);
	SoldierInput input();
	State state() const;
	uint32_t tick() const;

private:
	struct InputData
	{
		uint32_t tick;
		uint8_t input;
	};

	struct Data
	{
		fixvec2 startPosition;
		std::vector<InputData> inputs;
	};

	Data data_;
	State state_;
	uint32_t startTick_;
	size_t index_;

	void load(const char *filename);
	void save(const char *filename);
};
