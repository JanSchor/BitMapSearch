
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

// Error codes

// Code for starting program with too many arguments
#define TOO_MANY_ARGS_CODE -1
// Code for starting program with too few arguments
#define TOO_FEW_ARGS_CODE 0
// Code for starting program with wrong arguments
#define WRONG_ARGUMENT_CODE 10


// Program start arguments

// Code for starting program with --help argument
#define HELP_ARGUMENT_CODE 1
// Code for starting program with test argument
#define TEST_ARGUMENT_CODE 2
// Code for starting program with hline argument
#define HLINE_ARGUMENT_CODE 3
// Code for starting program with vline argument
#define VLINE_ARGUMENT_CODE 4
// Code for starting program with square argument
#define SQUARE_ARGUMENT_CODE 5

// Max file name length for input file
#define MAX_FILE_NAME_LENGTH 128

// Bitmap struct
typedef struct BitMap {
    int*** map;
    int sizeX;
    int sizeY;
} BitMap;


// Global bitmap declaration
BitMap bitMap;


// Frees the memory for BitMap
void freeBitMap(BitMap* bitMap) {
    for (int mapRowIdx = 0; mapRowIdx < bitMap->sizeY; mapRowIdx++) {
        for (int mapColIdx = 0; mapColIdx < bitMap->sizeX; mapColIdx++) {
            free(bitMap->map[mapRowIdx][mapColIdx]);
        }
        free(bitMap->map[mapRowIdx]);
    }
    free(bitMap->map);
}

// Allocates the memory for bitmap
int allocBitMap(BitMap* bm) {
    bm->map = malloc(bm->sizeY * sizeof(int**));
    if (!bm->map) return 1;
    for (int mapRowIdx = 0; mapRowIdx < bm->sizeY; mapRowIdx++) {
        bm->map[mapRowIdx] = malloc(bm->sizeX * sizeof(int*));
        if (!bm->map[mapRowIdx]) {
            freeBitMap(bm);
            return 1;
        }
        for (int mapColIdx = 0; mapColIdx < bm->sizeX; mapColIdx++) {
            bm->map[mapRowIdx][mapColIdx] = malloc(2 * sizeof(int));
            if (!bm->map[mapRowIdx][mapColIdx]) {
                freeBitMap(bm);
                return 1;
            }
        }
    }
    return 0;
}

// Initializes size for bitmap based on arguments
void initBitMap(BitMap* bm, int sizeX, int sizeY) {
    bm->sizeX = sizeX;
    bm->sizeY = sizeY;
}

// Loades bitmap file, returns 1 if bitmap is invalid
int loadBitMap(char fileName[MAX_FILE_NAME_LENGTH]) {
    int sizeX;
    int sizeY;
    FILE* mapFile = fopen(fileName, "r");
    if (!mapFile) {
        return 1;
    }
    
    // Check for first two numbers (size of the bitmap)
    while (fscanf(mapFile, "%d %d", &sizeY, &sizeX) != 2) {
        // if EOF found before second number, error
        if (fgetc(mapFile) == EOF) {
            fclose(mapFile);
            return 1;
        }
    }
    
    // Set size of the map and allocate memory for BitMap struct
    initBitMap(&bitMap, sizeX, sizeY);
    if (allocBitMap(&bitMap)) {
        fclose(mapFile);
        return 1;
    }

    char bitMapBitC;
    int bitMapBit;
    int idxX, idxY;
    int val0, val1;
    int i; // Needs to be declared outside of the loop
    // Looping through every character of the file until EOF not found or end of the map reached
    for (i = 0; i < sizeX*sizeY && bitMapBitC != EOF; bitMapBitC = fgetc(mapFile)) {
        // Whitespace jumps to the start of the loop
        if (isspace(bitMapBitC) || !bitMapBitC) continue;

        // Check for non 0 or 1 characters
        else if (!(bitMapBitC == '0' || bitMapBitC == '1')) {
            fclose(mapFile);
            return 1;
        }
        // Convert from char to number
        bitMapBit = bitMapBitC - '0';
        idxX = i % sizeX;
        idxY = i / sizeX;

        // Val 0 is number of 1s in the column (up)
        if (idxY == 0) val0 = bitMapBit;
        else val0 = (bitMap.map[idxY-1][idxX][0] + 1) * bitMapBit;

        // Val 1 is number of 1s in the row (to the left)
        if (idxX == 0) val1 = bitMapBit;
        else val1 = (bitMap.map[idxY][idxX-1][1] + 1) * bitMapBit;

        // Setting vals to the both elements of the position in the map
        bitMap.map[idxY][idxX][0] = val0;
        bitMap.map[idxY][idxX][1] = val1;

        // Increment inly on 0 or 1
        i++;
    }
    fclose(mapFile);

    // Return 1 if bitmap was not filled yet
    if (i < sizeX*sizeY) return 1;
    return 0;
}

// Tests if bitmap is valid
int testBitMap(char fileName[MAX_FILE_NAME_LENGTH]) {
    // Exit status takes 1 if bitmap is invalid
    int exitStatus = loadBitMap(fileName);
    freeBitMap(&bitMap);
    if (exitStatus == 1) fprintf(stderr, "Invalid");
    else printf("Valid");
    return exitStatus;
}

