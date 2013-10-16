function inherit(sub, base)
{
	function tmp() {}
	tmp.prototype = base.prototype;
	sub.prototype = new tmp();
	sub.prototype.constructor = sub;
	sub.prototype.base = base;
}
