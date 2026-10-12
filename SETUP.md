# Environment Setup

You need a Linux environment with a C compiler and a few standard tools. Any of
these works; pick one.

## Option A — A Linux VM (recommended for OSCP prep)

Install VirtualBox or VMware and run **Kali Linux** or **Ubuntu 22.04+**. Kali is
closest to the OSCP environment and has the tooling preinstalled. This is the
recommended path because the capstone mirrors an OSCP workflow.

## Option B — WSL2 (Windows)

On Windows 10/11:

```powershell
wsl --install -d Ubuntu
```

Reboot if prompted, then open "Ubuntu" from the Start menu. You now have a real
Linux userland.

## Option C — A cloud box

Any small Ubuntu/Debian droplet works for the non-GUI parts (everything except the
capstone's local-host checks, which you'll want to run on a box you can inspect).

---

## Install the toolchain

On Debian/Ubuntu/Kali:

```bash
sudo apt update
sudo apt install -y build-essential gdb valgrind make git strace ltrace manpages-dev
```

This gives you:

- `gcc` — the compiler (`build-essential` also pulls in `make` and the C library headers)
- `gdb` — the debugger (Module 11)
- `valgrind` — the memory-error / leak checker (Modules 07, 11)
- `strace` / `ltrace` — syscall / library-call tracers (Module 09)
- `man` pages for the standard library and syscalls — your most important reference

## Verify

```bash
gcc --version
make --version
gdb --version
echo 'int main(void){return 0;}' > /tmp/t.c && gcc -Wall -o /tmp/t /tmp/t.c && echo "toolchain OK"
```

If you see `toolchain OK`, you're ready. Open [`modules/00-setup-and-toolchain`](modules/00-setup-and-toolchain).

## Editor

Anything works. VS Code (with the C/C++ extension), Vim/Neovim, or `nano` are all
fine. You do **not** need an IDE for this course — the command line is the point.

## How to read a man page

You'll use these constantly. `man 2 open` is the kernel syscall `open`; `man 3 printf`
is the C library function. The section number matters:

- section **2** = system calls (kernel)
- section **3** = library functions (libc)

```bash
man 2 open      # the open(2) system call
man 3 printf    # the printf(3) library function
man man         # how man itself works
```
