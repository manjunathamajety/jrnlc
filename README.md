# jrnl

A small C journal for the command line. It uses the plain-text format of
Python `jrnl` and stays composable with ordinary Unix tools.

## Build

```sh
make
make format
make test
```

`make test` shallow-clones upstream `jrnl` with sparse checkout and runs the
compatibility checks against its `basic_onefile.journal` fixture.

MSVC:

```sh
make jrnl.exe
make TESTTARGET=jrnl.exe TESTBIN=./jrnl.exe test
```

`requirements.ini` lists the optional Win32 compatibility header. Fetch it with
`mor .` when desired. The program still falls back to native MSVC APIs.

## Adding entries

```sh
jrnl "finished the build"
echo "a note from stdin" | jrnl
jrnl --date "2030-01-02 03:00" "a dated note"
```

`--date` accepts `YYYY-MM-DD HH:MM`. Advanced date expressions are deliberately
external:

```sh
jrnl --date "$(advancedDate 'tomorrow at 3am')" "notes..."
```

Python users can use `jrnl` directly or a helper based on `parsedatetime` or
`python-dateutil`.

Tags are ordinary `@tags` in the entry text:

```sh
jrnl "fixed the build" @windows @work
jrnl @work
jrnl --tags
```

## Reading and searching

```sh
jrnl
jrnl -n 10
jrnl -contains build
jrnl -and @work @windows
jrnl @work -not @personal
jrnl -starred
jrnl -from 2025-01-01 -to 2025-01-31
jrnl -until "August 2020"
jrnl -on 2025-01-15
```

`-from` and `-to` are inclusive. `-until` is an upper-bound alias. `-on`
selects one date, or a whole month with input like `August 2020`.

Output formats:

```sh
jrnl --format pretty
jrnl --format short
jrnl --format json
jrnl --export text
jrnl --export json
```

## Journal selection

The default journal is local when `.jrnlc/journal.txt` exists; otherwise it is
the global journal at `$XDG_DATA_HOME/jrnl/journal.txt` or
`$HOME/.local/share/jrnl/journal.txt`.

Use explicit selection when needed:

```sh
jrnl --local "project note"
jrnl --global "personal note"
jrnl --journal ~/work.journal "work note"
```

`JRNL_FILE` can override the default global path.

For smaller files and safer recovery, daily journals are also supported:

```sh
jrnl --daily --journal-dir ~/.jrnl "note"
jrnl --daily --journal-dir ~/.jrnl -from 2025-01-01 -to 2025-01-31
```

They are named `journal-YYYY-MM-DD` and are read in lexical date order.

## Design

The design is intentionally small:

- Entries remain human-readable plain text and work with standard tools.
- Writes use a sibling temporary file, preserve the existing bytes, flush and
  sync the new file, then replace the journal atomically.
- Daily files limit the amount of data copied for each atomic update.
- Local journals provide project-specific notebooks; the global journal remains
  available explicitly.
- There is no database, cloud service, or built-in encryption. Use GPG or an
  encrypted filesystem when needed.
- Natural-language dates are delegated to external tools rather than embedded
  as a large parser.
- The program assumes a single writer. Atomic replacement protects readers
  from partial files, but does not coordinate concurrent writers.

## Limitations

This is not a full Python `jrnl` replacement. Editor mode, deletion, encryption,
and the complete Python configuration format are intentionally omitted. The
plain-text journal format and the common search/export operations are the
compatibility target.

## License

MIT.
