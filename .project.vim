
let s:has_ale = exists(':ALEInfo') != 0
let s:has_syntastic = exists(':SyntasticInfo') != 0
let s:has_vim_tags = exists(':TagsGenerate') != 0

function! s:SetupAle()
  let g:ale_cpp_gcc_options = '-std=c++17 -Wall ' . 
        \ join([
        \ '-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE', 
        \ '-I' . $PWD,
        \ '-I' . $PWD . '/third_party/spdlog/include/'],
        \ ' ')

  let g:ale_cpp_clang_options = g:ale_cpp_gcc_options
  let g:ale_cpp_cc_options = g:ale_cpp_gcc_options


  let g:ale_cpp_cpplint_options = '--filter=-whitespace/parens'

  let g:ale_cpp_clangcheck_options = '' .
        \ join([
        \ '-extra-arg=-std=c++17',
        \ '-extra-arg=-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE', 
        \ '-extra-arg=-I' . $PWD,
        \ '-extra-arg=-I' . $PWD . '/third_party/spdlog/include/'],
        \ ' ')

  let g:ale_linters['cpp'] = ['clang', 'clang++', 'cpplint']

endfunction

function! s:SetupSyntastic()
  let g:syntastic_cpp_compiler_options = 
        \ '-std=c++17 -Wall -Wextra  ' .
        \ '-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE'

  let g:syntastic_cpp_include_dirs = [
        \ $PWD,
        \ $PWD . '/third_party/spdlog/include/',
        \ ]

endfunction

function! s:SetupVimTags()
    let g:vim_tags_project_tags_command = 
                \ '{CTAGS} -R --extra=+f --fields=+l+i+m+s+S '
                \ . '{OPTIONS} {DIRECTORY} '
                \ . '2>/dev/null'
endfunction


if s:has_syntastic
  call s:SetupSyntastic()
endif

if s:has_vim_tags
  call s:SetupVimTags()
endif

if s:has_ale
  call s:SetupAle()
endif
