# AuctionHub Reflection

## 1. Algorithm justification
- A. Insertion sort: In a live auction, new bids arrive in nearly-sorted order with only occasional out-of-order entries. Insertion sort is ideal here because its inner loop stops as soon as it finds a smaller element, giving O(n) best-case / near-linear performance with very low constant overhead and no extra memory.
- B. Selection sort: The "Top 3 Bidders" widget only needs to repeatedly extract the highest remaining bid from a small set. Selection sort naturally places the maximum element at the front on each pass in-place and performs at most n - 1 swaps (O(n) writes).
- C. Interchange sort: Although interchange sort is O(n²), it is acceptable here because the batch size is capped at n ≤ 50. At that scale, n² is at most 2,500 comparisons (under a millisecond), so simple, easy-to-verify QA code is more practical than a complex O(n log n) algorithm.
- D. Bubble sort (early stop): Bubble sort directly compares adjacent pairs across the array. With a `swapped` flag, it can detect a stabilized (already-sorted) price window in a single O(n) pass and terminate immediately instead of running unnecessary iterations.
- E. Quick sort: End-of-day reports require sorting hundreds of thousands or millions of historical bids, where O(n²) algorithms are too slow. Quick sort runs in O(n log n) average time, works in-place with O(log n) stack space, and has strong cache locality.

## 2. Quick Sort runtime on 100,000 records
- Measured runtime: 17.72 ms (verified sorted: YES).
- Machine & compiler: Windows PC, compiled with `g++ -O2` (GCC / MSYS2).

## 3. One bug I encountered
In Task D (`bubbleSortEarlyStop`), my first version returned `!swapped` at the end of the function. When I tested it on the reversed array, it still printed `STABLE` because the loop always ends on a pass with no swaps once the array finishes sorting. I diagnosed it by tracing the `swapped` flag on the reversed test case, and fixed it by adding a separate `stableOnFirstPass` boolean that turns `false` if any swap happens when `i == 0`.

## 4. Three prompts I sent to the LLM
1. "Generate C++ code using std::mt19937 to produce 100,000 random Bid structs and write them to a file." -> Green, because it is only utility code for generating test data, not a sorting algorithm.
2. "Explain how median-of-three pivot selection works in Lomuto partition." -> Green, because I asked for a conceptual explanation before implementing it myself.
3. "My bubble sort with early stop returns STABLE even on reversed input; what is wrong with my flag condition?" -> Green, because I used the LLM to help debug my own logic.

## 5. Worst case of Quick Sort with median-of-three
Yes, the worst-case time complexity is still O(n²). Median-of-three avoids the common O(n²) trap on already-sorted or reverse-sorted data, which is why it works well in practice. However, because it is still a deterministic pivot rule, a specially crafted adversarial input (a "median-of-3 killer" sequence) can still force unbalanced partitions at every recursive call.