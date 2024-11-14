#
# Compiler flags
#
CC     = gcc
CFLAGS = -Wall
#-Werror -Wextra

#
# Project files
#
SRCS = libkmevents.c libkmevents_test.c
OBJS = $(SRCS:.c=.o)
LIBS = 
EXE  = libkmevents_test

#
# Debug build settings
#
DBGDIR = debug
DBGEXE = $(DBGDIR)/$(EXE)
DBGOBJS = $(addprefix $(DBGDIR)/, $(OBJS))
DBGCFLAGS = -ggdb3 -O0 -DDEBUG

#
# Release build settings
#
RELDIR = release
RELEXE = $(RELDIR)/$(EXE)
RELOBJS = $(addprefix $(RELDIR)/, $(OBJS))
RELCFLAGS = -O3 -DNDEBUG

.PHONY: all clean debug prep release remake

# Default build
#all: prep release debug
all: prep clean debug release

#
# Debug rules
#
debug: $(DBGEXE)
$(DBGEXE): $(DBGOBJS)
	$(CC) $(LIBS) -o $(DBGEXE) $^
$(DBGDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $(DBGCFLAGS) -o $@ $<

#
# Release rules
#
release: $(RELEXE)
	strip $(RELEXE)
$(RELEXE): $(RELOBJS)
	$(CC) $(LIBS) -o $(RELEXE) $^
$(RELDIR)/%.o: %.c
	$(CC) -c $(CFLAGS) $(RELCFLAGS) -o $@ $<

#
# Other rules
#
prep:
	@mkdir -p $(DBGDIR) $(RELDIR)

remake: clean all

clean:
	rm -f $(RELEXE) $(RELOBJS) $(DBGEXE) $(DBGOBJS)
