project(
  'mandelbrot',
  'c',
  version: '0.0.1',
  default_options: ['buildtype=release', 'c_std=c17'],
)

executable_name = 'mandelbrot'

src_files = files(
  'src/main.c',
)

release_c_args = ['-O3']
debug_c_args = ['-g']

raylib_dep = dependency('raylib', required: true)

executable_target = executable(
  executable_name,
  src_files,
  include_directories: include_directories('include'),
  dependencies: [raylib_dep],
  c_args: [release_c_args, debug_c_args],
  link_args: ['-lm'],
)

run_target(
  'debug',
  command: ['gdb', executable_target.full_path()],
  depends: executable_target,
)

run_target(
  'run',
  command: executable_target,
  depends: executable_target,
)
