# CpuToggle

This is a tool to help me run the old game TimeShift in WINE (via Lutris). TimeShift is a bit tricky to get working. You need D9VX or it crashes on the start of level 2, and it needs a virtual desktop in WINE to get the mouse working.

What is a little less common is that TimeShift is bugged and crashes if you have more than 8 CPUs; it's allocating a fixed (small) buffer for CPU information and has memory corruption if this overflows, or so I read. Some sources say the limit is 6 CPUs. Thread affinity does not help- it can still see the other CPUs and will still overflow the buffer.

In Windows, you can disable CPUs with msconfig.exe, and a reboot. But in Linux you can disable them on the fly, and that's what this program does. It works by writing 0 or 1 to /sys/devices/system/cpu/cpu#/online. Like this:

```bash
sudo ./cpu_toggle 4 # Disable all but 4 CPUs
```
or
```bash
sudo ./cpu_toggle # Re-enable all CPUs
```

Yes, it must run as root. That is why it's a C program; it make this work with Lutris, it needs to be setuid root as Lutris won't elevate it, so it needs to be implicit. Scripts can't be setuid-root, so here we are.
