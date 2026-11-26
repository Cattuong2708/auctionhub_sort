#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <chrono>
#include <random>
#include <algorithm>
#include <iomanip>

using namespace std;
using namespace std::chrono;

struct Bid {
    string bidderId;
    double amount;
    long long timestamp;
    int auctionId;
};

void printBids(const vector<Bid>& v, size_t limit = 10) {
    for (size_t i = 0; i < min(v.size(), limit); ++i) {
        cout << "[" << v[i].bidderId << "] $" << fixed << setprecision(2) << v[i].amount
             << " @ " << v[i].timestamp << "\n";
    }
    if (v.size() > limit) cout << "... (" << v.size() << " total)\n";
}

bool loadBids(const string& path, vector<Bid>& out) {
    out.clear();
    ifstream f(path);
    if (!f) return false;
    Bid b;
    while (f >> b.bidderId >> b.amount >> b.timestamp >> b.auctionId) {
        out.push_back(b);
    }
    return true;
}

bool isLess(const Bid& a, const Bid& b) {
    if (a.amount != b.amount) return a.amount < b.amount;
    return a.timestamp < b.timestamp;
}

bool isSortedAsc(const vector<Bid>& v) {
    for (size_t i = 1; i < v.size(); ++i) {
        if (isLess(v[i], v[i - 1])) return false;
    }
    return true;
}

void generateTestFiles() {
    ofstream fSmall("test_data/small.txt");
    if (fSmall) {
        fSmall << "alice 100.0 1000 1\n";
        fSmall << "bob 102.5 1005 1\n";
        fSmall << "carol 101.0 1003 1\n";
        fSmall << "dave 103.0 1010 1\n";
    }

    ofstream fLarge("test_data/large.txt");
    if (fLarge) {
        mt19937 rng(2026);
        uniform_real_distribution<double> priceDist(10.0, 50000.0);
        uniform_int_distribution<long long> timeDist(1700000000000LL, 1700099999999LL);
        uniform_int_distribution<int> auctionDist(1, 500);

        fLarge << fixed << setprecision(2);
        for (int i = 0; i < 100000; ++i) {
            fLarge << "user" << (i % 10000) << " "
                   << priceDist(rng) << " "
                   << timeDist(rng) << " "
                   << auctionDist(rng) << "\n";
        }
    }
}

void insertionSort(vector<Bid>& v) {
    int n = (int)v.size();
    for (int i = 1; i < n; ++i) {
        Bid key = v[i];
        int j = i - 1;
        while (j >= 0 && isLess(key, v[j])) {
            v[j + 1] = v[j];
            --j;
        }
        v[j + 1] = key;
    }
}

void selectionSort(vector<Bid>& v) {
    int n = (int)v.size();
    for (int i = 0; i < n - 1; ++i) {
        int maxIdx = i;
        for (int j = i + 1; j < n; ++j) {
            if (v[j].amount > v[maxIdx].amount) {
                maxIdx = j;
            }
        }
        if (maxIdx != i) {
            swap(v[i], v[maxIdx]);
        }
    }
}

void interchangeSort(vector<Bid>& v) {
    int n = (int)v.size();
    long long swaps = 0;
    for (int i = 0; i < n - 1; ++i) {
        for (int j = i + 1; j < n; ++j) {
            if (v[i].amount > v[j].amount) {
                swap(v[i], v[j]);
                ++swaps;
            }
        }
    }
    cout << "Total swaps: " << swaps << "\n";
}

bool bubbleSortEarlyStop(vector<Bid>& v) {
    int n = (int)v.size();
    bool stableOnFirstPass = true;
    for (int i = 0; i < n - 1; ++i) {
        bool swapped = false;
        for (int j = 0; j < n - 1 - i; ++j) {
            if (v[j].amount > v[j + 1].amount) {
                swap(v[j], v[j + 1]);
                swapped = true;
            }
        }
        if (i == 0 && swapped) {
            stableOnFirstPass = false;
        }
        if (!swapped) break;
    }
    return stableOnFirstPass;
}

int partitionMedianOfThree(vector<Bid>& v, int lo, int hi) {
    int mid = lo + (hi - lo) / 2;
    if (isLess(v[mid], v[lo])) swap(v[lo], v[mid]);
    if (isLess(v[hi], v[lo])) swap(v[lo], v[hi]);
    if (isLess(v[hi], v[mid])) swap(v[mid], v[hi]);
    swap(v[mid], v[hi]);

    Bid pivot = v[hi];
    int i = lo - 1;
    for (int j = lo; j < hi; ++j) {
        if (isLess(v[j], pivot)) {
            ++i;
            swap(v[i], v[j]);
        }
    }
    swap(v[i + 1], v[hi]);
    return i + 1;
}

void quickSort(vector<Bid>& v, int lo, int hi) {
    if (lo < hi) {
        int p = partitionMedianOfThree(v, lo, hi);
        quickSort(v, lo, p - 1);
        quickSort(v, p + 1, hi);
    }
}

void quickSortWrapper(vector<Bid>& v) {
    if (!v.empty()) {
        quickSort(v, 0, (int)v.size() - 1);
    }
}

int main() {
    generateTestFiles();

    vector<Bid> smallData;
    if (!loadBids("test_data/small.txt", smallData)) {
        cerr << "Cannot open test_data/small.txt\n";
        return 1;
    }

    cout << "=== Task A: Insertion Sort (Ascending) ===\n";
    vector<Bid> taskA = smallData;
    insertionSort(taskA);
    printBids(taskA);

    cout << "\n=== Task B: Selection Sort (Top 3 Descending) ===\n";
    vector<Bid> taskB = smallData;
    selectionSort(taskB);
    printBids(taskB, 3);

    cout << "\n=== Task C: Interchange Sort (Ascending) ===\n";
    vector<Bid> taskC = smallData;
    interchangeSort(taskC);
    printBids(taskC);

    cout << "\n=== Task D: Bubble Sort Early Stop ===\n";
    vector<Bid> sortedCase = taskA;
    bool s1 = bubbleSortEarlyStop(sortedCase);
    cout << "Already-sorted input -> " << (s1 ? "STABLE" : "UNSTABLE") << "\n";

    vector<Bid> reversedCase = taskB;
    bool s2 = bubbleSortEarlyStop(reversedCase);
    cout << "Reversed input       -> " << (s2 ? "STABLE" : "UNSTABLE") << "\n";

    cout << "\n=== Task E: Quick Sort (100,000 bids) ===\n";
    vector<Bid> largeData;
    if (!loadBids("test_data/large.txt", largeData)) {
        cerr << "Cannot open test_data/large.txt\n";
        return 1;
    }

    auto start = high_resolution_clock::now();
    quickSortWrapper(largeData);
    auto end = high_resolution_clock::now();
    double ms = duration<double, milli>(end - start).count();

    cout << "Records sorted: " << largeData.size() << "\n";
    cout << "Runtime: " << fixed << setprecision(2) << ms << " ms\n";
    cout << "Verified sorted: " << (isSortedAsc(largeData) ? "YES" : "NO") << "\n";
    printBids(largeData, 5);

    return 0;
}