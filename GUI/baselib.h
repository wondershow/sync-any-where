#include <string.h>
#include <stddef.h>

typedef struct _UnsyncFileList
{
	gchar *file_path;
	gchar *file_name;
	struct UnsyncFileList *next;
} UnsyncFileList;
