# Sheet 13 - I/O and Testing

## Coverage

For this assignment, we changed things up a bit. There is already an implementation provided for two string-matching algorithms - a naive implementation and the implementation according to Knuth-Morris-Pratt.
You don't have to spent any time on the implementation of the string-matching - just assume it is correct and works as intended! 
Your task is to fill the `tests/test_coverage.cpp` with tests to cover all lines of the string-matching implementations.

Tools exist to measure the line coverage of the input program after a test-suite has been run. Your task is to achieve a 100\% line-coverage for all source files.
You can add arbitrary tests to the `tests/test_coverage.cpp` file...

Use the previous homeworks as an example or come up with your own style of testing - the [google-test](https://github.com/google/googletest) setup is already provided.

## Expenses

To practice usage of streams and I/O operations to and from files, in this task, you will create a simple **Expense Tracker** program in C++ that uses file I/O and command-line interaction to manage a list of expenses. 
The program should allow users to:

1. View all recorded expenses.
2. Add new expenses.
3. Calculate the total amount of expenses.
4. Exit the program.

The program will store all expense data in a text file to ensure that the data persists between runs.

### Requirements

You implement the `void expense_tracker(int argc, const char** argv, std::istream& input)` function, which gets the argument from main and an input stream. Depending whether used in the testing environment or inside the CLI, the input corresponds to the console-input or a mocked version of it.

#### File Handling
Expenses should be stored in a text file.
The program should read the file when it starts and write to it whenever a new expense is added.

#### Command-Line Interface
Users will interact with the program through a menu-driven interface.

#### Expense Structure
- Each expense will have:
  - A **description** (e.g., "Groceries", "Electricity Bill").
  - An **amount** - no unit (e.g., `45.50`), stored as double precision floating point.

#### Persistence
Expenses should persist between program runs by saving and reading from the file. However, we do net require any specific format to store the data in (probably something similar to csv is simplest).

### Workflow

When the program starts, it should display a welcome message and present a menu with the following options:
```plaintext
Welcome to the Expense Tracker!

Choose an option:
1. View all expenses
2. Add a new expense
3. Calculate total expenses
4. Exit
```
The user can then select an option by entering the corresponding number and hitting enter.

#### View all Expanses
Print all current expanses in a tabular format:
```plaintext
Description       Amount
------------------------
Groceries         45.50
Electricity Bill  120.00
Internet Bill     60.00

```
- with a fixed header (first 2 lines)
- align left
- up to 18 characters of space for the description
- two decimals for the amount

#### Insert new Expenses
Inserting an expense should start a dialogue, where we first ask for the description (`Enter description:`) and afterwards for the amount (`Enter amount:`) which are both provided by the user. Once entered, the new expense should be immediately persistet to a file and added to the in-memory structure as well.

#### Total Expenses
This should sum up all expenses as print the total amount:
```plaintext
Total Expenses: 165.50
```
- two decimals for the amount

#### Exit
When your try to exit the tracker, the following message should pop up and the program should terminate. 
```plaintext
Goodbye!
```

For the exact formatting in each and every case, consult the tests to get a definite answer.

> for the first time around we actually provide a command-line interface to your written program alongside with "boring" test execution. The normal `eval.sh` generates a `cli` binary in the build folder in addition to running the tests. You can use the `cli` to interface your solutions, try different scenarios and making use of a selfwritten expense tracker


