
function returndemo(something)
{
	echo(something);
	
	return concat(concat(substr(something, 0, 7), " not"), substr(something, 7, sub(length(something), 7)));
}

echo(returndemo("This is going to crash"));
