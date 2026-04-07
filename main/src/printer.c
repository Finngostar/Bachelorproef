#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "printer.h"

void printInfo(char *message) {
    printf("[INFO]\t%s", message);
}

void printError(char *message) {
    printf("[ERROR]\t%s", message);
}