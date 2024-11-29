
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

// Error codes
#define TOO_MANY_ARGS_CODE -1
#define TOO_FEW_ARGS_CODE 0
#define WRONG_ARGUMENT_CODE 10

#define HELP_ARGUMENT_CODE 1
#define TEST_ARGUMENT_CODE 2
#define HLINE_ARGUMENT_CODE 3
#define VLINE_ARGUMENT_CODE 4
#define SQUARE_ARGUMENT_CODE 5

#define MAX_HEADER_LINE_LENGTH 128

typedef struct BitMap {
    int*** map;
    int sizeX;
    int sizeY;
} BitMap;


BitMap bitMap;


void freeBitMap(BitMap* bitMap) {
    for (int mapRowIdx = 0; mapRowIdx < bitMap->sizeY; mapRowIdx++) {
        for (int mapColIdx = 0; mapColIdx < bitMap->sizeX; mapColIdx++) {
            free(bitMap->map[mapRowIdx][mapColIdx]);
        }
        free(bitMap->map[mapRowIdx]);
    }
    free(bitMap->map);
}

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

void initBitMap(BitMap* bm, int sizeX, int sizeY) {
    bm->sizeX = sizeX;
    bm->sizeY = sizeY;
}


int loadBitMap() {
    char headerLine[MAX_HEADER_LINE_LENGTH];
    int sizeX;
    int sizeY;
    fgets(headerLine, sizeof(headerLine), stdin);
    sscanf(headerLine, "%d %d", &sizeY, &sizeX);

    printf("%d, %d\n", sizeX, sizeY);
    initBitMap(&bitMap, sizeX, sizeY);
    if (allocBitMap(&bitMap)) {
        fprintf(stderr, "Failed to allocate memory for bitmap!\n");
        return 1;
    }

    char bitMapBitC;
    int bitMapBit;
    int idxX, idxY;
    int val0, val1;
    int i;
    for (i = 0; i < sizeX*sizeY && bitMapBitC != EOF; bitMapBitC = fgetc(stdin)) {
        if (!(bitMapBitC == '0' || bitMapBitC == '1')) continue;
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
    if (i < sizeX*sizeY) return 1;
    return 0;
}

int testBitMap() {
    int exitStatus = loadBitMap();
    freeBitMap(&bitMap);
    if (exitStatus == 1) fprintf(stderr, "Invalid");
    else printf("Valid");
    return exitStatus;
}

int findHLine() {
    if (loadBitMap()) {
        fprintf(stderr, "Failed to load the bitmap!\n");
        freeBitMap(&bitMap);
        return 1;
    }
    int longestHLineX = -1;
    int longestHLineY = -1;
    int longestHLineV = 0;
    for (int rowIdx = 0; rowIdx < bitMap.sizeY; rowIdx++) {
        for (int colIdx = 0; colIdx < bitMap.sizeX; colIdx++) {
            if (bitMap.map[rowIdx][colIdx][1] > longestHLineV) {
                longestHLineV = bitMap.map[rowIdx][colIdx][1];
                longestHLineX = colIdx;
                longestHLineY = rowIdx;
            }
        }
    }
    if (longestHLineV == 0) {
        printf("Not found");
    } else {
        printf("%d %d %d %d\n",
            longestHLineY, longestHLineX-longestHLineV+1,
            longestHLineY, longestHLineX);
    }
    freeBitMap(&bitMap);
    return 0;
}

int findVLine() {
    if (loadBitMap()) {
        fprintf(stderr, "Failed to load the bitmap!\n");
        freeBitMap(&bitMap);
        return 1;
    }

    int longestVLineX = -1;
    int longestVLineY = -1;
    int longestVLineV = 0;
    for (int rowIdx = 0; rowIdx < bitMap.sizeY; rowIdx++) {
        for (int colIdx = 0; colIdx < bitMap.sizeX; colIdx++) {
            if (bitMap.map[rowIdx][colIdx][0] > longestVLineV) {
                longestVLineV = bitMap.map[rowIdx][colIdx][0];
                longestVLineX = colIdx;
                longestVLineY = rowIdx;
            }
        }
    }
    if (longestVLineV == 0) {
        printf("Not found");
    } else {
        printf("%d %d %d %d\n",
            longestVLineY-longestVLineV+1, longestVLineX,
            longestVLineY, longestVLineX);
    }
    freeBitMap(&bitMap);
    return 0;
}

int findSquare() {
    if (loadBitMap()) {
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
                if (bitMap.map[rowIdx-smaller+1][colIdx][1] >= smaller &&
                    bitMap.map[rowIdx][colIdx-smaller+1][0] >= smaller) {
                        largestSquareV = diagonal;
                        largestSquareX = colIdx;
                        largestSquareY = rowIdx;
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
        "--help - argument prints this help string.\n"
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
    if (argc == 1) programRunType = TOO_FEW_ARGS_CODE;
    else if (argc == 2) {
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
            return testBitMap();
        case HLINE_ARGUMENT_CODE:
            return findHLine();
        case VLINE_ARGUMENT_CODE:
            return findVLine();
        case SQUARE_ARGUMENT_CODE:
            return findSquare();
    }
    
    fprintf(stderr, "No program run type found!\n");
    return 1;
}
