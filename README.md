## connect to afp protocol on ubuntu 18.04
The command line tool needs ncurses-dev and libreadline-dev to compile. Install them
with :

```bash
sudo apt-get install ncurses-dev libreadline-dev
sudo apt-get install libgcrypt-dev libfuse-dev libgmp-dev libreadline-dev libncurses-dev
```

Compile the program:
```bash
./configure
make
sudo make install
```

libafpclient.so.0 may be missing (see: https://askubuntu.com/questions/886656/how-can-i-mount-an-afp-share) but is actually somewhere else
```bash
sudo cp /usr/local/lib/libafpclient.so.0 /lib/x86_64-linux-gnu
```

Example usage
```bash
mount_afp afp://foo.bar.edu ~/Desktop/mount
```

### Credits and license

This is a fork of the original afpfs-ng project that has gone unmaintained
for quite some time. It is so far the only available open source AFP client.

This repository includes many patches collected by the XBMC project
(www.xbmc.org) as well as mine, in a bid to improve stability, performance and
to implement new features.

Check AUTHORS for a somewhat complete list of contributors.

The original afpfs-ng webiste can be found at https://sites.google.com/site/alexthepuffin/home

This project retains the original author's license and is distributed under the GPL.
