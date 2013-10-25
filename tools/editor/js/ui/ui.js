(function() {

// -----------------------------------------------------------------------------
// Commands & key bindings
// -----------------------------------------------------------------------------

var commands = {};
var key_bindings = new Array(2*2*2*256);

function command(name, handler)
{
	if (name in commands)
	{
		commands[name].handler = handler;
	}
	else
	{
		commands[name] = {
			handler: handler,
			elements: [],
			bindings: [],
			disabled: false
		};
	}
}

function enable(command)
{
	commands[command].disabled = false;
	$(commands[command].elements).removeClass("disabled");
}

function disable(command)
{
	commands[command].disabled = true;
	$(commands[command].elements).addClass("disabled");
}

function bind(keys, cmd)
{
	keys = keys.split('+');

	if (keys.length === 0)
		return;

	for (var i = 0; i < keys.length; i++)
	{
		keys[i] = keys[i].toLowerCase().trim();

		if (keys[i].length === 0)
		{
			keys[i] = "+";
			keys.splice(i + 1, 1);
		}

		if (!(keys[i] in Key))
		{
			console.warn("Invalid key binding.", cmd);
			return;
		}
	}

	var key = keys[keys.length - 1];

	keys = keys.slice(0, keys.length - 1);

	var ALT = 1;
	var CTRL = 2;
	var SHIFT = 4;

	var mode = 0;

	mode |= keys.indexOf("alt") !== -1 ? ALT : 0;
	mode |= keys.indexOf("ctrl") !== -1 ? CTRL : 0;
	mode |= keys.indexOf("shift") !== -1 ? SHIFT : 0;

	key_bindings[mode * 256 + Key[key]] = cmd;

	if (!(cmd in commands))
		command(cmd, null);

	keys = [];

	if (mode & CTRL)  keys.push("Ctrl");
	if (mode & ALT)   keys.push("Alt");
	if (mode & SHIFT) keys.push("Shift");

	keys.push(key.toUpperCase());

	commands[cmd].bindings.push(keys.join("+"));
}

$(function() {
	$(document).on("keydown", function(event)
	{
		var mode = 0;

		mode |= event.altKey ? 1 : 0;
		mode |= event.ctrlKey ? 2 : 0;
		mode |= event.shiftKey ? 4 : 0;

		var command = key_bindings[mode * 256 + event.which];

		if (command in commands && !commands[command].disabled)
			commands[command].handler.call(window, event);
	});
});

// -----------------------------------------------------------------------------
// Constructors
// -----------------------------------------------------------------------------

function MenuBar(items)
{
	var menu = $(document.createElement("div"));

	menu.addClass("ui-menu-bar");

	for (var i = 0; i < items.length; i++)
		menu.append(items[i]);

	return menu[0];
}

function Menu(items)
{
	var menu = $(document.createElement("div"));
	var cont = $(document.createElement("div"));

	menu.addClass("ui-menu");

	menu.append(cont);

	for (var i = 0; i < items.length; i++)
		cont.append(items[i]);

	return menu[0];
}

function MenuItem(params)
{
	var item = $(document.createElement("div"));

	if (params.id)
		item.attr("id", params.id);

	if (params.submenu)
	{
		item.addClass("has-submenu icon-caret-right");
		item.data("submenu", params.submenu);
	}
	else if (params.command)
	{
		item.data("command", params.command);
		commands[params.command].elements.push(item[0]);
	}

	item.addClass("ui-menu-item");
	item.append($(document.createElement("label")).text(params.caption));

	return item[0];
}

function Separator(vertical)
{
	var separator = $(document.createElement("div"));
	separator.addClass("ui-separator");

	if (vertical)
		separator.addClass("vertical");

	return separator[0];
}

function Toolbar(params)
{
	var toolbar = $(document.createElement("div"));
	var layout = params.layout || "horizontal";

	var classname = layout === "horizontal" ? "ui-htoolbar" : "ui-vtoolbar";

	toolbar.addClass(classname);

	for (var i = 0; i < params.items.length; i++)
		toolbar.append(params.items[i]);

	return toolbar[0];
}

function IconButton(params)
{
	var button = $(document.createElement("div"));

	button.addClass("ui-icon-button");
	button.addClass(params.icon);

	if (params.id)
		button.attr("id", params.id);

	var tooltip = params.tooltip;

	if (params.command)
	{
		button.data("command", params.command);
		commands[params.command].elements.push(button[0]);

		if (commands[params.command].bindings.length > 0)
		{
			tooltip = tooltip ? tooltip + " " : "";
			tooltip += "(" + commands[params.command].bindings[0] + ")";
		}
	}

	if (tooltip)
		button.attr("title", tooltip);

	return button[0];
}

function StatusBar(content)
{
	var statusbar = $(document.createElement("div"));
	statusbar.append(content);
	statusbar.addClass("ui-statusbar");

	return statusbar[0];
}

// -----------------------------------------------------------------------------
// Functions
// -----------------------------------------------------------------------------

var capturedElement = null;

function captureHandler(e)
{
	e.stopPropagation();

	var newEvent = new $.Event(e.type, {
		pageX: e.pageX,
		pageY: e.pageY,
		which: e.which,
		ctrlKey: e.ctrlKey,
		shiftKey: e.shiftKey,
		altKey: e.altKey
	});

	$(capturedElement).trigger(newEvent);
}

function hasCapture(element)
{
	return element === capturedElement;
}

function capture(element)
{
	capturedElement = element;

	if (!element)
	{
		$(".ui-capture").remove();
		$(window).off("mouseup", captureHandler);
		return;
	}

	var capturer = $(document.createElement("div"));
	capturer.addClass("ui-capture");
	capturer.addClass($(element).attr("class"));

	$(window).on("mouseup", captureHandler);
	capturer.on("mousemove mousedown mouseup", captureHandler);

	capturer.appendTo(document.body);
}

// -----------------------------------------------------------------------------
// Menu actions
// -----------------------------------------------------------------------------

function open_menu(parentItem)
{
	var item = $(parentItem);
	var menu = $(item.data("submenu"));

	if (!menu)
		return;

	menu.appendTo("#ui-container");

	var offset = item.offset();
	var x = offset.left;
	var y = offset.top;

	if (item.parent().hasClass("ui-menu-bar"))
	{
		y += item.parent().height();
	}
	else
	{
		x += item.parent().width();
		y += menu.offset().top - menu.find(".ui-menu-item:first-child").offset().top;
	}

	menu.css("left", x + "px");
	menu.css("top", y + "px");
}

function close_menu(parentItem)
{
	var menu = $($(parentItem).data("submenu"));

	if (menu)
	{
		menu.trigger("close");
		menu.detach();
	}
}

// -----------------------------------------------------------------------------
// Menu events
// -----------------------------------------------------------------------------

(function() {
	var timer = null;
	var delay = 300;

	function show_submenu(item)
	{
		if (timer !== null)
		{
			clearTimeout(timer);
			timer = null;
		}

		var parentMenu = $(item).closest(".ui-menu");
		var activeItem = parentMenu.data("activeItem");

		if (activeItem)
		{
			$(activeItem).removeClass("active");
			close_menu(activeItem);
		}

		open_menu(item);
		$(item).addClass("active");
		parentMenu.data("activeItem", item);
	}

	function on_item_mousedown(evt)
	{
		if ($(this).data("submenu"))
			show_submenu(this);

		evt.stopPropagation();
	}

	function on_item_mouseup(evt)
	{
		var command = $(this).data("command");

		if ($(this).data("submenu"))
		{
			show_submenu(this);
		}
		else if (command && !$(this).hasClass("disabled"))
		{
			$(this).removeClass("hover");
			$(".ui-menu-bar").trigger("close");
			commands[command].handler.call(this);
		}

		evt.stopPropagation();
	}

	function on_item_mouseover()
	{
		$(this).addClass("hover");

		var parentMenu = $(this).closest(".ui-menu");
		var activeItem = parentMenu.data("activeItem");

		if (activeItem !== this)
		{
			$(activeItem).removeClass("active");

			if ($(this).data("submenu"))
			{
				if (timer !== null)
					clearTimeout(timer);

				var self = this;

				timer = setTimeout(function() {
					timer = null;
					show_submenu(self);
				}, delay);
			}
			else
			{
				$(activeItem).trigger("mouseout");
			}
		}
		else
		{
			$(activeItem).addClass("active");
		}
	}

	function on_item_mouseout(evt)
	{
		$(this).removeClass("hover");

		var parentMenu = $(this).closest(".ui-menu");
		var activeItem = parentMenu.data("activeItem");

		if ($(this).data("submenu"))
		{
			if (timer !== null)
			{
				clearTimeout(timer);
				timer = null;
			}

			var submenu = $(this).data("submenu");
			var target = evt.relatedTarget;

			if (activeItem === this && !($.contains(submenu, target) || target === submenu))
			{
				$(this).removeClass("active");

				timer = setTimeout(function() {
					timer = null;
					close_menu(activeItem);
					$(parentMenu).data("activeItem", null);
				}, delay);
			}
		}
	}

	function on_menu_close()
	{
		var activeItem = $(this).data("activeItem");

		if (activeItem)
		{
			$(activeItem).removeClass("active");
			$(this).data("activeItem", null);
			close_menu(activeItem);
		}
	}

	$(function() {
		$(document).on("mousedown mouseup", ".ui-menu", function(evt) { evt.stopPropagation(); });
		$(document).on("mousedown",  ".ui-menu .ui-menu-item", on_item_mousedown);
		$(document).on("mouseup",    ".ui-menu .ui-menu-item", on_item_mouseup);
		$(document).on("mouseenter", ".ui-menu .ui-menu-item", on_item_mouseover);
		$(document).on("mouseleave", ".ui-menu .ui-menu-item", on_item_mouseout);
		$(document).on("close",      ".ui-menu",               on_menu_close);
	});
})();

// -----------------------------------------------------------------------------
// MenuBar events
// -----------------------------------------------------------------------------

(function() {
	var pressedItem = null;
	var activeItem = null;

	function show_menu(item)
	{
		if (activeItem !== null && activeItem !== item)
		{
			$(activeItem).removeClass("active");
			close_menu(activeItem);
			activeItem = null;
		}

		if (item !== null && activeItem !== item)
		{
			activeItem = item;
			$(activeItem).addClass("active");
			open_menu(activeItem);
		}
	}

	function on_item_mousedown(evt)
	{
		if (activeItem !== this)
		{
			pressedItem = this;
			show_menu(this);
		}

		evt.stopPropagation();
	}

	function on_item_mouseup(evt)
	{
		evt.stopPropagation();

		if (pressedItem === this)
		{
			pressedItem = null;
			return;
		}

		show_menu(null);
	}

	function on_item_mouseover()
	{
		$(this).addClass("hover");

		if (activeItem !== null)
		{
			if (pressedItem)
				pressedItem = this;

			show_menu(this);

			$($(this).data("submenu")).trigger("close");
		}
	}

	function on_item_mouseout()
	{
		$(this).removeClass("hover");
	}

	function on_menubar_close()
	{
		show_menu(null);
	}

	$(function() {
		$(document).on("mousedown",  ".ui-menu-bar .ui-menu-item", on_item_mousedown);
		$(document).on("mouseup",    ".ui-menu-bar .ui-menu-item", on_item_mouseup);
		$(document).on("mouseenter", ".ui-menu-bar .ui-menu-item", on_item_mouseover);
		$(document).on("mouseleave", ".ui-menu-bar .ui-menu-item", on_item_mouseout);
		$(document).on("close",      ".ui-menu-bar",               on_menubar_close);
	});
})();

// -----------------------------------------------------------------------------
// IconButton
// -----------------------------------------------------------------------------

(function() {
	var pressedButton = null;

	function on_button_mousedown(event)
	{
		if (event.which !== 1)
			return;

		pressedButton = this;
	}

	function on_button_mouseup(event)
	{
		if (event.which !== 1)
			return;

		var command = $(this).data("command");

		if (command && pressedButton === this && !$(this).hasClass("disabled"))
			commands[command].handler.call(this);

		pressedButton = null;
	}

	$(function() {
		$(document).on("mousedown", ".ui-icon-button", on_button_mousedown);
		$(document).on("mouseup",   ".ui-icon-button", on_button_mouseup);
	});
})();

// -----------------------------------------------------------------------------
// General
// -----------------------------------------------------------------------------

$(function() {
	$(document).on("mousedown mouseup", function() {
		$(".ui-menu-bar").trigger("close");
	});

	var uicontainer = $(document.createElement("div"));
	uicontainer.attr("id", "ui-container");

	$(document.body).prepend(uicontainer);
});

// -----------------------------------------------------------------------------
// Exports
// -----------------------------------------------------------------------------

ui = {};

ui["MenuBar"]    = MenuBar;
ui["Menu"]       = Menu;
ui["MenuItem"]   = MenuItem;
ui["Toolbar"]    = Toolbar;
ui["IconButton"] = IconButton;
ui["Separator"]  = Separator;
ui["StatusBar"]  = StatusBar;
ui["capture"]    = capture;
ui["hasCapture"] = hasCapture;
ui["command"]    = command;
ui["bind"]       = bind;
ui["enable"]     = enable;
ui["disable"]    = disable;

})();
