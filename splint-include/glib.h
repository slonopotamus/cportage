
#include <assert.h>
#include <stddef.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>

#ifndef GLIB_H
#define GLIB_H

/*@-exportany@*/

/* gtypes.h */

typedef char   gchar;
typedef short  gshort;
typedef long   glong;
typedef int    gint;

typedef unsigned char   guchar;
typedef unsigned short  gushort;
typedef unsigned long   gulong;
typedef unsigned int    guint;

typedef float   gfloat;
typedef double  gdouble;

typedef void* gpointer;
typedef const void *gconstpointer;

typedef size_t gsize;
typedef ssize_t gssize;
typedef int32_t gint32;
typedef uint32_t guint32;
typedef bool gboolean;
typedef int GQuark;
typedef guint32 gunichar;

typedef gint            (*GCompareFunc)         (gconstpointer  a,
                                                 gconstpointer  b);
typedef gint            (*GCompareDataFunc)     (gconstpointer  a,
                                                 gconstpointer  b,
						 gpointer       user_data);
typedef gboolean        (*GEqualFunc)           (gconstpointer  a,
                                                 gconstpointer  b);
typedef void            (*GDestroyNotify)       (gpointer       data);
typedef void            (*GFunc)                (gpointer       data,
                                                 gpointer       user_data);
typedef guint           (*GHashFunc)            (gconstpointer  key);
typedef void            (*GHFunc)               (gpointer       key,
                                                 gpointer       value,
                                                 gpointer       user_data);
typedef void            (*GFreeFunc)            (gpointer       data);

#define FALSE false
#define TRUE true

/* gmacros.h */

#define G_BEGIN_DECLS
#define G_END_DECLS

#define G_STATIC_ASSERT(expr)

#define G_GNUC_EXTENSION
#define G_GNUC_MALLOC
#define G_GNUC_UNUSED
#define G_GNUC_WARN_UNUSED_RESULT

#define G_N_ELEMENTS(arr) (sizeof (arr) / sizeof (arr)[0])

#define G_CONST_RETURN const

#define GLIB_CHECK_VERSION(major,minor,micro) 0

/* gquark.h */

GQuark
g_quark_from_static_string(const gchar *string) /*@*/;

/* gmem.h */

/*@only@*/ gpointer g_malloc(gsize n_bytes) /*@*/;
/*@only@*/ gpointer g_malloc0(gsize n_bytes) /*@*/;
#define g_new(nobj, size) (nobj *)g_malloc(sizeof(nobj) * size)
#define g_new0(nobj, size) (nobj *)g_malloc0(sizeof(nobj) * size)
#define g_free free

/* gerror.h */

typedef struct {
    GQuark domain;
    gint code;
    /*@only@*/ gchar *message;
} GError;

void
g_error_free(/*@only@*/ GError *error) /*@modifies error@*/;

/*@printflike@*/ void
g_set_error(
    /*@null@*/ GError **err,
    GQuark domain,
    gint code,
    const gchar *format,
    ...
) /*@modifies *err@*/;

/* gmessages.h */

/*@mayexit@*/ /*@printflike@*/ void
g_critical(const gchar *format, ...) /*@modifies *stderr,errno@*/;

/*@exits@*/ /*@printflike@*/ void
g_error(const gchar *format, ...) /*@modifies *stderr,errno@*/;

/*@printflike@*/ void
g_warning(const gchar *format, ...) /*@modifies *stderr,errno@*/;

/*@printflike@*/ void
g_message(const gchar *format, ...) /*@modifies *stdout,errno@*/;

/*@printflike@*/ void
g_debug(const gchar *format, ...) /*@modifies *stdout,errno@*/;

/*@printflike@*/ void
g_print(const gchar *format, ...) /*@modifies *stdout,errno@*/;

/* gtestutils.h */

#define g_assert assert
/*@exits@*/ void
g_assert_not_reached(void) /*@*/;

/*@mayexit@*/ void
g_assert_no_error(/*@null@*/ GError *err) /*@*/;

int
g_strcmp0(/*@null@*/ const char *str1, /*@null@*/ const char *str2) /*@*/;

/* gconvert.h */

/*@constant GQuark G_CONVERT_ERROR; @*/

typedef enum {
  G_CONVERT_ERROR_NO_CONVERSION,
  G_CONVERT_ERROR_ILLEGAL_SEQUENCE,
  G_CONVERT_ERROR_FAILED,
  G_CONVERT_ERROR_PARTIAL_INPUT,
  G_CONVERT_ERROR_BAD_URI,
  G_CONVERT_ERROR_NOT_ABSOLUTE_PATH
} GConvertError;

