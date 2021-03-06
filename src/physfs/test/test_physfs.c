/**
 * Test program for PhysicsFS. May only work on Unix.
 *
 * Please see the file LICENSE in the source's root directory.
 *
 *  This file written by Ryan C. Gordon.
 */

#if HAVE_CONFIG_H
#  include <config.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>

#if (defined __MWERKS__)
#include <SIOUX.h>
#endif

#if (defined PHYSFS_HAVE_READLINE)
#include <unistd.h>
#include <readline/readline.h>
#include <readline/history.h>
#endif

#include <time.h>

#include "physfs.h"

#define TEST_VERSION_MAJOR  0
#define TEST_VERSION_MINOR  1
#define TEST_VERSION_PATCH  7

static FILE *history_file = NULL;

static void output_versions(void)
{
    PHYSFS_Version compiled;
    PHYSFS_Version linked;

    PHYSFS_VERSION(&compiled);
    PHYSFS_getLinkedVersion(&linked);

    printf("test_physfs version %d.%d.%d.\n"
           " Compiled against PhysicsFS version %d.%d.%d,\n"
           " and linked against %d.%d.%d.\n\n",
            TEST_VERSION_MAJOR, TEST_VERSION_MINOR, TEST_VERSION_PATCH,
            (int) compiled.major, (int) compiled.minor, (int) compiled.patch,
            (int) linked.major, (int) linked.minor, (int) linked.patch);
} /* output_versions */


static void output_archivers(void)
{
    const PHYSFS_ArchiveInfo **rc = PHYSFS_supportedArchiveTypes();
    const PHYSFS_ArchiveInfo **i;

    printf("Supported archive types:\n");
    if (*rc == NULL)
        printf(" * Apparently, NONE!\n");
    else
    {
        for (i = rc; *i != NULL; i++)
        {
            printf(" * %s: %s\n    Written by %s.\n    %s\n",
                    (*i)->extension, (*i)->description,
                    (*i)->author, (*i)->url);
        } /* for */
    } /* else */

    printf("\n");
} /* output_archivers */


static int cmd_quit(char *args)
{
    return(0);
} /* cmd_quit */


static int cmd_init(char *args)
{
    if (*args == '\"')
    {
        args++;
        args[strlen(args) - 1] = '\0';
    } /* if */

    if (PHYSFS_init(args))
        printf("Successful.\n");
    else
        printf("Failure. reason: %s.\n", PHYSFS_getLastError());

    return(1);
} /* cmd_init */


static int cmd_deinit(char *args)
{
    if (PHYSFS_deinit())
        printf("Successful.\n");
    else
        printf("Failure. reason: %s.\n", PHYSFS_getLastError());

    return(1);
} /* cmd_deinit */


static int cmd_addarchive(char *args)
{
    char *ptr = strrchr(args, ' ');
    int appending = atoi(ptr + 1);
    *ptr = '\0';

    if (*args == '\"')
    {
        args++;
        *(ptr - 1) = '\0';
    } /* if */

    /*printf("[%s], [%d]\n", args, appending);*/

    if (PHYSFS_addToSearchPath(args, appending))
        printf("Successful.\n");
    else
        printf("Failure. reason: %s.\n", PHYSFS_getLastError());

    return(1);
} /* cmd_addarchive */


static int cmd_removearchive(char *args)
{
    if (*args == '\"')
    {
        args++;
        args[strlen(args) - 1] = '\0';
    } /* if */

    if (PHYSFS_removeFromSearchPath(args))
        printf("Successful.\n");
    else
        printf("Failure. reason: %s.\n", PHYSFS_getLastError());

    return(1);
} /* cmd_removearchive */


