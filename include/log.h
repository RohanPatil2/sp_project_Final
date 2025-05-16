// log.h – logging prototypes
#ifndef LOG_H
#define LOG_H
/* log_type: positive = access, 0 = access, -1 = client error, -2 = fatal */
void write_to_log_file(int log_type, const char *content, const char *tag);
#endif
