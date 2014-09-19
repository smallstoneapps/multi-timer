# Multi Timer v3.0
# http://matthewtole.com/pebble/multi-timer/
#
# ----------------------
#
# The MIT License (MIT)
#
# Copyright © 2013 - 2014 Matthew Tole
#
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documentation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to whom the Software is
# furnished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
# --------------------
#
# wscript
#

import json
import datetime
import os
from sh import karma
from sh import uglifyjs
from sh import jshint
from sh import make

top = '.'
out = 'build'

def options(ctx):
  ctx.load('pebble_sdk')

def configure(ctx):
  ctx.load('pebble_sdk')

def distclean(ctx):
  ctx.load('pebble_sdk')
  try:
    os.remove('./src/js/pebble-js-app.js')
  except OSError as e:
    pass
  try:
    os.remove('./src/js/src/generated/appinfo.js')
  except OSError as e:
    pass
  try:
    os.remove('./src/generated/appinfo.h')
  except OSError as e:
    pass

def build(ctx):
  ctx.load('pebble_sdk')

  js_libs = [
    # '../src/js/src/libs/firebase.js',
    '../src/js/src/libs/socket.io.min.js',
    '../src/js/src/libs/http.js',
    '../src/js/src/libs/version.js',
    '../src/js/src/libs/pebble-ga.js',
    '../src/js/src/libs/keen.js',
    '../src/js/src/libs/js-message-queue.min.js',
  ]

  js_sources = [
    '../src/js/src/generated/appinfo.js',
    '../src/js/src/main.js',
    '../src/js/src/timer.js',
    '../src/js/src/timers.js'
  ]
  built_js = '../src/js/pebble-js-app.js'

  c_sources = ctx.path.ant_glob('src/**/*.c')

  # Generate appinfo.js
  ctx(rule=generate_appinfo_js, source='../appinfo.json', target='../src/js/src/generated/appinfo.js')

  # Generate appinfo.h
  ctx(rule=generate_appinfo_h, source='../appinfo.json', target='../src/generated/appinfo.h')

  # Run the C tests.
  ctx(rule=make_test);

  # Run jshint on all the JavaScript files
  ctx(rule=js_jshint, source=js_sources)

  # Run the suite of JS tests.
  # ctx(rule=js_karma)

  # Combine the source JS files into a single JS file.
  ctx(rule=concatenate_js, source=' '.join(js_libs + js_sources), target=built_js)

  # Build and bundle the Pebble app.
  ctx.pbl_program(source=c_sources,
    includes=lib_folders(ctx),
    target='pebble-app.elf')
  ctx.pbl_bundle(elf='pebble-app.elf', js=built_js)

# Return a list of all of the subfolders within the "src/libs/" folder
def lib_folders(ctx):
  folders = []
  libs = ctx.path.find_node('./src/libs')
  for folder in libs.listdir():
    folders.append(libs.find_node(folder).abspath())
  return folders

def generate_appinfo_h(task):
  ext_out = '.c'
  src = task.inputs[0].abspath()
  target = task.outputs[0].abspath()
  appinfo = json.load(open(src))

  f = open(target, 'w')
  write_comment_header(f, 'src/generated/appinfo.h', appinfo)
  f.write('#pragma once\n\n')
  f.write('#define VERSION_LABEL "{0}"\n'.format(appinfo["versionLabel"]))
  f.write('#define UUID "{0}"\n'.format(appinfo["uuid"]))
  f.write('#define DEBUG_MODE {0}\n'.format('true' if appinfo["debug"] else 'false'))
  f.write('#define DELIMITER_INNER \'{0}\'\n'.format(appinfo['delimiters']['inner']))
  f.write('#define DELIMITER_OUTER \'{0}\'\n'.format(appinfo['delimiters']['outer']))
  for key in appinfo['appKeys']:
    f.write('#define APP_KEY_{0} {1}\n'.format(key.upper(), appinfo['appKeys'][key]))
  f.close()

def generate_appinfo_js(task):
  src = task.inputs[0].abspath()
  target = task.outputs[0].abspath()
  data = open(src).read().strip()
  appinfo = json.load(open(src))

  f = open(target, 'w')
  write_comment_header(f, 'src/js/src/generated/appinfo.js', appinfo)
  f.write('/* exported AppInfo */\n\n')
  f.write('var AppInfo = ')
  f.write(data)
  f.write(';')
  f.close()

# Function to write the comment header for both the C and JS generated files.
# Thank goodness that they have the same comment syntax!
def write_comment_header(f, filename, appinfo):
  f.write('/*\n')
  f.write('\n')
  f.write('Multi Timer v{0}\n'.format(appinfo['versionLabel']))
  f.write('\n')
  f.write('http://matthewtole.com/pebble/multi-timer/\n')
  f.write('\n')
  f.write('----------------------\n')
  f.write('\n')
  f.write('The MIT License (MIT)\n')
  f.write('\n')
  f.write('Copyright © 2013 - {0} Matthew Tole\n'.format(datetime.datetime.now().year))
  f.write('\n')
  f.write('Permission is hereby granted, free of charge, to any person obtaining a copy\n')
  f.write('of this software and associated documentation files (the "Software"), to deal\n')
  f.write('in the Software without restriction, including without limitation the rights\n')
  f.write('to use, copy, modify, merge, publish, distribute, sublicense, and/or sell\n')
  f.write('copies of the Software, and to permit persons to whom the Software is\n')
  f.write('furnished to do so, subject to the following conditions:\n')
  f.write('\n')
  f.write('The above copyright notice and this permission notice shall be included in\n')
  f.write('all copies or substantial portions of the Software.\n')
  f.write('\n')
  f.write('THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR\n')
  f.write('IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,\n')
  f.write('FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE\n')
  f.write('AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER\n')
  f.write('LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,\n')
  f.write('OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN\n')
  f.write('THE SOFTWARE.\n')
  f.write('\n')
  f.write('--------------------\n')
  f.write('\n')
  f.write('{0}\n'.format(filename))
  f.write('\n')
  f.write('*/\n')
  f.write('\n')

def concatenate_js(task):
  inputs = (input.abspath() for input in task.inputs)
  uglifyjs(*inputs, o=task.outputs[0].abspath(), b=True, indent_level=2)

def make_test(task):
  ext_out = '.c'
  make()

def js_jshint(task):
  inputs = (input.abspath() for input in task.inputs)
  jshint(*inputs, config="../pebble-jshintrc")

def js_karma(task):
  ext_out = '.js'
  karma("start", single_run=True, reporters="dots")