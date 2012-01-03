/******************************************************************************\
*                                                                              *
*        Copyright (c) 2003, The Regents of the University of California       *
*      See the file COPYRIGHT for a complete copyright notice and license.     *
*                                                                              *
********************************************************************************
*
* Parse commandline functions.
*
\******************************************************************************/

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <string.h>

#include "ior.h"
#include "aiori.h"
#include "parse_options.h"
#include "utilities.h"

IOR_param_t initialTestParams;

static void RecalculateExpectedFileSize(IOR_param_t *params)
{
	params->expectedAggFileSize =
		params->blockSize * params->segmentCount * params->numTasks;
}

/*
 * Check and correct all settings of each test in queue for correctness.
 */
static void CheckRunSettings(IOR_test_t *tests)
{
	IOR_test_t *ptr;
	IOR_param_t *params;

	for (ptr = tests; ptr != NULL; ptr = ptr->next) {
		params = &ptr->params;
                /* If no write/read/check action requested, set both write and read */
                if (params->writeFile == FALSE
                    && params->readFile == FALSE
                    && params->checkWrite == FALSE
                    && params->checkRead == FALSE) {
                        params->readFile = TRUE;
                        params->writeFile = TRUE;
                }
                /* If numTasks set to 0, use all tasks */
                if (params->numTasks == 0) {
                        MPI_CHECK(MPI_Comm_size(MPI_COMM_WORLD,
						&params->numTasks),
                                  "MPI_Comm_size() error");
			RecalculateExpectedFileSize(params);
                }
        }
}

/*
 * Set flags from commandline string/value pairs.
 */
