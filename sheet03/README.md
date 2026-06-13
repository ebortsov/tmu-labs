# Sheet 03 - ODR, Preprocessor and Header/Implementation files

## Mistakes

You're spending your afternoon at a local school, teaching kids how to code.
You give them a simple task: find the amplitude (difference of maximum and minimum element) and the sum of an integer vector.
After a few hours, they came up with some promising source code. Unfortunately, it doesn't compile!
Since you don't want to discourage them, you decide to make their code work without modifying it - just by adding preprocessor macros.
Review the locked code in `mistakes.cpp` and add the preprocessor macros necessary to make the code work into `fixups.hpp`.
You can assume that the logic is right. You are only supposed to use preprocessor statements starting with a `#define`.

Furthermore, they explain their thoughts on the `DEBUG` macro: it should print the corresponding message, but to find it in all their output they also wanted to prepend the source location.
For printing, they just use the provided `log` method and set the parameters accordingly (current line, file, and the message).

## Header Files

Another student tried implementing some basic math operations.
However, right when he wanted to show his results, he accidentally deleted all header files.
While the kid burst in tears, it's on you to restore the header files (`math_operations.hpp` and `print_utils.hpp`)!
Beware, that he added some tricky cases to challenge himself to get a better understanding of includes, ODR, and linking.

But the most basic things that are missing in the header are:
- the function declarations
- some includes
- header guards
- and some defines, function definitions and variable declarations

Remember, you are not allowed to edit the `cpp` files!