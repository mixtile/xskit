#!/usr/bin/env python
#encoding: utf-8

from waflib import Utils
import os, datetime, sys

sys.path.append(os.path.abspath('tools/lib'))

import waftools

PLATFORM = Utils.unversioned_sys_platform()
APPNAME = 'xs'

def options(opt):
	# build target os list from 'pal' folder
	targetoses = [t for t in Utils.listdir('pal') if os.path.isfile(os.path.join('pal', t, 'wscript'))]
	targetoses = ', '.join(targetoses)
	opt.add_option('--targetos', default=PLATFORM, action='store', type='string', help=u'specify target operation system. [default: ' + PLATFORM + u'] [available: ' + targetoses + u']')
	# build for dynamic app or static
	opt.add_option('--dynamic-app', default=False, action='store_true', help=u'specify use dynamic app or not. [default: False]')

	# load tools
	opt.load('compiler_c')
	opt.load('compiler_cxx')
	if PLATFORM == 'darwin':
		opt.load('xcode', tooldir=waftools.location)
	elif PLATFORM == 'win32':
		opt.load('msdev', tooldir=waftools.location)

def configure(ctx):
	if PLATFORM == 'win32':
		ctx.load('msdev')
	ctx.recurse(os.path.join('pal', ctx.options.targetos))
	ctx.recurse('core')
	ctx.recurse('graphics')
	ctx.recurse('javascript')
	ctx.recurse('demos')

def build(bld):
	print('start building at ' + datetime.datetime.now().isoformat())
	print('targetos: ' + bld.options.targetos)

	# for MSVC tool
	bld.solution_name = 'xs.sln'

	bld.env.TOP_DIR = bld.path.abspath()

	bld.env.INC_PATH = os.path.join(bld.path.abspath(), 'pal', 'common', 'include')
	bld.env.INC_PATH = bld.env.INC_PATH + ' ' + os.path.join(bld.path.abspath(), 'pal', bld.options.targetos, 'include')
	bld.env.INC_PATH += ' ' + os.path.join(bld.path.abspath(), 'core', 'include')
	bld.env.INC_PATH += ' ' + os.path.join(bld.path.abspath(), 'graphics', 'include')
	bld.env.INC_PATH += ' ' + os.path.join(bld.path.abspath(), 'javascript', 'duktape','include')
	bld.env.INC_PATH += ' ' + os.path.join(bld.path.abspath(), 'javascript', 'canvasinterface','include')
	
	bld.env.LIBS = 'gtk-x11-2.0 gdk-x11-2.0 pangocairo-1.0 atk-1.0 cairo gdk_pixbuf-2.0 gio-2.0 pangoft2-1.0 pango-1.0 gobject-2.0 glib-2.0 fontconfig freetype'
	bld.env.PAL_LIB = 'pal' + bld.options.targetos;

	bld.recurse(os.path.join('pal', bld.options.targetos))
	bld.recurse('core')
	bld.recurse('graphics')
	bld.recurse('javascript')
	bld.recurse('demos')
