#include "turgen.h"

#define GREEN "\033[38;2;120;150;120m"
#define PROMPT_RESET "\033[0m"

void prompter(char *prompt_buffer, size_t MaxLength) {
    char host[64];
    char cwd[1024];

    const char *user = getenv("USER");

    if (!user) {
        user = "user";
    }

    if (gethostname(host, sizeof(host)) != 0) {
        strcpy(host, "unknown");
    }

    if (!getcwd(cwd, sizeof(cwd))) {
        strcpy(cwd, "?");
    }

    char *home = getenv("HOME");

    if (home != NULL && strncmp(cwd, home, strlen(home)) == 0) {
        char *path_remainder = cwd + strlen(home);

        snprintf(prompt_buffer, MaxLength, "[%s@%s] " GREEN "~%s" PROMPT_RESET " > ", user, host, path_remainder);
        return;
    }

    snprintf(prompt_buffer, MaxLength, "[%s@%s] " GREEN "%s" PROMPT_RESET " > ", user, host, cwd);
}
