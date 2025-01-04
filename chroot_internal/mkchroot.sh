#!/bin/bash

CHROOT_PATH="$1"

if [ -z "$CHROOT_PATH" ]; then
    echo "Usage: $0 <chroot_path>"
    exit 1
fi

# Create basic directory structure
mkdir -p "$CHROOT_PATH"/{bin,lib,lib64,etc,dev,proc,sys,home}

# Copy essential binaries
BINS="/bin/bash /bin/ls /bin/cat /bin/mkdir /bin/ps /bin/mount"
for bin in $BINS; do
    cp "$bin" "$CHROOT_PATH/bin/"
    # Copy required libraries
    ldd "$bin" | grep -v dynamic | cut -d " " -f 3 | grep -v '^$' | while read -r lib; do
        mkdir -p "$CHROOT_PATH$(dirname "$lib")"
        cp "$lib" "$CHROOT_PATH$lib"
    done
done

# Copy dynamic linker
cp /lib64/ld-linux-x86-64.so.2 "$CHROOT_PATH/lib64/"

# Create essential device nodes
mknod -m 666 "$CHROOT_PATH/dev/null" c 1 3
mknod -m 666 "$CHROOT_PATH/dev/zero" c 1 5
mknod -m 666 "$CHROOT_PATH/dev/random" c 1 8
mknod -m 666 "$CHROOT_PATH/dev/urandom" c 1 9
mknod -m 666 "$CHROOT_PATH/dev/tty" c 5 0

# Create basic etc files
echo "root:x:0:0:root:/root:/bin/bash" > "$CHROOT_PATH/etc/passwd"
echo "root:x:0:" > "$CHROOT_PATH/etc/group"

# Copy our custom binaries
cp ../system/rxsh "$CHROOT_PATH/bin/"
cp ../system/unixMicroKernel "$CHROOT_PATH/bin/"

echo "Chroot environment created at $CHROOT_PATH"