#include <ruby.h>
#include <ruby/encoding.h>
#include <rsonal_string.h>

VALUE Rsonal = Qnil;

void Init_rsonal();
VALUE rsonal_read_json(VALUE self, VALUE file);
VALUE rsonal_write_json(VALUE self, VALUE input);
void process_write_json_data(Rst* str, VALUE input);



void Init_rsonal() {
  Rsonal = rb_define_module("Rsonal");
  rb_define_singleton_method(Rsonal, "write_json", rsonal_write_json, 1);
}

void
process_write_json_fixnum(Rst* str, VALUE input)
{
  char conv[32];
  long val = NUM2LONG(input);
  if(snprintf(conv, 32, "%ld", val) > 32)
    rb_bug("rsonal: bug with integer conversion");
  rst_cat_clen(str, conv);
}

void
process_write_json_string(Rst* str, VALUE input)
{
  rst_add_char(str, '"');
  rst_cat_cstr(str, RSTRING_PTR(input), RSTRING_LEN(input));
  rst_add_char(str, '"');
}

void
process_write_json_array(Rst* str, VALUE input)
{
  int i;
  long siz;

  siz = RARRAY_LEN(input);
  rst_add_char(str, '[');
  for(i=0;i < siz;i++)
  {
    process_write_json_data(str, rb_ary_entry(input, i));
    if(i + 1 < siz)
      rst_cat_clen(str, ", ");
  }
  rst_add_char(str, ']');
}

void
process_write_json_symbol(Rst* str, VALUE input)
{
  process_write_json_string(str, rb_sym_to_s(input));
}

int
process_write_json_hash_inner(VALUE key, VALUE val, VALUE str)
{
  Rst* rst;
  rst = rst_unwrap(str);
  process_write_json_data(rst, key);
  rst_cat_clen(rst, ": ");
  process_write_json_data(rst, val);
  rst_add_char(rst, ',');
  return ST_CONTINUE;
}

void
process_write_json_hash(Rst* str, VALUE input)
{
  long old_len, new_len;
  rst_add_char(str, '{');

  old_len = rst_len(str);
  rb_hash_foreach(input, process_write_json_hash_inner, rst_wrap(str));
  new_len = rst_len(str);

  if(new_len > old_len)
    rst_chomp(str);
  rst_add_char(str, '}');
}

void
process_write_json_bool(Rst* str, int bool_input)
{
  if(bool_input)
    rst_cat_clen(str, "true");
  else
    rst_cat_clen(str, "false");
}

void
process_write_json_null(Rst* str)
{
  rst_cat_clen(str, "null");
}

void
process_write_json_other(Rst* str, VALUE input)
{
  process_write_json_data(str, rb_funcall(input, rb_intern("to_s"), 0));
}

void
process_write_json_data(Rst* str, VALUE input)
{
  switch(TYPE(input))
  {
    case T_FIXNUM: process_write_json_fixnum(str, input);break;
    case T_STRING: process_write_json_string(str, input);break;
    case T_ARRAY: process_write_json_array(str, input);break;
    case T_HASH: process_write_json_hash(str, input);break;
    case T_SYMBOL: process_write_json_symbol(str, input);break;
    case T_TRUE: process_write_json_bool(str, 1);break;
    case T_FALSE: process_write_json_bool(str, 0);break;
    case T_NIL: process_write_json_null(str); break;
    default: process_write_json_other(str, input);
  }
}

VALUE
rsonal_write_json(VALUE self, VALUE input)
{
  rb_encoding* utf8;
  Rst* rst;
  rst = rst_new();

  process_write_json_data(rst, input);

  utf8 = rb_enc_find("UTF-8");
  VALUE str = rb_enc_str_new_cstr("", utf8);
  rb_str_cat(str, rst->ptr, rst->len);
  rb_str_cat2(str, "");
  rst_free(rst);

  return str;
}