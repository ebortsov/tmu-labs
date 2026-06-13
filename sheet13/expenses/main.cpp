#include <iostream>

extern void expense_tracker(int argc, const char** argv, std::istream& input);

int main(int argc, char* argv[])
{
    expense_tracker(argc, const_cast<const char**>(argv), std::cin);
}