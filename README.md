# compression-algorithms
 
## LZW (Encoder & Decoder)
Algorithm description: https://youtu.be/8uFqfZOiwMc. By far the best video detailing LZW that I found.

This LZW encoder uses the tree data structure for dictionary searching. Which made it exceptionally faster compared to traditional for loops.
Note: I didn't delete any object instantiated by the new keyword.

|   Benchmarks  |            	|
|---------------|-------------|
| File          | enwik8      |
| Size          | 100MB       |
| Comp. ratio 	 | 2.75       	|
| Encoding time | 65sec~70sec |
| Decoding time | 6sec~9sec 	 |

| Hardware |                                        |
|----------|----------------------------------------|
| CPU      | 2.6 GHz Dual-Core Intel Core i5 (2013) |
| RAM      | 8 GB 1600 MHz DDR3                     |
| Storage  | SSD                                    |

| Software    |                        |
|-------------|------------------------|
| OS          | MacOS Catalina 10.15.2 |
| Language    | C++17                  |
| Compliler   | clang                  |
| Text editor | VSCode                 |

## Arithmitic coding (Encoder)

The implementation of arithmetic encoding here is the one described in the book 
named "Handbook of Data Compression" pages (270, 271, 272, 273, 274, 275). 
The book is published by "Springer", so it's free on EKB (if you are Egyptian).
The book explained it using decimal numbers but I used binary numbers for max compression.

The solution to underflow problem is detailed here "http://michael.dipperstein.com/arithmetic/index.html"
and here "https://philipstel.wordpress.com/2010/12/21/arithmetic-coding-algorithm-and-implementation-issues-2/".

This implementation of arithmetic coding encoded 40MB in about 15 seconds on my labtop (specs above).
