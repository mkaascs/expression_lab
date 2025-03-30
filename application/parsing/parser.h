#ifndef PARSER_H
#define PARSER_H

typedef enum {
    Parse, LoadPrefix, LoadPostfix, SavePrefix, SavePostfix, Eval
} CommandType;

int get_command_type(const char*, CommandType*);

#endif //PARSER_H
