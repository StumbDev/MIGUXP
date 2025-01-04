#!/bin/bash

CHROOT_PATH="$1"

if [ -z "$CHROOT_PATH" ]; then
    echo "Usage: $0 <chroot_path>"
    exit 1
fi

if [ ! -d "$CHROOT_PATH" ]; then
    echo "Error: Chroot directory does not exist"
    exit 1
fi

# Mount essential filesystems
mount -t proc proc "$CHROOT_PATH/proc"
mount -t sysfs sys "$CHROOT_PATH/sys"
mount -t devpts devpts "$CHROOT_PATH/dev/pts"

# Start the microkernel
chroot "$CHROOT_PATH" /bin/unixMicroKernel "$CHROOT_PATH" &
KERNEL_PID=$!

# Start the shell
chroot "$CHROOT_PATH" /bin/rxsh

# Cleanup
kill $KERNEL_PID
umount "$CHROOT_PATH/proc"
umount "$CHROOT_PATH/sys"
umount "$CHROOT_PATH/dev/pts"