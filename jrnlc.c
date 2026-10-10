/* See LICENSE for the MIT license. */
/* C implementation of a jrnl-compatible plain-text journal. */

#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#ifdef _WIN32
#include <direct.h>
#include <io.h>
#include <windows.h>
#define isatty _isatty
#define fileno _fileno
#define MKDIR(path) _mkdir(path)
#else
#include <dirent.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#define MKDIR(path) mkdir(path, 0755)
#endif

#define VERSION "0.1.0"

typedef struct {
	char date[32];
	char *text;
} Entry;
typedef struct {
	Entry *v;
	size_t n, cap;
} Entries;

typedef enum { SELECT_CONTAINS, SELECT_TAG, SELECT_STARRED } SelectorKind;
typedef struct {
	SelectorKind kind;
	const char *value;
	int negated;
} Selector;

static void die(const char *message)
{
	fprintf(stderr, "jrnlc: %s\n", message);
	exit(1);
}
static void *xmalloc(size_t n)
{
	void *p = malloc(n);
	if (!p) die("out of memory");
	return p;
}
static void *xrealloc(void *p, size_t n)
{
	p = realloc(p, n);
	if (!p) die("out of memory");
	return p;
}

static void add_entry(Entries *es, const char *date, const char *text)
{
	if (es->n == es->cap) {
		es->cap = es->cap ? es->cap * 2 : 16;
		es->v = xrealloc(es->v, es->cap * sizeof(*es->v));
	}
	strncpy(es->v[es->n].date, date, sizeof(es->v[es->n].date) - 1);
	es->v[es->n].date[sizeof(es->v[es->n].date) - 1] = '\0';
	es->v[es->n].text = xmalloc(strlen(text) + 1);
	strcpy(es->v[es->n].text, text);
	es->n++;
}

static void free_entries(Entries *es)
{
	size_t i;
	for (i = 0; i < es->n; i++) free(es->v[i].text);
	free(es->v);
}

typedef struct {
	char **v;
	size_t n, cap;
} Tags;

static void add_tag(Tags *tags, const char *tag, size_t len)
{
	size_t i;
	for (i = 0; i < tags->n; i++) if (strlen(tags->v[i]) == len
		                                  && !strncmp(tags->v[i], tag, len)) return;
	if (tags->n == tags->cap) {
		tags->cap = tags->cap ? tags->cap * 2 : 8;
		tags->v = xrealloc(tags->v, tags->cap * sizeof(*tags->v));
	}
	tags->v[tags->n] = xmalloc(len + 1);
	memcpy(tags->v[tags->n], tag, len);
	tags->v[tags->n++][len] = '\0';
}

static void collect_tags(const char *text, Tags *tags)
{
	size_t i;
	for (i = 0; text[i]; i++) if (text[i] == '@' && (i == 0
		    || (!isalnum((unsigned char)text[i - 1]) && text[i - 1] != '_'))) {
			size_t start = ++i;
			while (isalnum((unsigned char)text[i]) || text[i] == '_'
			       || text[i] == '-') i++;
			if (i > start) add_tag(tags, text + start, i - start);
			if (!text[i]) break;
		}
}

static void free_tags(Tags *tags)
{
	size_t i;
	for (i = 0; i < tags->n; i++) free(tags->v[i]);
	free(tags->v);
}

static int has_tag(const Entry *e, const char *wanted)
{
	Tags tags = {0};
	size_t i;
	int found = 0;
	collect_tags(e->text, &tags);
	for (i = 0; i < tags.n; i++) if (!strcmp(tags.v[i], wanted)
		                                 || (wanted[0] == '@' && !strcmp(tags.v[i], wanted + 1))) found = 1;
	free_tags(&tags);
	return found;
}

static int header(const char *line, char *date, size_t size)
{
	if (line[0] != '[') return 0;
	const char *close = strchr(line, ']');
	if (!close || close - line < 2 || (size_t)(close - line - 1) >= size) return 0;
	memcpy(date, line + 1, (size_t)(close - line - 1));
	date[close - line - 1] = '\0';
	return 1;
}

static void trim_newline(char *s)
{
	size_t n = strlen(s);
	while (n && (s[n-1] == '\n' || s[n-1] == '\r')) s[--n] = '\0';
}

static int valid_timestamp(const char *s)
{
	size_t i;
	if (strlen(s) != 16 || s[4] != '-' || s[7] != '-' || s[10] != ' '
	    || s[13] != ':') return 0;
	for (i = 0; i < 16; i++)
		if (i != 4 && i != 7 && i != 10 && i != 13
		    && !isdigit((unsigned char)s[i])) return 0;
	return 1;
}

