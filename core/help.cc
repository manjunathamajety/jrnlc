#include <help.h>


void global_help() {
    std::cout
        << "jrnl - a tiny terminal jrnl for your secrets\n"
        << "\n"
        << "Usage:\n"
        << "  jrnl add <entry> [tag]\n"
        << "      Add a new journal entry.\n"
        << "      - <entry>   : the text you want to store.\n"
        << "      - [tag]     : optional label for the entry.\n"
        << "      If no arguments are given, jrnl will read a single line\n"
        << "      from stdin (you can also pipe text into it).\n"
        << "\n"
        << "  jrnl show [range]\n"
        << "      Show journal entries.\n"
        << "      If no range is given, jrnl shows all entries.\n"
        << "\n"
        << "Ranges:\n"
        << "  *       : show all entries\n"
        << "  N       : show only entry with id N\n"
        << "  *N      : show the first N entries\n"
        << "  N*      : show the last N entries\n"
        << "\n"
        << "Examples:\n"
        << "  jrnl add \"met a cat today\" mood\n"
        << "  echo \"pipe this into the eternity\" | jrnl add\n"
        << "  jrnl show *\n"
        << "  jrnl show 3\n"
        << "  jrnl show *10\n"
        << "  jrnl show 5*\n"
        << "\n"
        << "Your secrets are stored where your config tells them to be.\n"
        << "If jrnl can't find its config, it will quietly create one for you.\n";
}



void add_help() {
    std::cout
        << "jrnl add - add a new entry to your journal\n"
        << "\n"
        << "Usage:\n"
        << "  jrnl add <entry> [tag]\n"
        << "\n"
        << "Behavior:\n"
        << "  - If <entry> is provided as an argument, jrnl uses that.\n"
        << "  - If [tag] is provided, it is stored alongside the entry.\n"
        << "  - If no arguments are given, jrnl reads a single line from stdin.\n"
        << "\n"
        << "Examples:\n"
        << "  jrnl add \"today was weird\" mood\n"
        << "  jrnl add \"debugged segfaults for 3 hours\"\n"
        << "  echo \"this came from a pipe\" | jrnl add\n"
        << "\n"
        << "jrnl will make sure the journal file and directories exist.\n"
        << "If the universe refuses to cooperate, it will loudly complain and hopefully, not break :)\n";
}


void show_help() {
    std::cout
        << "jrnl show - display entries from your journal\n"
        << "\n"
        << "Usage:\n"
        << "  jrnl show [range]\n"
        << "\n"
        << "Ranges:\n"
        << "  *       : show all entries\n"
        << "  N       : show only entry with id N\n"
        << "  *N      : show the first N entries\n"
        << "  N*      : show the last N entries\n"
        << "\n"
        << "Examples:\n"
        << "  jrnl show            # show everything\n"
        << "  jrnl show *          # same as above\n"
        << "  jrnl show 3          # show only entry with id 3\n"
        << "  jrnl show *5         # show first 5 entries\n"
        << "  jrnl show 10*        # show last 10 entries\n"
        << "\n"
        << "Entries are printed with:\n"
        << "  - zero-padded id\n"
        << "  - tag\n"
        << "  - timestamp\n"
        << "  - the text you wrote\n"
        << "\n"
        << "If you edit the jrnl file by hand and break something,\n"
        << "jrnl will try and skip entries and bail out on you.\n";
}

