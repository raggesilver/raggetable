project(
  'raggetable',
  'c',
  version: '0.1.2',
  default_options: ['warning_level=3', 'c_std=gnu99', 'optimization=3'],
)

lib_inc = include_directories('.')

lib_sources = files(
  'hashtable.c',
)

raggetable = library('raggetable', lib_sources, include_directories: lib_inc, install: true)
dep_raggetable = declare_dependency(
  link_with: [raggetable],
  include_directories: lib_inc,
)

install_headers('hashtable.h', subdir: 'raggetable')

pkg = import('pkgconfig')
pkg.generate(raggetable, version: meson.project_version(), subdirs: 'raggetable')

if get_option('tests')
  unity_subproject = subproject('unity')
  unity_dependency = unity_subproject.get_variable('unity_dep')

  test_sources = files(
    'hashtable.test.c',
  )

  hashtable_test = executable(
    'hashtable_test',
    test_sources,
    dependencies: [unity_dependency, dep_raggetable],
  )
  test('hashtable', hashtable_test)
endif
