                    CrappyScript - A really crappy scripting language

    Now, why am I calling this crappy? It's really simple - because it's REALLY quick & dirty
in how it's designed. It only works on strings (for now?), and it's designed as I go. This
language *MIGHT* appear in NanoShell, as the primary shell, however I'm not too sure about it.

This language is quite simple to parse since it does not feature infix operations (so if there
was a hypothetical arithmetic addition operation, it would be a built-in function instead of
an in-fix operator (so `add("2", "5")` instead of `"2" + "5"`).

    Please note that this code is very preliminary, and also, potentially unstable.

       This code is licensed under the MIT license. Check the LICENSE for details.

Building:

* Windows

    To build this on windows you may either open the provided Visual Studio project or use
  MinGW and type `make`.

* Linux

    To build this on Linux type `make`. Make sure you have `gcc` installed.

* NanoShell

    To build this on NanoShell, let's assume you can build the `List` utility.
	Copy its folder, change the Makefile to add `-DNANOSHELL` to the CFLAGS, and change the
file name output to the current folder's name. After that, type `make` and run it on NanoShell.

Important notes about this language:

* All integer values are represented as 64-bit signed integers internally.

* To print just a string, use either `echo ("text");` or `"text";`

* All variables are global.

* All functions are global.

* Functions declared inside of another function body will be accessible only as soon as
  the parent function has been called.

* To create a variable initialized to a string or an int, you can use either
  `let <varname> be <something>;` or `set <varname> to <something>;`. To leave a variable
  blank you can also say `let <varname>;` -- it will be initialized to 0 (int).
  Note that in both `let` and `set`'s cases you can use equals signs instead of `to` and `be`.
  Note that `var` is a synonym for `let`.

* An `if` or `while` statement's condition only accepts integer values. A condition is marked
  as "false" if its value is equal to zero, and "true" if it's non-zero.

* Arithmetic operations are most likely going to be function calls. I'll probably add them at
  some point.

* Calls to functions with no arguments can omit the parentheses. So `something;` instead of
  `something();`


Quirks of this language:
* Because of how functions and variables are used, you may get the value of a variable by
  calling it like a C function, so `varname()`.

* The simple string print (`"some text here";`) may actually omit the semicolon.

* You can name a variable with just numbers, or even some symbols. This has the added benefit
  of somewhat supporting UTF-8 characters as identifier names (well, as long as the standard
  C functions will let me). So yes, you can name your "hello" function `こんにちわ`.

* Global variables take priority over parameter names.

This language is VERY incomplete. A lot of features are still TBD, including, but not limited to:

* If and while statements.

* Function prototypes.

* String operations such as substring, length etc.

* Arithmetic operations on numbers.

* Reading input from the user.

* Loading a file other than "test.nss".

* Specifying command line parameters to the main function. These will be accessed through the
  special names $arg1 through $arg128, and $argcount

* For loops. Their syntax could be something like this:
  `for [init <statement>] [check <statement>] [step <statement>]`
  This may or may not be done after arithmetic operations.

* File I/O.

* Running programs from the file system.

* A port to NanoShell as a user program.

Statistics as of commit `7564e289`:

Parser: 819 lines
Runner: 420 lines
Tokenizer: 270 lines
Allocator: 150 lines
Shell: 129 lines
Built-in: 45 lines
Main file: 30 lines
NanoShell: 21 lines

Explanation for each component:
* NanoShell: This contains the LogMsg functions.

* Main file: Responsible for running the script file.

* Shell: This is responsible for running each phase of the script's execution, and error
  handling.

* Tokenizer: Tokenizes the code so that this program will have an easier time parsing it.

* Parser: Builds a statement tree which will be "run" by the runner.

* Runner: Runs the program itself.

* Allocator: Permits easy debugging of any memory leaks that may arise as a result of uncareful
  programming.

* Built-in: This file contains the code for all the built in commands supported. The runner
  imports them and makes them work when the script calls them.
