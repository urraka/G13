#pragma once

#include <g13/g13.h>
#include <g13/ui/Bar.h>
#include <gfx/gfx.h>
#include <hlp/ConfigFile.h>

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
	bool isColorOption(int menu, int option);
	void command();
	void onKeyPressed(const Event::KeyEvent &key);
	void onText(const Event::TextEvent &text);

	const string32_t *label(int menu, int option);
	string32_t *value(int menu, int option);

	void loadConfig();
	void saveConfig();

	enum Menu       { Main, Host, Join, MenuCount };
	enum MainOption { MainJoin, MainHost, MainQuit, MainOptionCount };
	enum HostOption { HostName, HostColor, HostPort, HostStart, HostOptionCount };
	enum JoinOption { JoinName, JoinColor, JoinAddress, JoinStart, JoinOptionCount };

	gfx::Text hostOptions_[HostOptionCount];
	gfx::Text joinOptions_[JoinOptionCount];
	gfx::Text mainOptions_[MainOptionCount];

	ui::Bar colorBar_;

	gfx::Text *currentOptions_;
	gfx::Text *menuTitles_[MenuCount];

	hlp::ConfigFile config_;

	string32_t name_;
	string32_t address_;
	string32_t port_;
	int        color_;

	int menu_;
	int optionCount_;
	int selected_;
};

}} // g13::stt