/*@null@*/ /*@only@*/ gchar *
g_filename_from_utf8(
    const gchar *utf8string,
    gssize len,
    /*@null@*/ gsize *bytes_read,
    /*@null@*/ gsize *bytes_written,
    /*@null@*/ GError **error
) /*@modifies *bytes_read,*bytes_written,*error@*/;

/*@null@*/ /*@only@*/ gchar *
g_filename_to_utf8(
    const gchar *opsysstring,
    gssize len,
    /*@null@*/ gsize *bytes_read,
    /*@null@*/ gsize *bytes_written,
    /*@null@*/ GError **error
) /*@modifies *bytes_read,*bytes_written,*error@*/;

/* gutils.h */

/*@only@*/ gchar *
g_path_get_dirname(const gchar *file_name) /*@*/;

/*@only@*/ gchar *
g_path_get_basename(const gchar *file_name) /*@*/;

gboolean
g_path_is_absolute(const gchar *file_name) /*@*/;

/* gfileutils.h */

/*@constant GQuark G_FILE_ERROR @*/

typedef enum {
    G_FILE_ERROR_EXIST,
    G_FILE_ERROR_ISDIR,
    G_FILE_ERROR_ACCES,
    G_FILE_ERROR_NAMETOOLONG,
    G_FILE_ERROR_NOENT,
    G_FILE_ERROR_NOTDIR,
    G_FILE_ERROR_NXIO,
    G_FILE_ERROR_NODEV,
    G_FILE_ERROR_ROFS,
    G_FILE_ERROR_TXTBSY,
    G_FILE_ERROR_FAULT,
    G_FILE_ERROR_LOOP,
    G_FILE_ERROR_NOSPC,
    G_FILE_ERROR_NOMEM,
    G_FILE_ERROR_MFILE,
    G_FILE_ERROR_NFILE,
    G_FILE_ERROR_BADF,
    G_FILE_ERROR_INVAL,
    G_FILE_ERROR_PIPE,
    G_FILE_ERROR_AGAIN,
    G_FILE_ERROR_INTR,
    G_FILE_ERROR_IO,
    G_FILE_ERROR_PERM,
    G_FILE_ERROR_NOSYS,
    G_FILE_ERROR_FAILED
} GFileError;

typedef enum {
    G_FILE_TEST_IS_REGULAR    = 1 << 0,
    G_FILE_TEST_IS_SYMLINK    = 1 << 1,
    G_FILE_TEST_IS_DIR        = 1 << 2,
    G_FILE_TEST_IS_EXECUTABLE = 1 << 3,
    G_FILE_TEST_EXISTS        = 1 << 4
} GFileTest;

gboolean
g_file_test(const gchar *filename, GFileTest test) /*@*/;

GFileError /*@alt gint@*/
g_file_error_from_errno(gint err_no) /*@*/;

gboolean
g_file_get_contents(
    const gchar *filename,
    /*@out@*/ gchar **contents,
    /*@null@*/ gsize *length,
    /*@null@*/ GError **error
) /*@modifies *contents,*length,*error,errno@*/ /*@globals fileSystem@*/;

/*@only@*/ gchar *
g_build_filename(const gchar *first_element, ...) /*@*/;

/* gdir.h */

typedef struct _GDir GDir;

/*@null@*/ /*@only@*/ GDir *
g_dir_open(
    const gchar *path,
    guint flags,
    /*@null@*/ GError **error
) /*@modifies *error,errno@*/ /*@globals fileSystem@*/;

/*@observer@*/ G_CONST_RETURN gchar *
g_dir_read_name(GDir *dir) /*@modifies *dir@*/;

void
g_dir_close(/*@only@*/ GDir *dir) /*@modifies dir@*/;

/* gstrfuncs.h */

/*@only@*/ gchar *
g_strdup(const gchar *str) /*@*/;

/*@printflike@*/ /*@only@*/ gchar *
g_strdup_printf(const gchar *format, ...) /*@*/;

/*@dependent@*/ gchar *
g_strchug(/*@returned@*/ gchar *string) /*@modifies *string@*/;

/*@dependent@*/ gchar *
g_strchomp(/*@returned@*/ gchar *string) /*@modifies *string@*/;

extern /*@dependent@*/ char *
g_strstrip(/*@returned@*/ char *string) /*@modifies *string@*/;
#define g_strstrip(string) g_strchomp(g_strchug(string))

