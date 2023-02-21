#ifndef CMD_PARSER_H_
#define CMD_PARSER_H_

#include "./editor.h"

typedef enum {
   COMMAND_LINE_OK = 0,
   COMMAND_LINE_EXIT_OK,
   COMMAND_LINE_EXIT_ERROR
} CommandLineStatus;

void command_line_print_usage(void);

CommandLineStatus command_line_check(int argc, char **argv);

CommandLineStatus command_line_parse(int argc, char **argv, Editor *editor);

#endif // CMD_PARSER_H_