static int cmd_enumerate(char *args)
{
    char **rc;

    if (*args == '\"')
    {
        args++;
        args[strlen(args) - 1] = '\0';
    } /* if */

    rc = PHYSFS_enumerateFiles(args);

    if (rc == NULL)
        printf("Failure. reason: %s.\n", PHYSFS_getLastError());
    else
    {
        int file_count;
        char **i;
        for (i = rc, file_count = 0; *i != NULL; i++, file_count++)
            printf("%s\n", *i);

        printf("\n total (%d) files.\n", file_count);
        PHYSFS_freeList(rc);
    } /* else */

    return(1);
} /* cmd_enumerate */


static int cmd_getdirsep(char *args)
{
    printf("Directory separator is [%s].\n", PHYSFS_getDirSeparator());
    return(1);
} /* cmd_getdirsep */


static int cmd_getlasterror(char *args)
{
    printf("last error is [%s].\n", PHYSFS_getLastError());
    return(1);
} /* cmd_getlasterror */


static int cmd_getcdromdirs(char *args)
{
    char **rc = PHYSFS_getCdRomDirs();

    if (rc == NULL)
        printf("Failure. Reason: [%s].\n", PHYSFS_getLastError());
    else
    {
        int dir_count;
        char **i;
        for (i = rc, dir_count = 0; *i != NULL; i++, dir_count++)
            printf("%s\n", *i);

        printf("\n total (%d) drives.\n", dir_count);
        PHYSFS_freeList(rc);
    } /* else */

    return(1);
} /* cmd_getcdromdirs */


static int cmd_getsearchpath(char *args)
{
    char **rc = PHYSFS_getSearchPath();

    if (rc == NULL)
        printf("Failure. reason: %s.\n", PHYSFS_getLastError());
    else
    {
        int dir_count;
        char **i;
        for (i = rc, dir_count = 0; *i != NULL; i++, dir_count++)
            printf("%s\n", *i);

        printf("\n total (%d) directories.\n", dir_count);
        PHYSFS_freeList(rc);
    } /* else */

    return(1);
} /* cmd_getcdromdirs */


static int cmd_getbasedir(char *args)
{
    printf("Base dir is [%s].\n", PHYSFS_getBaseDir());
    return(1);
} /* cmd_getbasedir */


static int cmd_getuserdir(char *args)
{
    printf("User dir is [%s].\n", PHYSFS_getUserDir());
    return(1);
} /* cmd_getuserdir */


static int cmd_getwritedir(char *args)
{
    printf("Write dir is [%s].\n", PHYSFS_getWriteDir());
    return(1);
} /* cmd_getwritedir */


static int cmd_setwritedir(char *args)
{
    if (*args == '\"')
    {
        args++;
        args[strlen(args) - 1] = '\0';
    } /* if */

    if (PHYSFS_setWriteDir(args))
        printf("Successful.\n");
    else
        printf("Failure. reason: %s.\n", PHYSFS_getLastError());

    return(1);
} /* cmd_setwritedir */


static int cmd_permitsyms(char *args)
{
    int num;

    if (*args == '\"')
    {
        args++;
        args[strlen(args) - 1] = '\0';
    } /* if */

    num = atoi(args);
    PHYSFS_permitSymbolicLinks(num);
    printf("Symlinks are now %s.\n", num ? "permitted" : "forbidden");
    return(1);
} /* cmd_permitsyms */


static int cmd_setsaneconfig(char *args)
{
    char *org;
    char *appName;
    char *arcExt;
    int inclCD;
    int arcsFirst;
    char *ptr = args;

        /* ugly. */
    org = ptr;
    ptr = strchr(ptr, ' '); *ptr = '\0'; ptr++; appName = ptr;
    ptr = strchr(ptr, ' '); *ptr = '\0'; ptr++; arcExt = ptr;
    ptr = strchr(ptr, ' '); *ptr = '\0'; ptr++; inclCD = atoi(arcExt);
    arcsFirst = atoi(ptr);

    if (strcmp(arcExt, "!") == 0)
        arcExt = NULL;

    if (PHYSFS_setSaneConfig(org, appName, arcExt, inclCD, arcsFirst))
        printf("Successful.\n");
    else
        printf("Failure. reason: %s.\n", PHYSFS_getLastError());

    return(1);
} /* cmd_setsaneconfig */


