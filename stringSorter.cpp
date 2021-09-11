#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "stringSorter.hpp"

int main(int argc, const char *argv[]) {
    printf("Program sorts strings in the provided file and prints them to the output one.\n\n");

    const char *inpFilePath = nullptr, *outFilePath = nullptr;
    SORT_FLAGS order = ASCENDING_ORDER, side = RIGHT_SIDE;
    int errorCode = handleCommandLineArgs(argc, argv, &side, &order, &inpFilePath, &outFilePath);
    if (errorCode) {
        return 1; // error code . 1
    }

    // ------
    FILE *inpFile = fopen(inpFilePath, "r");
    if (!inpFile) {
        printf("File wasn't opened. Provided path: %s\n", inpFilePath);
        return 1;
    }

    long signedSzFile = getFileSize(inpFile);
    if (signedSzFile == -1) {
        return 1;
    }
    size_t szFile = (size_t) signedSzFile;

    // +1 due to possible absence \n at the end of the file.
    char *rawData = (char*) calloc(szFile + 1, sizeof(char));
    if (!rawData) {
        printf("Memory wasn't allocated.\n");
        return 1;
    }

    if (readDataFromFile(inpFile, rawData, szFile)) {
        return 1;
    }
    fclose(inpFile);

    // +1 due to the string at the beginning of the file
    size_t nStrings = replaceChars('\n', '\0', rawData) + 1;
    stringData *strings = (stringData*) calloc(nStrings, sizeof(stringData));

    // szFile + 1 is correct because of alloc(szFile + 1)
    int nInitStrings = initStringPtrs(rawData, strings, szFile + 1);
    assert(nInitStrings == nStrings);
    // ----

    // добавить во0мозность сортировки 3 файлов
    int (*qsortCmp)(const void*, const void*) = nullptr;
    if      (order == ASCENDING_ORDER  && side == LEFT_SIDE)  qsortCmp = cmpStringsLeft;
    else if (order == ASCENDING_ORDER  && side == RIGHT_SIDE) qsortCmp = cmpStringsRight;
    else if (order == DESCENDING_ORDER && side == LEFT_SIDE)  qsortCmp = cmpStringsLeftReverse;
    else if (order == DESCENDING_ORDER && side == RIGHT_SIDE) qsortCmp = cmpStringsRightReverse;
    assert(qsortCmp);
    qsort(strings, nStrings, sizeof(strings[0]), qsortCmp);

    FILE *outFile = fopen(outFilePath, "w");
    if (!outFile) {
        printf("File wasn't opened. Provided path: %s\n", outFilePath);
        return -1;
    }
    printStringsToFile(outFile, strings, nStrings);
    fclose(outFile);

    free(strings);
    strings = nullptr;
    free(rawData);
    rawData = nullptr;

    return 0;
}

//: ./String_sorter --in ../data/test-file.txt --read --out output.txt \
//                  --side right --order descent --sort --output \
//                  --side left                  --sort --output \
//                  --output_beffer                              \
//                  --end

/**
 * Parse command line arguments.
 * @param argc, @param argv - main() args
 * @param key - flag to find in command line
 * @param useNextArg - whether to return argument(if it exists) after key
 * @return index in argv of found key(or next argument) or -1 if error has occurred.
 */
int parseCommandLineArgs(int argc, const char *argv[], const char *key, bool useNextArg) {
    assert(argv);
    assert(key);

    for (int i = 1; i < argc; ++i) {
        if (!strcmp(argv[i], key)) {
            if (useNextArg) {
                if (i == argc - 1) {
                    printf("Argument after %s was expected.\n", key);
                    return -1;
                }
                else {
                    return i + 1;
                }
            }
            return i;
        }
    }

    return -1;
}

/**
 * Handles command line arguments.
 * @param argc, @param argv main() args
 * @param side, @param order - SORT_FLAGS params to manage sorting parameters
 * @param inpFilePath, @param outFilePath - paths to files
 * @return 0 if arguments were handled successfully, 1 in other cases.
 */
int handleCommandLineArgs(int argc, const char *argv[], SORT_FLAGS *side, SORT_FLAGS *order, const char **inpFilePath,
                          const char **outFilePath) {
    assert(inpFilePath);
    assert(outFilePath);

    int sideArgPosition  = parseCommandLineArgs(argc, argv, "--side",  true);
    int orderArgPosition = parseCommandLineArgs(argc, argv, "--order", true);
    int inArgPosition    = parseCommandLineArgs(argc, argv, "--in",    true);
    int outArgPosition   = parseCommandLineArgs(argc, argv, "--out",   true);
    int helpArgPosition  = parseCommandLineArgs(argc, argv, "--help",  false);

    // optopt + можно написать прцоессор с обработкой - длинная опция, короткая, ссылка на обрабатывающую функцию
    if (sideArgPosition != -1) {
        if (!strcmp(argv[sideArgPosition], "left")) {
            *side = LEFT_SIDE;
        }
        else if (!strcmp(argv[sideArgPosition], "right")) {
            *side = RIGHT_SIDE;
        }
        else {
            printf("Incorrect sorting side was entered. left/right is available.\n");
            return 1;
        }
    }

    if (orderArgPosition != -1) {
        if (!strcmp(argv[orderArgPosition], "ascent")) {
            *order = ASCENDING_ORDER;
        }
        else if (!strcmp(argv[orderArgPosition], "descent")) {
            *order = DESCENDING_ORDER;
        }
        else {
            printf("Incorrect sorting order was entered. ascent/descent is available.\n");
            return 1;
        }
    }

    if (helpArgPosition != -1) {
        printf("Usage: ./String_sorter [--side left/right] [--order ascent/descent] --in input_file --out output_file.\n");
    }

    if (inArgPosition != -1) {
        *inpFilePath = argv[inArgPosition];
    }
    else {
        printf("Input file argument was not found.\n");
        return 1;
    }

    if (outArgPosition != -1) {
        *outFilePath = argv[outArgPosition];
    }
    else {
        printf("Output file argument was not found.\n");
        return 1;
    }

    return 0;
}

