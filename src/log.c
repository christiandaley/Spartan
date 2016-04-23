//
//  log.c
//  Spartan
//
//
//
//

#include <stdarg.h>
#include <stdio.h>
#include <string.h>
#include "log.h"

static int is_writing = 0;
static int do_output = 1;
static int first_write = 1;
static int newline = 1;

void write_to_log(const char *sender, const char *msg) {
    FILE *log_file;
    if (first_write) {
        log_file = fopen("log_spartan.txt", "w");
        fprintf(log_file, "Log file for Spartan\n------------------\n\n");
    } else {
        log_file = fopen("log_spartan.txt", "a");
    }
    
    first_write = 0;
    
    if (newline)
        fprintf(log_file, "%s: %s", sender, msg);
    else
        fprintf(log_file, "%s", msg);
    
    newline = msg[strlen(msg) - 1] == '\n';
    
    fflush(log_file);
    fclose(log_file);
}

void engine_output(const char *format, ...) {
    if (!do_output)
        return;
    
    do {} while (is_writing);
    is_writing = 1;
    
    
    char msg[1024];
    va_list args;
    
    va_start(args, format);
    vsprintf(msg, format, args);
    va_end(args);
    
    
    printf("%s", msg);
    fflush(stdout);
    
    write_to_log("Spartan", msg);
    
    is_writing = 0;
}

void output_off() {
    do_output = 0;
}

void output_on() {
    do_output = 1;
}