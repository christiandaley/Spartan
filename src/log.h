//
//  log.h
//  Spartan
//
//
//
//

#ifndef log_h
#define log_h

void write_to_log(const char *sender, const char *msg);
void engine_output(const char *format, ...);
void output_off();
void output_on();

#endif /* log_h */
