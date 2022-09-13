/******************************************************************************
 * Example 4 shows how to make a Parallel Pipeline to calculate the following
 * mathematical expression using 3 parallel threads for the 3 functions F, G
 * and H. There are two streams of input and for iteration i they are denoted
 * x[i] and z[i], and the output is y[i].
 * 
 *      y[i] = H(F(x[i]) + G(z[i]))
 * 
 * This is run in parallel by calculating F(x[i]) in the 1st thread and G(z[i])
 * in the 2nd thread, and saving / buffering the sum of these results to a
 * variable named F_G_sum_buffer, and using this buffer in the 3rd thread to
 * calculate H(F_G_sum_buffer).
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
 * Serial processing of two vectors with elements x[i] and z[i] to produce
 * H(F(x[i]) + G(z[i])) where the functions F, G and H are run in serial.
 * 
 * @param x_vec input data to be processed.
 * @param z_vec input data to be processed.
 */
void serial(vector<string> const& x_vec, vector<string> const& z_vec)
{
    cout << "Serial:" << endl;

    // Start timer.
    Timer timer;

    // For each element in the input vector.
    for (uint i=0; i<x_vec.size(); i++)
    {
        // Input strings for index i.
        string x_i = x_vec[i];
        string z_i = z_vec[i];

        // Output string for index i.
        string y_i = H(sum(F(x_i), G(z_i)));

        // Show result.
        cout << "Step " + to_string(i) + ":  Thread 1: " << y_i << endl;
    }

    // Show the elapsed time.
    cout << timer.elapsed() << endl;
}

/*****************************************************************************/

/**
 * Parallel processing of vectors with elements x[i] and z[i] to produce
 * H(F(x[i]) + G(z[i])) where the functions F, G and H are run in parallel.
 * 
 * @param x_vec input data to be processed.
 * @param z_vec input data to be processed.
 */
void parallel(vector<string> const& x_vec, vector<string> const& z_vec)
{
    cout << "Parallel:" << endl;

    // Start timer.
    Timer timer;

    // Buffered output of sums of functions F and G from previous iteration.
    string F_G_sum_buffer(no_data);

    // For each element in the input vector.
    // Note that we need +1 iteration because of the buffering and threading.
    for (uint i=0; i<x_vec.size() + 1; i++)
    {
        // Input string for index i. Or empty string if we are beyond the end.
        string x_i = (i < x_vec.size()) ? x_vec[i] : no_data;
        string z_i = (i < z_vec.size()) ? z_vec[i] : no_data;

        // Async execution of function F using the current input x_i.
        auto F_future = async(F, x_i);

        // Async execution of function G using the current input z_i.
        auto G_future = async(G, z_i);

        // Async execution of function H using the sum of buffered output
        // of the functions F and G from the previous iteration i-1.
        auto H_future = async(H, F_G_sum_buffer);

        // Wait for the functions to finish processing and get the results.
        string F_result = F_future.get();
        string G_result = G_future.get();
        string H_result = H_future.get();

        // Save the sum of the output of the functions F and G for use as input
        // to the function H in the next iteration of the for-loop.
        F_G_sum_buffer = sum(F_result, G_result);

        // Show result.
        cout << "Step " + to_string(i) + ":  Thread 1: " << F_result
             << "  Thread 2: " << G_result << "  Thread 3: " << H_result << endl;
    }

    // Show the elapsed time.
    cout << timer.elapsed() << endl;
}

/*****************************************************************************/

int main()
{
    // Generate vectors of strings for the input data.
    vector<string> x_vec = gen_vec_string(10, "x");
    vector<string> z_vec = gen_vec_string(10, "z");

    // Serial processing of all the vector elements.
    serial(x_vec, z_vec);

    // Show newline.
    cout << endl;

    // Parallel processing of all the vector elements.
    parallel(x_vec, z_vec);

    // No error.
    return 0;
}

/*****************************************************************************/
