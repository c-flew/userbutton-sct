#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdbool.h>
#include <getopt.h>

#include <unistd.h>
#include <sys/syscall.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <fcntl.h>

#ifndef RENAME_EXCHANGE
#define RENAME_EXCHANGE 2
#endif

#ifndef SYS_renameat2
#if defined(__x86_64__)
#define SYS_renameat2 314 // from arch/x86/syscalls/syscall_64.tbl
#elif defined(__i386__)
#define SYS_renameat2 353 // from arch/x86/syscalls/syscall_32.tbl
#endif
#endif

int swap_names(int first_fd, const char *first_path, int second_fd, const char *second_path) {
#if defined(__NR_renameat2)
    return renameat2(first_fd, first_path, second_fd, second_path, RENAME_EXCHANGE);
#elif defined(syscall)
    return syscall(SYS_renameat2, first_fd, first_path, second_fd, second_path, RENAME_EXCHANGE);
#else
    // TODO make this an atomic operation
    char temp_name[7] = "XXXXXX";
    if (!mktemp(temp_name)) return -1;
    int err;
    if ((err = renameat(first_fd, first_path, first_fd, temp_name)) != 0) return err;
    if ((err = renameat(second_fd, second_path, first_fd, first_path)) != 0) return err;
    if ((err = renameat(first_fd, temp_name, second_fd, second_path)) != 0) return err;
    return 0;
#endif
}


// Third-party
#include "inih/ini.h"

#define btoa(x) ((x) ? "true" : "false")
#define atob(x) (strcmp(x, "false") != 0)


typedef struct {
    bool launch_at_boot;
} config;

static config user_conf;

static char* newbuf;

static int handler(void* user, const char* section, const char* name, const char* value) {
    config* pconfig = (config*)user;



    static char prev_section[50] = "";

    if (strcmp(section, prev_section)) {
        char output[128] = "";
        sprintf(output, "%s[%s]\n", (prev_section[0] ? "\n" : ""), section);
        sprintf(newbuf + strlen(newbuf), output);
        strncpy(prev_section, section, sizeof(prev_section));
        prev_section[sizeof(prev_section) - 1] = '\0';
    }

    #define MATCH(a, b) (strcmp(a, b) == 0)

    bool found = false;
    if (MATCH(section, "LVRT")) {
        if (MATCH(name, "RTTarget.LaunchAppAtBoot")) {
            found = true;
            pconfig->launch_at_boot = strcmp(value, "\"True\"") == 0;

            sprintf(newbuf + strlen(newbuf), "%s %s", "RTTarget.LaunchAppAtBoot =", (pconfig->launch_at_boot) ? "\"False\"" : "\"True\"");
        }
    }


    #undef MATCH

    if (!found) {
        char output[128] = "";
        sprintf(output, "%s = %s\n", name, value);
        sprintf(newbuf + strlen(newbuf), output);
    }

    return 0;
}

void print_usage() {
    printf("usage: sct [-s true|false]");
}

int main(int argc, char** argv) {
    const char* filepath = "test.ini";
    const char* tmppath = "tmp.conf";



    static int help_flag;
    static int set_flag;
    static int path_flag;
    static struct option long_options[] = {
        {.name = "help", .has_arg = no_argument, .flag = &help_flag, .val = 0},
        {.name = "set", .has_arg = required_argument, .flag = &set_flag, .val = 0},
        {.name = "flag", .has_arg = required_argument, .flag = &set_flag, .val = 0}
    };

    int c;
    int option_index;
    while (1) {
        c = getopt_long(argc, argv, "hsp", long_options, &option_index);

        if (c == -1) break;

        switch (c) {
            case 0:
                break;
            case 'h':
                help_flag = 1;
                break;
            case 's':
                set_flag = 1;
                break;
            case 'p':
                path_flag = 1;
                break;
            case '?':
                break;
            default:
                printf("value not recognized");
                exit(1);
        }
    }

    if (help_flag) {
        print_usage();
        return 0;
    }

    if (set_flag) {
        if (argc - optind != 1) {
            print_usage();
            return 0;
        }
        user_conf.launch_at_boot = atob(argv[optind]);
    }

    if (path_flag) {
        if (argc - optind != 1) {
            print_usage();
            return 0;
        }
        filepath = argv[optind];
    }

    struct stat st;

    if (stat(filepath, &st) != 0) {
        printf("Could not stat conf file");
        return -1;
    }

    newbuf = (char*) malloc(st.st_size);

    config conf;

    FILE* cf = fopen(filepath, "rw");
    if (ini_parse_file(cf, handler, &conf) < 0) {
        printf("Can't load %s\n", filepath);
        return -1;
    }

    if (conf.launch_at_boot == user_conf.launch_at_boot && set_flag) {
        printf("No change made");
        return 0;
    }

    FILE* tmp = fopen(tmppath, "w");

    printf(newbuf);
    fprintf(tmp, newbuf, strlen(newbuf));

    swap_names(AT_FDCWD, filepath, AT_FDCWD, tmppath);

    fclose(tmp);
}

#undef btoa
#undef atob
