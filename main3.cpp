/******************************************************************************
 * Example 3 shows how to make a Parallel Pipeline to calculate the following
 * mathematical expression using two parallel threads for the two functions F
 * and G. The input for iteration i is denoted x[i] and the output is y[i].
 * 
 *      y[i] = F(x[i]) + G(F(x[i]))
 * 
 * This is run in parallel by calculating F(x[i]) in one thread and saving or
 * buffering the result to a variable named F_buffer, and using this in the
 * 2nd thread to calculate G(F_buffer), and then adding the results.
 * 
 * This introduces 1 extra iteration of latency.
 ******************************************************************************
 * This file is part of: https://github.com/Hvass-Labs/Parallel-Pipelines
 * Published under the MIT License. See the file LICENSE for details.
 * Copyright 2022 by Magnus Erik Hvass Pedersen.
 *****************************************************************************/

#include <iostream>
#include <string>
#include <thread>
#include <future>
#include <vector>

#include "common.hpp"

using namespace std;

/*****************************************************************************/

/**
 * Serial processing of a vector with element x[i] to produce
 * F(x[i]) + G(F(x[i])) where the functions F and G are run in serial.
 * 
 * @param x_vec input data to be processed.
 */
void serial(vector<string> const& x_vec)
{
    cout << "Serial:" << endl;

    // Start timer.
    Timer timer;

    // For each element in the input vector.
    for (uint i=0; i<x_vec.size(); i++)
    {
        // Input string for index i.
        string x_i = x_vec[i];

        // Calculate F(x_i) and save its result to avoid calculating it twice.
        string F_x_i = F(x_i);

        // Output string for index i.
        string y_i = sum(F_x_i, G(F_x_i));

        // Show result.
        cout << "Step " + to_string(i) + ":  Thread 1: " << y_i << endl;
    }

    // Show the elapsed time.
    cout << timer.elapsed() << endl;
}

/*****************************************************************************/

/**
 * Parallel processing of a vector with elements x[i] to produce
 * F(x[i]) + G(F(x[i])) where the functions F and G are run in parallel.
 * 
 * @param x_vec input data to be processed.
 */
void parallel(vector<string> const& x_vec)
{
    cout << "Parallel:" << endl;

    // Start timer.
    Timer timer;

    // Buffered output of function F from the previous iteration.
    string F_buffer(no_data);

    // For each element in the input vector.
    // Note that we need +1 iteration because of the buffering and threading.
    for (uint i=0; i<x_vec.size() + 1; i++)
    {
        // Input string for index i. Or empty string if we are beyond the end.
        string x_i = (i < x_vec.size()) ? x_vec[i] : no_data;

        // Async execution of function F using the current input x_i.
        auto F_future = async(F, x_i);

        // Async execution of function G using the buffered output of the
        // function F from the previous iteration i-1.
        auto G_future = async(G, F_buffer);

        // Wait for the functions to finish processing and get the results.
        string F_result = F_future.get();
        string G_result = G_future.get();

        // Output string for index i. Summation is assumed to be almost "free"
        // so it can be done in the main thread. Note that we sum the buffered
        // output of the function F for the previous input x_{i-1} so the index
        // matches with the output of the function G.
        string y = sum(F_buffer, G_result);

        // Save the output of the function F for use as input to the function G
        // in the next iteration of the for-loop.
        F_buffer = F_result;

        // Show result.
        cout << "Step " + to_string(i) + ":  Thread 1: " << F_result
             << "  Thread 2: " << G_result << "  Thread Main: " << y << endl;
    }

    // Show the elapsed time.
    cout << timer.elapsed() << endl;
}

/*****************************************************************************/

int main()
{
    // Generate vector of strings for the input data.
    vector<string> x_vec = gen_vec_string(10, "x");

    // Serial processing of all the vector elements.
    serial(x_vec);

    // Show newline.
    cout << endl;

    // Parallel processing of all the vector elements.
    parallel(x_vec);

    // No error.
    return 0;
}

/*****************************************************************************/
