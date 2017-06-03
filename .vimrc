let g:syntastic_c_compiler = $VITASDK.'/bin/arm-vita-eabi-gcc'
let g:syntastic_c_include_dirs = [".", $VITASDK.'/arm-vita-eabi/include', 'extern/sqlite3', 'extern/getline', 'extern/trim']
set colorcolumn=80