static int date_filter(const char *s, char out[11], int until)
{
	static const char *months[] = {"January", "February", "March", "April", "May", "June", "July", "August", "September", "October", "November", "December"};
	int y, m, d, i, days;
	char month[16];
	if (strlen(s) == 10 && s[4] == '-' && s[7] == '-'
	    && sscanf(s, "%4d-%2d-%2d", &y, &m, &d) == 3) {
		if (y < 1 || m < 1 || m > 12 || d < 1 || d > 31) return 0;
		memcpy(out, s, 10);
		out[10] = '\0';
		return 1;
	}
	if (sscanf(s, "%15s %d", month, &y) != 2 || y < 1 || y > 9999) return 0;
	for (i = 0; i < 12 && strcmp(month, months[i]); i++);
	if (i == 12) return 0;
	m = i + 1;
	if (until) {
		days = 31 - (m == 2) * 3 - (m == 4 || m == 6 || m == 9 || m == 11);
		if (m == 2 && (y % 4 == 0 && (y % 100 != 0 || y % 400 == 0))) days++;
		d = days;
	} else d = 1;
	snprintf(out, 11, "%04d-%02d-%02d", y, m, d);
	return 1;
}

static int is_starred(const Entry *e)
{
	const char *end = strchr(e->text, '\n');
	if (!end) end = e->text + strlen(e->text);
	while (end > e->text && isspace((unsigned char)end[-1])) end--;
	return end > e->text && end[-1] == '*';
}

static int selector_matches(const Entry *e, const Selector *s)
{
	int match;
	if (s->kind == SELECT_TAG) match = has_tag(e, s->value);
	else if (s->kind == SELECT_STARRED) match = is_starred(e);
	else match = strstr(e->text, s->value) != NULL;
	return s->negated ? !match : match;
}

static int entry_matches(const Entry *e, const Selector *selectors,
                         size_t count,
                         int and_mode, const char *from, const char *until)
{
	size_t i;
	int positive = 0, positive_matched = 0, matched = 0;
	if (from && strncmp(e->date, from, 10) < 0) return 0;
	if (until && strncmp(e->date, until, 10) > 0) return 0;
	for (i = 0; i < count; i++) {
		if (!selectors[i].negated) positive++;
		if (selectors[i].negated && !selector_matches(e, &selectors[i])) return 0;
		if (!selectors[i].negated && selector_matches(e, &selectors[i])) {
			matched = 1;
			positive_matched++;
		}
	}
	return !positive || (and_mode ? positive_matched == positive : matched);
}

static void load(const char *path, Entries *es)
{
	FILE *f = fopen(path, "r");
	char line[8192], date[32] = "", *text = NULL;
	size_t text_len = 0;
	if (!f) {
		if (errno == ENOENT) return;
		perror(path);
		exit(1);
	}
	while (fgets(line, sizeof(line), f)) {
		char d[32];
		trim_newline(line);
		if (header(line, d, sizeof(d))) {
			if (text) {
				add_entry(es, date, text);
				free(text);
				text = NULL;
				text_len = 0;
			}
			strcpy(date, d);
			{
				const char *body = strchr(line, ']') + 1;
				if (*body == ' ') body++;
				text_len = strlen(body);
				text = xmalloc(text_len + 1);
				memcpy(text, body, text_len + 1);
			}
		} else if (text && line[0]) {
			size_t n = strlen(line);
			text = xrealloc(text, text_len + n + 2);
			if (text_len) text[text_len++] = '\n';
			memcpy(text + text_len, line, n + 1);
			text_len += n;
		}
	}
	if (text) {
		add_entry(es, date, text);
		free(text);
	}
	fclose(f);
}

static int make_dir(const char *path)
{
	char *p = xmalloc(strlen(path) + 1), *q;
	strcpy(p, path);
	for (q = p + 1; *q; q++) if (*q == '/' || *q == '\\') {
			char save = *q;
			*q = '\0';
			if (*p && MKDIR(p) && errno != EEXIST) {
				free(p);
				return 0;
			}
			*q = save;
		}
	if (*p && MKDIR(p) && errno != EEXIST) {
		free(p);
		return 0;
	}
	free(p);
	return 1;
}

