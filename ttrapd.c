/*
     Tiny Trap Daemon - Bugs by Niels Koster

     DO NOT FORGET TO ADJUST THE TTRAPD_FILE
     AND TTRAPD_SYSTEM_CALL DEFINITIONS BELOW!
*/

#define TTRAPD_FILE "/etc/ssl/private/server.key"
#define TTRAPD_SYSTEM_CALL "(ps faxuwww; echo; netstat -n; echo; lsof 2>&1) | mail -s 'ttrapd ALERT' root >/dev/null 2>&1"

#include <stdio.h>
#include <stdlib.h>
#include <syslog.h>
#include <unistd.h>
#include <errno.h>
#include <limits.h>
#include <sys/inotify.h>
#include <sys/types.h>
#include <sys/stat.h>
#define EVENT_SIZE (sizeof (struct inotify_event))
#define EVENT_BUF_LEN (1024 * (EVENT_SIZE + 16))
#define EXIT_INOTIFY 600

int main(int argc, char *argv[]) {
    struct stat sb;
    const char *self = "/proc/self/exe";
    char *binary;
    ssize_t r, bufsiz;
    bufsiz = sb.st_size + 1;
    if (sb.st_size == 0)
        bufsiz = PATH_MAX;
    binary = malloc(bufsiz);
    if (binary == NULL) {
        exit(EXIT_FAILURE);
    }
    r = readlink(self, binary, bufsiz);
    if (r == -1) {
        exit(EXIT_FAILURE);
    }
    binary[r] = '\0';
    pid_t pid, sid;
    pid = fork();
    int length, i = 0;
    int fd;
    int wd;
    char buffer[EVENT_BUF_LEN];
    if (pid < 0) {
        exit(EXIT_FAILURE);
    } else {
        if (pid > 0) {
            exit(EXIT_SUCCESS);
        }
    }
    umask(0);
    sid = setsid();
    if (sid < 0) {
        exit(EXIT_FAILURE);
    }
    syslog (LOG_NOTICE, "[%s] loaded into background by (uid=%d, gid=%d)", argv[0], getuid(), getgid());
    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }
    fd = inotify_init();
    if (fd < 0) {
        perror( "inotify_init" );
    }
    // Here you specify the file you want to put the trigger on.
    wd = inotify_add_watch(fd, TTRAPD_FILE, IN_ACCESS);
    length = read(fd, buffer, EVENT_BUF_LEN);
    if (length < 0) {
        perror("read");
    }
    while (i < length) {
        struct inotify_event *event = (struct inotify_event *) &buffer[i];
        i += EVENT_SIZE + event->len;
    }
    inotify_rm_watch(fd, wd);
    close(fd);
    // Suspicious: log and run the system() call.
    syslog(LOG_ALERT, "Please make well-considered decisions.");
    system(TTRAPD_SYSTEM_CALL);
    // Sleep for 30 seconds to prevent syslog from getting filled too fast when in a "trigger loop".
    sleep(30);
    // Execute a new instance (or something else if you want) and exit the current process.
    system(binary);
    free(binary);
    exit(EXIT_INOTIFY);
}