static int cmd_mkdir(char *args)
{
    if (*args == '\"')
    {
        args++;
        args[strlen(args) - 1] = '\0';
    } /* if */

    if (PHYSFS_mkdir(args))
        printf("Successful.\n");
    else
        printf("Failure. reason: %s.\n", PHYSFS_getLastError());

    return(1);
} /* cmd_mkdir */


static int cmd_delete(char *args)
{
    if (*args == '\"')
    {
        args++;
        args[strlen(args) - 1] = '\0';
    } /* if */

    if (PHYSFS_delete(args))
        printf("Successful.\n");
    else
        printf("Failure. reason: %s.\n", PHYSFS_getLastError());

    return(1);
} /* cmd_delete */


static int cmd_getrealdir(char *args)
{
    const char *rc;

    if (*args == '\"')
    {
        args++;
        args[strlen(args) - 1] = '\0';
    } /* if */

    rc = PHYSFS_getRealDir(args);
    if (rc)
        printf("Found at [%s].\n", rc);
    else
        printf("Not found.\n");

    return(1);
} /* cmd_getrealdir */


static int cmd_exists(char *args)
{
    int rc;

    if (*args == '\"')
    {
        args++;
        args[strlen(args) - 1] = '\0';
    } /* if */

    rc = PHYSFS_exists(args);
    printf("File %sexists.\n", rc ? "" : "does not ");
    return(1);
} /* cmd_exists */


static int cmd_isdir(char *args)
{
    int rc;

    if (*args == '\"')
    {
        args++;
        args[strlen(args) - 1] = '\0';
    } /* if */

    rc = PHYSFS_isDirectory(args);
    printf("File %s a directory.\n", rc ? "is" : "is NOT");
    return(1);
} /* cmd_isdir */


static int cmd_issymlink(char *args)
{
    int rc;

    if (*args == '\"')
    {
        args++;
        args[strlen(args) - 1] = '\0';
    } /* if */

    rc = PHYSFS_isSymbolicLink(args);
    printf("File %s a symlink.\n", rc ? "is" : "is NOT");
    return(1);
} /* cmd_issymlink */


static int cmd_cat(char *args)
{
    PHYSFS_file *f;

    if (*args == '\"')
    {
        args++;
        args[strlen(args) - 1] = '\0';
    } /* if */

    f = PHYSFS_openRead(args);
    if (f == NULL)
        printf("failed to open. Reason: [%s].\n", PHYSFS_getLastError());
    else
    {
        while (1)
        {
            char buffer[128];
            PHYSFS_sint64 rc;
            PHYSFS_sint64 i;
            rc = PHYSFS_read(f, buffer, 1, sizeof (buffer));

            for (i = 0; i < rc; i++)
                fputc((int) buffer[i], stdout);

            if (rc < sizeof (buffer))
            {
                printf("\n\n");
                if (!PHYSFS_eof(f))
                {
                    printf("\n (Error condition in reading. Reason: [%s])\n\n",
                           PHYSFS_getLastError());
                } /* if */
                PHYSFS_close(f);
                return(1);
            } /* if */
        } /* while */
    } /* else */

    return(1);
} /* cmd_cat */


static int cmd_filelength(char *args)
{
    PHYSFS_file *f;

    if (*args == '\"')
    {
        args++;
        args[strlen(args) - 1] = '\0';
    } /* if */

    f = PHYSFS_openRead(args);
    if (f == NULL)
        printf("failed to open. Reason: [%s].\n", PHYSFS_getLastError());
    else
    {
        PHYSFS_sint64 len = PHYSFS_fileLength(f);
        if (len == -1)
            printf("failed to determine length. Reason: [%s].\n", PHYSFS_getLastError());
        else
            printf(" (cast to int) %d bytes.\n", (int) len);

        PHYSFS_close(f);
    } /* else */

    return(1);
} /* cmd_filelength */


