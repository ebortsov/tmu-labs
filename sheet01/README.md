# Sheet 01 - Project Setup with CMake

For the first week, you will start by setting up a new C++ project from scratch using CMake.
You are challenged to setup a working CMake project, which provides a `helloworld` target.

The executable is supposted to print `Hello World!` when called with now arguments and otherwise print `Hello <first-argument>!` both followed by a new line.

Examples:
```sh
~$ build/helloworld
Hello World!
~$ build/helloworld Flo
Hello Flo!
~$ build/helloworld "Mateusz Gienieczko"
Hello Mateusz Gienieczko!

```

The `eval.sh` script shows you which exact command will be executed to evaluate your solution.

> Take this weeks exercise as a chance to familiarize yourself with the assignment system, the expected working environment and other tooling around your journey of learning C++

Some information on the additional files in your repository (they will be part of all repositories in the future):
> We dont expect you to understand these (yet)
- `.gitignore` specifies some foles/folders that should not be included in your git submission (so far we exclude the build folder)
- `.clang-tidy` specifies the clang-tidy rules that your submission is checked against (can't be changed by you)
- `.clang-format` specifies the clang-format rules that your submission is format-checked against (can't be changed by you)
- `.deadline` specifies the deadline for the homework in GMT (can't be changed by you)
- `.keep` specifies the files/folders that are restored before the evaluation on our assessment servers (can't be changed by you)
- `Dockerfile` specifies the exact environment in which your submission is evaluated during grading (can't be changed by you)
- `assessment.sh` evaluates your solution locally by invoking the `eval.sh` scripts for each task.
Run it to grade your solution locally with `docker run --rm -v $PWD/:/homework <image-name> /homework/assessment.sh`
- `eval.sh` is used to assign points to a subtask (can't be changed by you)