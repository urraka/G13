(function() {

g13 = window.g13 || {};
g13["UI"] = UI;

function UI(editor)
{
	var ico = {
		"new":     "icon-file",
		"open":    "icon-folder-open-alt",
		"save":    "icon-save",
		"copy":    "icon-copy",
		"cut":     "icon-cut",
		"paste":   "icon-paste",
		"select":  "icon-pointer",
		"soldier": "icon-male",
		"zoomin":  "icon-zoom-in",
		"zoomout": "icon-zoom-out",
		"polygon": "icon-polygon"
	};

	var disabled = [];

	var cmd = {};
	cmd["new"]     = function() { editor.newMap(3000/2, 1600/2); }
	cmd["open"]    = null;
	cmd["save"]    = null;
	cmd["copy"]    = null;
	cmd["cut"]     = null;
	cmd["paste"]   = null;
	cmd["zoomin"]  = function() { editor.zoomIn(); };
	cmd["zoomout"] = function() { editor.zoomOut(); };
	cmd["select"]  = function() { editor.setTool("selection"); };
	cmd["polygon"] = function() { editor.setTool("polygon"); };
	cmd["soldier"] = function() { editor.setTool("soldier"); };

	// main menu

	this.file_new = ui.MenuItem({caption: "New...", handler: cmd["new"]});
	this.file_open = ui.MenuItem({caption: "Open...", handler: cmd["open"]});
	this.file_save = ui.MenuItem({caption: "Save", handler: cmd["save"]});

	this.edit_copy = ui.MenuItem({caption: "Copy", handler: cmd["copy"]});
	this.edit_cut = ui.MenuItem({caption: "Cut", handler: cmd["cut"]});
	this.edit_paste = ui.MenuItem({caption: "Paste", handler: cmd["paste"]});

	this.file = ui.MenuItem({
		caption: "File",
		submenu: ui.Menu([
			this.file_new,
			this.file_open,
			this.file_save
		])
	});

	this.edit = ui.MenuItem({
		caption: "Edit",
		submenu: ui.Menu([
			this.edit_copy,
			this.edit_cut,
			this.edit_paste,
			ui.MenuItem({
				caption: "Submenu",
				submenu: ui.Menu([
					ui.MenuItem({caption: "Something"}),
					ui.MenuItem({caption: "Something"})
				])
			})
		])
	});

	this.menubar = ui.MenuBar([
		this.file,
		this.edit
	]);

	disabled.push(this.file_save);
	disabled.push(this.edit_copy);
	disabled.push(this.edit_cut);
	disabled.push(this.edit_paste);

	// top toolbar

	this.tb_new     = ui.IconButton({icon: ico["new"],     tooltip: "New...",   handler: cmd["new"]});
	this.tb_open    = ui.IconButton({icon: ico["open"],    tooltip: "Open...",  handler: cmd["open"]});
	this.tb_save    = ui.IconButton({icon: ico["save"],    tooltip: "Save",     handler: cmd["save"]});
	this.tb_copy    = ui.IconButton({icon: ico["copy"],    tooltip: "Copy",     handler: cmd["copy"]});
	this.tb_cut     = ui.IconButton({icon: ico["cut"],     tooltip: "Cut",      handler: cmd["cut"]});
	this.tb_paste   = ui.IconButton({icon: ico["paste"],   tooltip: "Paste",    handler: cmd["paste"]});
	this.tb_zoomin  = ui.IconButton({icon: ico["zoomin"],  tooltip: "Zoom In",  handler: cmd["zoomin"]});
	this.tb_zoomout = ui.IconButton({icon: ico["zoomout"], tooltip: "Zoom Out", handler: cmd["zoomout"]});

	this.toolbar_top = ui.Toolbar({
		layout: "horizontal",
		items: [
			this.tb_new,
			this.tb_open,
			this.tb_save,
			ui.Separator(),
			this.tb_copy,
			this.tb_cut,
			this.tb_paste,
			ui.Separator(),
			this.tb_zoomin,
			this.tb_zoomout
		]
	});

	disabled.push(this.tb_save);
	disabled.push(this.tb_copy);
	disabled.push(this.tb_cut);
	disabled.push(this.tb_paste);

	// left toolbar

	this.tb_select  = ui.IconButton({icon: ico["select"],  tooltip: "Select",  handler: cmd["select"]});
	this.tb_polygon = ui.IconButton({icon: ico["polygon"], tooltip: "Polygon", handler: cmd["polygon"]});
	this.tb_soldier = ui.IconButton({icon: ico["soldier"], tooltip: "Soldier", handler: cmd["soldier"]});

	this.toolbar_left = ui.Toolbar({
		layout: "vertical",
		items: [
			this.tb_select,
			this.tb_polygon,
			this.tb_soldier
		]
	});

	disabled.push(this.tb_select);
	disabled.push(this.tb_polygon);
	disabled.push(this.tb_soldier);

	// statusbar

	this.statusbar = ui.StatusBar("<label>&nbsp;</label>");

	// panels

	this.panelTop = $("#panel-top")[0];
	this.panelLeft = $("#panel-left")[0];
	this.panelBottom = $("#panel-bottom")[0];
	this.panelView = $("#panel-view")[0];

	// add ui to DOM

	$(disabled).addClass("disabled");

	$(this.panelTop).append(
		this.menubar,
		ui.Separator(),
		this.toolbar_top,
		ui.Separator()
	);

	$(this.panelLeft).append(
		this.toolbar_left,
		ui.Separator(true)
	);

	$(this.panelBottom).append(
		ui.Separator(),
		this.statusbar
	);

	$(this.panelView).append(editor.getCanvas());

	// events

	$(document).on("contextmenu", function() { return false; });

	$(document).on("keydown", function(e) {
		if (e.ctrlKey && (e.which === 107 || e.which === 109 || e.which === 83 || e.which === 96))
			return false;
	});

	function fwd(event)
	{
		editor.event(event);
	}

	$(window).on("resize", fwd);
	$(document).on("keydown keyup", fwd);
	$(editor.getCanvas()).on("mousemove mousedown mouseup mouseenter mouseleave", fwd);
}

UI.prototype.on = {};

UI.prototype.on["newmap"] = function()
{
	var enable = [
		this.file_save,
		this.tb_save,
		this.tb_select,
		this.tb_polygon,
		this.tb_soldier
	];

	$(enable).removeClass("disabled");
}

})();
