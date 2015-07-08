"**`ttrapd`**"  is a very simple and lightweight trap, written in C, that can help to detect an intruder or "integrity violator" with root privileges in a UNIX-like computer system.

The "integrity violation" or "intrusion detection" pitfall is based upon the curiosity potency of intruders.

When a root privileged intruder is wandering a compromised system, most likely, he or she is trying to identify
sensitive system files. Followed by inspecting the content of a few files that are chosen, simply based upon their
interesting (or commonly used) names and locations.

For example, files like **`/etc/shadow`**, **`/etc/ssl/private/vpn.key`** or **`/etc/pf.conf`** can't be left without inspecting by generally every root privileged intruder. Especially, when you take in account that the intruder probably gained root privileges, just for the purpose to gain access to these sensitive files.

Every time when you access a file in a generic UNIX-like file system, mounted with "atime" enabled, the _access_ time
(atime) is updated to the _current_ time. Even when file access only involves _reading_!

This concept makes it possible to monitor a file for an atime update, and therefore to notice that somebody is currently using that file.

This is simply what **`ttrapd`** does. It forks itself into the background, and monitors a specified file for an atime update. This can be just a dummy file with an interesting name, ownership and file permissions. Only meant to trigger the intruders curiosity, as a decoy. In case the access time for this file is updated, **`ttrapd`** will notice and alert.

In practice, system administrators can together agree on a small set of (dummy) files that should never be touched and
run a few **`ttrapd`** daemons to monitor those files.

You can also place traps, specifically on real used private keys, like in this example: https://github.com/nkoster/tiny-trap-daemon/wiki/Example-Wiki

 **Install: https://github.com/nkoster/tiny-trap-daemon/wiki**