static int ensure_parent(const char *path)
{
	char *p = xmalloc(strlen(path) + 1), *slash;
	strcpy(p, path);
	slash = strrchr(p, '/');
#ifdef _WIN32
	{
		char *back = strrchr(p, '\\');
		if (!slash || (back && back > slash)) slash = back;
	}
#endif
	if (!slash) {
		free(p);
		return 1;
	}
	*slash = '\0';
	if (!*p) {
		free(p);
		return 1;
	}
	int ok = make_dir(p);
	free(p);
	return ok;
}

static int file_exists(const char *path)
{
	FILE *f = fopen(path, "r");
	if (f) {
		fclose(f);
		return 1;
	}
	return 0;
}

static const char *journal_path(const char *given, int local, int global)
{
	const char *env = given ? given : ((!local
	                                    && !global) ? getenv("JRNL_FILE") : NULL);
	static char path[4096];
	const char *home, *xdg;
	if (env) return env;
	if (!global && (local || file_exists(".jrnlc/journal.txt")))
		return ".jrnlc/journal.txt";
	xdg = getenv("XDG_DATA_HOME");
	home = getenv("HOME");
#ifdef _WIN32
	if (!home) home = getenv("USERPROFILE");
#endif
	if (xdg && *xdg)
		snprintf(path, sizeof(path), "%s/jrnl/journal.txt", xdg);
	else if (home)
		snprintf(path, sizeof(path), "%s/.local/share/jrnl/journal.txt", home);
	else
		strcpy(path, "journal.txt");
	return path;
}

typedef struct {
	char **v;
	size_t n, cap;
} Paths;

static void add_path(Paths *ps, const char *path)
{
	if (ps->n == ps->cap) {
		ps->cap = ps->cap ? ps->cap * 2 : 8;
		ps->v = xrealloc(ps->v, ps->cap * sizeof(*ps->v));
	}
	ps->v[ps->n] = xmalloc(strlen(path) + 1);
	strcpy(ps->v[ps->n++], path);
}

static int path_compare(const void *a, const void *b)
{
	return strcmp(*(const char *const *)a, *(const char *const *)b);
}

static void free_paths(Paths *ps)
{
	size_t i;
	for (i = 0; i < ps->n; i++) free(ps->v[i]);
	free(ps->v);
}

static int daily_name(const char *name)
{
	size_t i;
	if (strncmp(name, "journal-", 8) || strlen(name) != 18) return 0;
	for (i = 8; i < 18; i++)
		if (!isdigit((unsigned char)name[i]) && name[i] != '-') return 0;
	return name[12] == '-' && name[15] == '-';
}

static void daily_paths(const char *dir, Paths *ps)
{
#ifdef _WIN32
	char pattern[4096], path[4096];
	WIN32_FIND_DATAA data;
	HANDLE h;
	snprintf(pattern, sizeof(pattern), "%s\\journal-*", dir);
	h = FindFirstFileA(pattern, &data);
	if (h == INVALID_HANDLE_VALUE) return;
	do {
		if (!(data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)
		    && daily_name(data.cFileName)) {
			snprintf(path, sizeof(path), "%s\\%s", dir, data.cFileName);
			add_path(ps, path);
		}
	} while (FindNextFileA(h, &data));
	FindClose(h);
#else
	DIR *d = opendir(dir);
	struct dirent *entry;
	char path[4096];
	if (!d) {
		if (errno != ENOENT) {
			perror(dir);
			exit(1);
		}
		return;
	}
	while ((entry = readdir(d))) if (daily_name(entry->d_name)) {
			snprintf(path, sizeof(path), "%s/%s", dir, entry->d_name);
			add_path(ps, path);
		}
	closedir(d);
#endif
	qsort(ps->v, ps->n, sizeof(*ps->v), path_compare);
}

static void load_daily(const char *dir, Entries *es)
{
	Paths paths = {0};
	size_t i;
	daily_paths(dir, &paths);
	for (i = 0; i < paths.n; i++) load(paths.v[i], es);
	free_paths(&paths);
}

