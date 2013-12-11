#include "MainMenu.h"
#include "Multiplayer.h"

#include <g13/res.h>
#include <g13/math.h>
#include <hlp/utf8.h>
#include <hlp/split.h>
#include <hlp/to_int.h>

#include <g13/net/Player.h>

namespace g13 {
namespace stt {

static const uint32_t   fontSize = 16;
static const gfx::Color fontColor = gfx::Color(0xFF, 0xA0);
static const gfx::Color fontColorSel = gfx::Color(0xFF, 0xFF);

MainMenu::MainMenu()
	:	currentOptions_(mainOptions_),
		menuTitles_(),
		menu_(0),
		optionCount_(3),
		selected_(0)
{
	gfx::Font *font = res::font(res::DefaultFont);

	// menu titles

	menuTitles_[Main] = 0;
	menuTitles_[Host] = &mainOptions_[MainHost];
	menuTitles_[Join] = &mainOptions_[MainJoin];

	// default values

	name_    = hlp::utf8_decode("Player");
	address_ = hlp::utf8_decode("localhost:1234");
	port_    = hlp::utf8_decode("1234");

	// main options

	mainOptions_[MainJoin].value("Join");
	mainOptions_[MainHost].value("Create");
	mainOptions_[MainQuit].value("Quit");

	for (int i = 0; i < MainOptionCount; i++)
	{
		mainOptions_[i].font(font);
		mainOptions_[i].size(fontSize);
		mainOptions_[i].color(fontColor);
	}

	// host options

	hostOptions_[HostName].value(*label(Host, HostName) + *value(Host, HostName));
	hostOptions_[HostPort].value(*label(Host, HostPort) + *value(Host, HostPort));
	hostOptions_[HostStart].value("Play");

	for (int i = 0; i < HostOptionCount; i++)
	{
		hostOptions_[i].font(font);
		hostOptions_[i].size(fontSize);
		hostOptions_[i].color(fontColor);
	}

	// join options

	joinOptions_[JoinName].value(*label(Join, JoinName) + *value(Join, JoinName));
	joinOptions_[JoinAddress].value(*label(Join, JoinAddress) + *value(Join, JoinAddress));
	joinOptions_[JoinStart].value("Play");

	for (int i = 0; i < JoinOptionCount; i++)
	{
		joinOptions_[i].font(font);
		joinOptions_[i].size(fontSize);
		joinOptions_[i].color(fontColor);
	}

	setMenu(Main);
}

void MainMenu::draw(const Frame &frame)
{
	gfx::clear();

	const float lineHeight = fontSize * 2.0f;
	const float totalHeight = optionCount_ * lineHeight;

	const float W = sys::framebuffer_width();
	const float H = sys::framebuffer_height();
	const float Y = 0.5f * H - 0.5f * totalHeight + fontSize;

	if (menuTitles_[menu_] != 0)
	{
		gfx::matrix(mat2d::translate(20.0f, 20.0f + fontSize));
		gfx::draw(menuTitles_[menu_]);
	}

	for (int i = 0; i < optionCount_; i++)
	{
		const gfx::Text::Bounds &bounds = currentOptions_[i].bounds();

		float x = 0.5f * W - 0.5f * bounds.width - bounds.x;
		float y = Y + i * lineHeight;

		gfx::matrix(mat2d::translate(x, y));
		gfx::draw(&currentOptions_[i]);
	}
}

bool MainMenu::event(Event *evt)
{
	switch (evt->type)
	{
		case Event::KeyPressed:  onKeyPressed(evt->key); break;
		case Event::KeyRepeat:   onKeyPressed(evt->key); break;
		case Event::TextEntered: onText(evt->text);      break;
		default:                                         break;
	}

	return true;
}

void MainMenu::setMenu(int menu)
{
	setSelected(0);

	menu_ = menu;
	optionCount_ = 3;

	currentOptions_ = menu == Main ? mainOptions_ :
	                  menu == Host ? hostOptions_ :
	                  menu == Join ? joinOptions_ : 0;

	setSelected(0);
}

void MainMenu::setSelected(int selected)
{
	const uint32_t caret[] = {'_', 0};

	currentOptions_[selected_].color(fontColor);
	currentOptions_[selected].color(fontColorSel);

	if (isTextOption(menu_, selected_))
		currentOptions_[selected_].value(*label(menu_, selected_) + *value(menu_, selected_));

	if (isTextOption(menu_, selected))
		currentOptions_[selected].value(*label(menu_, selected) + *value(menu_, selected) + caret);

	selected_ = selected;
}

bool MainMenu::isTextOption(int menu, int option)
{
	switch (menu)
	{
		case Host: return option == HostName || option == HostPort;
		case Join: return option == JoinName || option == JoinAddress;
		default:   return false;
	}
}

void MainMenu::command()
{
	switch (menu_)
	{
		case Main:
		{
			switch (selected_)
			{
				case MainHost: setMenu(Host); break;
				case MainJoin: setMenu(Join); break;
				case MainQuit: sys::exit();   break;
			}
		}
		break;

		case Host:
		{
			switch (selected_)
			{
				case HostName:  setSelected((selected_ + 1) % optionCount_); break;
				case HostPort:  setSelected((selected_ + 1) % optionCount_); break;

				case HostStart:
				{
					std::string port;
					hlp::utf8_encode(port_, port);

					g13::set_state(new Multiplayer(name_, hlp::to_int(port)));

					delete this;
				}
				break;
			}
		}
		break;

		case Join:
		{
			switch (selected_)
			{
				case JoinName:    setSelected((selected_ + 1) % optionCount_); break;
				case JoinAddress: setSelected((selected_ + 1) % optionCount_); break;

				case JoinStart:
				{
					std::string addr;
					hlp::utf8_encode(address_, addr);
					hlp::strvector parts = hlp::split(addr, ':');

					g13::set_state(new Multiplayer(name_, parts[0].c_str(), hlp::to_int(parts[1])));

					delete this;
				}
				break;
			}
		}
		break;
	}
}

void MainMenu::onKeyPressed(const Event::KeyEvent &key)
{
	switch (key.code)
	{
		case sys::Escape:
		{
			if (menu_ == Main)
				sys::exit();
			else
				setMenu(Main);
		}
		break;

		case sys::Tab:
		{
			if (key.shift)
				setSelected((selected_ + optionCount_ - 1) % optionCount_);
			else
				setSelected((selected_ + 1) % optionCount_);
		}
		break;

		case sys::Down:
			setSelected((selected_ + 1) % optionCount_);
			break;

		case sys::Up:
			setSelected((selected_ + optionCount_ - 1) % optionCount_);
			break;

		case sys::Enter:
			command();
			break;

		case sys::Backspace:
		{
			if (isTextOption(menu_, selected_))
			{
				const uint32_t caret[] = {'_', 0};
				const string32_t *label = MainMenu::label(menu_, selected_);
				string32_t *value = MainMenu::value(menu_, selected_);

				if (value->size() > 0)
				{
					value->resize(value->size() - 1);
					currentOptions_[selected_].value(*label + *value + caret);
				}
			}
		}
		break;

		case 'C':
		{
			if (key.ctrl && isTextOption(menu_, selected_))
			{
				std::string str;
				hlp::utf8_encode(*value(menu_, selected_), str);
				sys::clipboard(str.c_str());
			}
		}
		break;

		case 'V':
		{
			if (key.ctrl && isTextOption(menu_, selected_))
			{
				const char *clipboard = sys::clipboard();

				if (clipboard != 0)
				{
					const uint32_t caret[] = {'_', 0};
					const string32_t *label = MainMenu::label(menu_, selected_);
					string32_t *value = MainMenu::value(menu_, selected_);

					*value += hlp::utf8_decode(clipboard);

					if (value == &name_ && value->size() > net::Player::MaxNameLength)
						value->resize(net::Player::MaxNameLength);

					currentOptions_[selected_].value(*label + *value + caret);
				}
			}
		}
		break;
	}
}

void MainMenu::onText(const Event::TextEvent &text)
{
	if (isTextOption(menu_, selected_))
	{
		const uint32_t caret[] = {'_', 0};
		const string32_t *label = MainMenu::label(menu_, selected_);
		string32_t *value = MainMenu::value(menu_, selected_);

		if (value != &name_ || value->size() < net::Player::MaxNameLength)
		{
			*value += text.ch;
			currentOptions_[selected_].value(*label + *value + caret);
		}
	}
}

const string32_t *MainMenu::label(int menu, int option)
{
	static const uint32_t _name[] = {'N', 'a', 'm', 'e', ':', ' ', 0};
	static const uint32_t _port[] = {'P', 'o', 'r', 't', ':', ' ', 0};
	static const uint32_t _addr[] = {'S', 'e', 'r', 'v', 'e', 'r', ':', ' ', 0};

	static string32_t name    = _name;
	static string32_t port    = _port;
	static string32_t address = _addr;

	switch (menu)
	{
		case Host: return option == HostName ? &name :
		                  option == HostPort ? &port : 0;

		case Join: return option == JoinName    ? &name :
		                  option == JoinAddress ? &address : 0;
	}

	return 0;
}

string32_t *MainMenu::value(int menu, int option)
{
	switch (menu)
	{
		case Host: return option == HostName ? &name_ :
		                  option == HostPort ? &port_ : 0;

		case Join: return option == JoinName    ? &name_ :
		                  option == JoinAddress ? &address_ : 0;
	}

	return 0;
}

}} // g13::stt
