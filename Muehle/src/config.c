//#define DEBUG

#include "muehle.h"
#include <ctype.h>

void parseConfig(char *file, CONFIG *parameters) {
  /* Takes the given file and parameter struct and manages the help functions to
   * get the values out of the file and into the struct*/
  FILE *content = openFile(file);

  char *line = (char *)malloc(sizeof(*content));
  char *name = (char *)malloc(sizeof(*content));
  char *value = (char *)malloc(sizeof(*content));
  char limit;

  while (fgets(line, sizeof(*content), content)) {
    DEBUG_PRINT("%s", line);
    trimBlanks(line);
    DEBUG_PRINT("%s,%d,%s", parameters->Hostname, parameters->Portnummer,
                parameters->Gamekind);
    sscanf(line, "%[^'=]%c%s", name, &limit, value);
    DEBUG_PRINT("%s,%d,%s", parameters->Hostname, parameters->Portnummer,
                parameters->Gamekind);
    DEBUG_PRINT("%s,%s", name, value);
    placeValues(name, value, parameters);
  }
  DEBUG_PRINT("%s,%d,%s", parameters->Hostname, parameters->Portnummer,
              parameters->Gamekind);

  checkValidity(parameters);

  free(line);
  free(name);
  free(value);
}

FILE *openFile(char *file) {
  /* Add the path to the File and open the file in read mode. Expects a human
   * readable file. Returns a pointer to the content of the file. */
  char path[50] = "config/";
  char *filePath = strcat(path, file);
  FILE *content = fopen(filePath, "rt");
  if (!content) {
    perror("Problem mit der angegebenen Datei");
    DEBUG_PRINT("filePath: %s", filePath);
    exit(EXIT_FAILURE);
  }
  return content;
}

void trimBlanks(char *sContent) {
  /* Strip all space and tab character from a line.*/
  char *character = sContent;
  do
    while (isblank(*character))
      character++;
  while ((*sContent++ = *character++));
}

void placeValues(char *name, char *value, CONFIG *parameters) {
  /* places given values in the parameters struct for connection */
  if (!strcasecmp(name, "HOSTNAME")) {
    sprintf(parameters->Hostname, "%s", value);
  } else if (!strcasecmp(name, "PORTNUMMER")) {
    int number = atoi(value);
    parameters->Portnummer = number;
  } else if (!strcasecmp(name, "GAMEKINDNAME")) {
    sprintf(parameters->Gamekind, "%s", value);
  } else {
    DEBUG_PRINT("%s,%s", name, value);
    printf("Die Datei enthaelt einen unbekannten Wert.\n");
    exit(EXIT_FAILURE);
  }
}

void checkValidity(CONFIG *parameters) {
  /* Checks whether all 3 parameters were provided or not */
  if (!(*parameters->Hostname && parameters->Portnummer &&
        *parameters->Gamekind)) {
    printf("Es fehlen Argumente in der .conf\n");
    exit(EXIT_FAILURE);
  }
}
