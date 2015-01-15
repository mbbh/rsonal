#include <ruby.h>

typedef struct _struct_rst {
  char* ptr;
  long len;
  long max_size;
  long free_space;
} Rst;

typedef struct _struct_rst_rb_ptr {
  Rst* ptr;
} RstPtr;

Rst* rst_new();
void rst_free(Rst* ptr);
Rst* rst_unwrap(VALUE rst);
VALUE rst_wrap(Rst* rst);
void rst_init();
void rst_cat_cstr(Rst* dst, const char* input, long len);
void rst_cat_clen(Rst* dst, const char* input);
long rst_len(Rst* ptr);
void rst_chomp(Rst* ptr);
void rst_add_char(Rst* dst, const char chr);