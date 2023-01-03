
help;

# This dumps the arguments passed in.
echo(concat(str(argc)," parameters were passed in."));

let argNum be 0;
while lt(argNum, argc) do
{
	echo(concat(concat("Argument ",str(argNum)),concat(" is ",str(args(argNum)))));
	set argNum to add(argNum,1);
}

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

