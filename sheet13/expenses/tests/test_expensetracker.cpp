#include <cstdio>
#include <fstream>
#include <gtest/gtest.h>
#include <sstream>
#include <string>

extern void expense_tracker(int argc, const char** argv, std::istream& input);
// Mock main function for testing
void runExpenseTracker(std::istream& input, const char* expectedOutput, const std::string& filename)
{
    testing::internal::CaptureStdout();

    if (!filename.empty()) {
        const char* argv[] = {"expense-tracker", const_cast<char*>(filename.c_str())};
        expense_tracker(2, argv, input);
    } else {
        const char* argv[] = {"expense-tracker"};
        expense_tracker(1, argv, input);
    }

    if (expectedOutput != nullptr) {
        std::string str(expectedOutput);
        str += "Goodbye!\n";
        ASSERT_EQ(testing::internal::GetCapturedStdout(), str);
    } else {
        testing::internal::GetCapturedStdout();
    }
}

TEST(ExpenseTrackerTests, Exit)
{
    std::string filename = "test_empty.txt";
    std::ofstream file(filename); // Create an empty file
    file.close();

    std::istringstream input("4\n"); // View expenses, then exit
    std::ostringstream output;

    const char* expectedOutput = "Welcome to the Expense Tracker!\n"
                                 "\nYour options:\n"
                                 "1. View all expenses\n"
                                 "2. Add a new expense\n"
                                 "3. Calculate total expenses\n"
                                 "4. Exit\n"
                                 "Choose an option: ";

    runExpenseTracker(input, expectedOutput, filename);
    std::remove(filename.c_str());
}

// Test: Start with an empty file
TEST(ExpenseTrackerTests, StartWithEmptyFile)
{
    std::string filename = "test_empty.txt";
    std::ofstream file(filename); // Create an empty file
    file.close();

    std::istringstream input("1\n4\n"); // View expenses, then exit
    std::ostringstream output;

    const char* expectedOutput = "Welcome to the Expense Tracker!\n"
                                 "\nYour options:\n"
                                 "1. View all expenses\n"
                                 "2. Add a new expense\n"
                                 "3. Calculate total expenses\n"
                                 "4. Exit\n"
                                 "Choose an option: "
                                 "No expenses recorded yet.\n"
                                 "Choose an option: ";

    runExpenseTracker(input, expectedOutput, filename);
    std::remove(filename.c_str());
}

// Test: Add a single expense and view it
TEST(ExpenseTrackerTests, AddAndViewSingleExpense)
{
    std::string filename = "test_add_view.txt";
    std::remove(filename.c_str());

    std::istringstream input("2\nDinner\n30.00\n1\n4\n"); // Add expense, view, then exit

    const char* expectedOutput = "Welcome to the Expense Tracker!\n"
                                 "\nYour options:\n"
                                 "1. View all expenses\n"
                                 "2. Add a new expense\n"
                                 "3. Calculate total expenses\n"
                                 "4. Exit\n"
                                 "Choose an option: "
                                 "Enter description: Enter amount: "
                                 "Choose an option: "
                                 "Description       Amount\n"
                                 "------------------------\n"
                                 "Dinner            30.00\n"
                                 "Choose an option: ";
    runExpenseTracker(input, expectedOutput, filename);
    std::remove(filename.c_str());
}

// Test: Add multiple expenses and calculate the total
TEST(ExpenseTrackerTests, AddMultipleExpensesAndCalculateTotal)
{
    std::string filename = "test_total.txt";
    std::remove(filename.c_str());

    std::istringstream input(
        "2\nGroceries\n45.50\n"
        "2\nElectricity Bill\n120.00\n"
        "2\nInternet Bill\n60.00\n"
        "3\n4\n4\n"); // Add three expenses, calculate total, then exit

    const char* expectedOutput = "Welcome to the Expense Tracker!\n"
                                 "\nYour options:\n"
                                 "1. View all expenses\n"
                                 "2. Add a new expense\n"
                                 "3. Calculate total expenses\n"
                                 "4. Exit\n"
                                 "Choose an option: "
                                 "Enter description: Enter amount: Choose an option: "
                                 "Enter description: Enter amount: Choose an option: "
                                 "Enter description: Enter amount: Choose an option: "
                                 "Total Expenses: 225.50\n"
                                 "Choose an option: ";

    runExpenseTracker(input, expectedOutput, filename);
    std::remove(filename.c_str());
}

// Test: Persist expenses between runs
TEST(ExpenseTrackerTests, PersistExpensesBetweenRuns)
{
    std::string filename = "test_persistence.txt";

    // First run: Add an expense
    {
        std::istringstream input("2\nCoffee\n5.00\n4\n"); // Add expense, then exit
        runExpenseTracker(input, nullptr, filename);
    }

    // Second run: View expenses
    {
        std::istringstream input("1\n4\n"); // View expenses, then exit
        const char* expectedOutput = "Welcome to the Expense Tracker!\n"
                                     "\nYour options:\n"
                                     "1. View all expenses\n"
                                     "2. Add a new expense\n"
                                     "3. Calculate total expenses\n"
                                     "4. Exit\n"
                                     "Choose an option: "
                                     "Description       Amount\n"
                                     "------------------------\n"
                                     "Coffee            5.00\n"
                                     "Choose an option: ";

        runExpenseTracker(input, expectedOutput, filename);
    }
    std::remove(filename.c_str());
}
