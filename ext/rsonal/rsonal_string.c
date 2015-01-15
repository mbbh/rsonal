#include <stdlib.h>
#include <rsonal_string.h>

VALUE rst_class = 0;

Rst*
rst_new()
{
  Rst *rst;
  rst = malloc(sizeof(Rst));
  rst->ptr = malloc(sizeof(char)*64);
  rst->free_space = 64;
  rst->max_size = 64;
  rst->len = 0;
  return rst;
}

void
rst_free(Rst* ptr)
{
  free(ptr->ptr);
  free(ptr);
}

void
rst_ptr_free(RstPtr* ptr)
{
  free(ptr);
}

Rst*
rst_unwrap(VALUE rst)
{
  RstPtr *ptr;
  Data_Get_Struct(rst, RstPtr, ptr);
  return ptr->ptr;
}

VALUE
rst_wrap(Rst* rst)
{
  RstPtr* ptr;
  RstPtr unwrap_ptr;
  if(rst_class == 0)
    rst_init();

  ptr = malloc(sizeof(RstPtr));
  ptr->ptr = rst;
  return Data_Wrap_Struct(rst_class, 0, rst_ptr_free, ptr);
}

void
rst_init()
{
  rst_class = rb_define_class("RST", rb_cString);
}

void
rst_resize(Rst* ptr, long len)
{
  long n_len;
  char* nptr;

  n_len = 128;
  while(n_len < len)
    n_len *= 2;

  nptr = realloc(ptr->ptr, sizeof(char)*(ptr->max_size + n_len));
  ptr->ptr = nptr;
  ptr->max_size += n_len;
  ptr->free_space += n_len;
}

long
rst_len(Rst* ptr)
{
  return ptr->len;
}

void
rst_cat_cstr(Rst* dst, const char* input, long len)
{
  if(dst->free_space <= (len+8))
    rst_resize(dst, len);

  memcpy(&(dst->ptr[sizeof(char)*dst->len]), (void*)input, len*sizeof(char));
  dst->free_space -= len;
  dst->len += len;
}

void
rst_add_char(Rst* dst, const char chr)
{
  if(dst->free_space < 8)
    rst_resize(dst, 8);

  dst->ptr[sizeof(char)*dst->len] = chr;
  dst->len += 1;
  dst->free_space -= 1;
}

void
rst_cat_clen(Rst* dst, const char* input)
{
  rst_cat_cstr(dst, input, strlen(input));
}

void
rst_chomp(Rst* ptr)
{
  ptr->free_space += 1;
  ptr->len -= 1;
}