#define WRITESTR "The cat sat on the mat.\n\n"

static int cmd_append(char *args)
{
    PHYSFS_file *f;

    if (*args == '\"')
    {
        args++;
        args[strlen(args) - 1] = '\0';
    } /* if */

    f = PHYSFS_openAppend(args);
    if (f == NULL)
        printf("failed to open. Reason: [%s].\n", PHYSFS_getLastError());
    else
    {
        size_t bw = strlen(WRITESTR);
        PHYSFS_sint64 rc = PHYSFS_write(f, WRITESTR, 1, bw);
        if (rc != bw)
        {
            printf("Wrote (%d) of (%d) bytes. Reason: [%s].\n",
                   (int) rc, (int) bw, PHYSFS_getLastError());
        } /* if */
        else
        {
            printf("Successful.\n");
        } /* else */

        PHYSFS_close(f);
    } /* else */

    return(1);
} /* cmd_append */


static int cmd_write(char *args)
{
    PHYSFS_file *f;

    if (*args == '\"')
    {
        args++;
        args[strlen(args) - 1] = '\0';
    } /* if */

    f = PHYSFS_openWrite(args);
    if (f == NULL)
        printf("failed to open. Reason: [%s].\n", PHYSFS_getLastError());
    else
    {
        size_t bw = strlen(WRITESTR);
        PHYSFS_sint64 rc = PHYSFS_write(f, WRITESTR, 1, bw);
        if (rc != bw)
        {
            printf("Wrote (%d) of (%d) bytes. Reason: [%s].\n",
                   (int) rc, (int) bw, PHYSFS_getLastError());
        } /* if */
        else
        {
            printf("Successful.\n");
        } /* else */

        PHYSFS_close(f);
    } /* else */

    return(1);
} /* cmd_write */


static void modTimeToStr(PHYSFS_sint64 modtime, char *modstr, size_t strsize)
{
    time_t t = (time_t) modtime;
    char *str = ctime(&t);
    strncpy(modstr, str, strsize);
    modstr[strsize-1] = '\0';
} /* modTimeToStr */


static int cmd_getlastmodtime(char *args)
{
    PHYSFS_sint64 rc = PHYSFS_getLastModTime(args);
    if (rc == -1)
        printf("Failed to determine. Reason: [%s].\n", PHYSFS_getLastError());
    else
    {
        char modstr[64];
        modTimeToStr(rc, modstr, sizeof (modstr));
        printf("Last modified: %s (%ld).\n", modstr, (long) rc);
    } /* else */

    return(1);
} /* cmd_getLastModTime */


/* must have spaces trimmed prior to this call. */
static int count_args(const char *str)
{
    int retval = 0;
    int in_quotes = 0;

    if (str != NULL)
    {
        for (; *str != '\0'; str++)
        {
            if (*str == '\"')
                in_quotes = !in_quotes;
            else if ((*str == ' ') && (!in_quotes))
                retval++;
        } /* for */
        retval++;
    } /* if */

    return(retval);
} /* count_args */


static int cmd_help(char *args);

typedef struct
{
    const char *cmd;
    int (*func)(char *args);
    int argcount;
    const char *usage;
} command_info;

