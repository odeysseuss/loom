cc := clang
cflags := -std=c11 -Wall -Wextra -pedantic -MMD -MP -D_GNU_SOURCE
incdir := -Istructs -Imem -Inet
objdir := build/obj
bindir := build/target

MODE ?= debug
ifeq ($(MODE), release)
	cflags += -O2 -ffast-math -march=native
else
	cflags += -g3 -fsanitize=address,undefined,leak
endif

srcs := $(wildcard howto/*.c)
objs := $(srcs:howto/%.c=$(objdir)/%.o)
deps := $(srcs:howto/%.c=$(objdir)/%.d)
exec := $(srcs:howto/%.c=$(bindir)/%)
-include $(deps)

.PHONY: all test run clean

all: $(exec)

$(bindir)/%: $(objdir)/%.o | $(bindir)
	$(cc) $(cflags) $< -o $@

$(objdir)/%.o: howto/%.c | $(objdir)
	$(cc) $(cflags) $(incdir) -c $< -o $@

$(objdir) $(bindir):
	mkdir -p $@

clean:
	rm -rf $(bindir) $(objdir)
