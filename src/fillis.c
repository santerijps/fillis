#include <stdio.h>
#include <windows.h>
#include <tlhelp32.h>
#include "fillis.h"


char *ARGV_CMD = NULL;
char *ARGV_IGNORE = NULL;
IndexPair ARGV_IGNORE_PAIRS[32];
unsigned int ARGV_IGNORE_PAIRS_LEN = 0;

unsigned int FILE_WRITE_INDEX = 0;
DWORD FILE_WRITE_TIMES[FILE_WRITE_TIMES_SIZE];
STARTUPINFO SI;
PROCESS_INFORMATION PI;


int main(int argc, char **argv) {

  ParseArgs(argc, argv);

  while (TRUE) {
    if (SomeFileWasWrittenTo(".", TRUE)) {
      KillProcessTree();
      LaunchProcess(ARGV_CMD);
    }
    Sleep(SLEEP_TIME);
  }

  return 0;

}


void PrintUsage() {
  printf(
    "Usage: fillis [command] [options?]\n"
    "\n"
    "Options:\n"
    "  -h, --help                                   Show app usage.\n"
    "  -i, --ignore    [Comma separated values]     Exact file/directory names to ignore (not listen to).\n"
    "                                               The value of this options should be a comma separated list.\n"
    "                                               E.g. --ignore '.vscode,node_modules'\n"
  );
}


void ParseArgs(int argc, char **argv) {

  if (argc < 2) {
    printf("Invalid input. Enter '%s -h' for usage.\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  for (int i = 1; i < argc; i++) {

    if (!strcmp(argv[i], "--help") || !strcmp(argv[i], "-h")) {
      PrintUsage();
      exit(0);
    }

    else if ((!strcmp(argv[i], "--ignore") || !strcmp(argv[i], "-i")) && argc > i) {
      i += 1;
      ParseIgnorePairs(argv[i]);
      ARGV_IGNORE = argv[i];
    }

    else {
      ARGV_CMD = argv[i];
    }

  }

}


BOOL SomeFileWasWrittenTo(const char *dir_path, int reset_index) {

  int result = FALSE;
  WIN32_FIND_DATA file_data;
  HANDLE file_handle = NULL;
  char search_path[SEARCH_PATH_SIZE] = {0};

  if (reset_index) {
    FILE_WRITE_INDEX = 0;
  }

  sprintf(search_path, "%s/*.*", dir_path);
  file_handle = FindFirstFile(search_path, &file_data);

  if (file_handle == INVALID_HANDLE_VALUE) {
    return 0;
  }

  WhileLoopTop: while (TRUE) {

    if (!FindNextFile(file_handle, &file_data)) {
      break;
    }

    if (!strcmp(file_data.cFileName, ".") || !strcmp(file_data.cFileName, "..")) {
      continue;
    }

    for (unsigned int i = 0; i < ARGV_IGNORE_PAIRS_LEN; i++) {
      if (SubStringEquals(ARGV_IGNORE, ARGV_IGNORE_PAIRS[i].start, ARGV_IGNORE_PAIRS[i].stop, file_data.cFileName)) {
        goto WhileLoopTop;
      }
    }

    sprintf(search_path, "%s\\%s", dir_path, file_data.cFileName);

    if (file_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
      result = SomeFileWasWrittenTo(search_path, FALSE) || result;
      continue;
    }

    if (FILE_WRITE_TIMES[FILE_WRITE_INDEX] != file_data.ftLastWriteTime.dwLowDateTime) {
      result = TRUE;
    }

    FILE_WRITE_TIMES[FILE_WRITE_INDEX] = file_data.ftLastWriteTime.dwLowDateTime;
    FILE_WRITE_INDEX += 1;

  }

  FindClose(file_handle);
  return result;

}


DWORD LaunchProcess(char *run) {

  ZeroMemory(&SI, sizeof(SI));
  ZeroMemory(&PI, sizeof(PI));
  SI.cb = sizeof(SI);

  char cmd[LAUNCH_PROCESS_RUN_SIZE] = {0};
  sprintf(cmd, "%s /c %s", SHELL, run);

  DWORD flags = NORMAL_PRIORITY_CLASS | CREATE_UNICODE_ENVIRONMENT;
  if (!CreateProcess(SHELL, cmd, NULL, NULL, FALSE, flags, NULL, NULL, &SI, &PI)) {
    return GetLastError();
  }

  return 0;

}


void KillProcessTree() {

  PROCESSENTRY32 pe;
  HANDLE hSnap, hChildProc;

  ZeroMemory(&pe, sizeof(PROCESSENTRY32));
  pe.dwSize = sizeof(PROCESSENTRY32);
  hSnap = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

  if (Process32First(hSnap, &pe)) while (TRUE) {

    if (pe.th32ParentProcessID == PI.dwProcessId) {
      hChildProc = OpenProcess(PROCESS_ALL_ACCESS, FALSE, pe.th32ProcessID);
      if (hChildProc) {
        TerminateProcess(hChildProc, CTRL_BREAK_EVENT);
        CloseHandle(hChildProc);
      }
    }

    if (!Process32Next(hSnap, &pe)) {
      break;
    }

  }

  TerminateThread(PI.hThread, CTRL_BREAK_EVENT);
  TerminateProcess(PI.hProcess, CTRL_BREAK_EVENT);
  CloseHandle(PI.hThread);
  CloseHandle(PI.hProcess);

}


void ParseIgnorePairs(char *s) {

  size_t length = strlen(s);
  unsigned char i, j;

  for (i = 0, j = 0; i < length; i++) {
    if (s[i] == ',') {
      IndexPair pair = { .start = j, .stop = i };
      ARGV_IGNORE_PAIRS[ARGV_IGNORE_PAIRS_LEN++] = pair;
      j = i + 1;
    }
  }

  if (j < length) {
    IndexPair pair = { .start = j, .stop = length };
    ARGV_IGNORE_PAIRS[ARGV_IGNORE_PAIRS_LEN++] = pair;
  }

}


BOOL SubStringEquals(char *s1, unsigned int start, unsigned int stop, char *s2) {

  unsigned int s1_len = stop - start;
  unsigned int s2_len = strlen(s2);

  if (s1_len != s2_len) {
    return FALSE;
  }

  for (unsigned int s1_i = start, s2_i = 0; s1_i < stop; s1_i++, s2_i++) {
    if (s1[s1_i] != s2[s2_i]) {
      return FALSE;
    }
  }

  return TRUE;

}