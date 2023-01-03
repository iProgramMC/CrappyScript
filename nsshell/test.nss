
function returndemo(something)
{
	echo(something);
	
	return concat(something, " Actually not.");
}

echo(returndemo("Is this going to crash?"));
