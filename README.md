# Hash Table Implementation in C

This project provides a basic implementation of a hash table data structure in C. A hash table is a fundamental data structure that allows efficient key-value lookups.

## Features

- Dynamic resizing: The hash table automatically resizes itself to maintain a balanced load factor.
- Collision handling: Collisions are resolved using double hashing.
- Basic operations: The hash table supports key insertion, key lookup, and key deletion.
- Memory management: Memory is properly managed, and there is a function to clean up and deallocate the hash table.

## Usage

A simple example is implemented in main() function. 

To compile and run,

```
gcc hash_table.c prime.c -o test
./test
```

To use this hash table implementation in your C project, follow these steps:

1. Clone this repository:

   ```bash
   git clone https://github.com/WankhedeAmey/project-C-hash_table.git
