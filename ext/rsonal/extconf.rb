require 'mkmf'
extension_name = 'rsonal'
dir_config(extension_name)
abort "missing malloc()" unless have_func "malloc"
abort "missing free()"   unless have_func "free"
abort "missing ruby.h"   unless have_header "ruby.h"
create_makefile("rsonal/rsonal")
