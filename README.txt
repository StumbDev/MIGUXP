MIGUXP - Miguel's Unix Experience
================================

A modern Unix V clone implementation with a terminal-based desktop environment.

Project Overview
---------------
MIGUXP is a chroot-based Unix V clone written in C++17, featuring a microkernel architecture,
a custom shell, and a terminal-based desktop environment (MDE). It provides a lightweight
and educational Unix-like environment.

Components
----------
1. UnixMicroKernel
   - Basic process management
   - System call handling
   - Chroot environment support

2. rxsh (Shell)
   - Command line interface
   - Command history
   - Process execution

3. MicroBox
   - Minimal BusyBox clone
   - Essential Unix commands:
     * ls: List directory contents
     * cat: Concatenate files
     * pwd: Print working directory
     * mkdir: Create directories
     * rm: Remove files
     * whoami: Display current user

4. MDE (Miguel's Desktop Environment)
   - Terminal-based UI using ncurses
   - Window management
   - Built-in applications:
     * Text Editor (F1)
     * File Manager (F2)
   - Color support
   - Panel-based interface

Requirements
------------
- C++17 compatible compiler (g++)
- ncurses library
- panel library
- menu library
- Unix-like environment (Linux/BSD)
- make build system

Building
--------
1. Standard build:
   make

2. Install components:
   make install

3. Run the system:
   make run

4. Debug build:
   make debug

5. Clean build files:
   make clean

Directory Structure
------------------
/system/
  - Core system components source files
  - Microkernel and desktop environment

/chroot_internal/
  - Chroot environment setup scripts
  - Runtime management scripts

/build/
  - Compiled binaries
  - System root directory
  - Installation files

Usage
-----
1. Build the system:
   make

2. Install components:
   make install

3. Start MIGUXP:
   make run

4. In MDE:
   - F1: Text Editor
   - F2: File Manager
   - F10: Exit
   - ESC: Close current application

Development
-----------
- All source files are in the system/ directory
- Use make debug for development builds
- Shell scripts in chroot_internal/ handle environment setup

License
-------
This software is open source and available under the MIT License.

Created by: Miguel
Date: 2025-01-03
