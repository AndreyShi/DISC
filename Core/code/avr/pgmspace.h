//заглушки
#include <memory.h>
#define PROGMEM
#define PSTR(x) x
#define PGM_P const char *

#define strncmp_P(x,y,z) strncmp(x,y,z)
#define strncpy_P(x,y,z) strncpy(x,y,z)
#define memcpy_P(x,y,z) memcpy(x,y,z)
#define pgm_read_byte_near(x) *x
#define pgm_read_byte(x) *x
#define strlen_P(x) strlen(x)
#define strcpy_P(x,y) strcpy(x,y)
#define strcmp_P(x,y) strcmp(x,y)

#define __malloc_heap_start (char*)1
#define __malloc_heap_end (char*)0x5fff
