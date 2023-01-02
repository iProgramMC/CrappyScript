                      CrappyScript - A really crappy scripting language

    Now, why am I calling this crappy? It's really simple - because it's REALLY quick & dirty
in how it's designed. It only works on strings (for now?), and it's designed as I go. This
language *MIGHT* appear in NanoShell, as the primary shell, however I'm not too sure about it.

This language is quite simple to parse since it does not feature infix operations (so if there
was a hypothetical arithmetic addition operation, it would be a built-in function instead of
an in-fix operator (so `add("2", "5")` instead of `"2" + "5"`).

         This code is licensed under the MIT license. Check LICENSE for details.



Important notes about this language:

* The language only works with strings. Why? Well, this might be used as NanoShell's
  new command line shell.

* To print just a string, use either `echo ("text");` or `"text";`

* All variables are global.

* All functions are global.

* Functions declared inside of another function body will be accessible only as soon as
  the parent function has been called.

* To create a variable initialized to a string, you can use either
  `let <varname> be <something>;` or `set <varname> to <something>;`. To leave a variable
  blank you can also say `let <varname>;`.
  Note that in both `let` and `set`'s cases you can use equals signs instead of `to` and `be`.
  Note that `var` is a synonym for `let`.

* An `if` or `while` statement checks if the string is blank, NULL (internally) or equal to "0"
  before jumping into the `else` branch. `equals(strA, strB)` will return "0" if the strings
  don't match, and "1" if they do.

* Arithmetic operations are, er, complicated. I'll probably add them at some point.

* Calls to functions with no arguments can omit the parentheses. So `something;` instead of
  `something();`

* The 


Quirks of this language:
* Because of how functions and variables are used, you may get the value of a variable by
  calling it like a C function, so `varname()`.

* The simple string print (`"some text here";`) may actually omit the semicolon.

* You can name a variable with just numbers, or even some symbols. This has the added benefit
  of somewhat supporting UTF-8 characters as identifier names (well, as long as the standard
  C functions will let me). So yes, you can name your "hello" function `こんにちわ`.

* Global variables take over parameter names.
