#include <glib-object.h>

/* giotypes.h */

typedef struct _GFile GFile;

/* gfile.h */

/*@only@*/ GFile *
g_file_new_for_path(const char *path) /*@*/;

/*@null@*/ /*@only@*/ char *
g_file_get_relative_path(
    GFile *parent,
    GFile *descendant
) /*@modifies errno@*/;
