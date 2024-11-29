
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

// Error codes
#define TOO_MANY_ARGS_CODE -1
#define TOO_FEW_ARGS_CODE 0
#define WRONG_ARGUMENT_CODE 10

// Program argument codes
#define HELP_ARGUMENT_CODE 1
#define TEST_ARGUMENT_CODE 2
#define HLINE_ARGUMENT_CODE 3
#define VLINE_ARGUMENT_CODE 4
#define SQUARE_ARGUMENT_CODE 5

// String length defines
#define MAX_HEADER_LINE_LENGTH 128
#define MAX_FILE_NAME_LENGTH 128

// Bitmap struct
typedef struct BitMap {
    int*** map;
    int sizeX;
    int sizeY;
} BitMap;


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
    
    while (fscanf(mapFile, "%d %d", &sizeY, &sizeX) != 2) {
        if (fgetc(mapFile) == EOF) {
            fclose(mapFile);
            return 1;
        }
    }
    
    initBitMap(&bitMap, sizeX, sizeY);
    if (allocBitMap(&bitMap)) {
        fclose(mapFile);
        return 1;
    }

    char bitMapBitC;
    int bitMapBit;
    int idxX, idxY;
    int val0, val1;
    int i;
    for (i = 0; i < sizeX*sizeY && bitMapBitC != EOF; bitMapBitC = fgetc(mapFile)) {
        if (isspace(bitMapBitC) || !bitMapBitC) continue;
        else if (!(bitMapBitC == '0' || bitMapBitC == '1')) {
            fclose(mapFile);
            return 1;
        } 
        bitMapBit = bitMapBitC - '0';
        idxX = i % sizeX;
        idxY = i / sizeX;
        if (idxY == 0) val0 = bitMapBit;
        else val0 = (bitMap.map[idxY-1][idxX][0] + 1) * bitMapBit;
        if (idxX == 0) val1 = bitMapBit;
        else val1 = (bitMap.map[idxY][idxX-1][1] + 1) * bitMapBit;
        bitMap.map[idxY][idxX][0] = val0;
        bitMap.map[idxY][idxX][1] = val1;
        i++;
    }
    fclose(mapFile);
    if (i < sizeX*sizeY) return 1;
    return 0;
}

// Tests if bitmap is valid
int testBitMap(char fileName[MAX_FILE_NAME_LENGTH]) {
    int exitStatus = loadBitMap(fileName);
    freeBitMap(&bitMap);
    if (exitStatus == 1) fprintf(stderr, "Invalid");
    else printf("Valid");
    return exitStatus;
}

// Finds the longes line based on argument
int findLine(int lineType, char fileName[MAX_FILE_NAME_LENGTH]) { // 0 for vertical, 1 for horizontal
    if (loadBitMap(fileName)) {
        fprintf(stderr, "Failed to load the bitmap!\n");
        freeBitMap(&bitMap);
        return 1;
    }

    int longestLineX = -1;
    int longestLineY = -1;
    int longestLineV = 0;
    for (int rowIdx = 0; rowIdx < bitMap.sizeY; rowIdx++) {
        for (int colIdx = 0; colIdx < bitMap.sizeX; colIdx++) {
            if (bitMap.map[rowIdx][colIdx][lineType] > longestLineV) {
                longestLineV = bitMap.map[rowIdx][colIdx][lineType];
                longestLineX = colIdx;
                longestLineY = rowIdx;
            }
        }
    }
    if (longestLineV == 0) {
        printf("Not found");
    } else {
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

    int largestSquareX = -1;
    int largestSquareY = -1;
    int largestSquareV = 0;
    int smaller;
    int val0, val1;
    for (int rowIdx = 0; rowIdx < bitMap.sizeY; rowIdx++) {
        for (int colIdx = 0; colIdx < bitMap.sizeX; colIdx++) {
            val0 = bitMap.map[rowIdx][colIdx][0];
            val1 = bitMap.map[rowIdx][colIdx][1];
            smaller = val0*(val0<val1) + val1*(val1<=val0);
            if (smaller <= largestSquareV) continue;
            for (int diagonal = smaller; diagonal > largestSquareV; diagonal--) {
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
    if (largestSquareV == 0) {
        printf("Not found");
    } else {
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

/* Returns number based on start argument:
    -1 for too many arguments (error)
    0 for only 1 argument (error)
    1 for help
    2 for test
    3 for horizontal line search
    4 for vertical line search
    5 for square search
    10 for right argument count, but wrong argument (error) */
int programStartType(int argc, char *argv[]) {
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