void DecodeDirective(char *line, IOR_param_t *params)
{
        char option[MAX_STR];
        char value[MAX_STR];
        int rc;

        rc = sscanf(line, " %[^=# \t\r\n] = %[^# \t\r\n] ", option, value);
        if (rc != 2 && rank == 0) {
                fprintf(stdout, "Syntax error in configuration options: %s\n",
                        line);
                MPI_CHECK(MPI_Abort(MPI_COMM_WORLD, -1), "MPI_Abort() error");
        }
        if (strcasecmp(option, "api") == 0) {
                strcpy(params->api, value);
        } else if (strcasecmp(option, "testnum") == 0) {
                params->TestNum = atoi(value);
        } else if (strcasecmp(option, "debug") == 0) {
                strcpy(params->debug, value);
        } else if (strcasecmp(option, "platform") == 0) {
                strcpy(params->platform, value);
        } else if (strcasecmp(option, "testfile") == 0) {
                strcpy(params->testFileName, value);
        } else if (strcasecmp(option, "hintsfilename") == 0) {
                strcpy(params->hintsFileName, value);
        } else if (strcasecmp(option, "deadlineforstonewalling") == 0) {
                params->deadlineForStonewalling = atoi(value);
        } else if (strcasecmp(option, "maxtimeduration") == 0) {
                params->maxTimeDuration = atoi(value);
        } else if (strcasecmp(option, "outlierthreshold") == 0) {
                params->outlierThreshold = atoi(value);
        } else if (strcasecmp(option, "nodes") == 0) {
                params->nodes = atoi(value);
        } else if (strcasecmp(option, "repetitions") == 0) {
                params->repetitions = atoi(value);
        } else if (strcasecmp(option, "intertestdelay") == 0) {
                params->interTestDelay = atoi(value);
        } else if (strcasecmp(option, "readfile") == 0) {
                params->readFile = atoi(value);
        } else if (strcasecmp(option, "writefile") == 0) {
                params->writeFile = atoi(value);
        } else if (strcasecmp(option, "fileperproc") == 0) {
                params->filePerProc = atoi(value);
        } else if (strcasecmp(option, "reordertasksconstant") == 0) {
                params->reorderTasks = atoi(value);
        } else if (strcasecmp(option, "taskpernodeoffset") == 0) {
                params->taskPerNodeOffset = atoi(value);
        } else if (strcasecmp(option, "reordertasksrandom") == 0) {
                params->reorderTasksRandom = atoi(value);
        } else if (strcasecmp(option, "reordertasksrandomSeed") == 0) {
                params->reorderTasksRandomSeed = atoi(value);
        } else if (strcasecmp(option, "checkwrite") == 0) {
                params->checkWrite = atoi(value);
        } else if (strcasecmp(option, "checkread") == 0) {
                params->checkRead = atoi(value);
        } else if (strcasecmp(option, "keepfile") == 0) {
                params->keepFile = atoi(value);
        } else if (strcasecmp(option, "keepfilewitherror") == 0) {
                params->keepFileWithError = atoi(value);
        } else if (strcasecmp(option, "multiFile") == 0) {
                params->multiFile = atoi(value);
        } else if (strcasecmp(option, "quitonerror") == 0) {
                params->quitOnError = atoi(value);
        } else if (strcasecmp(option, "segmentcount") == 0) {
                params->segmentCount = StringToBytes(value);
		RecalculateExpectedFileSize(params);
        } else if (strcasecmp(option, "blocksize") == 0) {
                params->blockSize = StringToBytes(value);
		RecalculateExpectedFileSize(params);
        } else if (strcasecmp(option, "transfersize") == 0) {
                params->transferSize = StringToBytes(value);
        } else if (strcasecmp(option, "setalignment") == 0) {
                params->setAlignment = StringToBytes(value);
        } else if (strcasecmp(option, "singlexferattempt") == 0) {
                params->singleXferAttempt = atoi(value);
        } else if (strcasecmp(option, "individualdatasets") == 0) {
                params->individualDataSets = atoi(value);
        } else if (strcasecmp(option, "intraTestBarriers") == 0) {
                params->intraTestBarriers = atoi(value);
        } else if (strcasecmp(option, "nofill") == 0) {
                params->noFill = atoi(value);
        } else if (strcasecmp(option, "verbose") == 0) {
                params->verbose = atoi(value);
        } else if (strcasecmp(option, "settimestampsignature") == 0) {
                params->setTimeStampSignature = atoi(value);
        } else if (strcasecmp(option, "collective") == 0) {
                params->collective = atoi(value);
        } else if (strcasecmp(option, "preallocate") == 0) {
                params->preallocate = atoi(value);
        } else if (strcasecmp(option, "storefileoffset") == 0) {
                params->storeFileOffset = atoi(value);
        } else if (strcasecmp(option, "usefileview") == 0) {
                params->useFileView = atoi(value);
        } else if (strcasecmp(option, "usesharedfilepointer") == 0) {
                params->useSharedFilePointer = atoi(value);
        } else if (strcasecmp(option, "useo_direct") == 0) {
                params->useO_DIRECT = atoi(value);
        } else if (strcasecmp(option, "usestrideddatatype") == 0) {
                params->useStridedDatatype = atoi(value);
        } else if (strcasecmp(option, "showhints") == 0) {
                params->showHints = atoi(value);
        } else if (strcasecmp(option, "showhelp") == 0) {
                params->showHelp = atoi(value);
        } else if (strcasecmp(option, "uniqueDir") == 0) {
                params->uniqueDir = atoi(value);
        } else if (strcasecmp(option, "useexistingtestfile") == 0) {
                params->useExistingTestFile = atoi(value);
        } else if (strcasecmp(option, "fsyncperwrite") == 0) {
                params->fsyncPerWrite = atoi(value);
        } else if (strcasecmp(option, "fsync") == 0) {
                params->fsync = atoi(value);
        } else if (strcasecmp(option, "randomoffset") == 0) {
                params->randomOffset = atoi(value);
        } else if (strcasecmp(option, "applicationMemory") == 0) {
                params->applicationMemory = StringToBytes(value);
        } else if (strcasecmp(option, "lustrestripecount") == 0) {
#ifndef HAVE_LUSTRE_LUSTRE_USER_H
                ERR("ior was not compiled with Lustre support");
#endif
                params->lustre_stripe_count = atoi(value);
                params->lustre_set_striping = 1;
        } else if (strcasecmp(option, "lustrestripesize") == 0) {
#ifndef HAVE_LUSTRE_LUSTRE_USER_H
                ERR("ior was not compiled with Lustre support");
#endif
                params->lustre_stripe_size = StringToBytes(value);
                params->lustre_set_striping = 1;
        } else if (strcasecmp(option, "lustrestartost") == 0) {
#ifndef HAVE_LUSTRE_LUSTRE_USER_H
                ERR("ior was not compiled with Lustre support");
#endif
                params->lustre_start_ost = atoi(value);
                params->lustre_set_striping = 1;
        } else if (strcasecmp(option, "lustreignorelocks") == 0) {
#ifndef HAVE_LUSTRE_LUSTRE_USER_H
                ERR("ior was not compiled with Lustre support");
#endif
                params->lustre_ignore_locks = atoi(value);
#if USE_UNDOC_OPT
        } else if (strcasecmp(option, "corruptFile") == 0) {
                params->corruptFile = atoi(value);
        } else if (strcasecmp(option, "fillTheFileSystem") == 0) {
                params->fillTheFileSystem = atoi(value);
        } else if (strcasecmp(option, "includeDeleteTime") == 0) {
                params->includeDeleteTime = atoi(value);
        } else if (strcasecmp(option, "multiReRead") == 0) {
                params->multiReRead = atoi(value);
        } else if (strcasecmp(option, "nfs_rootpath") == 0) {
                strcpy(params->NFS_rootPath, value);
        } else if (strcasecmp(option, "nfs_servername") == 0) {
                strcpy(params->NFS_serverName, value);
        } else if (strcasecmp(option, "nfs_servercount") == 0) {
                params->NFS_serverCount = atoi(value);
#endif                          /* USE_UNDOC_OPT */
        } else if (strcasecmp(option, "numtasks") == 0) {
                params->numTasks = atoi(value);
		RecalculateExpectedFileSize(params);
        } else {
                if (rank == 0)
                        fprintf(stdout, "Unrecognized parameter \"%s\"\n",
                                option);
                MPI_CHECK(MPI_Abort(MPI_COMM_WORLD, -1), "MPI_Abort() error");
        }
}

