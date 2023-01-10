#ifndef _INC_WINDOWS
  #include <windows.h>
#endif

#define FILE_WRITE_TIMES_SIZE 512
#define SEARCH_PATH_SIZE 1024
#define LAUNCH_PROCESS_RUN_SIZE 512
#define SLEEP_TIME 200
#define SHELL "C:\\Windows\\System32\\WindowsPowerShell\\v1.0\\powershell.exe"

typedef struct IndexPair {
  unsigned char start;
  unsigned char stop;
} IndexPair;

void PrintUsage();
void ParseArgs(int argc, char **argv);
int SomeFileWasWrittenTo(const char *dir_path, int reset_index);
DWORD LaunchProcess(char *run);
void KillProcessTree();
void ParseIgnorePairs(char *s);
BOOL SubStringEquals(char *s1, unsigned int start, unsigned int stop, char *s2);