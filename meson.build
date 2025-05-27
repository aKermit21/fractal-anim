project('fractal-anim', 'cpp',
  version : '1.0',
  default_options : ['warning_level=3', 'cpp_std=c++17',
                     'buildtype=release', 'b_ndebug=if-release'])

my_src = [
 'src/aggreg.cpp',
 'src/animation.cpp',
 'src/autoscale.cpp',
 'src/colors.cpp',
 'src/dbg_report.cpp',
 'src/demo_func.cpp',
 'src/draw.cpp',
 'src/light.cpp',
 'src/logtxt.cpp',
 'src/cfg_toml.cpp',
 'src/main.cpp',
 'src/opt_lyra.cpp',
 'src/recurrence.cpp',
 'src/text_draw.cpp',
 'src/transform.cpp',
 'src/vec2angle.cpp',
 'src/vec2rotate.cpp',
 'src/windy.cpp']

# Configuration constant/string
conf_data = configuration_data()
conf_data.set('my_project', meson.project_name())
conf_data.set('proj_version', meson.project_version())
configure_file(input : 'src/config.h.in',
               output : 'config.h',
               configuration : conf_data)
             
# Dependencies
#
# Lyra - command line arguments parser header only library for c++
# https://github.com/bfgroup/Lyra
# embedded here as (in) subprojects
lyra_dep = dependency('lyra', fallback : ['Lyra', 'lyra_dep'])

# Toml parser/serializer embedded here as (WrapDB) subprojects
tomlplusplus_dep = dependency('tomlplusplus', fallback : ['tomlplusplus', 'tomlplusplus_dep'])

# SFML - simple 2D graphic (and PC interface) library
# https://www.sfml-dev.org/
# shall be MANUALLY installed (at least those 3 modules)
sfml1_dep = dependency('sfml-graphics', version : '>=2.5.1')
sfml2_dep = dependency('sfml-system', version : '>=2.5.1')
sfml3_dep = dependency('sfml-window', version : '>=2.5.1')

sfml_all_dep = [ sfml1_dep, sfml2_dep, sfml3_dep ]

executable('frexe', sources : my_src,
             dependencies : [sfml_all_dep, lyra_dep, tomlplusplus_dep],
             install : true)

# SFML program needs to load fonts to display text
install_data('fonts/text_fonts.ttf')

# Initial toml file with set of snaphots
install_data('log/fractal-anim-cfg.toml')
