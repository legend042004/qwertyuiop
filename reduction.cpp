#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <limits>
#include <omp.h>

using namespace std;

int main() {
    const int SIZE = 1000000;
    vector<int> arr(SIZE);

    // Fill the array with random numbers
    srand(static_cast<unsigned int>(time(0)));
    for (int i = 0; i < SIZE; ++i)
        arr[i] = rand() % 10000;

    // Sequential computation
    int seq_min = INT_MAX;
    int seq_max = INT_MIN;
    long long seq_sum = 0;

    double start_seq = omp_get_wtime();
    for (int i = 0; i < SIZE; ++i) {
        if (arr[i] < seq_min) seq_min = arr[i];
        if (arr[i] > seq_max) seq_max = arr[i];
        seq_sum += arr[i];
    }
    double seq_avg = static_cast<double>(seq_sum) / SIZE;
    double end_seq = omp_get_wtime();

    // Parallel computation using OpenMP
    int par_min = INT_MAX;
    int par_max = INT_MIN;
    long long par_sum = 0;

    double start_par = omp_get_wtime();
    #pragma omp parallel for reduction(min:par_min) reduction(max:par_max) reduction(+:par_sum)
    for (int i = 0; i < SIZE; ++i) {
        int val = arr[i];
        if (val < par_min) par_min = val;
        if (val > par_max) par_max = val;
        par_sum += val;
    }
    double par_avg = static_cast<double>(par_sum) / SIZE;
    double end_par = omp_get_wtime();

    // Output sequential results
    cout << "Sequential Results:\n";
    cout << "Min     = " << seq_min << "\n";
    cout << "Max     = " << seq_max << "\n";
    cout << "Sum     = " << seq_sum << "\n";
    cout << "Average = " << seq_avg << "\n";
    cout << "Execution Time: " << (end_seq - start_seq) << " seconds\n\n";

    // Output parallel results
    cout << "Parallel Results:\n";
    cout << "Min     = " << par_min << "\n";
    cout << "Max     = " << par_max << "\n";
    cout << "Sum     = " << par_sum << "\n";
    cout << "Average = " << par_avg << "\n";
    cout << "Execution Time: " << (end_par - start_par) << " seconds\n";

    return 0;
}