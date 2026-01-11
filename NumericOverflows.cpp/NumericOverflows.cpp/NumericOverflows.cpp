// NumericOverflows.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>     // std::cout
#include <limits>       // std::numeric_limits
#include <typeinfo> // ADDED: Needed for typeid(T).name() so we can print the current type in the test output.

// ADDED: We need a clean way to send two things back to the test code:
//  1) the number we ended up with, and
//  2) whether the operation finished safely.
// This avoids using "special" return values like -1, which can be a real value or behave
// differently for unsigned types, chars, and floating-point numbers.
template <typename T>
struct CalcResult
{
    T value{};            // The result (or the last safe value if we had to stop early)
    bool success{ true }; // true = all steps completed safely, false = we prevented overflow/underflow
};


/// <summary>
/// Template function to abstract away the logic of:
///   start + (increment * steps)
/// </summary>
/// <typeparam name="T">A type that with basic math functions</typeparam>
/// <param name="start">The number to start with</param>
/// <param name="increment">How much to add each step</param>
/// <param name="steps">The number of steps to iterate</param>
/// <returns>start + (increment * steps)</returns>
template <typename T>
CalcResult<T> add_numbers(T const& start, T const& increment, unsigned long int const& steps)
{
    CalcResult<T> out{};   // ADDED: holds both the running total and a success/failure flag
    out.value = start;     // ADDED: start the running total at the starting value

    for (unsigned long int i = 0; i < steps; ++i)
    {
        // ADDED: grab the valid range for this type (int, unsigned, float, etc.)
        // so we can check limits before we change the value.
        const T maxVal = std::numeric_limits<T>::max();
        const T lowVal = std::numeric_limits<T>::lowest(); // ADDED: the lowest value this type can hold

        // ADDED: Check *before* we add.
        // If the next add would push us past the type’s limits, we stop early and report failure.
        if (increment > T{ 0 })
        {
            // Positive increment: would we go above max?
            if (out.value > (maxVal - increment))
            {
                out.success = false; // ADDED: tell the caller we prevented an overflow
                return out;          // ADDED: return the last safe value
            }
        }
        else if (increment < T{ 0 })
        {
            // Negative increment: would we go below the lowest value?
            if (out.value < (lowVal - increment))
            {
                out.success = false; // ADDED: tell the caller we prevented an underflow
                return out;          // ADDED: return the last safe value
            }
        }

        out.value += increment; // UPDATED: safe to add now (we already checked the limits)
    }

    return out; // UPDATED: return both the final value and whether it completed safely
}


/// <summary>
/// Template function to abstract away the logic of:
///   start - (increment * steps)
/// </summary>
/// <typeparam name="T">A type that with basic math functions</typeparam>
/// <param name="start">The number to start with</param>
/// <param name="increment">How much to subtract each step</param>
/// <param name="steps">The number of steps to iterate</param>
/// <returns>start - (increment * steps)</returns>

template <typename T>
CalcResult<T> subtract_numbers(T const& start, T const& decrement, unsigned long int const& steps)
{
    CalcResult<T> out{};   // ADDED: holds both the running total and a success/failure flag
    out.value = start;     // ADDED: start the running total at the starting value

    for (unsigned long int i = 0; i < steps; ++i)
    {
        // ADDED: grab the valid range for this type (int, unsigned, float, etc.)
        // so we can check limits before we change the value.
        const T maxVal = std::numeric_limits<T>::max();
        const T lowVal = std::numeric_limits<T>::lowest();

        // ADDED: Check *before* we subtract.
        // If the next subtract would push us past the type’s limits, we stop early and report failure.
        // This prevents the underflow/overflow from ever happening.
        if (decrement > T{ 0 })
        {
            // Positive decrement: would we go below the lowest value?
            if (out.value < (lowVal + decrement))
            {
                out.success = false; // ADDED: tell the caller we prevented an underflow
                return out;          // ADDED: return the last safe value
            }
        }
        else if (decrement < T{ 0 })
        {
            // Negative decrement: subtracting a negative is the same as adding.
            // Would that push us above max?
            if (out.value > (maxVal + decrement)) // decrement is negative here
            {
                out.success = false; // ADDED: tell the caller we prevented an overflow
                return out;          // ADDED: return the last safe value
            }
        }

        out.value -= decrement; // UPDATED: safe to subtract now (we already checked the limits)
    }

    return out; // UPDATED: return both the final value and whether it completed safely
}



