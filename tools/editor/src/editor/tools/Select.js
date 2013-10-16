(function() {

g13 = window.g13 || {};
g13["tools"] = g13["tools"] || {};
g13["tools"]["Select"] = Select;

function Select() {}

Select.prototype.toolactivate = function(editor)
{
	editor.setCursor("select");
}

})();
