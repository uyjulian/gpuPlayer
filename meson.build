project('gpuPlayer', ['c', 'cpp'], default_options: ['optimization=0', 'debug=true', 'warning_level=1'])

windows = import('windows')
cpp = meson.get_compiler('cpp')
host_system = host_machine.system()

comctl32_dep = cpp.find_library('comctl32')

gpuPlayer_src = [
	'gpuPlayer/about.cpp',
	'gpuPlayer/config.cpp',
	'gpuPlayer/externals.cpp',
	'gpuPlayer/folder.cpp',
	'gpuPlayer/log.cpp',
	'gpuPlayer/menu.cpp',
	'gpuPlayer/options.cpp',
	'gpuPlayer/player.cpp',
	'gpuPlayer/rebuild.cpp',
	'gpuPlayer/reg.cpp',
	'gpuPlayer/stdafx.cpp',
	'gpuPlayer/tabctrl.cpp',
	'gpuPlayer/winmain.cpp',
	'external/bzip2/blocksort.c',
	'external/bzip2/bzlib.c',
	'external/bzip2/compress.c',
	'external/bzip2/crctable.c',
	'external/bzip2/decompress.c',
	'external/bzip2/huffman.c',
	'external/bzip2/randtable.c',
	'external/zlib/adler32.c',
	'external/zlib/compress.c',
	'external/zlib/crc32.c',
	'external/zlib/deflate.c',
	'external/zlib/inffast.c',
	'external/zlib/inflate.c',
	'external/zlib/inftrees.c',
	'external/zlib/trees.c',
	'external/zlib/uncompr.c',
	'external/zlib/zutil.c',
]
gpuPlayer_rc_src = [
	'gpuPlayer/gpuPlayer.rc',
]
gpuPlayer_src += windows.compile_resources(gpuPlayer_rc_src)
gpuPlayer_include_directories = [
	'gpuPlayer',
	'external/bzip2',
	'external/zlib',
]
gpuPlayer_cppflags = []
gpuPlayer_ldflags = []
if host_system == 'windows' and cpp.get_id() != 'msvc' and cpp.get_id() != 'clang-cl'
	gpuPlayer_ldflags += ['-static']
endif
gpuPlayer_dependson = [
	comctl32_dep,
]
gpuPlayer = executable('gpuPlayer', sources: gpuPlayer_src, include_directories: include_directories(gpuPlayer_include_directories), cpp_args: gpuPlayer_cppflags, link_args: gpuPlayer_ldflags, dependencies: gpuPlayer_dependson, install: true)

if host_system == 'windows' and cpp.get_id() != 'msvc' and cpp.get_id() != 'clang-cl'
archive = custom_target(
    'archive',
    output: meson.project_name() + '-' + meson.project_version() + '-win32.7z',
    input: [gpuPlayer, 'Readme.txt', 'License.txt'],
    build_by_default: true,
    command: [find_program('7z'), 'a', '@OUTPUT@', '@INPUT@']
)
endif