static const command_info commands[] =
{
    { "quit",           cmd_quit,           0, NULL                         },
    { "q",              cmd_quit,           0, NULL                         },
    { "help",           cmd_help,           0, NULL                         },
    { "init",           cmd_init,           1, "<argv0>"                    },
    { "deinit",         cmd_deinit,         0, NULL                         },
    { "addarchive",     cmd_addarchive,     2, "<archiveLocation> <append>" },
    { "removearchive",  cmd_removearchive,  1, "<archiveLocation>"          },
    { "enumerate",      cmd_enumerate,      1, "<dirToEnumerate>"           },
    { "ls",             cmd_enumerate,      1, "<dirToEnumerate>"           },
    { "getlasterror",   cmd_getlasterror,   0, NULL                         },
    { "getdirsep",      cmd_getdirsep,      0, NULL                         },
    { "getcdromdirs",   cmd_getcdromdirs,   0, NULL                         },
    { "getsearchpath",  cmd_getsearchpath,  0, NULL                         },
    { "getbasedir",     cmd_getbasedir,     0, NULL                         },
    { "getuserdir",     cmd_getuserdir,     0, NULL                         },
    { "getwritedir",    cmd_getwritedir,    0, NULL                         },
    { "setwritedir",    cmd_setwritedir,    1, "<newWriteDir>"              },
    { "permitsymlinks", cmd_permitsyms,     1, "<1or0>"                     },
    { "setsaneconfig",  cmd_setsaneconfig,  4, "<appName> <arcExt> <includeCdRoms> <archivesFirst>" },
    { "mkdir",          cmd_mkdir,          1, "<dirToMk>"                  },
    { "delete",         cmd_delete,         1, "<dirToDelete>"              },
    { "getrealdir",     cmd_getrealdir,     1, "<fileToFind>"               },
    { "exists",         cmd_exists,         1, "<fileToCheck>"              },
    { "isdir",          cmd_isdir,          1, "<fileToCheck>"              },
    { "issymlink",      cmd_issymlink,      1, "<fileToCheck>"              },
    { "cat",            cmd_cat,            1, "<fileToCat>"                },
    { "filelength",     cmd_filelength,     1, "<fileToCheck>"              },
    { "append",         cmd_append,         1, "<fileToAppend>"             },
    { "write",          cmd_write,          1, "<fileToCreateOrTrash>"      },
    { "getlastmodtime", cmd_getlastmodtime, 1, "<fileToExamine>"            },
    { NULL,             NULL,              -1, NULL                         }
};


static void output_usage(const char *intro, const command_info *cmdinfo)
{
    if (cmdinfo->argcount == 0)
        printf("%s \"%s\" (no arguments)\n", intro, cmdinfo->cmd);
    else
        printf("%s \"%s %s\"\n", intro, cmdinfo->cmd, cmdinfo->usage);
} /* output_usage */


static int cmd_help(char *args)
{
    const command_info *i;

    printf("Commands:\n");
    for (i = commands; i->cmd != NULL; i++)
        output_usage("  -", i);

    return(1);
} /* output_cmd_help */


static void trim_command(const char *orig, char *copy)
{
    const char *i;
    char *writeptr = copy;
    int spacecount = 0;
    int have_first = 0;

    for (i = orig; *i != '\0'; i++)
    {
        if (*i == ' ')
        {
            if ((*(i + 1) != ' ') && (*(i + 1) != '\0'))
            {
                if ((have_first) && (!spacecount))
                {
                    spacecount++;
                    *writeptr = ' ';
                    writeptr++;
                } /* if */
            } /* if */
        } /* if */
        else
        {
            have_first = 1;
            spacecount = 0;
            *writeptr = *i;
            writeptr++;
        } /* else */
    } /* for */

    *writeptr = '\0';

    /*
    printf("\n command is [%s].\n", copy);
    */
} /* trim_command */