/*
 * Parse a single line, which may contain multiple comma-seperated directives
 */
void ParseLine(char *line, IOR_param_t * test)
{
        char *start, *end;

        start = line;
        do {
                end = strchr(start, ',');
                if (end != NULL)
                        *end = '\0';
                DecodeDirective(start, test);
                start = end + 1;
        } while (end != NULL);

}

/*
 * Determines if the string "haystack" contains only the string "needle", and
 * possibly whitespace before and after needle.  Function is case insensitive.
 */
int contains_only(char *haystack, char *needle)
{
        char *ptr, *end;

        end = haystack + strlen(haystack);
        /* skip over leading shitspace */
        for (ptr = haystack; ptr < end; ptr++) {
                if (!isspace(*ptr))
                        break;
        }
        /* check for "needle" */
        if (strncasecmp(ptr, needle, strlen(needle)) != 0)
                return 0;
        /* make sure the rest of the line is only whitspace as well */
        for (ptr += strlen(needle); ptr < end; ptr++) {
                if (!isspace(*ptr))
                        return 0;
        }

        return 1;
}

/*
 * Read the configuration script, allocating and filling in the structure of
 * global parameters.
 */
IOR_test_t *ReadConfigScript(char *scriptName)
{
        int test_num = 0;
        int runflag = 0;
        char linebuf[MAX_STR];
        char empty[MAX_STR];
        FILE *file;
        IOR_test_t *head = NULL;
        IOR_test_t *tail = NULL;
        IOR_test_t *newTest = NULL;

        /* Initialize the first test */
        head = CreateTest(&initialTestParams, test_num++);
        tail = head;

        /* open the script */
        file = fopen(scriptName, "r");
        if (file == NULL)
                ERR("fopen() failed");

        /* search for the "IOR START" line */
        while (fgets(linebuf, MAX_STR, file) != NULL) {
                if (contains_only(linebuf, "ior start")) {
                        break;
                }
        }

        /* Iterate over a block of IOR commands */
        while (fgets(linebuf, MAX_STR, file) != NULL) {
                /* skip empty lines */
                if (sscanf(linebuf, "%s", empty) == -1)
                        continue;
                /* skip lines containing only comments */
                if (sscanf(linebuf, " #%s", empty) == 1)
                        continue;
                if (contains_only(linebuf, "ior stop")) {
			AllocResults(tail);
                        break;
                } else if (contains_only(linebuf, "run")) {
			AllocResults(tail);
                        runflag = 1;
                } else if (runflag) {
                        /* If this directive was preceded by a "run" line, then
                           create and initialize a new test structure */
			runflag = 0;
			tail->next = CreateTest(&tail->params, test_num++);
			tail = tail->next;
                        ParseLine(linebuf, &tail->params);
		} else {
                        ParseLine(linebuf, &tail->params);
                }
        }

        /* close the script */
        if (fclose(file) != 0)
                ERR("fclose() of script file failed");

        return head;
}

/*
 * Parse Commandline.
 */
