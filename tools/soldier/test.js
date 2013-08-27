function main()
{
	var stage = new Kinetic.Stage({ container: "container", width: 400, height: 400 });
	var layer = new Kinetic.Layer();
	var rect  = new Kinetic.Rect({ fill: "#CCC", width: 400, height: 400 });

	layer.add(rect);
	stage.add(layer);
}
