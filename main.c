#include <stdio.h>
#include <stdlib.h>
#include <termios.h>
#include <unistd.h>
#include <sys/wait.h>

#include <string.h>
#include <errno.h>
#include <signal.h>

#include "lib/sout.h"

void enableRaw(struct termios *orgTerminal);
void disableRaw(struct termios *orgTerminal);
void execute(char *buffer);
void arrows(char *inputBuffer, int *inputLength, char *prompt, int *totalHistoryCount, int *historyIndex, char historyList[10][1024]);

struct termios cookedTerminal;

int main() {
    char buffer[1024];
    int position = 0;
    char character;
    char prompt[] = "fash ~> ";
    char history[10][1024];
    int count = 0;
    int historyPosition = -1;

    enableRaw(&cookedTerminal);

    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sigaction(SIGINT, &sa, NULL);

    sout("%s", prompt);

    while (1) {
        int bytes = read(STDIN_FILENO, &character, 1);
        if (bytes == -1) {
            if (errno == EINTR) {
                sout("\r\n");
                position = 0;
                sout("%s", prompt);
                continue;
            }
            break;
        }

        if (bytes == 0) continue;

        switch (character) {
        case 13:
            buffer[position] = '\0';
            sout("\r\n");
            if (position > 0) {
                strcpy(history[count % 10], buffer);
                count++;
                execute(buffer);
            }
            sout("%s", prompt);
            position = 0;
            break;

        case 127:
            if (position > 0) {
                position--;
                sout("\b \b");
            }
            break;

        case 27:
            arrows(buffer, &position, prompt, &count, &historyPosition, history);
            break;
        
        case 4:
            sout("\r\nexit\r\n");
            disableRaw(&cookedTerminal);
            exit(0);
            break;

        default:
            if (position < 1023) {
                buffer[position++] = character;
                sout("%c", character);
            }
            break;
        }
    }

    disableRaw(&cookedTerminal);
    return 0;
}

void enableRaw(struct termios *orgTerminal) {
    struct termios rawTerminal;
    tcgetattr(STDIN_FILENO, orgTerminal);
    rawTerminal = *orgTerminal;
    rawTerminal.c_iflag &= ~(BRKINT | ICRNL | INPCK | ISTRIP | IXON);
    rawTerminal.c_oflag &= ~(OPOST);
    rawTerminal.c_cflag |= (CS8);
    rawTerminal.c_lflag &= ~(ECHO | ICANON | IEXTEN | ISIG);
    rawTerminal.c_cc[VMIN] = 0;
    rawTerminal.c_cc[VTIME] = 1;
    tcsetattr(STDIN_FILENO, TCSAFLUSH, &rawTerminal);
}

void disableRaw(struct termios *orgTerminal) {
    tcsetattr(STDIN_FILENO, TCSAFLUSH, orgTerminal);
}

void execute(char *buffer) {
    char *argv[1024];
    int b = 0;
    char *token = strtok(buffer, " ");
    while (token != NULL) {
        argv[b] = token;
        b++;
        token = strtok(NULL, " ");
    }
    argv[b] = NULL;

    if (argv[0] == NULL) return;
    disableRaw(&cookedTerminal);

    if (strcmp(argv[0], "exit") == 0) {
        sout("exit\r\n");
        disableRaw(&cookedTerminal);
        exit(0);
    }

    pid_t pid = fork();
    if (pid == 0) {
        struct sigaction sa;
        sa.sa_handler = SIG_DFL;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;
        sigaction(SIGINT, &sa, NULL);

        execvp(argv[0], argv);
        if (errno == ENOENT) {
            sout("\r\nfash: %s: command not found\r\n", argv[0]);
        }
        exit(1);
    } else if (pid > 0) {
        wait(NULL);
    }
    enableRaw(&cookedTerminal);
}



void arrows(char *inputBuffer, int *inputLength, char *prompt, int *totalHistoryCount, int *historyIndex, char historyList[10][1024]) {
    char seq[2];
    if (read(STDIN_FILENO, &seq[0], 1) <= 0 || read(STDIN_FILENO, &seq[1], 1) <= 0) return;
    if (seq[0] != '[') return;

    // Up Arrow (A)
    if (seq[1] == 'A') {
        if (*historyIndex < *totalHistoryCount - 1 && *historyIndex < 9) {
            (*historyIndex)++;
        }
    } 
    // Down Arrow (B)
    else if (seq[1] == 'B') {
        if (*historyIndex >= 0) {
            (*historyIndex)--;
        }
    } else return;

    sout("\r\033[K%s", prompt);

    if (*historyIndex >= 0) {
        int target = (*totalHistoryCount - 1 - *historyIndex) % 10;
        if (target < 0) target += 10; 

        strcpy(inputBuffer, historyList[target]);
        *inputLength = strlen(inputBuffer);
        sout("%s", inputBuffer);
    } else {
        *inputLength = 0;
        inputBuffer[0] = '\0';
    }
}
