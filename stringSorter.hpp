int handleCommandLineArgs(int argc, char *argv[], char **inpFilePath, char **outFilePath);
int getFileSize(const char *filePath);
int readDataFromFile(const char filePath[], char **rawData);
int replaceChars(char fromChar, char toChar, char *string);
void initStringPtrs(char *rawData, char **strings, int nStrings, unsigned int szFile);
int cmpStrings(const void *str1, const void *str2);
int printStringsToFile(const char filePath[], char **strings, int nStrings);