# Push-Swap-Tester

Push-Swap-Tester is a program designed to test the performance and correctness of the `push_swap` program. It generates random arrays of integers, runs the `push_swap` program to sort them, and then verifies the results using the `checker` program.

## Installation

To install the program, run the following command:

```sh
make
```

This will compile the necessary components and create the `push_swap_tester` executable.

## Usage

```sh
./push_swap_tester [options]
```

## Options

- `-n <tests>`: Number of tests per array size (default: 10, -1 for infinite)
- `-s <start>`: Minimum array size (default: 1)
- `-e <end>`: Maximum array size (default: 500)
- `-st <steps>`: Testing steps (default: 1)
- `-l <limit>`: Limit of operations to show a warning (default: 5500)
- `-h`: Display this help message

## Example

```sh
./push_swap_tester -n 20 -s 5 -e 100 -st 5 -l 7000
```

This command runs 20 tests for each array size from 5 to 100, incrementing by 5, and sets the operation limit to 7000.

## Output

The program outputs the minimum and maximum number of operations performed by `push_swap` for each array size. It also indicates whether the result is correct (`[OK]`), incorrect (`[KO]`), or if there was an error (`[Error]`).

## Notes

- Ensure that the `push_swap` and `checker` executables are in the same directory as this tester program.
- The program generates an `output.txt` file to store the results of `push_swap` for verification by `checker`.
