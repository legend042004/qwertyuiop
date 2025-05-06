#include <iostream>
#include <vector>
#include <cstdlib>
#include <ctime>
#include <omp.h>

using namespace std;

void sequentialBubbleSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n-1; ++i)
        for (int j = 0; j < n-i-1; ++j)
            if (arr[j] > arr[j+1])
                swap(arr[j], arr[j+1]);
}

void parallelBubbleSort(vector<int>& arr) {
    int n = arr.size();
    for (int i = 0; i < n; ++i) {
        int start = i % 2;
        #pragma omp parallel for
        for (int j = start; j < n - 1; j += 2) {
            if (arr[j] > arr[j + 1])
                swap(arr[j], arr[j + 1]);
        }
    }
}

void merge(vector<int>& arr, int l, int m, int r) {
    int n1 = m - l + 1, n2 = r - m;
    vector<int> L(n1), R(n2);

    for (int i = 0; i < n1; ++i) L[i] = arr[l + i];
    for (int i = 0; i < n2; ++i) R[i] = arr[m + 1 + i];

    int i = 0, j = 0, k = l;
    while (i < n1 && j < n2)
        arr[k++] = (L[i] <= R[j]) ? L[i++] : R[j++];
    while (i < n1) arr[k++] = L[i++];
    while (j < n2) arr[k++] = R[j++];
}

void sequentialMergeSort(vector<int>& arr, int l, int r) {
    if (l < r) {
        int m = l + (r - l) / 2;
        sequentialMergeSort(arr, l, m);
        sequentialMergeSort(arr, m + 1, r);
        merge(arr, l, m, r);
    }
}

void parallelMergeSort(vector<int>& arr, int l, int r, int depth = 0) {
    if (l < r) {
        int m = l + (r - l) / 2;

        if (depth < 4) {
            #pragma omp parallel sections
            {
                #pragma omp section
                parallelMergeSort(arr, l, m, depth + 1);
                #pragma omp section
                parallelMergeSort(arr, m + 1, r, depth + 1);
            }
        } else {
            parallelMergeSort(arr, l, m, depth + 1);
            parallelMergeSort(arr, m + 1, r, depth + 1);
        }

        merge(arr, l, m, r);
    }
}

void printPerformance(const string& label, double time) {
    cout << label << " Time: " << time << " seconds\n";
}

int main() {
    const int SIZE = 10000;
    vector<int> arr1(SIZE), arr2, arr3, arr4;

    srand(time(0));
    for (int i = 0; i < SIZE; ++i) arr1[i] = rand() % 100000;

    arr2 = arr3 = arr4 = arr1;

    // Sequential Bubble Sort
    double t1 = omp_get_wtime();
    sequentialBubbleSort(arr1);
    double t2 = omp_get_wtime();
    printPerformance("Sequential Bubble Sort", t2 - t1);

    // Parallel Bubble Sort
    t1 = omp_get_wtime();
    parallelBubbleSort(arr2);
    t2 = omp_get_wtime();
    printPerformance("Parallel Bubble Sort", t2 - t1);

    // Sequential Merge Sort
    t1 = omp_get_wtime();
    sequentialMergeSort(arr3, 0, arr3.size() - 1);
    t2 = omp_get_wtime();
    printPerformance("Sequential Merge Sort", t2 - t1);

    // Parallel Merge Sort
    t1 = omp_get_wtime();
    #pragma omp parallel
    {
        #pragma omp single
        parallelMergeSort(arr4, 0, arr4.size() - 1);
    }
    t2 = omp_get_wtime();
    printPerformance("Parallel Merge Sort", t2 - t1);

    return 0;
}
