# Multi Timer v2.7.0
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
    os.remove('../src/js/pebble-js-app.js')
    os.remove('../src/js/src/appinfo.js')
    os.remove('../src/generated/appinfo.h')
  except OSError as e:
    pass

def build(ctx):
  ctx.load('pebble_sdk')

  js_libs = [
    '../src/js/src/libs/firebase.js',
  ]

  js_sources = [
    '../src/js/src/appinfo.js',
    '../src/js/src/version.js',
    '../src/js/src/config.js',
    '../src/js/src/timers.js',
    '../src/js/src/main.js'
  ]
  built_js = '../src/js/pebble-js-app.js'

  c_sources = ctx.path.ant_glob('src/**/*.c')

  # Generate appinfo.js
  ctx(rule=generate_appinfo_js, source='../appinfo.json', target='../src/js/src/appinfo.js')

  # Generate appinfo.h
  ctx(rule=generate_appinfo_c, source='../appinfo.json', target='../src/generated/appinfo.h')

  # Run the C tests.
  ctx(rule=make_test);

  # Run jshint on all the JavaScript files
  # ctx(rule=js_jshint, source=js_sources)

  # Run the suite of JS tests.
  # ctx(rule=js_karma)

  # Combine the source JS files into a single JS file.
  # ctx(rule=concatenate_js, source=' '.join(js_libs + js_sources), target=built_js)

  # Build and bundle the Pebble app.
  ctx.pbl_program(source=c_sources, target='pebble-app.elf')
  # ctx.pbl_bundle(elf='pebble-app.elf', js=built_js)
  ctx.pbl_bundle(elf='pebble-app.elf')

def generate_appinfo_c(task):
  ext_out = '.c'
  src = task.inputs[0].abspath()
  target = task.outputs[0].abspath()
  appinfo = json.load(open(src))

  f = open(target, 'w')

  f.write('/***\n')
  f.write(' * Multi Timer\n')
  f.write(' * Copyright © 2013 - 2014 Matthew Tole\n')
  f.write(' *\n')
  f.write(' * generated/appinfo.h\n')
  f.write(' ***/\n\n')
  f.write('#pragma once\n\n')
  f.write('#define VERSION_LABEL "{0}"\n'.format(appinfo["versionLabel"]))
  f.write('#define VERSION_CODE {0}\n'.format(appinfo["versionCode"]))
  f.write('#define UUID "{0}"\n'.format(appinfo["uuid"]))
  for key in appinfo['appKeys']:
    f.write('#define APP_KEY_{0} {1}\n'.format(key.upper(), appinfo['appKeys'][key]))
  f.close()

def generate_appinfo_js(task):
  src = task.inputs[0].abspath()
  target = task.outputs[0].abspath()
  data = open(src).read().strip()

  f = open(target, 'w')
  f.write('/* exported AppInfo */\n\n')
  f.write('var AppInfo = ')
  f.write(data)
  f.write(';')
  f.close()

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