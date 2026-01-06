#include <help.h>

void global_help() {
    std::cout
        << "jrnl — a tiny, plain-text terminal journal\n"
        << "\n"
        << "Usage:\n"
        << "  jrnl <command> [options]\n"
        << "\n"
        << "Commands:\n"
        << "  init        Initialize a journal\n"
        << "  add         Add a new entry\n"
        << "  show        Display entries\n"
        << "  backup      Create a journal backup\n"
        << "  help        Show help for a command\n"
        << "\n"
        << "Common options:\n"
        << "  --local     Operate on local journal (default if present)\n"
        << "  --global    Operate on global journal\n"
        << "\n"
        << "Ranges (used with `show`):\n"
        << "  *       All entries\n"
        << "  N       Entry with id N\n"
        << "  :N      First N entries\n"
        << "  N:      Last N entries\n"
        << "\n"
        << "Example:\n"
        << "  jrnl add \"met a cat\" mood\n"
        << "  jrnl show :10 --global\n";
}

void add_help() {
    std::cout
        << "jrnl add — add a journal entry\n"
        << "\n"
        << "Usage:\n"
        << "  jrnl add <entry> [tag] [--local|--global]\n"
        << "  jrnl add              (read entry from stdin)\n"
        << "\n"
        << "Options:\n"
        << "  --local     Use local journal\n"
        << "  --global    Use global journal\n"
        << "\n"
        << "Notes:\n"
        << "  - If no entry is given, a single line is read from stdin\n"
        << "  - Journal files and directories are created automatically\n";
}

void show_help() {
    std::cout
        << "jrnl show — display journal entries\n"
        << "\n"
        << "Usage:\n"
        << "  jrnl show [range] [--local|--global]\n"
        << "\n"
        << "Ranges:\n"
        << "  *       All entries\n"
        << "  N       Entry with id N\n"
        << "  :N      First N entries\n"
        << "  N:      Last N entries\n"
        << "\n"
        << "Options:\n"
        << "  --local     Use local journal\n"
        << "  --global    Use global journal\n"
        << "\n"
        << "Notes:\n"
        << "  - Malformed entries may be skipped\n";
}

void init_help() {
    std::cout
        << "jrnl init — initialize a journal\n"
        << "\n"
        << "Usage:\n"
        << "  jrnl init --local\n"
        << "  jrnl init --global\n"
        << "\n"
        << "Options:\n"
        << "  --local     Create a journal in the current directory\n"
        << "  --global    Create a user-wide journal\n"
        << "\n"
        << "Notes:\n"
        << "  - Creates journal files, directories, and config if missing\n"
        << "  - Existing journals are not overwritten\n";
}

void backup_help() {
    std::cout
        << "jrnl backup — create a journal backup\n"
        << "\n"
        << "Usage:\n"
        << "  jrnl backup [name] [--local|--global]\n"
        << "\n"
        << "Options:\n"
        << "  --local     Backup local journal\n"
        << "  --global    Backup global journal\n"
        << "\n"
        << "Notes:\n"
        << "  - If no name is provided, a timestamp is used\n"
        << "  - Backups are written to the configured backup directory\n";
}


