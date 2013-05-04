#include "Game.h"
#include "Entities/Soldier.h"
#include "Replay.h"
#include "Debugger.h"

#include <iostream>
#include <sstream>
#include <fstream>

Replay::Replay() : state_(Idle) {}

void Replay::play(const char *filename, Soldier *soldier)
{
	assert(state_ == Idle);

	load(filename);

	soldier->reset(data_.startPosition);
	startTick_ = game->tick();
	index_ = 0;
	state_ = Playing;

	DBG( std::cout << "[" << game->tick() << "] Replay started." << std::endl; );
}

void Replay::stop()
{
	assert(state_ == Playing);

	state_ = Idle;

	DBG( std::cout << "Replay ended." << std::endl; );
}

void Replay::startRecording(Soldier *soldier)
{
	assert(state_ == Idle);

	data_.inputs.resize(0);
	data_.startPosition = soldier->physics.position;
	soldier->reset(data_.startPosition); // hack so i don't have to store all the actual initial state (velocity, currentNode, etc)
	startTick_ = game->tick();
	state_ = Recording;

	DBG( std::cout << "Recording started." << std::endl; );
}

void Replay::stopRecording(const char *filename)
{
	assert(state_ == Recording);

	save(filename);
	state_ = Idle;

	DBG( std::cout << "Recording ended." << std::endl; );
}

Replay::State Replay::state() const
{
	return state_;
}

uint32_t Replay::tick() const
{
	return state_ == Idle ? 0 : game->tick() - startTick_;
}

void Replay::input(const SoldierInput *input)
{
	if (state_ != Recording)
		return;

	uint8_t prevInput = 0;

	if (data_.inputs.size() > 0)
		prevInput = data_.inputs[data_.inputs.size() - 1].input;

	uint8_t currentInput = input->serialize();

	if (currentInput != prevInput)
	{
		InputData inputData;

		inputData.tick = (uint32_t)(game->tick() - startTick_);
		inputData.input = currentInput;

		data_.inputs.push_back(inputData);
	}
}

SoldierInput Replay::input()
{
	assert(state_ == Playing);

	SoldierInput result;

	if (index_ == data_.inputs.size())
	{
		stop();
		return result;
	}

	uint32_t tick = (uint32_t)(game->tick() - startTick_);

	if (tick == data_.inputs[index_].tick)
	{
		result.unserialize(data_.inputs[index_].input);
		index_++;
	}
	else if (index_ > 0)
	{
		result.unserialize(data_.inputs[index_ - 1].input);
	}

	return result;
}

void Replay::load(const char *filename)
{
	std::ifstream file(filename, std::ifstream::in | std::ifstream::binary | std::ifstream::ate);

	if (!file.is_open())
		return;

	size_t size = file.tellg();

	if (size > sizeof(data_.startPosition))
	{
		size -= sizeof(data_.startPosition);

		if (size % sizeof(InputData) != 0)
			return;

		data_.inputs.resize(size / sizeof(InputData));

		file.seekg(0, std::ifstream::beg);
		file.read(reinterpret_cast<char*>(&data_.startPosition), sizeof(data_.startPosition));
		file.read(reinterpret_cast<char*>(&data_.inputs[0]), size);
	}
}

void Replay::save(const char *filename)
{
	if (data_.inputs.size() == 0)
		return;

	std::ofstream file(filename, std::ofstream::out | std::ofstream::trunc | std::ofstream::binary);

	if (!file.is_open())
		return;

	file.write((const char*)(&data_.startPosition), sizeof(data_.startPosition));
	file.write((const char*)(&data_.inputs[0]), data_.inputs.size() * sizeof(InputData));
	file.close();
}

void Replay::Log::update(Replay *replay, Soldier *soldier)
{
	if (replay->state() == Replay::Playing)
	{
		if (replay->tick() > 0)
		{
			std::stringstream ss;
			ss << "[" << replay->tick() << "] [(" << prevPosition_.x << ", " << prevPosition_.y << "), ";
			ss << "(" << soldier->physics.position.x << ", " << soldier->physics.position.y << ")]" << std::endl;
			data_ += ss.str();
		}

		prevPosition_ = soldier->physics.position;
	}
}

void Replay::Log::save(const char *filename)
{
	std::ofstream file(filename, std::ofstream::out | std::ofstream::trunc);
	file << data_;
	data_.empty();
}