/*@observer@*/ gchar *
g_strerror(gint errnum) /*@*/;

/*@only@*/ gchar **
g_strsplit(
    const gchar *string,
    const gchar *delimiter,
    gint max_tokens
) /*@*/;

/*@only@*/ gchar *
g_strjoinv(const gchar *separator, gchar **str_array) /*@*/;

gint
g_ascii_strcasecmp(const gchar *s1,  const gchar *s2) /*@*/;

/*@only@*/ gchar *
g_strconcat(const gchar *string1, ...) /*@*/;

guint
g_strv_length(gchar **str_array) /*@*/;

void
g_strfreev(/*@null@*/ /*@only@*/ gchar **str_array) /*@modifies str_array@*/;

/*@observer@*/ gchar *
g_dgettext(const gchar *domain, const gchar *msgid) /*@*/;

/* gstring.h */

typedef struct {
    /*@only@*/ gchar *str;
    gsize len;    
    gsize allocated_len;
} GString;

/*@only@*/ GString *
g_string_new(const gchar *init) /*@*/;

/*@null@*/ /*@only@*/ gchar *
g_string_free(
    /*@only@*/ GString *string,
    gboolean free_segment
) /*@modifies string@*/;

/*@exposed@*/ GString *
g_string_append(
    /*@returned@*/ GString *string,
    const gchar	*val
) /*@modifies *string@*/;

/*@exposed@*/ GString *
g_string_append_c(
    /*@returned@*/ GString *string,
    gchar c
) /*@modifies *string@*/;

/*@printflike@*/ void
g_string_append_printf(
    GString *string, const gchar *format, ...
) /*@modifies *string@*/;

/* gunicode.h */

gboolean
g_utf8_validate(
    const gchar *str,
    gssize max_len,
    /*@null@*/ const gchar **end
) /*@modifies *end@*/;

gint
g_utf8_collate(const gchar *str1, const gchar *str2) /*@*/;

/*@null@*/ gchar *
g_utf8_strchr(
    /*@returned@*/ const gchar *p,
    gssize len,
    gunichar c
) /*@*/;

/* ghash.h */
typedef struct _GHashTable GHashTable;

/*@only@*/ GHashTable *
g_hash_table_new_full(
    GHashFunc hash_func,
    GEqualFunc key_equal_func,
    /*@null@*/ GDestroyNotify key_destroy_func,
    /*@null@*/ GDestroyNotify value_destroy_func
) /*@*/;

void
g_hash_table_destroy(
    /*@only@*/ GHashTable *hash_table
) /*@modifies hash_table@*/;

void
g_hash_table_insert(
    GHashTable *hash_table,
    /*@keep@*/ gpointer key,
    /*@keep@*/ /*@null@*/ gpointer value
) /*@modifies *hash_table@*/;

/*@null@*/ /*@dependent@*/ gpointer
g_hash_table_lookup(GHashTable *hash_table, gconstpointer key) /*@*/;

gboolean
g_hash_table_lookup_extended(
    GHashTable *hash_table,
    gconstpointer lookup_key,
    /*@null@*/ /*@out@*/ gpointer *orig_key,
    /*@null@*/ /*@out@*/ gpointer *value
) /*@modifies *orig_key,*value@*/;

gboolean
g_hash_table_steal(
    GHashTable *hash_table,
    gconstpointer key
) /*@modifies *hash_table@*/;

gboolean
g_str_equal(gconstpointer v1, gconstpointer v2) /*@*/;

guint
g_str_hash(gconstpointer v) /*@*/;

/* gspawn.h */

/*@constant GQuark G_SPAWN_ERROR @*/

typedef void (* GSpawnChildSetupFunc) (gpointer user_data);

