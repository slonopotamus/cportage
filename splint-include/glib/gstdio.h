#include <stdio.h>
#include <glib.h>

/*@dependent@*/ /*@null@*/ FILE *
g_fopen(const gchar *filename, const gchar *mode) /*@modifies errno,fileSystem@*/;
