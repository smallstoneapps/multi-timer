from sh import jshint

top = '.'
out = 'build'

def options(ctx):
    ctx.load('pebble_sdk')

def configure(ctx):
    ctx.load('pebble_sdk')
    jshint.bake(['--config', '~/Projects/Pebble/pebble-jshintrc'])

def build(ctx):
    ctx.load('pebble_sdk')

    jshint("src/js/pebble-js-app.js")

    ctx.pbl_program(source=ctx.path.ant_glob('src/**/*.c'),
                    target='pebble-app.elf')

    ctx.pbl_bundle(elf='pebble-app.elf',
                   js=ctx.path.ant_glob('src/js/**/*.js'))
