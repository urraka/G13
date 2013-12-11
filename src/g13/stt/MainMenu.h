#pragma once

#include <g13/g13.h>
#include <gfx/gfx.h>
#include <gfx/Color.h>

#include "State.h"

namespace g13 {
namespace stt {

class MainMenu : public State
{
public:
	MainMenu();

	void update(Time dt) {}
	void draw(const Frame &frame);
	bool event(Event *evt);

private:
	void setMenu(int menu);
	void setSelected(int selected);
	bool isTextOption(int menu, int option);
	void command();
	void onKeyPressed(const Event::KeyEvent &key);
	void onText(const Event::TextEvent &text);

	const string32_t *label(int menu, int option);
	string32_t *value(int menu, int option);

	enum Menu { Main, Host, Join, MenuCount };
	enum MainOption { MainJoin, MainHost, MainQuit, MainOptionCount };
	enum HostOption { HostName, HostPort, HostStart, HostOptionCount };
	enum JoinOption { JoinName, JoinAddress, JoinStart, JoinOptionCount };

	gfx::Text hostOptions_[HostOptionCount];
	gfx::Text joinOptions_[JoinOptionCount];
	gfx::Text mainOptions_[MainOptionCount];

	gfx::Text *currentOptions_;
	gfx::Text *menuTitles_[MenuCount];

	string32_t name_;
	string32_t address_;
	string32_t port_;

	int menu_;
	int optionCount_;
	int selected_;
};

}} // g13::stt
