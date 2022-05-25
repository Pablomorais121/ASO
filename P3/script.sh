#!/bin/bash
make
dd bs=4096 count=100 if=/dev/zero of=image
./mkassoofs image
sudo su
121
insmod assoofs.ko
mkdir mnt
mount -o loop -t assoofs image mnt
dmesg