typedef enum {
  G_SPAWN_ERROR_FORK,   /* fork failed due to lack of memory */
  G_SPAWN_ERROR_READ,   /* read or select on pipes failed */
  G_SPAWN_ERROR_CHDIR,  /* changing to working dir failed */
  G_SPAWN_ERROR_ACCES,  /* execv() returned EACCES */
  G_SPAWN_ERROR_PERM,   /* execv() returned EPERM */
  G_SPAWN_ERROR_2BIG,   /* execv() returned E2BIG */
  G_SPAWN_ERROR_NOEXEC, /* execv() returned ENOEXEC */
  G_SPAWN_ERROR_NAMETOOLONG, /* ""  "" ENAMETOOLONG */
  G_SPAWN_ERROR_NOENT,       /* ""  "" ENOENT */
  G_SPAWN_ERROR_NOMEM,       /* ""  "" ENOMEM */
  G_SPAWN_ERROR_NOTDIR,      /* ""  "" ENOTDIR */
  G_SPAWN_ERROR_LOOP,        /* ""  "" ELOOP   */
  G_SPAWN_ERROR_TXTBUSY,     /* ""  "" ETXTBUSY */
  G_SPAWN_ERROR_IO,          /* ""  "" EIO */
  G_SPAWN_ERROR_NFILE,       /* ""  "" ENFILE */
  G_SPAWN_ERROR_MFILE,       /* ""  "" EMFLE */
  G_SPAWN_ERROR_INVAL,       /* ""  "" EINVAL */
  G_SPAWN_ERROR_ISDIR,       /* ""  "" EISDIR */
  G_SPAWN_ERROR_LIBBAD,      /* ""  "" ELIBBAD */
  G_SPAWN_ERROR_FAILED       /* other fatal failure, error->message
                              * should explain
                              */
} GSpawnError;

typedef enum {
    G_SPAWN_LEAVE_DESCRIPTORS_OPEN = 1 << 0,
    G_SPAWN_DO_NOT_REAP_CHILD      = 1 << 1,
    /* look for argv[0] in the path i.e. use execvp() */
    G_SPAWN_SEARCH_PATH            = 1 << 2,
    /* Dump output to /dev/null */
    G_SPAWN_STDOUT_TO_DEV_NULL     = 1 << 3,
    G_SPAWN_STDERR_TO_DEV_NULL     = 1 << 4,
    G_SPAWN_CHILD_INHERITS_STDIN   = 1 << 5,
    G_SPAWN_FILE_AND_ARGV_ZERO     = 1 << 6
} GSpawnFlags;

gboolean
g_spawn_sync(
    /*@null@*/ const gchar *working_directory,
    gchar **argv,
    /*@null@*/ gchar **envp,
    int flags,
    /*@null@*/ GSpawnChildSetupFunc child_setup,
    /*@null@*/ gpointer user_data,
    /*@null@*/ /*@out@*/ gchar **standard_output,
    /*@null@*/ /*@out@*/ gchar **standard_error,
    /*@null@*/ /*@out@*/ gint *exit_status,
    /*@null@*/ GError **error
) /*@modifies *standard_output,*standard_error,*error,*stdout,*stderr,errno,fileSystem@*/;

gboolean
g_spawn_command_line_sync(
    const gchar *command_line,
    /*@null@*/ /*@out@*/ gchar **standard_output,
    /*@null@*/ /*@out@*/ gchar **standard_error,
    /*@null@*/ /*@out@*/ gint *exit_status,
    /*@null@*/ GError **error
) /*@modifies *standard_output,*standard_error,*error,*stdout,*stderr,errno,fileSystem@*/;

/* glist.h */

typedef struct _GList GList;

struct _GList {
  /*@dependent@*/ /*@null@*/ gpointer data;
  /*@dependent@*/ /*@null@*/ GList *next;
  /*@dependent@*/ /*@null@*/ GList *prev;
};

/*@only@*/ GList *
g_list_append(
    /*@keep@*/ /*@null@*/ GList *list,
    /*@keep@*/ /*@null@*/ gpointer data
) /*@modifies *list@*/;

/*@only@*/ GList *
g_list_prepend(
    /*@keep@*/ /*@null@*/ GList *list,
    /*@keep@*/ /*@null@*/ gpointer data
) /*@modifies *list@*/;

void
g_list_free(
    /*@null@*/ /*@only@*/ GList *list
) /*@modifies list@*/;

void
g_list_foreach(
    /*@null@*/ GList *list,
    GFunc func,
    /*@null@*/ gpointer user_data
) /*@modifies *list,user_data@*/;

/*@only@*/ GList *
g_list_insert_sorted(
    /*@null@*/ GList *list,
    /*@null@*/ gpointer data,
    GCompareFunc func
) /*@modifies *list@*/;

/* gregex.h */

