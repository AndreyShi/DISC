//Р·Р°РіР»СѓС€РєРё
#define PROGMEM
#define PSTR(x) x
#define PGM_P const char *

#define strncmp_P(x,y,z) z
#define strncpy_P(x,y,z);
#define memcpy_P(x,y,z)
#define pgm_read_byte_near(x) *x
#define pgm_read_byte(x) *x
#define strlen_P(x) 1
#define strcpy_P(x,y);
#define strcmp_P(x,y) 1

#define __malloc_heap_start (char*)1
#define __malloc_heap_end (char*)0x5fff
