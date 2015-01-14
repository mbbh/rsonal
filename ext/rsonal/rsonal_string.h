#include <ruby.h>

typedef struct _struct_rst {
  char* ptr;
  long free_space;
  long last_alloc;
  long max_size;
} Rst;

Rst* rst_new();
void rst_free(Rst* ptr);
Rst* rst_unwrap(VALUE rst);
VALUE rst_wrap(Rst* rst);
void rst_init();
void rst_cat_cstr(Rst* dst, const char* input, long len);
void rst_cat_clen(Rst* dst, const char* input);