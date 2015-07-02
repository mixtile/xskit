#!/usr/bin/env python
# -*- encoding: utf-8 -*-
# Michel Mooij, michel.mooij7@gmail.com

'''
Summary
-------
Setup and configure multiple C/C++ build environments and configure
common C/C++ tools used when cross-compiling. When using this module
the following tools will be loaded and configured autmatically as well:

- cmake
- codeblocks
- cppcheck
- doxygen
- eclipse
- gnucc
- indent
- makefile
- msdev
- bdist
- tree


.. warning::
	This module has been marked as deprecated and will be removed in 
	release version 0.5.0; use the *ccenv* module instead.

Usage
-----
The code snippet below provides an example of how a complete build environment
can be created allowing you to build, not only for the host system, but also 
for one or more target platforms using a C/C++ cross compiler::

	#!/usr/bin/env python
	# -*- encoding: utf-8 -*-

	import os, waftools
	from waftools import ccross

	top = '.'
	out = 'build'
	prefix = 'output'

	VERSION = '0.0.1'
	APPNAME = 'cross-test'

	def options(opt):
		opt.add_option('--prefix', dest='prefix', default=prefix, 
						help='installation prefix [default: %r]' % prefix)
		opt.load('ccross', tooldir=waftools.location)

	def configure(conf):
		conf.load('ccross')

	def build(bld):
		ccross.build(bld, trees=['components'])

	for var in ccross.variants():
		for ctx in ccross.contexts():
			name = ctx.__name__.replace('Context','').lower()
			class _t(ctx):
				__doc__ = "%ss '%s'" % (name, var)
				cmd = name + '_' + var
				variant = var

When loading and configuring the *ccross* tool, as shown in the example above, all 
required C/C++ tools for each build environment variant (i.e. native or cross-
compile) will be loaded and configured as well; e.g. compilers, makefile-, cmake-, 
eclipse-, codeblocks- and msdev exporters, cppcheck source code checking, doxygen 
documentation creation will be available for each build variant. Cross compile 
build environments can be specified in a seperate .INI file (named ccross.ini 
in the example above) using following syntax::

	[arm]
	c = gcc
	cxx = g++
	prefix = arm-linux-gnueabihf

The section name, *arm* in the example above, specifies the name of the cross-compile
build environment variant. The prefix combined with compiler type (c,cxx) will be 
used in order to create the concrete names of the cross compile toolchain 
binaries::

	AR	= arm-linux-gnueabihf-ar
	CC	= arm-linux-gnueabihf-gcc
	CXX	= arm-linux-gnueabihf-g++

Concrete build scripts (i.e. wscript files) for components can be placed somewhere 
within the *components* sub-directory. Any top level wscript file of a tree (being 
*components* in this example) will be detected and incorporated within the build 
environment. Any wscript files below those top level script files will have to be 
included using the *bld.recurse('../somepath')* command from the top level script 
of that tree.
'''

import os
import sys
try:
	import ConfigParser as configparser
except:
	import configparser

from waflib import Scripting, Errors, Logs, Utils
from waflib.Build import BuildContext, CleanContext, InstallContext, UninstallContext
from waflib.Tools.compiler_c import c_compiler
from waflib.Tools.compiler_cxx import cxx_compiler

import waftools
from waftools.codeblocks import CodeblocksContext
from waftools.makefile import MakeFileContext
from waftools.eclipse import EclipseContext


CCROSS_INI='ccross.ini'
CCROSS_ARG='--ccross'
CCROSS_OPT='ccross'


def options(opt):
	opt.add_option('--all', dest='all', default=False, action='store_true', 
				help='execute command for cross-compile environments as well')
				
	opt.add_option('--cchost', dest='cchost', default=False, action='store_true', 
				help='use the default C/C++ compiler for this platform')

	opt.add_option(CCROSS_ARG, dest=CCROSS_OPT, default=CCROSS_INI, action='store', 
				help='cross compile configuration')

	opt.load('compiler_c')
	opt.load('compiler_cxx')
	opt.load('cmake', tooldir=waftools.location)
	opt.load('codeblocks', tooldir=waftools.location)
	opt.load('cppcheck', tooldir=waftools.location)
	opt.load('doxygen', tooldir=waftools.location)
	opt.load('eclipse', tooldir=waftools.location)
	opt.load('gnucc', tooldir=waftools.location)
	opt.load('makefile', tooldir=waftools.location)
	opt.load('msdev', tooldir=waftools.location)
	opt.load('bdist', tooldir=waftools.location)
	opt.load('tree', tooldir=waftools.location)
	opt.load('indent', tooldir=waftools.location)


def configure(conf):
	conf.check_waf_version(mini='1.7.6', maxi='1.9.0')	
	conf.env.PREFIX = str(conf.env.PREFIX).replace('\\', '/')
	conf.env.CCROSSINI = getattr(conf.options, CCROSS_OPT)
	conf.env.CCROSS = get_ccross(conf.env.CCROSSINI)
	
	c = c_compiler.copy()
	cxx = cxx_compiler.copy()
	host = Utils.unversioned_sys_platform()
	if host not in c_compiler:
		host = 'default'
	configure_cross(conf, host)
	configure_host(conf, host, c, cxx)


