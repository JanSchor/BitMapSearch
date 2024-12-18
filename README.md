
# BitMapSearch
University project for searching for patterns inside bitmap.
Entire program runs inside the command line.
Based on starting argument of program, it should do one of five things:

 - `--help` should print the help of program to show all the options for starting arguments.
 - `test` should check for validity of the bitmap. The bitmap must fulfill some rules.
 - `hline` should search for the longest horizontal line inside the bitmap.
 - `vline` should search for the longest vertical line inside the bitmap.
 - `square` should search for biggest square, the square does not need to be filled.

After the program start argument, there should be the filename of file containing the bitmap.
Inside the repo, there is a file with simple bitmap named `map.txt`.

## BitMap
The bitmap has some specific rules on how should it look.
First two numbers should tell the size of bitmap (columns, rows).
After that there should be data of the bitmap.
Valid bitmap has only numbers `0` and `1`.
Example of the map is here:
```
4 5
0 0 1 1 1
0 0 1 0 1
1 0 1 1 1
1 1 1 1 1
```
The bitmap is considered valid even if there is more than one white char between numbers.
New line characters are not necessary so the bitmap on just one line is considered valid.

## Algorithm used
The main part of the program is the algorithm used for finding the patterns.
For that I made my own algorithm.

### Processing the data
The most important part of the algorithm is at the data procesing.
The bitmap is not loaded as `1`'s and `0`'s, but as two values representing number of `1`'s above and to the left of each bit.
For better understanding, here is picture of the loaded bitmap:

<picture>
  <source media="(prefers-color-scheme: dark)" srcset="https://github.com/JanSchor/JanSchor/blob/main/images/bitmap-conv-dark.png?raw=true">
  <source media="(prefers-color-scheme: light)" srcset="https://github.com/JanSchor/JanSchor/blob/main/images/bitmap-conv-light.png?raw=true">
  <img alt="Algorith conversion image">
</picture>

In the left side of picture, we can see the bitmap on the input.
Right side shows the processed bitmap.
For example the green pair `(3 3)` says that there is 3x `1` to the left (itself included) and 3x `1` upwards (itself included).
So the first value says how many `1`'s are above (until `0` is found).
The second number does the same, but upwards.
Calculating every next number is very simple, because we just need to check values to the **left** and **up** and add 1 to it.

### Searching for patterns
After this it is very simple to find biggest horizontal and vertical line.
For **horizontal** we just need to check for biggest second number.
For **verical**, we need to check for biggest first number.

**Squares** are a little bit more complicated.
We need to check every pair of numbers and find the lower of these two. In example above, we can see the green pair `(3 3)`.
This pair indicates, that there are 3 `1`'s to the left and 3 `1`'s upwards.
Lower number of these two is 3 (they are both the same).
With this information we know, that we can construct square of max size 3.
By the (3, 3) par we already know that 2 sides of the square are completed.
We just need to check for the rest.
We look 2 bits upwars and we can se, there is number 3 on second place (red 3).
This tells the algorithm, that there are 3 `1`s to the left.
This means, that we know of 3 sides of the square and we just need to check for the lsat one.
For that, we look 2 positions to the left from original green `(3 3)`.
Since there is number 3 on first place (blue 3), we know, there are 3 `1`'s upwards.
Now we know, there is 3 by 3 square at these coordinates.

For better understanding, I made this animation that shows the entire process of the algorithm:

![Convertion](https://github.com/JanSchor/JanSchor/blob/main/gifs/bitmap-conv-anim-dark.gif?raw=true")

## How to run
You need [*GCC*](https://gcc.gnu.org/install/) installed on your machine.
After instalation run following command in the folder with the source file.

`cc -std=c11 -Wall -Wextra -Werror figsearch.c -o figsearch`

After compiling run the following command to start the program:

`./figsearch argument FILE`

As `argument` choose any option from the list above.
For `FILE` choose any bitmap file. There is example map in the repository.
Argument `--help` does not accept any file.

### Program output
After running the program, specific output for each argument is printed.

 - The `--help` argument prints just list of all the arguments.
 - The `test` argument prints only two statements `valid` or `invalid`.
 - All the search arguments (`hline`, `vline`, `square`) print out four numbers. First two represent starting coordinates of the shape, second two represent ending coordinates of the shape (columns first).

Example input and output:
```
./figsearch hline map.txt
3 0 3 4

./figsearch square map.txt
0 2 2 4
```