static int atomic_append(const char *path, const char *date, const char *text)
{
	char temp[4096];
	FILE *src = NULL, *out = NULL;
	unsigned char buffer[8192];
	size_t n;
	long long bytes = 0;
	int attempt;
	if (!ensure_parent(path)) return 0;
	src = fopen(path, "rb");
	if (!src && errno != ENOENT) return 0;
	for (attempt = 0; attempt < 100; attempt++) {
#ifdef _WIN32
		snprintf(temp, sizeof(temp), "%s.tmp.%lu.%d", path,
		         (unsigned long)GetCurrentProcessId(), attempt);
#else
		snprintf(temp, sizeof(temp), "%s.tmp.%lu.%d", path,
		         (unsigned long)getpid(), attempt);
#endif
		out = fopen(temp, "wb");
		if (out) break;
		if (errno != EEXIST) break;
	}
	if (!out) {
		if (src) fclose(src);
		return 0;
	}
	if (src) {
		while ((n = fread(buffer, 1, sizeof(buffer), src)) != 0) {
			if (fwrite(buffer, 1, n, out) != n) goto fail;
			bytes += (long long)n;
		}
		if (ferror(src)) goto fail;
		fclose(src);
		src = NULL;
	}
	if (bytes && fputc('\n', out) == EOF) goto fail;
	if (fprintf(out, "[%s] %s\n", date, text) < 0) goto fail;
	if (fflush(out) != 0) goto fail;
#ifdef _WIN32
	if (_commit(fileno(out)) != 0) goto fail;
	if (!FlushFileBuffers((HANDLE)_get_osfhandle(fileno(out)))) goto fail;
#else
	if (fsync(fileno(out)) != 0) goto fail;
#endif
	if (fclose(out) != 0) {
		out = NULL;
		goto fail;
	}
	out = NULL;
#ifdef _WIN32
	if (!MoveFileExA(temp, path,
	                 MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) goto fail;
#else
	if (rename(temp, path) != 0) goto fail;
#endif
	return 1;
fail:
	if (src) fclose(src);
	if (out) fclose(out);
#ifdef _WIN32
	DeleteFileA(temp);
#else
	unlink(temp);
#endif
	return 0;
}

static void json_string(const char *s)
{
	putchar('"');
	for (; *s; s++) {
		unsigned char c = (unsigned char)*s;
		if (c == '"' || c == '\\') printf("\\%c", c);
		else if (c == '\n') printf("\\n");
		else if (c == '\r') printf("\\r");
		else if (c == '\t') printf("\\t");
		else if (c < 32) printf("\\u%04x", c);
		else putchar(c);
	}
	putchar('"');
}

static void print_entry(const Entry *e, const char *format)
{
	if (!strcmp(format, "json")) {
		printf("{\"date\":");
		json_string(e->date);
		printf(",\"text\":");
		json_string(e->text);
		printf("}\n");
	} else if (!strcmp(format, "short")) printf("%s %s\n", e->date, e->text);
	else printf("[%s] %s\n\n", e->date, e->text);
}

static void print_json(const Entries *es, int n, const Selector *selectors,
                       size_t selector_count, int and_mode, const char *from,
                       const char *until)
{
	size_t i, shown = 0, skip = 0, matching = 0;
	Tags all = {0};
	for (i = 0; i < es->n;
	     i++) if (entry_matches(&es->v[i], selectors, selector_count, and_mode,
		                            from, until)) {
			matching++;
			collect_tags(es->v[i].text, &all);
		}
	if (n >= 0 && matching > (size_t)n) skip = matching - (size_t)n;
	printf("{\"entries\":[");
	for (i = 0; i < es->n;
	     i++) if (entry_matches(&es->v[i], selectors, selector_count, and_mode,
		                            from, until)) {
			if (skip) {
				skip--;
				continue;
			}
			if (shown++) putchar(',');
			printf("{\"date\":");
			json_string(es->v[i].date);
			printf(",\"text\":");
			json_string(es->v[i].text);
			printf(",\"tags\":[");
			{
				Tags tags = {0};
				size_t j;
				collect_tags(es->v[i].text, &tags);
				for (j = 0; j < tags.n; j++) {
					if (j) putchar(',');
					json_string(tags.v[j]);
				}
				free_tags(&tags);
			}
			printf("]}");
		}
	printf("],\"tags\":[");
	for (i = 0; i < all.n; i++) {
		if (i) putchar(',');
		json_string(all.v[i]);
	}
	puts("]}");
	free_tags(&all);
}

static int read_stdin(char *text, size_t size)
{
	char line[8192];
	int got = 0;
	if (isatty(fileno(stdin))) return 0;
	while (fgets(line, sizeof(line), stdin)) {
		size_t used = strlen(text), n;
		trim_newline(line);
		n = strlen(line);
		if (used && used + 1 < size) text[used++] = '\n';
		if (used + n >= size) n = size - used - 1;
		memcpy(text + used, line, n);
		text[used + n] = '\0';
		got = 1;
		if (used + n + 1 >= size) break;
	}
	return got;
}

static void help(void)
{
	puts("Usage: jrnl [TEXT...] | jrnl [search options]\n\n"
	     "  -n N                 show the last N entries\n"
	     "  --format short|pretty|json  format displayed entries\n"
	     "  --export json|text   export filtered entries\n"
	     "  -from DATE           show entries on or after DATE\n"
	     "  -to DATE             show entries on or before DATE\n"
	     "  -until DATE          show entries on or before DATE\n"
	     "  -on DATE             show entries on DATE\n"
	     "  -contains TERM      search entry text (repeatable)\n"
	     "  -and, -not           combine or negate the next selector\n"
	     "  @tag, -starred       filter by tag or starred entries\n"
	     "  --tags               list tags\n"
	     "  --date DATE          use DATE (YYYY-MM-DD HH:MM) for new entries\n"
	     "  --journal PATH       use PATH instead of the default journal\n"
	     "  --daily              use journal-YYYY-MM-DD files\n"
	     "  --journal-dir DIR    directory for daily journals\n"
	     "  --local, --global    select the local or global default journal\n"
	     "  --help, --version");
}

int main(int argc, char **argv)
{
	const char *path_arg = NULL, *format = "pretty", *export_kind = NULL;
	const char *journal_dir = NULL;
	int n = -1, tags = 0, i, output = 0, and_mode = 0, pending_not = 0;
	int daily = 0, local = 0, global = 0;
	const char *date_override = NULL;
	char from_date[11], until_date[11];
	const char *from = NULL, *until = NULL;
	Selector selectors[64];
	size_t selector_count = 0;
	Entries es = {0};
	char text[65536] = "";
	for (i = 1; i < argc; i++) {
		if (!strcmp(argv[i], "--help")) {
			help();
			return 0;
		}
		if (!strcmp(argv[i], "--version")) {
			puts("jrnlc-c " VERSION);
			return 0;
		}
		if (!strcmp(argv[i], "--journal") && i + 1 < argc) {
			path_arg = argv[++i];
			continue;
		}
		if (!strcmp(argv[i], "--daily")) {
			daily = 1;
			continue;
		}
		if (!strcmp(argv[i], "--journal-dir") && i + 1 < argc) {
			journal_dir = argv[++i];
			daily = 1;
			continue;
		}
		if (!strcmp(argv[i], "--local")) {
			local = 1;
			continue;
		}
		if (!strcmp(argv[i], "--global")) {
			global = 1;
			continue;
		}
		if (!strcmp(argv[i], "--date") && i + 1 < argc) {
			date_override = argv[++i];
			if (!valid_timestamp(date_override))
				die("--date requires YYYY-MM-DD HH:MM");
			continue;
		}
		if (!strcmp(argv[i], "-n") && i + 1 < argc) {
			n = atoi(argv[++i]);
			if (n < 0) die("-n requires a non-negative number");
			output = 1;
			continue;
		}
		if (!strcmp(argv[i], "--format") && i + 1 < argc) {
			format = argv[++i];
			output = 1;
			continue;
		}
		if (!strcmp(argv[i], "--export") && i + 1 < argc) {
			export_kind = argv[++i];
			output = 1;
			continue;
		}
		if ((!strcmp(argv[i], "-from") || !strcmp(argv[i], "-until")
		     || !strcmp(argv[i], "-to"))
		    && i + 1 < argc) {
			int upper = strcmp(argv[i], "-from") != 0;
			if (!date_filter(argv[++i], upper ? until_date : from_date, upper ? 1 : 0))
				die("invalid date: use 2025-01-15 or \"August 2020\"");
			if (upper) until = until_date;
			else from = from_date;
			output = 1;
			continue;
		}
		if (!strcmp(argv[i], "-on") && i + 1 < argc) {
			const char *date = argv[++i];
			if (!date_filter(date, from_date, 0) || !date_filter(date, until_date, 1))
				die("invalid -on date: use 2025-01-15 or \"August 2020\"");
			from = from_date;
			until = until_date;
			output = 1;
			continue;
		}
		if (!strcmp(argv[i], "-contains") && i + 1 < argc) {
			if (selector_count == 64) die("too many search selectors");
			selectors[selector_count++] = (Selector) {
				SELECT_CONTAINS, argv[++i], pending_not
			};
			pending_not = 0;
			output = 1;
			continue;
		}
		if (!strcmp(argv[i], "-starred")) {
			if (selector_count == 64) die("too many search selectors");
			selectors[selector_count++] = (Selector) {
				SELECT_STARRED, NULL, pending_not
			};
			pending_not = 0;
			output = 1;
			continue;
		}
		if (!strcmp(argv[i], "-and")) {
			and_mode = 1;
			output = 1;
			continue;
		}
		if (!strcmp(argv[i], "-not")) {
			if (pending_not) die("-not requires a selector");
			pending_not = 1;
			output = 1;
			continue;
		}
		if (!strcmp(argv[i], "--tags")) {
			tags = 1;
			output = 1;
			continue;
		}
		if (argv[i][0] == '-') die("unknown option");
		if (argv[i][0] == '@') {
			if (selector_count == 64) die("too many search selectors");
			selectors[selector_count++] = (Selector) {
				SELECT_TAG, argv[i] + 1, pending_not
			};
			pending_not = 0;
			output = 1;
			continue;
		}
		if (text[0]) strncat(text, " ", sizeof(text) - strlen(text) - 1);
		strncat(text, argv[i], sizeof(text) - strlen(text) - 1);
	}
	if (pending_not) die("-not requires a selector");
	if (local && global) die("--local and --global are mutually exclusive");
	if (path_arg && daily) die("--journal and --daily are mutually exclusive");
	if (strcmp(format, "short") && strcmp(format, "pretty")
	    && strcmp(format, "json"))
		die("--format must be short, pretty, or json");
	if (!output && !text[0]) read_stdin(text, sizeof(text));
	char daily_path[4096], daily_dir[4096];
	const char *path = journal_path(path_arg, local, global);
	if (daily) {
		if (!journal_dir) {
			if (local || (!global && file_exists(".jrnlc/journal.txt"))) {
				journal_dir = ".jrnlc";
			} else {
				char *slash;
				strcpy(daily_dir, path);
				slash = strrchr(daily_dir, '/');
#ifdef _WIN32
				{
					char *back = strrchr(daily_dir, '\\');
					if (!slash || (back && back > slash)) slash = back;
				}
#endif
				if (slash) *slash = '\0';
				journal_dir = slash ? daily_dir : ".";
			}
		}
		load_daily(journal_dir, &es);
	} else load(path, &es);
	if (!output && text[0]) {
		time_t now = time(NULL);
		struct tm *tm = localtime(&now);
		char date[32], day[11];
		strftime(date, sizeof(date), "%Y-%m-%d %H:%M", tm);
		if (date_override) strcpy(date, date_override);
		if (daily) {
			strftime(day, sizeof(day), "%Y-%m-%d", tm);
#ifdef _WIN32
			snprintf(daily_path, sizeof(daily_path), "%s\\journal-%s", journal_dir, day);
#else
			snprintf(daily_path, sizeof(daily_path), "%s/journal-%s", journal_dir, day);
#endif
			path = daily_path;
		}
		if (!atomic_append(path, date, text)) {
			perror(path);
			free_entries(&es);
			return 1;
		}
		free_entries(&es);
		return 0;
	}
	if (tags) {
		Tags unique = {0};
		size_t j;
		for (i = 0; (size_t)i < es.n; i++) collect_tags(es.v[i].text, &unique);
		for (j = 0; j < unique.n; j++) printf("@%s\n", unique.v[j]);
		free_tags(&unique);
	} else if (export_kind) {
		if (!strcmp(export_kind, "json")) print_json(&es, -1, selectors,
			        selector_count, and_mode, from, until);
		else if (!strcmp(export_kind, "text")) {
			for (i = 0; (size_t)i < es.n; i++)
				if (entry_matches(&es.v[i], selectors, selector_count, and_mode, from, until))
					print_entry(&es.v[i], "pretty");
		} else die("--export must be json or text");
	} else {
		size_t matching = 0, skip = 0;
		for (i = 0; (size_t)i < es.n;
		     i++) if (entry_matches(&es.v[i], selectors, selector_count, and_mode, from,
			                            until)) matching++;
		if (n >= 0 && matching > (size_t)n) skip = matching - (size_t)n;
		if (!strcmp(format, "json")) print_json(&es, n, selectors, selector_count,
			                                        and_mode, from, until);
		else for (i = 0; (size_t)i < es.n;
			          i++) if (entry_matches(&es.v[i], selectors, selector_count, and_mode, from,
				                                 until)) {
					if (skip) {
						skip--;
						continue;
					}
					print_entry(&es.v[i], format);
				}
	}
	free_entries(&es);
	return 0;
}