static int process_command(char *complete_cmd)
{
    const command_info *i;
    char *cmd_copy = malloc(strlen(complete_cmd) + 1);
    char *args;
    int rc = 1;

    if (cmd_copy == NULL)
    {
        printf("\n\n\nOUT OF MEMORY!\n\n\n");
        return(0);
    } /* if */

    trim_command(complete_cmd, cmd_copy);
    args = strchr(cmd_copy, ' ');
    if (args != NULL)
    {
        *args = '\0';
        args++;
    } /* else */

    if (cmd_copy[0] != '\0')
    {
        for (i = commands; i->cmd != NULL; i++)
        {
            if (strcmp(i->cmd, cmd_copy) == 0)
            {
                if ((i->argcount >= 0) && (count_args(args) != i->argcount))
                    output_usage("usage:", i);
                else
                    rc = i->func(args);
                break;
            } /* if */
        } /* for */

        if (i->cmd == NULL)
            printf("Unknown command. Enter \"help\" for instructions.\n");

#if (defined PHYSFS_HAVE_READLINE)
        add_history(complete_cmd);
        if (history_file)
        {
            fprintf(history_file, "%s\n", complete_cmd);
            fflush(history_file);
        } /* if */
#endif

    } /* if */

    free(cmd_copy);
    return(rc);
} /* process_command */


static void open_history_file(void)
{
#if (defined PHYSFS_HAVE_READLINE)
#if 0
    const char *envr = getenv("TESTPHYSFS_HISTORY");
    if (!envr)
        return;
#else
    char envr[256];
    strcpy(envr, PHYSFS_getUserDir());
    strcat(envr, ".testphys_history");
#endif

    if (access(envr, F_OK) == 0)
    {
        char buf[512];
        FILE *f = fopen(envr, "r");
        if (!f)
        {
            printf("\n\n"
                   "Could not open history file [%s] for reading!\n"
                   "  Will not have past history available.\n\n",
                    envr);
            return;
        } /* if */

        do
        {
            fgets(buf, sizeof (buf), f);
            if (buf[strlen(buf) - 1] == '\n')
                buf[strlen(buf) - 1] = '\0';
            add_history(buf);
        } while (!feof(f));

        fclose(f);
    } /* if */

    history_file = fopen(envr, "ab");
    if (!history_file)
    {
        printf("\n\n"
               "Could not open history file [%s] for appending!\n"
               "  Will not be able to record this session's history.\n\n",
                envr);
    } /* if */
#endif
} /* open_history_file */


int main(int argc, char **argv)
{
    char *buf = NULL;
    int rc = 0;

#if (defined __MWERKS__)
    extern tSIOUXSettings SIOUXSettings;
    SIOUXSettings.asktosaveonclose = 0;
    SIOUXSettings.autocloseonquit = 1;
    SIOUXSettings.rows = 40;
    SIOUXSettings.columns = 120;
#endif

    printf("\n");

    if (!PHYSFS_init(argv[0]))
    {
        printf("PHYSFS_init() failed!\n  reason: %s.\n", PHYSFS_getLastError());
        return(1);
    } /* if */

    output_versions();
    output_archivers();

    open_history_file();

    printf("Enter commands. Enter \"help\" for instructions.\n");

    do
    {
#if (defined PHYSFS_HAVE_READLINE)
        buf = readline("> ");
#else
        int i;
        buf = malloc(512);
        memset(buf, '\0', 512);
        printf("> ");
        for (i = 0; i < 511; i++)
        {
            int ch = fgetc(stdin);
            if (ch == EOF)
            {
                strcpy(buf, "quit");
                break;
            } /* if */
            else if ((ch == '\n') || (ch == '\r'))
            {
                buf[i] = '\0';
                break;
            } /* else if */
            else if (ch == '\b')
            {
                if (i > 0)
                    i--;
            } /* else if */
            else
            {
                buf[i] = (char) ch;
            } /* else */
        } /* for */
#endif

        rc = process_command(buf);
        free(buf);
    } while (rc);

    if (!PHYSFS_deinit())
        printf("PHYSFS_deinit() failed!\n  reason: %s.\n", PHYSFS_getLastError());

    if (history_file)
        fclose(history_file);

/*
    printf("\n\ntest_physfs written by ryan c. gordon.\n");
    printf(" it makes you shoot teh railgun bettar.\n");
*/

    return(0);
} /* main */

/* end of test_physfs.c ... */

