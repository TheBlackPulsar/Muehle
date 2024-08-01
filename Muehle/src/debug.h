#ifdef DEBUG
#define DEBUG_PRINT(...)                                                       \
  printf("\t");                                                                \
  printf(__VA_ARGS__);                                                         \
  printf("\n")
#else
#define DEBUG_PRINT(...)
#endif

/* If you use DEBUG_PRINT("*"); * will be printed.
 * This print will only happen, if #define DEBUG is the first line in the file.
 * With this it is not necessary to place tmp prints and remove them manually.
 * Also: this Define addes a tab in front of the DEBUG_PRINT and a \n at the
 * end.
 */

/* cmd with running gameID for tests
 * sysprak-client -g 0w9gxl72yrgum
 */

/* cmd for Valgrind
 * valgrind --leak-check=full --trace-children=yes sysprak-client -g 0w9gxl72yrgum
 */
