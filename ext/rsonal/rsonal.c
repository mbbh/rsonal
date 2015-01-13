#include <ruby.h>
#include <ruby/encoding.h>

VALUE Rsonal = Qnil;

void Init_rsonal();
VALUE rsonal_read_json(VALUE self, VALUE file);
VALUE rsonal_write_json(VALUE self, VALUE input);
void process_write_json_data(VALUE str, VALUE input);



void Init_rsonal() {
  Rsonal = rb_define_module("Rsonal");
  rb_define_singleton_method(Rsonal, "read_json", rsonal_read_json, 1);
  rb_define_singleton_method(Rsonal, "write_json", rsonal_write_json, 1);
}

VALUE
rsonal_read_json(VALUE self, VALUE file)
{
  printf("welcome to the ruby land\n");
  printf("%s\n", StringValueCStr(file));
  return Qnil;
}

void
process_write_json_fixnum(VALUE str, VALUE input)
{
  char conv[32];
  long val = NUM2LONG(input);
  if(snprintf(conv, 32, "%ld", val) > 32)
    rb_bug("rsonal: bug with integer conversion");
  rb_str_cat(str, conv, strlen(conv));
}

void
process_write_json_string(VALUE str, VALUE input)
{
  struct RString *rstr = RSTRING(input);
  rb_str_cat2(str, "\"");
  rb_str_cat(str, RSTRING_PTR(rstr), RSTRING_LEN(rstr));
  rb_str_cat2(str, "\"");
}

void
process_write_json_array(VALUE str, VALUE input)
{
  int i;
  long siz;
  struct RArray *rarr;

  rarr = RARRAY(input);
  siz = RARRAY_LEN(rarr);
  rb_str_cat2(str, "[");
  for(i=0;i < siz;i++)
  {
    process_write_json_data(str, rb_ary_entry(input, i));
    if(i + 1 < siz)
      rb_str_cat2(str, ", ");
  }
  rb_str_cat2(str, "]");
}

void
process_write_json_symbol(VALUE str, VALUE input)
{
  process_write_json_string(str, rb_sym_to_s(input));
}

int
process_write_json_hash_inner(VALUE key, VALUE val, VALUE str)
{
  process_write_json_data(str, key);
  rb_str_cat2(str, ": ");
  process_write_json_data(str, val);
  rb_str_cat2(str, ",");
  return ST_CONTINUE;
}

void
process_write_json_hash(VALUE str, VALUE input)
{
  long old_len, new_len;
  rb_str_cat2(str, "{");

  old_len = RSTRING_LEN(str);
  rb_hash_foreach(input, process_write_json_hash_inner, str);
  new_len = RSTRING_LEN(str);

  if(new_len > old_len)
    rb_str_set_len(str, RSTRING_LEN(str)-1);
  rb_str_cat2(str, "}");
}

void
process_write_json_bool(VALUE str, int bool_input)
{
  if(bool_input)
    rb_str_cat2(str, "true");
  else
    rb_str_cat2(str, "false");
}

void
process_write_json_null(VALUE str)
{
  rb_str_cat2(str, "null");
}

void
process_write_json_other(VALUE str, VALUE input)
{
  process_write_json_data(str, rb_funcall(input, rb_intern("to_s"), 0));
}

void
process_write_json_data(VALUE str, VALUE input)
{
  switch(TYPE(input))
  {
    case T_FIXNUM: process_write_json_fixnum(str, input);break;
    case T_STRING: process_write_json_string(str, input);break;
    case T_ARRAY: process_write_json_array(str, input);break;
    case T_HASH: process_write_json_hash(str, input);break;
    case T_SYMBOL: process_write_json_symbol(str, input);break;
    case T_TRUE: process_write_json_bool(str, 0);break;
    case T_FALSE: process_write_json_bool(str, 1);break;
    case T_NIL: process_write_json_null(str); break;
    default: process_write_json_other(str, input);
  }
}

VALUE
rsonal_write_json(VALUE self, VALUE input)
{
  rb_encoding* utf8;
  utf8 = rb_enc_find("UTF-8");
  VALUE str = rb_enc_str_new_cstr("", utf8);
  process_write_json_data(str, input);
  return str;
}