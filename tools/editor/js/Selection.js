(function() {

g13["Selection"] = Selection;

function compare(a, b) { return b - a; }

// -----------------------------------------------------------------------------
// Selection
// -----------------------------------------------------------------------------

function Selection()
{
	this.objects = [];
	this.bounds = {x: 0, y: 0, w: 0, h: 0};
}

Selection.prototype.computeBounds = function()
{
	var N = this.objects.length;

	if (N > 0)
	{
		rect_assign(this.bounds, this.objects[0].bounds);

		for (var i = 1; i < N; i++)
			rect_expand(this.bounds, this.objects[i].bounds);
	}
}

Selection.prototype.add = function(object)
{
	if (object.constructor === Array)
	{
		var objects = object;

		if (objects.length === 0)
			return;

		var add = null;

		if (this.objects.length === 0)
		{
			rect_assign(this.bounds, objects[0].bounds);
			add = objects;
		}
		else
		{
			add = [];

			for (var i = 0; i < objects.length; i++)
			{
				if (!this.contains(objects[i]))
					add.push(objects[i]);
			}
		}

		for (var i = 0; i < add.length; i++)
		{
			rect_expand(this.bounds, add[i].bounds);
			this.objects.push(add[i]);
		}
	}
	else
	{
		if (this.objects.length === 0)
		{
			rect_assign(this.bounds, object.bounds);
			this.objects.push(object);
		}
		else if (!this.contains(object))
		{
			rect_expand(this.bounds, object.bounds);
			this.objects.push(object);
		}
	}
}

Selection.prototype.toggle = function(object)
{
	if (this.objects.length === 0)
	{
		this.add(object);
		return;
	}

	if (object.constructor === Array)
	{
		var objects = object;

		if (objects.length === 0)
			return;

		var add = [];
		var remove = [];

		for (var i = 0; i < objects.length; i++)
		{
			var index = this.objects.indexOf(objects[i]);

			if (index !== -1)
				remove.push(index);
			else
				add.push(objects[i]);
		}

		if (remove.length === 0)
		{
			for (var i = 0; i < add.length; i++)
			{
				rect_expand(this.bounds, add[i].bounds);
				this.objects.push(add[i]);
			}
		}
		else
		{
			remove.sort(compare);

			for (var i = 0; i < remove.length; i++)
				this.objects.splice(remove[i], 1);

			for (var i = 0; i < add.length; i++)
				this.objects.push(add[i]);

			this.computeBounds();
		}
	}
	else
	{
		var index = this.objects.indexOf(object);

		if (index !== -1)
		{
			this.objects.splice(index, 1);
			this.computeBounds();
		}
		else
		{
			rect_expand(this.bounds, object.bounds);
			this.objects.push(object);
		}
	}
}

Selection.prototype.remove = function(object)
{
	if (this.objects.length === 0)
		return;

	var recompute = false;

	switch (object.constructor)
	{
		case Number:
		{
			var index = object;

			if (index >= 0 && index < this.objects.length)
			{
				this.objects.splice(index, 1);
				recompute = true;
			}
		}
		break;

		case Array:
		{
			var objects = object;
			var remove = [];

			for (var i = 0; i < objects.length; i++)
			{
				var index = this.objects.indexOf(objects[i]);

				if (index !== -1)
					remove.push(index);
			}

			if (remove.length > 0)
			{
				remove.sort(compare);

				for (var i = 0; i < remove.length; i++)
					this.objects.splice(remove[i], 1);

				recompute = true;
			}
		}
		break;

		default:
		{
			var index = this.objects.indexOf(object);

			if (index !== -1)
			{
				this.objects.splice(index, 1);
				recompute = true;
			}
		}
		break;
	}

	if (recompute)
		this.computeBounds();
}

Selection.prototype.find = function(object)
{
	return this.objects.indexOf(object);
}

Selection.prototype.contains = function(object)
{
	return this.objects.indexOf(object) !== -1;
}

Selection.prototype.clear = function()
{
	this.objects.splice(0, this.objects.length);
}

Selection.prototype.isEmpty = function()
{
	return this.objects.length === 0;
}

})();
