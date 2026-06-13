# Sheet 02 - First Steps into C++

The CMake projects are provided already and you are only supposed to change the respective *.cpp files!

## Mandelbrot

The _Mandelbrot set_ is the set of complex numbers `c = x + yi` for which the function `f_c(z) = z^2 + c` does not diverge when iterated from `z = 0` (i.e. `f_c(0)`, `f_c(f_c(0))`, `f_c(f_c(f_c(0)))`, ...).
Your task in this exercise is to write a C++ program which prints an image of the Mandelbrot set in a certain area `A` of the complex plane, which is defined by its center point and a width and height.

The program should compute and print an image of the Mandelbrot set in this area `A` to stdout.
For this, we assume that each character represents one _pixel_.
Thus, by printing a certain number of output lines with the same number of characters per line, we can generate a rectangular `image` in the terminal.
This image should have the same aspect ratio as the rectangular area of the complex plane defined by `A`, i.e. your program should compute the number of output lines and characters per line as follows:
- Each output line contains exactly 200 characters.
- The number of output lines should be chosen in such a way that the aspect ratio of the output matches the aspect ratio of `A`.
That is, the number of output lines depends on the aspect ratio of `A`, as well as the aspect ratio of the font used by the terminal.
You can assume that the aspect ratio of the font is around `0.4`.

Subsequently, the program needs to iterate over the output pixels and perform the following computations:
- Compute the complex number `c = x + yi` in `A` that corresponds to the current `pixel` by linear interpolation between the boundaries of `A`
- Iteratively compute `z_(i + 1) = f_c(z_i)` with `z_0 = 0`
- If at any point the absolute value of `z_i` is greater than `2`, we assume that `f_c` diverges, i.e. `c` is not in the Mandelbrot set (print ` `)
- If `f_c` has not diverged after `200` iterations, we assume that `c` is in the Mandelbrot set (print `x`)

## Simple VM
In this task you will implement a simple VM (virtual machine) that supports four integer registers and a few arithmetic operations on them.

The VM has exactly four integer registers. Each register must be able to hold a 32 bit integer (i.e. int32_t).

The instructions are provided as an input stream. You can fetch the next `int32_t` token by calling `next()` function.
The binary format looks like this: `<opcode> [<arguments>...]`. The opcode is a `int32_t` between 0 and 100.
It identifies the instructions that is executed. Depending on the opcode there can be zero or more arguments following.
Each arguemnt itself is represented as `int32_t` as well.
An `<argument>` is either a register identifier (numbers from 0 to 3) or an integer immediate.

The instructions supported by the VM can be seen in this Table.
You don't have to take care of invalid input programs.

opcode + args        | name   | description
---------------------|------- |-------------
0                    | halt   | End of execution of the VM
10 `<ireg>` `<iimm>` | movi   | Store the value from `<iimm>` to register `<ireg>`
20 `<ireg>`          | loada  | Copy the value from `<ireg>` into register zero
21 `<ireg>`          | storea | Copy the value from register zero into `<ireg>`
22                   | swapab | Swap values from registers zero and one
50                   | addi   | Add registers zero and one and store them into register zero
51                   | subi   | Substract register one from zero and store the result into register zero
53                   | muli   | Multiply registers zero and one and store the result into register zero
54                   | divi   | Divide the value in register zero by the value in register one using integer division. Store the quotient in register zero and the remainder in register one.


Your implementation must support all instructions described in that table.
All arithmetic integer instructions (opcodes 50–54) should wrap their values on over- and underflow, e.g., adding one to the largest possible integer results in the smallest possible one.
When a program tries to divide by zero with the division instructions (opcode 54), the VM should indicate that by calling the provided function `division_by_zero()` and return zero afterwards.

> Note: In C++ overflow of signed integers is undefined behavior, while this VM explicitly allows it! Your implementation must be able to handle this. Hint: Conversion between signed integer types of different sizes follows the rules of integral conversions and is always allowed.

A program that loads the values 123 and 456 into registers zero and one, adds them, and stores the result in register zero could look like this:
```
10 0 123
10 1 456
20 0
22
20 1
50
21 0
0
```