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

}

void
process_write_json_data(VALUE str, VALUE input)
{
  switch(TYPE(input))
  {
    case T_FIXNUM: process_write_json_fixnum(str, input);break;
    case T_STRING: process_write_json_string(str, input);break;
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