/**
 * Gets size of the provided file in bytes.
 * @param filePath - path to file
 * @return size in bytes or -1 if error was found.
 */
long getFileSize(FILE *inpFile) {
    rewind(inpFile); // !!!!!
    fseek(inpFile, 0, SEEK_END);
    long szFile = ftell(inpFile);
    rewind(inpFile);

    return szFile;
}

/**
 * Reads all data from the provided file to given pointer assuming memory has been already allocated.
 * @param inpFIle - path to file to read from
 * @param rawData - pointer to allocated memory
 * @param szFile - size of allocated memory
 * @return 0 if data was read successfully, -1 in other cases.
 */
int readDataFromFile(FILE *inpFile, char *rawData, size_t szFile) {
    assert(inpFile);
    assert(rawData);

    size_t readingResult = fread(rawData, sizeof(char), szFile, inpFile); //fread на windows /r вычищает сдвигами в текстовом файле
    if (readingResult != szFile) { // readingResult - размер файла, на винде упадёт
        printf("Data from file wasn't read.\n");
        return -1;
    }
    szFile = readingResult;//////// ifdef win unix
    // \0 нужно ставить после readingResult

    rewind(inpFile);
    return 0;
}

/**
 * Replaces chars in the provided string.
 * @param fromChar - char which will be replaced assuming it is not \0
 * @param toChar  - char to replace with
 * @param string - pointer to string which will be changed
 * @return number of changed symbols.
 */
size_t replaceChars(char fromChar, char toChar, char string[]) {
    assert(fromChar != '\0');
    assert(string);

    size_t nStrings = 0;
    int i = 0;
    while (string[i] != '\0') {
        if (string[i] == fromChar) {
            string[i] = toChar;
            nStrings++;
        }
        i++;
    }

    return nStrings;
}

/**
 * Initializes pointers assuming strings was divided by \0.
 * @param rawData - pointer to data with all strings
 * @param strings - array of stringData struct with length and str fields
 * @param nStrings - number of strings to be sure data was initialized correctly
 * @param szFile - size of allocated data
 * @return number of initialized strings.
 */
int initStringPtrs(char *rawData, stringData strings[], size_t szFile) {
    assert(rawData);
    assert(strings);

    int nInits = 0;

    strings[nInits++].str = rawData;
    for (size_t i = 1; i < szFile - 1; ++i) {
        if (rawData[i] == '\0') {
            strings[nInits].str = rawData + i + 1;
            strings[nInits - 1].length = size_t(strings[nInits].str - strings[nInits - 1].str);
            ++nInits;
        }
    }
    strings[nInits - 1].length = size_t(rawData + szFile - strings[nInits-1].str);

    return nInits;
}

/**
 * Left-side tring comparator.
 */
int cmpStringsLeft(const void *str1, const void *str2)  {
    return strcmp(((const stringData*) str1)->str, ((const stringData*) str2)->str);
}

/**
 * Left-side reverse string comparator.
 */
int cmpStringsLeftReverse(const void *str1, const void *str2) {
    return -cmpStringsLeft(str1, str2);
}
// последний элемент в массиве указателей сделать нулевым
// не учитывать пунктцации

/**
 * strcmp analog with comparing strings in reverse order.
 * @param str1, @param str2 - string pointers
 * @param length1, @param length2 - string lengths with \0
 * @return 1 if str1 > str2, 0 if str1 == str2, -1 if str1 < str2.
 */
int strcmpReverseOrder(const stringData *str1, const stringData *str2) {
    assert(str1->str);
    assert(str1->length >= 0);
    assert(str2->str);
    assert(str2->length >= 0);

    size_t minLength = (str1->length < str2->length) ? str1->length : str2->length;
    size_t nChecks = 0;
    char *curStr1ptr = str1->str + str1->length - 1,
         *curStr2ptr = str2->str + str2->length - 1;
    while (nChecks++ <= minLength) {
        char symbol1 = *(curStr1ptr--);
        char symbol2 = *(curStr2ptr--);

        if (symbol1 > symbol2) {
            return 1;
        }
        if (symbol1 < symbol2) {
            return -1;
        }
    }

    if (str1->length > str2->length) {
        return 1;
    }
    if (str2->length < str1->length) {
        return -1;
    }

    return 0;
}

/**
 * Right-side string comparator.
 */
int cmpStringsRight(const void *str1, const void *str2)  {
    return strcmpReverseOrder((const stringData*) str1, (const stringData*) str2);
}

/**
 * Right-side reverse string comparator.
 */
int cmpStringsRightReverse(const void *str1, const void *str2) {
    return -cmpStringsRight(str1, str2);
}

/**
 * Prints strings to file
 * @param outFile - path to output file
 * @param strings - array of char*(pointers to strings)
 * @param nStrings - number of strings
 * @return 0 if data was printed successfully, -1 if other cases.
 */
int printStringsToFile(FILE *outFile, const stringData strings[], size_t nStrings) {
    assert(outFile);
    assert(strings);

    for (size_t i = 0; i < nStrings - 1; ++i) {
        fprintf(outFile, "%s\n", strings[i].str);
    }
    fprintf(outFile, "%s", strings[nStrings - 1].str);

    return 0;
}
