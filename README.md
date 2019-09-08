# CpuToggle

This is a tool to help me run the old game TimeShift in WINE (via Lutris). TimeShift is a bit tricky to get working. You need D9VX or it crashes on the start of level 2, and it needs a virtual desktop in WINE to get the mouse working.

What is a little less common is that TimeShift is bugged and crashes if you have more than 8 CPUs; it's allocating a fixed (small) buffer for CPU information and has memory corruption if this overflows, or so I read. Some sources say the limit is 6 CPUs. Thread affinity does not help- it can still see the other CPUs and will still overflow the buffer. The option to restrict the game to one core doesn't work either- it won't even start on one core.

In Windows, you can disable CPUs with msconfig.exe, and a reboot. But in Linux you can disable them on the fly, and that's what this program does. It works by writing 0 or 1 to /sys/devices/system/cpu/cpu#/online. Like this:

```bash
sudo ./cpu_toggle 4 # Disable all but 4 CPUs
```
or
```bash
sudo ./cpu_toggle # Re-enable all CPUs
```

Yes, it must run as root. That is why it's a C program; it make this work with Lutris, it needs to be setuid root as Lutris won't elevate it, so it needs to be implicit. Scripts can't be setuid-root, so here we are.

Lutris can't seem to run executables like this, so they must be wrapped in scripts. I've provide 'disable_cpus.sh' and 'enable_cpus.sh' for this. To install this mess, build cpu_toggle, and copy it to your TimeShift directory. Place both scripts in the same directory. Mark them all as executable, and mark cpu_toggle as setuid, and owned as root, thus:

```bash
chmod +x enable_cpus.sh disable_cpus.sh cpu_toggle
sudo chmod +s cpu_toggle
sudo chown root cpu_toggle
```

Then set up TimeShift in Lutris; in "System options" provide disable_cpus.sh as the "Pre-launch command", and also turn on "Wait for pre-launch command completion". Provide enable_cpus.sh as the "Post-exit command".

This arrangement will disable all but 4 CPUs before starting TimeShift, and will re-enable them afterwards. That allows TimeShift to start up.
