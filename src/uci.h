//
//  uci.h
//  Spartan
//
//
//
//

#ifndef uci_h
#define uci_h

#include <stdio.h>

// the main loop for the program
int uciloop();
void print_engine_info();
void print_engine_options();
void init();
void position(char *input);
void moves(char *input);
void fen(char *input);
void go(char *input);
void set_option(char *input);

#endif /* uci_h */