def configure_host(conf, host, c, cxx):	
	conf.setenv('')
	conf.msg('Configure environment', '%s (host)' % host, color='PINK')	
	if conf.options.cchost:
		c_compiler[host] = c[host]
		cxx_compiler[host] = cxx[host]
	else:
		c_compiler[host] = ['gcc']
		cxx_compiler[host] = ['g++']

	configure_base(conf)
	conf.load('cmake')
	conf.load('doxygen')
	conf.load('bdist')
	conf.load('indent')


def configure_cross(conf, host):
	'''create and configure cross compile environments

 	uses the the configuration data as specified in the *.ini*
	file using configparser.ExtendedInterpolation syntax.
	'''
	prefix = conf.env.PREFIX
	ccross = conf.env.CCROSS
	
	for name, ini in ccross.items(): # setup cross compile environment(s)
		conf.setenv(name)
		conf.msg('Configure environment', '%s (cross)' % name, color='PINK')
		conf.env.CCROSS = ccross
		conf.env.PREFIX = '%s/opt/%s' % (prefix, name)
		conf.env.BINDIR = '%s/opt/%s/bin' % (prefix, name)
		conf.env.LIBDIR = '%s/opt/%s/lib' % (prefix, name)
		
		for (var, action, value) in ini['env']:
			if action == 'set':
				conf.env[var] = value
			else:
				conf.env.append_unique(var, value)

		c_compiler[host] = ini['c']
		cxx_compiler[host] = ini['cxx']

		head = ini['prefix']
		for tail in ini['c']:
			if tail != 'msvc':
				try:
					cc = '%s-%s' % (head, tail) if head else tail
					conf.find_program(cc, var='CC')
				except Errors.ConfigurationError:
					Logs.debug("program '%s' not found" % (cc))
				else:
					break
		
		for tail in ini['cxx']:
			if tail != 'msvc':
				try:
					cxx = '%s-%s' % (head, tail) if head else tail
					conf.find_program(cxx, var='CXX')
				except Errors.ConfigurationError:
					Logs.debug("program '%s' not found" % (cxx))
				else:
					break

		if ini['c'][0] != 'msvc':
			ar = '%s-ar' % (head) if head else 'ar'
			conf.find_program(ar, var='AR')
		configure_base(conf)


def configure_base(conf):
	try:
		conf.load('compiler_c unity')
		conf.load('compiler_cxx unity')
		conf.load('batched_cc')
	except Errors.ConfigurationError: # retry without unity,batched_cc
		conf.load('compiler_c')
		conf.load('compiler_cxx')
	conf.load('cppcheck')
	conf.load('codeblocks')
	conf.load('eclipse')
	conf.load('gnucc')
	conf.load('makefile')
	conf.load('msdev')
	conf.load('tree')


def build(bld, trees=[]):
	if bld.variant:
		libs = bld.env.CCROSS[bld.variant]['shlib']
		for lib in libs:
			bld.read_shlib(lib, paths=bld.env.LIBPATH)

	if bld.options.all and not bld.variant:
		if bld.cmd in ('build', 'clean', 'install', 'uninstall', 'codeblocks', 'makefile', 'eclipse'):
			for variant in bld.env.CCROSS.keys():
				Scripting.run_command('%s_%s' % (bld.cmd, variant))

	for tree in trees:
		for script in waftools.get_scripts(tree, 'wscript'):
			bld.recurse(script)

		
def get_ccross(fname):
	'''Returns dictionary of cross-compile build environments. Dictionary key name
	depict the environment name (i.e. variant name).

	
	:param fname: Complete path to the config.ini file
	:type fname: str
	'''
	if not os.path.exists(fname):
		Logs.warn("CCROSS: ini file '%s' not found!" % fname)
	cross = {}
	c = configparser.ConfigParser()
	c.read(fname)
	for s in c.sections():
		cross[s] = {'prefix' : None, 'shlib' : [], 'env' : [], 'c': ['gcc'], 'cxx': ['g++']}
		if c.has_option(s, 'c'):
			cross[s]['c'] = c.get(s,'c').split(',')
		if c.has_option(s, 'cxx'):
			cross[s]['cxx'] = c.get(s,'cxx').split(',')
		if c.has_option(s, 'prefix'):
			cross[s]['prefix'] = c.get(s,'prefix')
		if c.has_option(s, 'shlib'):
			cross[s]['shlib'] = [l for l in str(c.get(s,'shlib')).split(',') if len(l)]
		if c.has_option(s, 'env'):
			cross[s]['env'] = [l.split('\t') for l in c.get(s,'env').splitlines() if len(l)]
	return cross


def variants(fname=None):
	'''Returns a list of variant names; i.e. a list of names for build environments 
	that have been defined in the 'ccross.ini' configuration file.
	
	:param fname: Complete path to the config.ini file
	:type fname: str
	'''
	if not fname:
		fname = CCROSS_INI
		opt = '%s=' % CCROSS_ARG
		for a in sys.argv:
			if a.startswith(opt):
				fname = a.replace(opt, '')

	cross = get_ccross(fname)
	return list(cross.keys())


def contexts():
	'''Returns a list of cross-compile build contexts.
	
	:param name: Complete path to the config.ini file
	:type name: list of waflib.Build.BuildContext
	'''
	return [ BuildContext, CleanContext, InstallContext, UninstallContext, CodeblocksContext, MakeFileContext, EclipseContext ]