typedef enum {
    G_REGEX_CASELESS          = 1 << 0,
    G_REGEX_MULTILINE         = 1 << 1,
    G_REGEX_DOTALL            = 1 << 2,
    G_REGEX_EXTENDED          = 1 << 3,
    G_REGEX_ANCHORED          = 1 << 4,
    G_REGEX_DOLLAR_ENDONLY    = 1 << 5,
    G_REGEX_UNGREEDY          = 1 << 9,
    G_REGEX_RAW               = 1 << 11,
    G_REGEX_NO_AUTO_CAPTURE   = 1 << 12,
    G_REGEX_OPTIMIZE          = 1 << 13,
    G_REGEX_DUPNAMES          = 1 << 19,
    G_REGEX_NEWLINE_CR        = 1 << 20,
    G_REGEX_NEWLINE_LF        = 1 << 21,
    G_REGEX_NEWLINE_CRLF      = G_REGEX_NEWLINE_CR | G_REGEX_NEWLINE_LF
} GRegexCompileFlags;

typedef struct {
} GRegex;

typedef struct {
} GMatchInfo;

/*@null@*/ /*@only@*/
GRegex *
g_regex_new(
    const gchar *pattern,
    int compile_options,
    int match_options,
    /*@null@*/ GError **error
) /*@modifies *error@*/;

gint
g_regex_get_string_number(const GRegex *regex, const gchar *name) /*@*/;

gint
g_regex_get_capture_count(const GRegex *regex) /*@*/;

gboolean
g_regex_match_full(
    const GRegex *regex,
    const gchar *string,
    gssize string_len,
    gint start_position,
    gint match_options,
    /*@null@*/ /*@out@*/ GMatchInfo **match_info,
    /*@null@*/ GError **error
) /*@modifies *match_info,*error@*/;

gchar **
g_regex_split(
    const GRegex *regex,
    const gchar *string,
    int match_options
) /*@*/;

/*@null@*/ gchar *
g_match_info_fetch(const GMatchInfo *match_info, gint match_num) /*@*/;

/*@observer@*/ GRegex *
g_match_info_get_regex(const GMatchInfo *match_info) /*@*/;

/*@observer@*/ const gchar *
g_match_info_get_string(const GMatchInfo *match_info) /*@*/;

void
g_match_info_free(/*@only@*/ GMatchInfo *match_info) /*@modifies match_info@*/;

/* goption.h */

typedef struct _GOptionContext GOptionContext;
typedef struct _GOptionGroup   GOptionGroup;
typedef struct _GOptionEntry   GOptionEntry;

typedef enum {
    G_OPTION_FLAG_HIDDEN		= 1 << 0,
    G_OPTION_FLAG_IN_MAIN		= 1 << 1,
    G_OPTION_FLAG_REVERSE		= 1 << 2,
    G_OPTION_FLAG_NO_ARG		= 1 << 3,
    G_OPTION_FLAG_FILENAME	= 1 << 4,
    G_OPTION_FLAG_OPTIONAL_ARG    = 1 << 5,
    G_OPTION_FLAG_NOALIAS	        = 1 << 6
} GOptionFlags;

typedef enum {
    G_OPTION_ARG_NONE,
    G_OPTION_ARG_STRING,
    G_OPTION_ARG_INT,
    G_OPTION_ARG_CALLBACK,
    G_OPTION_ARG_FILENAME,
    G_OPTION_ARG_STRING_ARRAY,
    G_OPTION_ARG_FILENAME_ARRAY,
    G_OPTION_ARG_DOUBLE,
    G_OPTION_ARG_INT64
} GOptionArg;

/*@constant const char *G_OPTION_REMAINING @*/
#define G_OPTION_REMAINING ""

struct _GOptionEntry {
    /*@observer@*/ const gchar *long_name;
    gchar        short_name;
    gint         flags;

    GOptionArg   arg;
    /*@null@*/ /*@observer@*/ gpointer     arg_data;

    /*@null@*/ /*@observer@*/ const gchar *description;
    /*@null@*/ /*@observer@*/ const gchar *arg_description;
};

/*@only@*/ GOptionContext *
g_option_context_new(/*@null@*/ const gchar *parameter_string) /*@*/;

void
g_option_context_free(/*@only@*/ GOptionContext *context) /*@modifies context@*/;

void
g_option_context_set_help_enabled(
    GOptionContext *context,
    gboolean help_enabled
) /*@modifies *context@*/;

void
g_option_context_add_main_entries(
    GOptionContext *context,
    const GOptionEntry *entries,
    /*@null@*/ const gchar *translation_domain
) /*@modifies *context@*/;

/*@mayexit@*/ gboolean
g_option_context_parse(
    GOptionContext *context,
    gint *argc,
    gchar ***argv,
    /*@null@*/ GError **error
) /*@modifies *argc,*argv,*error,*stderr,*stdout,errno@*/;

#endif
