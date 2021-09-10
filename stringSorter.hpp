struct stringData {
    char *str = nullptr;
    size_t length = 0;
};

enum SORT_FLAGS {
    UNKNOWN = 0,
    ASCENDING_ORDER,
    DESCENDING_ORDER,
    LEFT_SIDE,
    RIGHT_SIDE
};

int handleCommandLineArgs(int argc, const char *argv[], SORT_FLAGS *side, SORT_FLAGS *order, const char **inpFilePath,
                          const char **outFilePath);
int parseCommandLineArgs(int argc, const char *argv[], const char *key, bool useNextArg);
long getFileSize(FILE *inpFile);
int readDataFromFile(FILE *inpFile, char *rawData, size_t szFile);
size_t replaceChars(char fromChar, char toChar, char string[]);
int initStringPtrs(char *rawData, stringData strings[], size_t szFile);
int cmpStringsLeft(const void *str1, const void *str2);
int cmpStringsLeftReverse(const void *str1, const void *str2);
int strcmpReverseOrder(const stringData *str1, const stringData *str2);
int cmpStringsRight(const void *str1, const void *str2);
int cmpStringsRightReverse(const void *str1, const void *str2);
int printStringsToFile(FILE *outFile, const stringData strings[], size_t nStrings);