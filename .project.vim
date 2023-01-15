scriptencoding utf-8

let s:has_ale = exists(':ALEInfo') != 0
let s:has_syntastic = exists(':SyntasticInfo') != 0
let s:has_vim_tags = exists(':TagsGenerate') != 0

let s:include_paths = [
      \ $PWD,
      \ $PWD . '/third_party/spdlog/include/',
      \ $PWD . '/third_party/cppzmq',
      \ '/usr/include/c++/9/',
      \ '/usr/include/x86_64-linux-gnu/c++/9/',
      \ ]

function! s:SetupAle()
  " echom 'Settting up ALE'
  let g:ale_cpp_gcc_options = '-std=c++17 -Wall -DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE'
  for l:path in s:include_paths 
    let g:ale_cpp_gcc_options = g:ale_cpp_gcc_options . ' -I' . l:path
  endfor

  let g:ale_cpp_clang_options = g:ale_cpp_gcc_options
  let g:ale_cpp_cc_options = g:ale_cpp_gcc_options


  let g:ale_cpp_cpplint_options = '--filter=-whitespace/parens'

  let g:ale_cpp_clangcheck_options = '' .
        \ join([
        \ '-extra-arg=-std=c++17',
        \ '-extra-arg=-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE'],
        \ ' ')

  for l:path in s:include_paths 
    let g:ale_cpp_clangcheck_options = g:ale_cpp_clangcheck_options . ' -extra-arg=-I' . l:path
  endfor

  let g:ale_linters['cpp'] = ['clang', 'clang++', 'cpplint']

endfunction

function! s:SetupSyntastic()
  " echom 'Settting up Syntastic'
  let g:syntastic_cpp_compiler_options = 
        \ '-std=c++17 -Wall -Wextra  ' .
        \ '-DSPDLOG_ACTIVE_LEVEL=SPDLOG_LEVEL_TRACE'

  let g:syntastic_cpp_include_dirs = [
        \ $PWD,
        \ $PWD . '/third_party/spdlog/include/',
        \ ]

endfunction

function! s:SetupVimTags()
  " echom 'Settting up vim tags'
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
