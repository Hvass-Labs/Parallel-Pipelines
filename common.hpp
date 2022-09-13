/******************************************************************************
 * Common constants, funtions and classes.
 ******************************************************************************
 * This file is part of: https://github.com/Hvass-Labs/Parallel-Pipelines
 * Published under the MIT License. See the file LICENSE for details.
 * Copyright 2022 by Magnus Erik Hvass Pedersen.
 *****************************************************************************/

#include <string>
#include <thread>
#include <chrono>
#include <vector>

using namespace std;

/*****************************************************************************/

// Indicates there is no data to process.
static string const no_data = "--";

// Sleep-time for each dummy processing function.
static auto const sleep_time = 100ms;

/*****************************************************************************/

/** Dummy processing function F */
string F(string const& x)
{
    // Simulate heavy processing.
    this_thread::sleep_for(sleep_time);

    return "F(" + x + ")";
}

/** Dummy processing function G */
string G(string const& x)
{
    // Simulate heavy processing.
    this_thread::sleep_for(sleep_time);

    return "G(" + x + ")";
}

/** Dummy processing function H */
string H(string const& x)
{
    // Simulate heavy processing.
    this_thread::sleep_for(sleep_time);
    
    return "H(" + x + ")";
}

/** Dummy processing function + */
string sum(string const& x, string const& y)
{
    // This is assumed to be an almost free operation so there is no sleep.
    return x + " + " + y;
}

/*****************************************************************************/

/**
 * Generate a vector of strings, where each string consists of the given prefix
 * and a suffix for its index in the vector.
 * 
 * @param n Number of strings in the vector.
 * @param prefix Prefix for each string.
 * @return Vector of strings.
 */
vector<string> gen_vec_string(int n, string const& prefix)
{
    // Initialize an empty vector of strings.
    vector<string> vec;

    // For each index.
    for (int i=0; i<n; i++)
    {
        // Generate each string and append it to the vector.
        vec.push_back(prefix + "_" + to_string(i));
    }

    return vec;
}

/*****************************************************************************/

/** Timer for measuring elapsed runtime. */
class Timer
{
    private:
        // Type used to measure time.
        using clock_type = std::chrono::high_resolution_clock;

        // Time-stamp for when this object was created.
        clock_type::time_point time_start;

    public:
        // Object constructor.
        Timer() : time_start(clock_type::now()) {}

        /**
         * Measure the elapsed time in milli-sec since the object was created.
         * 
         * @return string with the elapsed time.
         */
        string elapsed()
        {
            // Time now.
            auto time_now = std::chrono::high_resolution_clock::now();

            // Time difference.
            chrono::duration<double, milli> time_dif = time_now - time_start;

            return "Elapsed time: " + to_string(time_dif.count()) + "ms";
        }
};

/*****************************************************************************/
