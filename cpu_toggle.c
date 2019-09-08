#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <dirent.h>
#include <ctype.h>
#include <stdbool.h>
#include <string.h>
#include <limits.h>
#include <errno.h>

/* Write text to a specific file. */
static void
write_text (char *destination, char *text)
{
    FILE *f = fopen (destination, "w");

    if (f == NULL)
    {
        perror ("Unable to open file; am I running as root?");
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
prefix (char *text, char *candidate_prefix)
{
    for (; *candidate_prefix; ++candidate_prefix, ++text)
    {
        if (!*text)             /* check if text ends before prefix */
            return false;

        if (*text != *candidate_prefix) /* check for mismatch */
            return false;
    }

    return true;
}

/* Test if a directory-name is in the form "cpuN", where N is
an integer. Returns -1 if not, but the actual number if so. */
static int
identify_cpu_directory (char *directory_name)
{
    if (!prefix (directory_name, "cpu"))
        return -1;

    char *number_part = directory_name + 3;
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
    char *online = "/online";

    struct dirent *ep;
    DIR *dp = opendir (dir);
    int max_name = PATH_MAX - strlen (dir) - strlen (online) - 1;

    if (dp != NULL)
    {
        while (ep = readdir (dp))
        {
            int cpu_no = identify_cpu_directory (ep->d_name);
            if (cpu_no > 0 && strlen (ep->d_name) <= max_name)  /* CPU 0 cannot be deactivated */
            {
                char destination[PATH_MAX];
                strcpy (destination, dir);
                strcat (destination, ep->d_name);
                strcat (destination, online);

                char *flag_text = cpus_desired < 0
                    || cpu_no < cpus_desired ? "1" : "0";
                printf ("%s <- %s\n", destination, flag_text);
                write_text (destination, flag_text);
            }
        }

        closedir (dp);
    }
    else
        perror ("Unable to open the directory /sys/devices/system/cpu.");
}

int
main (int argc, char *argv[])
{
    if (argc == 1)
        apply_cpu_toggle (INT_MAX);
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
