#include "turgen.h"
#include "error.h"

#define LIMIT 32

static int operator(char c) {
    return c == '&' ||
           c == '|' ||
           c == ';' ||
           c == '<' ||
           c == '>';
}

char **tokenize(char *input) {
    if (!input)
        return NULL;

    int cap = LIMIT;
    int count = 0;

    char **args = malloc(cap * sizeof(char *));
    if (!args)
        return NULL;

    char *reader = input;
    char *writer = malloc(strlen(input) * 2 + 1);
    if (!writer) {
        free(args);
        return NULL;
    }

    bool active = false;
    char quote = '\0';
    bool escaped = false;

    while (*reader)
    {
        if (count >= cap - 1)
        {
            cap *= 2;

            char **tmp = realloc(args, cap * sizeof(char *));
            if (!tmp)
            {
                free(writer);
                free(args);
                return NULL;
            }

            args = tmp;
        }

        char c = *reader;

        if (escaped)
        {
            if (!active)
            {
                args[count++] = writer;
                active = true;
            }

            *writer++ = c;
            escaped = false;
            reader++;
            continue;
        }

        if (c == '\\' && quote != '\'')
        {
            escaped = true;

            if (!active)
            {
                args[count++] = writer;
                active = true;
            }

            reader++;
            continue;
        }

        if (quote)
        {
            if (c == quote)
            {
                *writer++ = c;
                quote = '\0';
            }
            else
            {
                *writer++ = c;
            }

            reader++;
            continue;
        }

        if (c == '\'' || c == '"')
        {
            quote = c;

            if (!active)
            {
                args[count++] = writer;
                active = true;
            }
            
            *writer++ = c;
            reader++;
            continue;
        }
            
        if (operator(c))
        {
            bool hasFd = false;

            if (active && (c == '<' || c == '>')) {
                char *token = args[count - 1];
                hasFd = true;

                while (token < writer) {
                    if (!isdigit((unsigned char)*token)) {
                        hasFd = false;
                        break;
                    }
                    token++;
                }
            }

            if (active && !hasFd) {
                *writer++ = '\0';
                active = false;
            }

            if (!active) {
                args[count++] = writer;
                active = true;
            }

            *writer++ = *reader++;

            if (*reader == c && (c == '&' || c == '|' || c == '<' || c == '>')) {
                *writer++ = *reader++;
            } else if ((c == '<' || c == '>') && *reader == '&') {
                *writer++ = *reader++;

                if (*reader == '-') {
                    *writer++ = *reader++;
                } else {
                    while (isdigit((unsigned char)*reader)) {
                        *writer++ = *reader++;
                    }
                }
            }   

            *writer++ = '\0';
            active = false;
            continue;
        }

        if (isspace((unsigned char)c))
        {
            if (active)
            {
                *writer++ = '\0';
                active = false;
            }

            reader++;
            continue;
        }

        if (!active)
        {
            args[count++] = writer;
            active = true;
        }

        *writer++ = c;
        reader++;
    }

    if (active)
    {
        *writer++ = '\0';
    }

    if (quote) {
        char q_str[2] = {quote, '\0'};
        _error(ERR_SYNTAX, q_str);
        free(writer);
        free(args);
        return NULL;
    }

    args[count] = NULL;
    return args;
}
