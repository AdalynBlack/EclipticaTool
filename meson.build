project('ecliptica-tool', 'c', default_options: ['c_std=gnu11'])
add_project_arguments('-D_XOPEN_SOURCE', language:'c')

subdir('po')

gtk4 = dependency('gtk4')

gnome = import('gnome')
resouces = gnome.compile_resources('ecliptica-resources', 'resources.xml')

executable(
   'EclipticaTool',
   'src/main.c',
   'src/home_state.c',
   'src/log_parser.c',
   'src/game_state.c',
   resouces,
   dependencies: [gtk4]
)