//  NOTE:
//    You will see the unary ('+') operator used in front of the variables in the test_XXX methods.
//    This forces the output to be a number for cases where cout would assume it is a character. 

template <typename T>
void test_overflow()
{
    // TODO: The add_numbers template function will overflow in the second method call
    //        You need to change the add_numbers method to:
    //        1. Detect when an overflow will happen
    //        2. Prevent it from happening
    //        3. Return the correct value when no overflow happened or
    //        4. Return something to tell test_overflow the addition failed
    //        NOTE: The add_numbers method must remain a template in the NumericFunctions header.
    //
    //        You need to change the test_overflow method to:
    //        1. Detect when an add_numbers failed
    //        2. Inform the user the overflow happened
    //        3. A successful result displays the same result as before you changed the method
    //        NOTE: You cannot change anything between START / END DO NOT CHANGE
    //              The test_overflow method must remain a template in the NumericOverflows source file
    //
    //  There are more than one possible solution to this problem. 
    //  The solution must work for all of the data types used to call test_overflow() in main().

    // START DO NOT CHANGE
    //  how many times will we iterate
    const unsigned long int steps = 5;
    // how much will we add each step (result should be: start + (increment * steps))
    const T increment = std::numeric_limits<T>::max() / steps;
    // whats our starting point
    const T start = 0;

    std::cout << "Overflow Test of Type = " << typeid(T).name() << std::endl;
    // END DO NOT CHANGE

    // UPDATED: add_numbers() no longer returns just a number.
    // It returns a small "package" (CalcResult<T>) that includes:
    //   - value: the result we computed (or the last safe value if we had to stop)
    //   - success: whether we finished all the steps without going out of range
    std::cout << "\tAdding Numbers Without Overflow (" << +start << ", " << +increment << ", " << steps << ") = ";

    // UPDATED: Store both the result and the success flag so we can tell if the math was safe.
    auto r1 = add_numbers<T>(start, increment, steps);

    // ADDED: Print a clear status plus the numeric result.
    // boolalpha makes it print true/false instead of 1/0.
    // Overflow is just the opposite of success (if success is false, overflow was prevented).
    std::cout << std::boolalpha
        << "Overflow: " << (!r1.success)
        << " Result: " << +r1.value
        << std::noboolalpha << std::endl;

    std::cout << "\tAdding Numbers With Overflow (" << +start << ", " << +increment << ", " << (steps + 1) << ") = ";

    // UPDATED: This call is meant to push the calculation past the type’s limit.
    // If adding would go above the maximum value, add_numbers() stops early and sets success=false.
    auto r2 = add_numbers<T>(start, increment, steps + 1);

    // ADDED: Show whether we prevented overflow, and show the last safe value we reached.
    std::cout << std::boolalpha
        << "Overflow: " << (!r2.success)
        << " Result: " << +r2.value
        << std::noboolalpha << std::endl;

}

