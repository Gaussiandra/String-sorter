struct stringData {
    char *str = nullptr;
    int length = -1;
};

enum SORT_FLAGS {
    UNKNOWN = 0,
    ASCENDING_ORDER,
    DESCENDING_ORDER,
    LEFT_SIDE,
    RIGHT_SIZE
};

int handleCommandLineArgs(int argc, char *argv[], SORT_FLAGS *side, SORT_FLAGS *order, char **inpFilePath,
                          char **outFilePath);
size_t getFileSize(FILE *inpFile);
int readDataFromFile(FILE *inpFile, char *rawData, size_t szFile);
int replaceChars(char fromChar, char toChar, char string[]);
int initStringPtrs(const char *rawData, stringData strings[], size_t szFile);
int cmpStringsLeft(const void *str1, const void *str2);
int cmpStringsLeftReverse(const void *str1, const void *str2);
int strcmpReverseOrder(const char *str1, int length1, const char *str2, int length2);
int cmpStringsRight(const void *str1, const void *str2);
int cmpStringsRightReverse(const void *str1, const void *str2);
int printStringsToFile(FILE *outFile, const stringData strings[], int nStrings);