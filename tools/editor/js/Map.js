(function() {

g13["Map"] = Map;

var cache = {
	sprite: new gfx.Sprite()
};

function Map()
{
	this.width = 0;
	this.height = 0;
	this.soldiers = [];
	this.polygons = [];
	this.surfaces = [];
	this.selection = new g13.Selection();
	this.view = {x: 0, y: 0, zoom: 1};
	this.history = { index: -1, actions: [] };

	this.spriteBatch = new gfx.SpriteBatch(5, gfx.Dynamic);
}

Map.prototype.resize = function(width, height)
{
	this.width = width;
	this.height = height;
}

Map.prototype.retrieve = function(x, y, w, h)
{
	var objects = [];
	var collections = [this.soldiers, this.polygons, this.surfaces];

	if (arguments.length === 2)
	{
		for (var i = 0; i < collections.length; i++)
		{
			for (var j = 0; j < collections[i].length; j++)
			{
				if (collections[i][j].hittest(x, y))
					objects.push(collections[i][j]);
			}
		}
	}
	else
	{
		for (var i = 0; i < collections.length; i++)
		{
			for (var j = 0; j < collections[i].length; j++)
			{
				if (collections[i][j].intersects(x, y, w, h))
					objects.push(collections[i][j]);
			}
		}
	}

	return objects;
}

Map.prototype.add = function(object)
{
	switch (object.constructor)
	{
		case g13.Soldier: this.soldiers.push(object); break;
		case g13.Polygon: this.polygons.push(object); break;
		case g13.Surface: this.surfaces.push(object); break;
	}
}

Map.prototype.remove = function(object)
{
	switch (object.constructor)
	{
		case g13.Soldier: array_remove(this.soldiers, object); break;
		case g13.Polygon: array_remove(this.polygons, object); break;
		case g13.Surface: array_remove(this.surfaces, object); break;
	}
}

Map.prototype.draw = function(editor)
{
	for (var i = 0; i < this.polygons.length; i++)
		this.polygons[i].draw(editor);

	for (var i = 0; i < this.surfaces.length; i++)
		this.surfaces[i].draw(editor);

	var spriteBatch = this.spriteBatch;

	if (this.soldiers.length > 0)
	{
		if (spriteBatch.maxSize < this.soldiers.length)
			spriteBatch.resize(this.soldiers.length);

		spriteBatch.clear();

		for (var i = 0; i < this.soldiers.length; i++)
			spriteBatch.add(this.soldiers[i].sprite(cache.sprite));

		spriteBatch.upload();
		spriteBatch.texture = editor.getResource("soldier");
		spriteBatch.draw();
	}
}

Map.prototype.serialize = function()
{
}

Map.prototype.unserialize = function(data)
{
}

})();
