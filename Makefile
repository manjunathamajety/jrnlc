.POSIX:

SHELL = sh
CC = cc
CFLAGS = -std=c11 -O2 -Wall -Wextra
SRC = jrnlc.c

TESTTARGET = jrnl
TESTBIN = ./jrnl
UPSTREAM_URL = https://github.com/jrnl-org/jrnl.git
UPSTREAM = .jrnl-upstream
FIXTURE = ${UPSTREAM}/tests/data/journals/basic_onefile.journal
NULL = /dev/null

.PHONY: all format test clean

all: jrnl

format:
	astyle --options=.astylerc ${SRC}

jrnl.exe: ${SRC}
	cl.exe /nologo /TC /W3 /O2 /Fe:jrnl.exe ${SRC}

jrnl: ${SRC}
	${CC} ${CFLAGS} -o $@ ${SRC}

${FIXTURE}:
	rm -rf ${UPSTREAM}
	git clone --depth 1 --no-checkout ${UPSTREAM_URL} ${UPSTREAM}
	cd ${UPSTREAM} && git sparse-checkout set --no-cone /tests/data/journals/basic_onefile.journal && git checkout

test: ${TESTTARGET} ${FIXTURE}
	${TESTBIN} --version >${NULL}
	${TESTBIN} --help >${NULL}
	${TESTBIN} --journal ${FIXTURE} -until "August 2020" --format json >${NULL}
	${TESTBIN} --journal ${FIXTURE} -contains second -on 2020-08-31 >${NULL}
	${TESTBIN} --journal ${FIXTURE} @tagtwo >${NULL}
	@echo tests passed

clean:
	rm -rf jrnl jrnl.exe *.obj *.pdb NUL ${UPSTREAM}
