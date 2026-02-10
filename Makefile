cc := clang
cflags := -std=c11 -Wall -Wextra -pedantic -MMD -MP
debflags := -g3 -fsanitize=address,undefined,leak
relflags := -O2 -ffast-math -march=native
testflags := -lcriterion
incdir := -Istructs
exdir := build/examples
testdir := build/tests

MODE ?= debug
ifeq ($(MODE), release)
	cflags += $(relflags)
else
	cflags += $(debflags)
endif

examplefiles := $(wildcard howto/*.c)
examples := $(examplefiles:howto/%.c=$(exdir)/%)
testfiles := $(wildcard tests/*.c)
tests := $(testfiles:tests/%.c=$(testdir)/%)

.PHONY: all examples tests run clean

all: examples tests
examples: $(examples)
tests: $(tests)

$(exdir)/%: howto/%.c | $(exdir)
	$(cc) $(cflags) $(incdir) $^ -o $@

$(testdir)/%: tests/%.c | $(testdir)
	$(cc) $(cflags) $(incdir) $^ -o $@ $(testflags)

$(exdir) $(testdir):
	mkdir -p $@

run: $(tests)
	@for t in $(tests); do \
		echo "[TEST]: $$t"; \
		./$$t; \
		echo ""; \
	done

clean:
	rm -rf build
