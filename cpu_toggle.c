#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>

/* Write text to a specific file. */
static void
write_value (char *filename, char *text)
{
    FILE *f = fopen (filename, "w");

    if (f == NULL)
    {
        fprintf (stderr, "Unable to open file; am I running as root?\n");
        exit (1);
    }

    fputs (text, f);
    fclose (f);
}

/* Test if a string contains only digits. */
static bool
is_number (char *text)
{
    for (char *ch = text; *ch; ++ch)
        if (!isdigit (*ch))
            return false;

    return true;
}

/* Test if a string is a prefix of another string */
static bool
prefix (char *str, char *prefix)
{
    for (; *prefix; ++prefix, ++str)
    {
        if (!*str)
            return false;

        if (*str != *prefix)
            return false;
    }

    return true;
}

/* Test if a file-name is in the form "cpuN", where N is
an integer. Returns -1 if not, but the actual number if so. */
static int
identify_file (char *name)
{
    if (!prefix (name, "cpu"))
        return -1;

    char *number_part = name + 3;
    if (!is_number (number_part))
        return -1;

    return atoi (number_part);
}

/* The real work is here; this scans a special directory for suitable CPU subdirectories,
and upon finding them it enables or disables them as appropriate. */
static void
apply_cpu_toggle (int cpus_desired)
{
    char *dir = "/sys/devices/system/cpu/";
    struct dirent *ep;
    DIR *dp = opendir (dir);

    if (dp != NULL)
    {
        while (ep = readdir (dp))
        {
            int cpu_no = identify_file (ep->d_name);
            if (cpu_no > 0)     /* CPU 0 cannot be deactivated */
            {
                char filename[PATH_MAX];
                strcpy (filename, dir);
                strcat (filename, ep->d_name);
                strcat (filename, "/online");

                char *value = cpus_desired < 0
                    || cpu_no < cpus_desired ? "1" : "0";
                printf ("%s <- %s\n", filename, value);
                write_value (filename, value);
            }
        }

        closedir (dp);
    }
    else
        perror ("Couldn't open the directory");
}

int
main (int argc, char *argv[])
{
    if (argc == 1)
        apply_cpu_toggle (-1);
    else if (argc == 2 && is_number (argv[1]))
    {
        int desired = atoi (argv[1]);
        apply_cpu_toggle (desired);
    }
    else
    {
        fprintf (stderr, "Usage: cpu_toggle [number of cpus]\n");
        return 1;
    }

    return 0;
}