template <typename T>
void test_underflow()
{
    // TODO: The subtract_numbers template function will underflow in the second method call
    //        You need to change the subtract_numbers method to:
    //        1. Detect when an underflow will happen
    //        2. Prevent it from happening
    //        3. Return the correct value when no underflow happened or
    //        4. Return something to tell test_underflow the subtraction failed
    //        NOTE: The subtract_numbers method must remain a template in the NumericFunctions header.
    //
    //        You need to change the test_underflow method to:
    //        1. Detect when an subtract_numbers failed
    //        2. Inform the user the underflow happened
    //        3. A successful result displays the same result as before you changed the method
    //        NOTE: You cannot change anything between START / END DO NOT CHANGE
    //              The test_underflow method must remain a template in the NumericOverflows source file
    //
    //  There are more than one possible solution to this problem. 
    //  The solution must work for all of the data types used to call test_overflow() in main().

    // START DO NOT CHANGE
    //  how many times will we iterate
    const unsigned long int steps = 5;
    // how much will we subtract each step (result should be: start - (increment * steps))
    const T decrement = std::numeric_limits<T>::max() / steps;
    // whats our starting point
    const T start = std::numeric_limits<T>::max();

    std::cout << "Underflow Test of Type = " << typeid(T).name() << std::endl;
    // END DO NOT CHANGE

    // UPDATED: subtract_numbers() no longer returns just a number.
// It returns a small "package" (CalcResult<T>) that includes:
//   - value: the result we computed (or the last safe value if we had to stop)
//   - success: whether we finished all the steps without going out of range
    std::cout << "\tSubtracting Numbers Without Overflow (" << +start << ", " << +decrement << ", " << steps << ") = ";

    // UPDATED: Store both the result and the success flag so we can tell if the math was safe.
    auto r1 = subtract_numbers<T>(start, decrement, steps);

    // ADDED: Print a clear status plus the numeric result.
    // boolalpha makes it print true/false instead of 1/0.
    // Underflow is just the opposite of success (if success is false, underflow was prevented).
    std::cout << std::boolalpha
        << "Underflow: " << (!r1.success)
        << " Result: " << +r1.value
        << std::noboolalpha << std::endl;

    std::cout << "\tSubtracting Numbers With Overflow (" << +start << ", " << +decrement << ", " << (steps + 1) << ") = ";

    // UPDATED: This call is meant to push the calculation past the type’s limit.
    // If subtracting would go below the minimum value, subtract_numbers() stops early and sets success=false.
    auto r2 = subtract_numbers<T>(start, decrement, steps + 1);

    // ADDED: Show whether we prevented underflow, and show the last safe value we reached.
    std::cout << std::boolalpha
        << "Underflow: " << (!r2.success)
        << " Result: " << +r2.value
        << std::noboolalpha << std::endl;
}

void do_overflow_tests(const std::string& star_line)
{
    std::cout << std::endl << star_line << std::endl;
    std::cout << "*** Running Overflow Tests ***" << std::endl;
    std::cout << star_line << std::endl;

    // Testing C++ primative times see: https://www.geeksforgeeks.org/c-data-types/
    // signed integers
    test_overflow<char>();
    test_overflow<wchar_t>();
    test_overflow<short int>();
    test_overflow<int>();
    test_overflow<long>();
    test_overflow<long long>();

    // unsigned integers
    test_overflow<unsigned char>();
    test_overflow<unsigned short int>();
    test_overflow<unsigned int>();
    test_overflow<unsigned long>();
    test_overflow<unsigned long long>();

    // real numbers
    test_overflow<float>();
    test_overflow<double>();
    test_overflow<long double>();
}

void do_underflow_tests(const std::string& star_line)
{
    std::cout << std::endl << star_line << std::endl;
    std::cout << "*** Running Underflow Tests ***" << std::endl; // Fixed typo
    std::cout << star_line << std::endl;

    // Testing C++ primative times see: https://www.geeksforgeeks.org/c-data-types/
    // signed integers
    test_underflow<char>();
    test_underflow<wchar_t>();
    test_underflow<short int>();
    test_underflow<int>();
    test_underflow<long>();
    test_underflow<long long>();

    // unsigned integers
    test_underflow<unsigned char>();
    test_underflow<unsigned short int>();
    test_underflow<unsigned int>();
    test_underflow<unsigned long>();
    test_underflow<unsigned long long>();

    // real numbers
    test_underflow<float>();
    test_underflow<double>();
    test_underflow<long double>();
}

/// <summary>
/// Entry point into the application
/// </summary>
/// <returns>0 when complete</returns>
int main()
{
    //  create a string of "*" to use in the console
    const std::string star_line = std::string(50, '*');

    std::cout << "Starting Numeric Underflow / Overflow Tests!" << std::endl;

    // run the overflow tests
    do_overflow_tests(star_line);

    // run the underflow tests
    do_underflow_tests(star_line);

    std::cout << std::endl << "All Numeric Underflow / Overflow Tests Complete! By Joseph Wilfong" << std::endl;

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu
