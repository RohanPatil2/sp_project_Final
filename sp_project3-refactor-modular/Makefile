# -------- directories -------------------------------------------------
SRCDIR   := src
INCDIR   := include
BINDIR   := bin
CGIDIR   := cgi-bin
CONFDIR  := conf
LOGDIR   := logs

# -------- source file groups -----------------------------------------
CGISRC   := $(SRCDIR)/GET.c $(SRCDIR)/POST.c
SRCS     := $(filter-out $(CGISRC),$(wildcard $(SRCDIR)/*.c))
OBJS     := $(SRCS:.c=.o)

CC       := gcc
CFLAGS   := -Wall -Wextra -g -I$(INCDIR)

ifeq ($(shell uname),Darwin)
  CFLAGS += -D_DARWIN_C_SOURCE
endif

# -------- default target ---------------------------------------------
all: prep_dirs $(BINDIR)/server cgi

# -------- server build -----------------------------------------------
$(BINDIR)/server: $(OBJS)
	$(CC) $(CFLAGS) $^ -o $@

# -------- CGI helpers -------------------------------------------------
cgi: $(CGIDIR)/GET.cgi $(CGIDIR)/POST.cgi

$(CGIDIR)/GET.cgi: $(SRCDIR)/GET.c
	$(CC) $(CFLAGS) $< -o $@

$(CGIDIR)/POST.cgi: $(SRCDIR)/POST.c
	$(CC) $(CFLAGS) $< -o $@

# -------- housekeeping ------------------------------------------------
clean:
	rm -rf $(BINDIR) $(CGIDIR)/*.cgi $(SRCDIR)/*.o

prep_dirs:
	@mkdir -p $(BINDIR) $(CGIDIR) $(CONFDIR) $(LOGDIR)

.PHONY: all cgi clean prep_dirs
