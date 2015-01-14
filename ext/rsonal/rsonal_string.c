#include <stdlib.h>
#include <rsonal_string.h>

VALUE rst_class = 0;

Rst*
rst_new()
{
  Rst *rst;
  rst = malloc(sizeof(rst));
  rst->ptr = malloc(sizeof(char)*8);
  rst->free_space = 8;
  rst->last_alloc = 8;
  rst->max_size = 8;
  return rst;
}

void
rst_free(Rst* ptr)
{
  free(ptr->ptr);
  free(ptr);
}

Rst*
rst_unwrap(VALUE rst)
{
  Rst* unwrap_rst;
  Data_Get_Struct(rst, Rst, unwrap_rst);
  return unwrap_rst;
}

VALUE
rst_wrap(Rst* rst)
{
  if(rst_class == 0)
    rst_init();

  return Data_Wrap_Struct(rst_class, 0, rst_free, rst);
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

  n_len = ptr->last_alloc;
  while(n_len < len)
    n_len *= 2;

  ptr->ptr = realloc(ptr->ptr, ptr->max_size + n_len);
  ptr->max_size += n_len;
  ptr->free_space += n_len;
  ptr->last_alloc = n_len;
}

void
rst_cat_cstr(Rst* dst, const char* input, long len)
{
  if(dst->free_space < len)
    rst_resize(dst, len);
  strncat(dst->ptr, input, len);
  dst->free_space -= len;
}

void
rst_cat_clen(Rst* dst, const char* input)
{
  rst_cat_cstr(dst, input, strlen(input));
}