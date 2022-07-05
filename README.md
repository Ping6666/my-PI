# my-PI

Compute PI at any precision by Chudnovsky formula, with FFT multiply and binary splitting.

## code

### BigFloat

Support float with any precision. (Hopefully can)

1. Assignment operators
2. Arithmetic operators (prefix / postfix operators)
3. Comparison operators
4. Mathematics
    1. power
    2. reciprocal
    3. inverse square root
5. Arithmetic logic circuits
    1. complementer
    2. fullAdder
    3. fullSubtractor

### FFT (Fast Fourier Transform)

Implement FFT in Big Float multiplication.

1. Convert frequency domain ( uint32_t ) to time domain ( complex<double> )
2. Convert time domain back to frequency domain

### PI_Chudnovsky

[Chudnovsky algorithm - Wikipedia](https://en.wikipedia.org/wiki/Chudnovsky_algorithm)

Compute PI by Chudnovsky formula and binary splitting algorithm.

### BBP_Formula

[Bailey–Borwein–Plouffe formula - Wikipedia](https://en.wikipedia.org/wiki/Bailey%E2%80%93Borwein%E2%80%93Plouffe_formula)

Compute any number that can be represented by BBP (Bailey–Borwein–Plouffe) formula.

## Comments

Just learning my coding skills. 

But still can't find what part causes the overflow problem. In the correct precision, don't know why my code is about 100x slower than most other codes that can compute PI. (don't even said the famous "y-cruncher")

PS. Most of the time spend on final division part of Chudnovsky algorithm.

### BUGs

Can only compute 50k correct digits (5555 9-digit) of PI, because there are unknown overflow problems.

### TODO

1. Deprecate vector in BigFloat or use some faster data structure.
2. Implement BBP algorithm on computing a specific digit (n-th digit) of the particular number.
