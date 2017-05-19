/*
     Tiny Trap Daemon - Bugs by Niels Koster

     DO NOT FORGET TO ADJUST THE TTRAPD_FILE
     AND TTRAPD_SYSTEM_CALL DEFINITIONS BELOW!
*/

#define TTRAPD_FILE "/etc/ssl/private/server.key"
#define TTRAPD_SYSTEM_CALL "(ps faxuwww;echo;netstat -n;echo;lsof 2>&1)|gzip -9|base64|mail -s 'Daily Cron' root >/dev/null 2>&1"
#define TTRAPD_SLEEP_TIME 30

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
#define EXIT_FAILURE_INOTIFY_INIT 601
#define EXIT_FAILURE_INOTIFY_READ 602

int main(int argc, char *argv[]) {
    struct stat sb;
    const char *link = "/proc/self/exe";
    char *self;
    ssize_t r;
    self = malloc(PATH_MAX);
    if (self == NULL) {
        exit(EXIT_FAILURE);
    }
    r = readlink(link, self, PATH_MAX);
    if (r == -1) {
        exit(EXIT_FAILURE);
    }
    self[r] = '\0';
    pid_t pid, sid;
    pid = fork();
    int length, i = 0, fd, wd;
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
    syslog(LOG_NOTICE, "[%s] loaded into background by (uid=%d, gid=%d)", argv[0], getuid(), getgid());
    if ((chdir("/")) < 0) {
        exit(EXIT_FAILURE);
    }
    fd = inotify_init();
    if (fd < 0) {
        exit(EXIT_FAILURE_INOTIFY_INIT);
    }
    wd = inotify_add_watch(fd, TTRAPD_FILE, IN_ACCESS);
    length = read(fd, buffer, EVENT_BUF_LEN);
    if (length < 0) {
        exit(EXIT_FAILURE_INOTIFY_READ);
    }
    while (i < length) {
        struct inotify_event *event = (struct inotify_event *) &buffer[i];
        i += EVENT_SIZE + event->len;
    }
    inotify_rm_watch(fd, wd);
    close(fd);
    syslog(LOG_ALERT, "Success");
    system(TTRAPD_SYSTEM_CALL);
    sleep(TTRAPD_SLEEP_TIME);
    system(self);
    free(self);
    exit(EXIT_INOTIFY);
}
