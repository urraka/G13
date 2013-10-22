(function() {

g13 = window.g13 || {};
g13["UI"] = UI;

function UI(editor)
{
	var panels = {
		"top":    $("#panel-top")[0],
		"left":   $("#panel-left")[0],
		"bottom": $("#panel-bottom")[0],
		"view":   $("#panel-view")[0]
	};

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

	ui.command("new"      , function() { editor.newMap(3000/2, 1600/2); });
	ui.command("open"     , function() {                                });
	ui.command("save"     , function() {                                });
	ui.command("copy"     , function() {                                });
	ui.command("cut"      , function() {                                });
	ui.command("paste"    , function() {                                });
	ui.command("zoomin"   , function() { editor.zoomIn();               });
	ui.command("zoomout"  , function() { editor.zoomOut();              });
	ui.command("resetzoom", function() { editor.setZoom(1);             });
	ui.command("select"   , function() { editor.setTool("selection");   });
	ui.command("polygon"  , function() { editor.setTool("polygon");     });
	ui.command("soldier"  , function() { editor.setTool("soldier");     });
	ui.command("cancel"   , function() { editor.cancel();               });

	ui.bind("ctrl+n"      , "new");
	ui.bind("ctrl+o"      , "open");
	ui.bind("ctrl+s"      , "save");
	ui.bind("ctrl+c"      , "copy");
	ui.bind("ctrl+x"      , "cut");
	ui.bind("ctrl+v"      , "paste");
	ui.bind("+"           , "zoomin");
	ui.bind("-"           , "zoomout");
	ui.bind("ctrl+numpad0", "resetzoom");
	ui.bind("q"           , "select");
	ui.bind("p"           , "polygon");
	ui.bind("s"           , "soldier");
	ui.bind("escape"      , "cancel");

	$(panels["top"]).append(
		ui.MenuBar([
			ui.MenuItem({
				caption: "File",
				submenu: ui.Menu([
					ui.MenuItem({caption: "New...",  command: "new"}),
					ui.MenuItem({caption: "Open...", command: "open"}),
					ui.MenuItem({caption: "Save",    command: "save"})
				])
			}),
			ui.MenuItem({
				caption: "Edit",
				submenu: ui.Menu([
					ui.MenuItem({caption: "Copy",  command: "copy"}),
					ui.MenuItem({caption: "Cut",   command: "cut"}),
					ui.MenuItem({caption: "Paste", command: "paste"})
				])
			})
		]),
		ui.Separator(),
		ui.Toolbar({
			layout: "horizontal",
			items: [
				ui.IconButton({icon: ico["new"],     tooltip: "New...",   command: "new"}),
				ui.IconButton({icon: ico["open"],    tooltip: "Open...",  command: "open"}),
				ui.IconButton({icon: ico["save"],    tooltip: "Save",     command: "save"}),
				ui.Separator(),
				ui.IconButton({icon: ico["copy"],    tooltip: "Copy",     command: "copy"}),
				ui.IconButton({icon: ico["cut"],     tooltip: "Cut",      command: "cut"}),
				ui.IconButton({icon: ico["paste"],   tooltip: "Paste",    command: "paste"}),
				ui.Separator(),
				ui.IconButton({icon: ico["zoomin"],  tooltip: "Zoom In",  command: "zoomin"}),
				ui.IconButton({icon: ico["zoomout"], tooltip: "Zoom Out", command: "zoomout"})
			]
		}),
		ui.Separator()
	);

	$(panels["left"]).append(
		ui.Toolbar({
			layout: "vertical",
			items: [
				ui.IconButton({id: "tb-select",  icon: ico["select"],  tooltip: "Select",  command: "select"}),
				ui.IconButton({id: "tb-polygon", icon: ico["polygon"], tooltip: "Polygon", command: "polygon"}),
				ui.IconButton({id: "tb-soldier", icon: ico["soldier"], tooltip: "Soldier", command: "soldier"})
			]
		}),
		ui.Separator(true)
	);

	$(panels["bottom"]).append(
		ui.Separator(),
		ui.StatusBar("<label>Some status...</label>")
	);

	$(panels["view"]).append(editor.getCanvas());

	ui.disable("save");
	ui.disable("copy");
	ui.disable("cut");
	ui.disable("paste");
	ui.disable("select");
	ui.disable("polygon");
	ui.disable("soldier");

	this.panels = panels;

	this.tools = {
		"select":  $("#tb-select")[0],
		"polygon": $("#tb-polygon")[0],
		"soldier": $("#tb-soldier")[0]
	};

	// events

	$(document).on("contextmenu", function()
	{
		return false;
	});

	$(document).on("keydown", function(e)
	{
		var prevent = e.ctrlKey && (
			e.which === Key["+"] ||
			e.which === Key["-"] ||
			e.which === Key["s"] ||
			e.which === Key["n"] ||
			e.which === Key["numpad0"]
		);

		if (prevent)
			return false;
	});

	function fwd(event)
	{
		editor.event(event);
	}

	$(window).on("resize", fwd);
	$(document).on("keydown keyup", fwd);
	$(editor.getCanvas()).on("mousemove mousedown mouseup mouseenter mouseleave", fwd);

	// cursors

	var cursors = {};

	cursors["pointer"]   = {file: "left_ptr.png",  x:  7, y:  4};
	cursors["pan"]       = {file: "grabbing.png",  x: 13, y: 11};
	cursors["cross"]     = {file: "tcross.png",    x: 11, y: 11};
	cursors["crosshair"] = {file: "crosshair.png", x: 11, y: 11};
	cursors["move"]      = {file: "move.cur"};

	for (var i in cursors)
	{
		cursors[i].image = new Image();
		cursors[i].image.src = "res/cursors/" + cursors[i].file;

		var coords = "";

		if ("x" in cursors[i])
			coords = " " + cursors[i].x + " " + cursors[i].y;

		cursors[i].css = "url('res/cursors/" + cursors[i].file + "')" + coords + ", default";
	}

	cursors["none"] = {css: "none"};

	this.cursors = cursors;
}

UI.prototype.on = {};

UI.prototype.on["newmap"] = function()
{
	ui.enable("save");
	ui.enable("select");
	ui.enable("polygon");
	ui.enable("soldier");
}

})();
