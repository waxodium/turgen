#include "turgen.h"


bool tokenPath(const char *token) {
    if (token == NULL) return false;
    
    if (strcmp(token, "/") == 0) return true;
    
    if (token[0] == '/' || token[0] == '~' || token[0] == '.' || strchr(token, '/')) {
        return true;
    }
    
    return false;
}
