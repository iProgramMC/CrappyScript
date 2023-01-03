
let someInt be 10;

while someInt do
{
	echo(concat("SomeInt is ",str(someInt)));
	set someInt to sub(someInt, 1);
}
finally
{
	echo("Finally.");
}

