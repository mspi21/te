#include "./cmd_parser.h"
#include "./utils.h"

#include <string.h>

void command_line_print_usage(void) {
    printf("Usage:\n");
    printf("TODO\n");
}

CommandLineStatus command_line_check(int argc, char **argv) {
    if(argc > 2) {
        command_line_print_usage();
        return COMMAND_LINE_EXIT_ERROR;
    }
    if(argc == 2 && (!strcmp(argv[1], "-h") || !strcmp(argv[1], "--help"))) {
        command_line_print_usage();
        return COMMAND_LINE_EXIT_OK;
    }
    return COMMAND_LINE_OK;
}

CommandLineStatus command_line_parse(int argc, char **argv, Editor *editor) {
    if(argc < 2)
        return COMMAND_LINE_OK;
    
    if(argc == 2) {
        if(editor_load_file_from_path(editor, strdup(argv[1])))
            return COMMAND_LINE_OK;
        return COMMAND_LINE_EXIT_ERROR;
    }

    command_line_print_usage();
    return COMMAND_LINE_EXIT_ERROR;
}