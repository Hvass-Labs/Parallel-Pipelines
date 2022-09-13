# Parallel Pipelines for Streaming Data

[Original repository on GitHub](https://github.com/Hvass-Labs/Parallel-Pipelines)

Original author is [Magnus Erik Hvass Pedersen](http://www.hvass-labs.org)


## Introduction

This is a demonstration using C++ source-code of a little-known method for parallelizing the computation of serially dependent functions on streaming data, which is a particular kind of Parallel Pipeline. It can be used to turn a serial computation into a parallel computation whenever you have serially chained or nested functions that are working on streaming data.

For example, in audio processing this can be used to make audio effects that are connected in series instead run in parallel on multiple CPU cores. This can be used to greatly improve the multi-core CPU efficiency of Digital Audio Workstations (DAW).

All of this is explained in more detail in the [paper](https://github.com/Hvass-Labs/Parallel-Pipelines/raw/main/pedersen2022parallel-pipelines.pdf).


## Example

Consider the expression `y[i] = G(F(x[i]))` where `F` and `G` are some functions, and `x[i]` is the input data and `y[i]` is the output data for index `i`. Because the output of the function `F` is used as the input to the function `G`, the two functions are said to be serially dependent and it may seem impossible to calculate them in parallel.

But when `x[i]` is just one element in a stream of data e.g. with `i` going from `0` to some value `n-1`, then we can parallelize the nested computation of the functions `F` and `G` by first computing `F(x[i])` in one thread and saving the result in a variable called `F_buffer`, and in the second thread we then use the buffer that was written in the previous iteration to compute `y[i-1] = G(F_buffer)`. Once the two threads are both finished with their computations, we update `F_buffer` with the new result from the function `F`. This can be written as simplified pseudo-code:

    for (int i=0; i<n+1; i++)
    {
        Use thread 1 to calculate F(x[i]);
        Use thread 2 to calculate G(F_buffer);
        Wait for both threads to finish;
        Get final result from thread 2: y[i-1] = G(F_buffer);
        Update buffer with result from thread 1: F_buffer = F(x[i]);
    }

The only drawback of doing the computation as a Parallel Pipeline, is that it needs 1 extra iteration to finish the entire stream of input-data, because the output for iteration `i` is `y[i-1]` due to the buffering. If this is being used in a real-time application, then it would mean an extra iteration of latency / delay before the final result is available, which may be undesirable. If the input data `x[i]` is actually an array or block of numbers, as is the case with audio processing, then the latency can be reduced by using mini-blocks instead, as explained in the [paper](https://github.com/Hvass-Labs/Parallel-Pipeline/raw/main/pedersen2022parallel-pipelines.pdf). 


## Source-Code in C++

The source-code in C++ gives 4 examples of different expressions that can be calculated in a Parallel Pipeline. From these examples you can understand the underlying idea of buffering the output of each thread, and create a Parallel Pipeline for your own particular problem.

For simplicity, these examples use "dummy" functions with strings as input and output data. The dummy functions "sleep" their execution thread for 100 msec to simulate heavy processing. The summation + operator does not "sleep" the thread in these examples.

- `main1.cpp` shows how to calculate `y[i] = G(F(x[i]))` using 2 parallel threads.
- `main2.cpp` shows how to calculate `y[i] = H(G(F(x[i])))` using 3 parallel threads.
- `main3.cpp` shows how to calculate `y[i] = F(x[i]) + G(F(x[i]))` using 2 parallel threads.
- `main4.cpp` shows how to calculate `y[i] = H(F(x[i]) + G(z[i]))` using 3 parallel threads.


## How To Run

The easiest way to download and install this is by using `git` from the command-line:

    git clone https://github.com/Hvass-Labs/Parallel-Pipelines.git

This creates the directory `Parallel-Pipelines` and downloads all the files to it.

You can also [download](https://github.com/Hvass-Labs/Parallel-Pipelines/archive/master.zip) the contents of the GitHub repository as a Zip-file and extract it manually.

To build the C++ source-code into executable files, run the following commands in a shell. This was implemented on Linux and uses the `g++` compiler and `make` tools:

    cd Parallel-Pipelines
    make -B

This should have created four executable files named `main1` to `main4` which can be run as follows:

    ./main1

This prints the following output to the screen. Note that the parallel execution has finished in nearly half the time of the serial execution, except for approximately 100 msec, which is the extra latency of one iteration, as described above. This can also be seen by the `G(--)` in the first iteration and `F(--)` in the last iteration, which means that the functions are called with empty input at the start and end of the input stream.

    Serial:
    Step 0:  Thread 1: G(F(x_0))
    Step 1:  Thread 1: G(F(x_1))
    Step 2:  Thread 1: G(F(x_2))
    Step 3:  Thread 1: G(F(x_3))
    Step 4:  Thread 1: G(F(x_4))
    Step 5:  Thread 1: G(F(x_5))
    Step 6:  Thread 1: G(F(x_6))
    Step 7:  Thread 1: G(F(x_7))
    Step 8:  Thread 1: G(F(x_8))
    Step 9:  Thread 1: G(F(x_9))
    Elapsed time: 2005.375444ms

    Parallel:
    Step 0:  Thread 1: F(x_0)  Thread 2: G(--)
    Step 1:  Thread 1: F(x_1)  Thread 2: G(F(x_0))
    Step 2:  Thread 1: F(x_2)  Thread 2: G(F(x_1))
    Step 3:  Thread 1: F(x_3)  Thread 2: G(F(x_2))
    Step 4:  Thread 1: F(x_4)  Thread 2: G(F(x_3))
    Step 5:  Thread 1: F(x_5)  Thread 2: G(F(x_4))
    Step 6:  Thread 1: F(x_6)  Thread 2: G(F(x_5))
    Step 7:  Thread 1: F(x_7)  Thread 2: G(F(x_6))
    Step 8:  Thread 1: F(x_8)  Thread 2: G(F(x_7))
    Step 9:  Thread 1: F(x_9)  Thread 2: G(F(x_8))
    Step 10:  Thread 1: F(--)  Thread 2: G(F(x_9))
    Elapsed time: 1107.676666ms


## License (MIT)

This is published under the [MIT License](https://github.com/Hvass-Labs/Parallel-Pipelines/blob/main/LICENSE) which allows very broad use for both academic and commercial purposes.

You are very welcome to modify and use this source-code in your own project. Please keep a link to the [original repository](https://github.com/Hvass-Labs/Parallel-Pipelines).
