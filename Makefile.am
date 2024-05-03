AM_CPPFLAGS = -I$(top_srcdir)/include
AM_CFLAGS = -Wall -Wextra -pedantic -g
AM_LDFLAGS = -fsanitize=address,leak,undefined

bin_PROGRAMS = clox

include_HEADERS = $(top_srcdir)/include/*.h

clox_SOURCES = \
  src/lox.c \
  src/token.c \
  src/scanner.c \
  src/parser.c \
  src/ast_print.c

