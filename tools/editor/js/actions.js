(function() {

g13["actions"] = {};
g13["actions"]["move_objects"]   = move_objects;
g13["actions"]["add_objects"]    = add_objects;
g13["actions"]["remove_objects"] = remove_objects;

function move_objects(editor, data)
{
	var objects = data.objects;
	var positions = data.positions;

	for (var i = 0; i < objects.length; i++)
		objects[i].moveTo(positions[i].x, positions[i].y);

	editor.getSelection().clear();
	editor.getSelection().add(objects);
}

function add_objects(editor, data)
{
	var objects = data.objects;

	for (var i = 0; i < objects.length; i++)
		editor.map.add(objects[i]);

	if (data.select)
	{
		editor.getSelection().clear();
		editor.getSelection().add(objects);
	}
}

function remove_objects(editor, data)
{
	var objects = data.objects;

	for (var i = 0; i < objects.length; i++)
		editor.map.remove(objects[i]);

	editor.getSelection().remove(objects);
}

})();
