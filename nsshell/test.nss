function test(parm)
{
	echo(concat(str(parm), " world!"));
}

function test2(parm)
{
	echo(int(parm));
}

test("42");
test(42);
test2(42);
test2("42");
test2("0x45");
test2("089645");
