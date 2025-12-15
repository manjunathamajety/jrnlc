# jrnl

A tiny terminal-based journaling tool written in C++.

`jrnl` is a personal project built to be simple, fast, and predictable. It stores
journal entries in a plain-text file and focuses on minimalism rather than features.
No database, no cloud sync — just text on disk.

This project also served as a learning exercise in modern C++, POSIX file handling,
and basic CLI design.

## Features

- Add journal entries from command-line arguments or stdin
- Optional tags for entries
- Display entries using ranges
- Filter entries by time (`--before`, `--after`)
- Atomic file writes (write-to-temp + rename)
- Plain-text, human-readable storage format
- Config file to store journal path, color codes 
- Optional ANSI colorized output (configurable)

## Example Usage

```sh
jrnl add "met a cat today" mood
jrnl add "this came from stdin"
echo "piped thoughts" | jrnl add

jrnl show *  #displays the entire jrnl
jrnl show 3  #displays the 3rd entry
jrnl show *5 #displays first 5 entries
jrnl show 5* #displays last 5 entries
jrnl show --after "2025-01-01 10:00"
jrnl show --before "2025-01-10 18:30"
jrnl show --after "2025-01-10 00:00" --before "2025-02-10 00:00"
jrnl show | grep "new-year" #can be composed for more search options

Both range based and time based filters can be composed with one another. 

```sh
jrnl show --after "2025-01-01 10:00" "*5"
jrnl show --after "2025-01-01 10:00" "5*"

jrnl show --before "2025-01-10 18:30" "*9"
jrnl show --before "2025-01-10 18:30" "9*"
```
Backup can be generated as follows;
```sh
jrnl backup #autogenerates backup's name with current timestamp
jrnl backup "backup@newyear" #custom backup name
```

## Storage Format

Journal entries are stored as one entry per line in a plain-text file:

```sh
id;tag;timestamp;text
```
- `id`        : Auto-generated numeric identifier
- `tag`       : User-provided label (defaults to `jrnl`)
- `timestamp` : Unix timestamp (`time_t`)
- `text`      : Journal entry content

This format is intentionally simple so entries can be inspected, backed up,
or processed using standard Unix tools.

Manual edits are possible, but malformed or partially corrupted entries
may be skipped when loading.

## Configuration

On first run, `jrnl` automatically creates a configuration file at:

- `$XDG_CONFIG_HOME/jrnl/jrnl.txt`
- or `~/.config/jrnl/jrnl.txt` if `XDG_CONFIG_HOME` is not set

The configuration file allows customization of:

- Journal storage path
- Backup directory
- ANSI color codes (256-color) for output

Example configuration:
```sh
PATH$/home/user/.local/share/jrnl/journal.txt

BACKUP_PATH$/home/user/.local/share/jrnl/backup

Id$32
Tag$34
Time$32
Text$37
```

Lines starting with `#` are treated as comments.

## Installation

### Build from source

`jrnl` uses CMake for building and installation.

Requirements:
- CMake ≥ 3.10
- A C++17-compatible compiler (GCC or Clang)
- POSIX-compatible system (Linux)

Clone the repository and build:

```sh
git clone https://github.com/<your-username>/jrnl.git
cd jrnl
mkdir build
cd build
cmake ..
make
```
To install the binary to your system 
```sh
sudo make install
```

## Design Notes

- Journal IDs are generated automatically by the tool
- Timestamps are stored as `time_t`
- Entries are appended in chronological order
- Writes are atomic (write to a temporary file, then rename)
- Journal files and parent directories are created automatically if missing

The tool assumes it is the sole writer of the journal file.
Concurrent writes or external modification are not supported.

## Non-goals

`jrnl` intentionally does not aim to:

- Provide encryption or secrecy guarantees, Human-readability is the aim
- Support concurrent writers
- Replace full-featured journaling or note-taking applications
- Offer cloud sync or cross-device features

It aims to follow Unix philosophy of "Do one thing, but do it well". Hence the features to pipe with other unix tools as well. 