IOR_test_t *ParseCommandLine(int argc, char **argv)
{
        static const char *opts =
          "A:a:b:BcCQ:ZX:d:D:YeEf:FgG:hHi:j:J:IkKlmM:nN:o:O:pPqrRs:St:T:uU:vVwWxz";
        int c, i;
        static IOR_test_t *tests = NULL;

        /* suppress getopt() error message when a character is unrecognized */
        opterr = 0;

        init_IOR_Param_t(&initialTestParams);
        GetPlatformName(initialTestParams.platform);
        initialTestParams.writeFile = initialTestParams.readFile = FALSE;
        initialTestParams.checkWrite = initialTestParams.checkRead = FALSE;

        while ((c = getopt(argc, argv, opts)) != -1) {
                switch (c) {
                case 'A':
                        initialTestParams.TestNum = atoi(optarg);
                        break;
                case 'a':
                        strcpy(initialTestParams.api, optarg);
                        break;
                case 'b':
                        initialTestParams.blockSize = StringToBytes(optarg);
			RecalculateExpectedFileSize(&initialTestParams);
                        break;
                case 'B':
                        initialTestParams.useO_DIRECT = TRUE;
                        break;
                case 'c':
                        initialTestParams.collective = TRUE;
                        break;
                case 'C':
                        initialTestParams.reorderTasks = TRUE;
                        break;
                case 'Q':
                        initialTestParams.taskPerNodeOffset = atoi(optarg);
                        break;
                case 'Z':
                        initialTestParams.reorderTasksRandom = TRUE;
                        break;
                case 'X':
                        initialTestParams.reorderTasksRandomSeed = atoi(optarg);
                        break;
                case 'd':
                        initialTestParams.interTestDelay = atoi(optarg);
                        break;
                case 'D':
                        initialTestParams.deadlineForStonewalling =
                            atoi(optarg);
                        break;
                case 'Y':
                        initialTestParams.fsyncPerWrite = TRUE;
                        break;
                case 'e':
                        initialTestParams.fsync = TRUE;
                        break;
                case 'E':
                        initialTestParams.useExistingTestFile = TRUE;
                        break;
                case 'f':
                        tests = ReadConfigScript(optarg);
                        break;
                case 'F':
                        initialTestParams.filePerProc = TRUE;
                        break;
                case 'g':
                        initialTestParams.intraTestBarriers = TRUE;
                        break;
                case 'G':
                        initialTestParams.setTimeStampSignature = atoi(optarg);
                        break;
                case 'h':
                        initialTestParams.showHelp = TRUE;
                        break;
                case 'H':
                        initialTestParams.showHints = TRUE;
                        break;
                case 'i':
                        initialTestParams.repetitions = atoi(optarg);
                        break;
                case 'I':
                        initialTestParams.individualDataSets = TRUE;
                        break;
                case 'j':
                        initialTestParams.outlierThreshold = atoi(optarg);
                        break;
                case 'J':
                        initialTestParams.setAlignment = StringToBytes(optarg);
                        break;
                case 'k':
                        initialTestParams.keepFile = TRUE;
                        break;
                case 'K':
                        initialTestParams.keepFileWithError = TRUE;
                        break;
                case 'l':
                        initialTestParams.storeFileOffset = TRUE;
                        break;
		case 'M':
                        initialTestParams.applicationMemory = StringToBytes(optarg);
                        break;
                case 'm':
                        initialTestParams.multiFile = TRUE;
                        break;
                case 'n':
                        initialTestParams.noFill = TRUE;
                        break;
                case 'N':
                        initialTestParams.numTasks = atoi(optarg);
			RecalculateExpectedFileSize(&initialTestParams);
                        break;
                case 'o':
                        strcpy(initialTestParams.testFileName, optarg);
                        break;
                case 'O':
                        ParseLine(optarg, &initialTestParams);
                        break;
                case 'p':
                        initialTestParams.preallocate = TRUE;
                        break;
                case 'P':
                        initialTestParams.useSharedFilePointer = TRUE;
                        break;
                case 'q':
                        initialTestParams.quitOnError = TRUE;
                        break;
                case 'r':
                        initialTestParams.readFile = TRUE;
                        break;
                case 'R':
                        initialTestParams.checkRead = TRUE;
                        break;
                case 's':
                        initialTestParams.segmentCount = atoi(optarg);
			RecalculateExpectedFileSize(&initialTestParams);
                        break;
                case 'S':
                        initialTestParams.useStridedDatatype = TRUE;
                        break;
                case 't':
                        initialTestParams.transferSize = StringToBytes(optarg);
                        break;
                case 'T':
                        initialTestParams.maxTimeDuration = atoi(optarg);
                        break;
                case 'u':
                        initialTestParams.uniqueDir = TRUE;
                        break;
                case 'U':
                        strcpy(initialTestParams.hintsFileName, optarg);
                        break;
                case 'v':
                        initialTestParams.verbose++;
                        break;
                case 'V':
                        initialTestParams.useFileView = TRUE;
                        break;
                case 'w':
                        initialTestParams.writeFile = TRUE;
                        break;
                case 'W':
                        initialTestParams.checkWrite = TRUE;
                        break;
                case 'x':
                        initialTestParams.singleXferAttempt = TRUE;
                        break;
                case 'z':
                        initialTestParams.randomOffset = TRUE;
                        break;
                default:
                        fprintf(stdout,
                                "ParseCommandLine: unknown option `-%c'.\n",
                                optopt);
                }
        }

        for (i = optind; i < argc; i++)
                fprintf(stdout, "non-option argument: %s\n", argv[i]);

        /* If an IOR script was not used, initialize test queue to the defaults */
        if (tests == NULL) {
                tests = CreateTest(&initialTestParams, 0);
		AllocResults(tests);
        }

        CheckRunSettings(tests);

        return (tests);
}
