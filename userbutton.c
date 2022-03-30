#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>

// Third-party
#include "inih/ini.h"

#define btoa(x) ((x) ? "true" : "false")

FILE* exec_format(char* cmd) {
    printf(cmd);
    printf("\n");
    return popen(cmd, "r");
}


typedef struct {
    bool launch_at_boot;
} config;

static int handler(void* user, const char* section, const char* name, const char* value) {
    config* pconfig = (config*)user;

    #define MATCH(a, b) strcmp(a, b) == 0


    static char prev_section[50] = "";

    if (strcmp(section, prev_section)) {
        printf("%s[%s]\n", (prev_section[0] ? "\n" : ""), section);
        strncpy(prev_section, section, sizeof(prev_section));
        prev_section[sizeof(prev_section) - 1] = '\0';
    }

    bool found = false;
    if (MATCH(section, "LVRT")) {
        if (MATCH(name, "RTTarget.LaunchAppAtBoot")) {
            found = true;
            pconfig->launch_at_boot = strcmp(value, "\"True\"") == 0;

            char output[128] = "";
            sprintf(output, "%s %s", "RTTarget.LaunchAppAtBoot = ", (pconfig->launch_at_boot) ? "\"False\"" : "\"True\"");
            printf("%s\n", output);
        }
    }
    if (!found) {
        printf("%s = %s\n", name, value);
    }

    #undef MATCH
    return 0;
}

int main() {
    char buf[2048];

    FILE* fp = exec_format("ls -l");

    char ch = 0;

    size_t max_iter = sizeof(buf) / sizeof(char);
    for (size_t i = 0; (ch = fgetc(fp)) != EOF; ++i) {
        if (i >= max_iter) {
            fprintf(stderr, "ERROR: buf is not large enough");
            return -1;
        }
        buf[i] = ch;
    }

    printf("%s\n", buf);


    int ec = pclose(fp);
    // TODO: I forgot how to handle ec

    config conf;

    if (ini_parse("test.ini", handler, &conf) < 0) {
        printf("Can't load 'test.ini'\n");
        return -1;
    }

    printf("%s\n", btoa(conf.launch_at_boot));


}
