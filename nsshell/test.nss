
function test
{
	echo("hi!");
	
	# return "Test!"
}

function argumentTest(parm)
{
	echo(concat(concat("Argument test! And the argument passed in is \e[91m\"", parm), "\"\e[0m!"));
}

function IntentionalStackOverflow(sucks)
{
	echo(sucks);
	IntentionalStackOverflow(concat(sucks,"a"));
}

argumentTest("yoo");

function testing
{
	# Yes you can do this.
	function testing2
	{
		echo("hello from testing2");
	}
	
	testing2;
}

echo(equals("hello","hello"));
echo(equals("hello","hello1"));

help;

let something;
let vartest be "Hello";

echo(concat("Variable vartest is ", vartest));

set vartest to "Testing";

"I am printing something TempleOS style!";

echo("hello");
echo("hello1");
echo("hello2");
echo("hello3");

echo(concat("Variable vartest is ",vartest)); # And a comment here.

echo(getver());

ver;

test;

echo(test());

testing;
testing;

IntentionalStackOverflow("");

# A comment here.