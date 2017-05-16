![Tiny Trap](ttrapd.png "Tiny Trap")

"`ttrapd`"  is a very simple and lightweight trap, written in C, that can help to detect an intruder or "integrity violator" with root privileges in a Linux computer system.

The "integrity violation" or "intrusion detection" pitfall is based upon the curiosity potency of intruders.

When a root privileged intruder is wandering a compromised system, most likely, he or she is trying to identify
sensitive system files. Followed by inspecting the content of a few files that are chosen, simply based upon their
interesting (or commonly used) names and locations.

For example, files like `/etc/shadow`, `/etc/ssl/private/vpn.key` or `/etc/pf.conf` can't be left without inspecting by generally every root privileged intruder.
Especially, when you take in account that the intruder probably gained root privileges, just for the purpose to gain access to these sensitive files.

In Linux, the inotify API provides a mechanism for monitoring filesystem events.
From the [man](http://man7.org/linux/man-pages/man7/inotify.7.html) page: "Inotify can be used to monitor individual files, or to monitor directories.
When a directory is monitored, inotify will return events for the directory itself, and for files inside the directory."

This concept makes it possible to monitor a file for an inotify event, and therefore to notice that somebody is currently using that file.

This is simply what `ttrapd` does. It forks itself into the background, and monitors a specified file for an inotify event.
This can be just a dummy file with an interesting name, ownership and file permissions.
Only meant to trigger the intruders curiosity, as a decoy. In case the inotify event has been triggered, `ttrapd` will notice and alert.

In practice, system administrators can together agree on a small set of (dummy) files that should _never_ be touched, and run a few `ttrapd` daemons to monitor those files.
With this approach, you might detect persons within your own organization, that violate integrity.

You can also place traps on real used private keys. This approach is best for detecting hackers, intruders that do not belong to your organization.
Check this example: https://gitlab.com/nkoster/tiny-trap-daemon/wikis/Example-Wiki

 **Install: https://gitlab.com/nkoster/tiny-trap-daemon/wikis/home**

Note for OpenBSD: I am converting this concept into `kqueue()` logic. I'll create a new project for that. Not finished yet.