// Finds the longes line based on argument
// 0 for vertical, 1 for horizontal
int findLine(int lineType, char fileName[MAX_FILE_NAME_LENGTH]) {
    if (loadBitMap(fileName)) {
        fprintf(stderr, "Failed to load the bitmap!\n");
        freeBitMap(&bitMap);
        return 1;
    }

    // Setting coords of default line to -1 and length to 0
    int longestLineX = -1;
    int longestLineY = -1;
    int longestLineV = 0;

    // Looping through every element of the map
    for (int rowIdx = 0; rowIdx < bitMap.sizeY; rowIdx++) {
        for (int colIdx = 0; colIdx < bitMap.sizeX; colIdx++) {
            // checking if the current line is longest
            if (bitMap.map[rowIdx][colIdx][lineType] > longestLineV) {
                // Replacing values with current longest line
                longestLineV = bitMap.map[rowIdx][colIdx][lineType];
                longestLineX = colIdx;
                longestLineY = rowIdx;
            }
        }
    }
    // Printing Not found if line was not found
    if (longestLineV == 0) {
        printf("Not found");
    } else {
        // Printing coords of the line if line was found
        printf("%d %d %d %d\n",
            longestLineY - longestLineV*!lineType + !lineType,
            longestLineX - longestLineV*lineType + lineType,
            longestLineY, longestLineX);
    }
    freeBitMap(&bitMap);
    return 0;
}

// Finds the largest square inside bitmap
int findSquare(char fileName[MAX_FILE_NAME_LENGTH]) {
    if (loadBitMap(fileName)) {
        fprintf(stderr, "Failed to load the bitmap!\n");
        freeBitMap(&bitMap);
        return 1;
    }

    // Setting coords of default square to -1 and length to 0
    int largestSquareX = -1;
    int largestSquareY = -1;
    int largestSquareV = 0;
    int smaller; // buffer for smaller of the two values for each element
    int val0, val1;
    for (int rowIdx = 0; rowIdx < bitMap.sizeY; rowIdx++) {
        for (int colIdx = 0; colIdx < bitMap.sizeX; colIdx++) {
            // Getting the values to separate variables for easier handeling
            val0 = bitMap.map[rowIdx][colIdx][0];
            val1 = bitMap.map[rowIdx][colIdx][1];

            // Loading smaller buffer
            smaller = val0*(val0<val1) + val1*(val1<=val0);
            if (smaller <= largestSquareV) continue;

            // Checking for all available sizes of the squares
            for (int diagonal = smaller; diagonal > largestSquareV; diagonal--) {
                // Check if bigger square found
                if (bitMap.map[rowIdx-diagonal+1][colIdx][1] >= diagonal &&
                    bitMap.map[rowIdx][colIdx-diagonal+1][0] >= diagonal) {
                        largestSquareV = diagonal;
                        largestSquareX = colIdx;
                        largestSquareY = rowIdx;
                        break;
                }
            }
        }
    }
    // Printing Not found if square was not found
    if (largestSquareV == 0) {
        printf("Not found");
    } else {
        // Printing coords of the square if it was found
        printf("%d %d %d %d\n",
            largestSquareY-largestSquareV+1, largestSquareX-largestSquareV+1,
            largestSquareY, largestSquareX);
    }
    freeBitMap(&bitMap);
    return 0;
}


// Prints the help
int helpPrint() {
    printf(
        "The program should be run with arguments after the name of file.\n"
        "--help - prints this help string.\n"
        "test - checks the bitmap file and returns if something is wrong.\n"
        "hline - finds the longest horizontal line in the bitmap.\n"
        "vline - finds the longest vertical line in the bitmap.\n"
        "square - finds the biggest square in the bitmap.\n"
    );
    return 0;
}

// Finds the argument for program start and returns its code
// All the codes are difined avode
int programStartType(int argc, char *argv[]) {
    // Check for error codes
    int programRunType = TOO_MANY_ARGS_CODE;
    if ((argc == 1) || (argc == 2 && !(strcmp(argv[1], "--help") == 0))) {
        programRunType = TOO_FEW_ARGS_CODE;
    }
    else if (argc == 3 || strcmp(argv[1], "--help") == 0) {
        if (strcmp(argv[1], "--help") == 0) programRunType = HELP_ARGUMENT_CODE;
        else if (strcmp(argv[1], "test") == 0) programRunType = TEST_ARGUMENT_CODE;
        else if (strcmp(argv[1], "hline") == 0) programRunType = HLINE_ARGUMENT_CODE;
        else if (strcmp(argv[1], "vline") == 0) programRunType = VLINE_ARGUMENT_CODE;
        else if (strcmp(argv[1], "square") == 0) programRunType = SQUARE_ARGUMENT_CODE;
        else programRunType = WRONG_ARGUMENT_CODE;
    }
    return programRunType;
}

// Print error into stderr and return 1 for program end
int wrongArgument(int argType) {
    if (argType == TOO_MANY_ARGS_CODE) {
        fprintf(stderr, "Too many arguments on program start! Code: %d\n", argType);
    } else if (argType == TOO_FEW_ARGS_CODE) {
        fprintf(stderr, "Not enough arguments on start of program! Code: %d\n", argType);
    } else if (argType == WRONG_ARGUMENT_CODE) {
        fprintf(stderr, "The argument on program start is not right! Code: %d\n", argType);
    }
    return 1;
}

// Main func
int main(int argc, char *argv[]) {
    int programRunType = programStartType(argc, argv);
    switch (programRunType) {
        case TOO_MANY_ARGS_CODE:
        case TOO_FEW_ARGS_CODE:
        case WRONG_ARGUMENT_CODE:
            return wrongArgument(programRunType);
        case HELP_ARGUMENT_CODE:
            return helpPrint();
        case TEST_ARGUMENT_CODE:
            return testBitMap(argv[2]);
        case HLINE_ARGUMENT_CODE:
            return findLine(1, argv[2]);
        case VLINE_ARGUMENT_CODE:
            return findLine(0, argv[2]);
        case SQUARE_ARGUMENT_CODE:
            return findSquare(argv[2]);
    }
    
    fprintf(stderr, "No program run type found!\n");
    return 1;
}
