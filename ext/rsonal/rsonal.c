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
  char conv[64];
  long val = NUM2LONG(input);
  snprintf(conv, 64, "%ld", val);
  rst_cat_clen(str, conv);
}

// int
// numdigits(double dbl)
// {
//   long count = 0;
//   while(abs(dbl) >= 0.0000001)
//   {
//     dbl = dbl * 10;
//     count = count + 1;
//     num = num - (int)num;
//   }
// }

void
process_write_json_float(Rst* str, VALUE input)
{
  char conv[32];
  double dbl = NUM2DBL(input);
  snprintf(conv, 32, "%09e", dbl);
  rst_cat_clen(str, conv);
}

void
process_write_json_bignum(Rst* str, VALUE input)
{
  int i;
  long len;
  VALUE res;
  if(FIXNUM_P(input))
  {
    process_write_json_fixnum(str, input);
    return;
  }
  res = rb_big2str(input,10);
  rst_cat_cstr(str, RSTRING_PTR(res), RSTRING_LEN(res));
}

int
is_unicode_seq_start(const unsigned char c)
{
  return (c >= 0xC0 && c <= 0xFD);
}

unsigned short arr_numbytes[] = {0, 0x7E, 0x3E, 0x1E, 0x0E, 0x06};

int
utf_numbytes(const unsigned short inp)
{
  unsigned i;
  for(i=1;i < 6;i++)
  {
    if((inp >> i) == arr_numbytes[i])
      return 6-i;
  }
  return 0;
}

unsigned int
offset_mask(int offset)
{
  return 0xFF >> (offset+1);
}

const char*
check_str_escape(const char chr)
{
  switch(chr)
  {
    case '"': return "\\\"";
    case '\t': return "\\t";
    case '\b': return "\\b";
    case '\r': return "\\r";
    case '\f': return "\\f";
  }
  return "";
}

int
check_needs_str_escape(const char* str, long len)
{
  int i;
  for(i=0;i<len;i++)
  {
    if(str[i] == '"' || str[i] == '\t' || str[i] == '\b' ||
      str[i] == '\r' || str[i] == '\f')
      return 1;
  }
  return 0;
}

void
process_write_json_utf8(Rst* str, VALUE input)
{
  int i, j, off;
  unsigned long wchar;
  char conv[8] = {0};
  const unsigned char* ptr = (unsigned char*)RSTRING_PTR(input);
  const int siz = RSTRING_LEN(input);

  for(i=0;i < siz;i++)
  {
    if(!is_unicode_seq_start(ptr[i]))
    {
      rst_add_char_check(str, ptr[i], &check_str_escape);
      continue;
    }
    off = utf_numbytes(ptr[i]);
    wchar = ptr[i] & offset_mask(off);
    for(j=0;j < off && (i+j) < siz;j++)
    {
      wchar <<= 6;
      wchar |= (ptr[++i] & 0x3F);
    };
    off = snprintf(conv, 8, "\\u%02lx%02lx", (wchar & 0xFF00)>>8, wchar&0xFF);
    rst_cat_cstr(str, conv, off);
  }
}

void
process_write_json_string(Rst* str, VALUE input)
{
  const char* enc_name = rb_enc_get(input)->name;
  rst_add_char(str, '"');
  if(!strncmp("US-ASCII", enc_name, 8))
    rst_cat_cstr_check(str, RSTRING_PTR(input), RSTRING_LEN(input),
      &check_str_escape, &check_needs_str_escape);
  else if(!strncmp("UTF-8", enc_name, 5))
    process_write_json_utf8(str, input);
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
    case T_FLOAT: process_write_json_float(str, input); break;
    case T_BIGNUM: process_write_json_bignum(str, input);break;
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