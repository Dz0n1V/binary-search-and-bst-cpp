# Binary search and BST

![C++17](https://img.shields.io/badge/C%2B%2B-17-blue.svg)
![License: MIT](https://img.shields.io/badge/license-MIT-green.svg)

| # | Project | Core topic |
|---|---------|------------|
| 1 | [Temperature monitoring](#task-1--temperature-monitoring) | Binary search on a sorted array (first/last occurrence, range queries) |
| 2 | [Hotel loyalty program](#task-2--hotel-loyalty-program-binary-search-tree) | Binary search tree with parent pointers, deletion, balanced construction |

Both programs are menu-driven, use only the C++ standard library, and compile without warnings (`-Wall -Wextra -Wpedantic`).

---

## Table of contents

- [Task 1 – Temperature monitoring](#task-1--temperature-monitoring)
- [Task 2 – Hotel loyalty program (binary search tree)](#task-2--hotel-loyalty-program-binary-search-tree)
- [Repository structure](#repository-structure)
- [Build and run](#build-and-run)
- [Testing](#testing)
- [Design notes and complexity](#design-notes-and-complexity)
- [Limitations](#limitations)
- [License](#license)

---

## Task 1 – Temperature monitoring

A weather station records the air temperature (whole degrees Celsius) every day at noon, for up to 365 days. At the end of the year the readings are kept as an array **sorted in non-decreasing order**.

**Implemented features**

- **Manual input** – the user enters the number of measurements (1–365) and then the temperatures. Values that break the non-decreasing order or are not valid integers are rejected and re-requested.
- **Random generation** – the user gives the number of measurements and a temperature range `min max`. The program draws pseudo-random temperatures (duplicates allowed) with `std::mt19937` and sorts them.
- **Temperature record search** – tells whether a temperature was recorded and returns the **first and last position** where it occurs, using a modified **binary search**.
- **Range search** – lists every reading in `[temp_min, temp_max]` together with its position, using the same binary-search technique.
- Extras: clear all data, print all data.

**Example session** (user input after each `:`)

```
-----MENU-----
1. Manual Data Input
2. Generate Random Data
3. Search Specific Temperature
4. Search Temperatures In Range
5. Clear All Data
6. Print Data
0. Exit
Your Choice: 1
Number Of Measurements (1-365): 6
Temperature 1/6: -5
Temperature 2/6: 0
Temperature 3/6: 0
Temperature 4/6: 0
Temperature 5/6: 12
Temperature 6/6: 20
Successfully Added 6 Measurements.
Your Choice: 3
Temperature In Question: 0
Temperature On First Position: 1
Temperature On Last Position: 3
Your Choice: 4
Temperature Range (min max, separated by a space): -2 12
Temperatures In Range (Temperature, Position): (0, 1), (0, 2), (0, 3), (12, 4)
```

Positions are **0-based** indices into the sorted array.

---

## Task 2 – Hotel loyalty program (binary search tree)

Hotel "Balkan" runs a loyalty program. Each guest collects points for every stay, and the guests are stored in a **binary search tree** keyed by loyalty points. A node holds the points, first name, last name, and pointers to the left child, right child and **parent**.

**Implemented features**

| Feature | Details |
|---------|---------|
| Build the tree | Loads guests from a CSV file and builds a tree of **minimal height** from the sorted list (the middle element becomes the root, recursively, built level by level with a queue) |
| Search by points | Standard BST lookup |
| Increase points | Finds the guest by name, removes the node (CLRS-style `transplant` deletion) and re-inserts it with the new total, so the BST property always holds |
| Print – in order | Lists guests sorted by points |
| Print – parent/children | Level-order dump showing each node with its left and right child |
| Delete the tree | Frees all nodes (also done automatically by the destructor) |
| Closest higher guest | For a given guest, finds the guest with the **closest higher** number of points |

**CSV format** (the first line is a header and is skipped):

```
broj_poena,ime,prezime
180,Petar,Nikolic
265,Jovana,Djordjevic
```

The loader validates every line (three fields, numeric points, non-empty names), reports the offending line number, tolerates Windows line endings, and rejects duplicate entries (same points, first name and last name).

**Example session**

```
-----MENU-----
1. Load Guests From Csv
2. Search Guest By Points
3. Increase Guest Points By Name
4. Print Tree Inorder
5. Print Tree Parent-Children
6. Delete Tree
7. Find Guest With Closest Higher Points By Name
0. Exit
Choose An Option: 1
Enter Csv Filename: data/gosti_poeni_50.csv
Tree Successfully Built With Minimal Height From Csv.
Choose An Option: 2
Enter Points To Search: 180
Guest Found: Petar Nikolic - 180 Points.
Choose An Option: 7
Enter Guest First Name: Marko
Enter Guest Last Name: Lazic
Guest With Closest Higher Points:
435 - Milica Ristic
Choose An Option: 3
Enter Guest First Name: Marko
Enter Guest Last Name: Lazic
Enter Points To Increase: 1000
Points Were Incremented.
Choose An Option: 7
Enter Guest First Name: Marko
Enter Guest Last Name: Lazic
Guest With Closest Higher Points:
1370 - Filip Todorovic
```

---

## Repository structure

```
.
├── src/
│   ├── task1_temperature_monitoring.cpp
│   └── task2_hotel_loyalty_bst.cpp
├── data/
│   ├── gosti_poeni_50.csv      # 50 sample guests
│   └── gosti_poeni_100.csv     # 95 sample guests
├── tests/
│   ├── test_task1.py
│   └── test_task2.py
├── Makefile
├── LICENSE
└── README.md
```

---

## Build and run

Requirements: a C++17 compiler (GCC or Clang) and `make`. Python 3 is only needed for the tests.

```bash
git clone https://github.com/Dz0n1V/binary-search-and-bst-cpp.git
cd binary-search-and-bst-cpp

make                                   # builds both programs into build/

./build/task1_temperature_monitoring
./build/task2_hotel_loyalty_bst
```

Without `make`:

```bash
g++ -std=c++17 -O2 -Wall -Wextra src/task1_temperature_monitoring.cpp -o task1
g++ -std=c++17 -O2 -Wall -Wextra src/task2_hotel_loyalty_bst.cpp -o task2
```

When Task 2 asks for the CSV file name, enter e.g. `data/gosti_poeni_50.csv`.

---

## Testing

```bash
make test
```

This rebuilds both programs with **AddressSanitizer and UndefinedBehaviorSanitizer** and runs two black-box test suites that drive the programs through their menus:

- **Task 1** – 300 randomized scenarios (about 5,400 queries) checked against a brute-force reference: first/last occurrence, missing values, and range queries; plus input validation (unsorted input, bad numbers, limits), re-entering data after clearing, random generation (count, range, ordering) and end-of-input handling.
- **Task 2** – on both sample CSV files: the tree is a valid BST with minimal height, every guest can be found by points, in-order output is sorted, the *closest higher* result is verified for every guest, 40 random point increases keep the tree valid and values correct, deleting the tree and operating on an empty tree works; plus error handling for malformed lines, duplicates, empty/missing files and Windows line endings.

---

## Design notes and complexity

### Task 1 (n = number of measurements, k = number of results)

All searches are built from two binary-search primitives: *first index with value ≥ key* and *first index with value > key*.

| Operation | Time |
|-----------|------|
| First / last occurrence of a temperature | O(log n) |
| Range search | O(log n + k) |
| Random generation | O(n log n) (sorting) |
| Manual input | O(n) |

Using these "bounds" instead of a classic search followed by a linear scan keeps the first/last lookup logarithmic even when a temperature occurs on hundreds of days.

### Task 2 (n = number of guests, h = tree height)

| Operation | Time |
|-----------|------|
| Build from the sorted list | O(n log n) including sorting, O(n) for the construction itself; height = ⌈log₂(n+1)⌉ |
| Search by points | O(h) |
| Increase points (lookup by name + delete + insert) | O(n + h) – the tree is ordered by points, so a lookup by name needs a traversal |
| Closest higher guest (lookup by name + successor search) | O(n + h) |
| In-order print / delete tree | O(n) |

- All traversals and the tree deletion are **iterative** (explicit stack/queue), so there is no risk of stack overflow on degenerate trees.
- Node deletion handles all three cases (no child, one child, two children) via a `transplant` helper that keeps parent pointers consistent.
- `BST` follows RAII: the destructor frees all nodes and copying is disabled.
- Integer input is parsed strictly (`12abc` is rejected), point increments are checked for `int` overflow, and the end of the input stream terminates the program cleanly.

---

## Limitations

- Guests are identified by name for the *increase points* and *closest higher* operations. If several guests share the same first and last name (with different points), the first one in in-order traversal is used.
- The sample CSV files contain the header `broj_poena,ime,prezime` (Serbian for "points, first name, last name"); the loader skips the first line regardless of its content.
- Data lives in memory only and is not saved between runs.

---

## License

Released under the [MIT License](LICENSE).
