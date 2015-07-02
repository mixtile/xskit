#!/usr/bin/env python
# -*- encoding: utf-8 -*-
# Michel Mooij, michel.mooij7@gmail.com

'''
Summary
-------
Setup and configure multiple C/C++ build environments and configure
common tools for C/C++ projects. When using this module the following
tools will be loaded and configured automatically:

- cmake
- codeblocks
- cppcheck
- doxygen
- eclipse
- indent
- makefile
- msdev
- bdist
- tree


Usage
-----
The code snippet below provides an example of how a complete build environment
can be created allowing you to build, not only for the host system, but also 
for one or more target platforms using, for instance, a C/C++ cross compiler::

    #!/usr/bin/env python
    # -*- encoding: utf-8 -*-

    import os, waftools
    from waftools import ccenv

    top = '.'
    out = 'build'
    ini = os.path.abspath('ccenv.ini').replace('\\', '/')

    VERSION = '0.0.1'
    APPNAME = 'example'

    def options(opt):
        opt.load('ccenv', tooldir=waftools.location)

    def configure(conf):
        conf.load('ccenv')

    def build(bld):
        ccenv.build(bld, trees=['components'])

    for var in ccenv.variants(ini):
        for ctx in ccenv.contexts():
            name = ctx.__name__.replace('Context','').lower()
            class _t(ctx):
                __doc__ = "%ss '%s'" % (name, var)
                cmd = name + '_' + var
                variant = var

When loading and configuring the *ccenv* tool, as shown in the example above, all 
required C/C++ tools for each build environment variant (i.e. native or cross-
compile) will be loaded and configured as well; e.g. compilers, makefile-, cmake-, 
eclipse-, codeblocks- and msdev exporters, cppcheck source code checking, doxygen 
documentation creation will be available for each build variant. Additional (ccross)
compile build environments can be specified in a seperate .INI file (named ccenv.ini 
in the example above) using following syntax::

    [arm]
    prefix = arm-linux-gnueabihf

    [msvc]
    c = msvc
    cxx = msvc

The section name, *arm* in the example above, specifies the name of the compile
build environment variant. The prefix combined with compiler type (c,cxx) will be 
used in order to create the concrete names of the cross compile toolchain 
binaries::

    AR  = arm-linux-gnueabihf-ar
    CC  = arm-linux-gnueabihf-gcc
    CXX = arm-linux-gnueabihf-g++

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

from waflib import Scripting, Errors, Logs, Utils, Context
from waflib.Build import BuildContext, CleanContext, InstallContext, UninstallContext
from waflib.Tools.compiler_c import c_compiler
from waflib.Tools.compiler_cxx import cxx_compiler

import waftools
from waftools.codeblocks import CodeblocksContext
from waftools.makefile import MakeFileContext
from waftools.eclipse import EclipseContext


CCENV_INI='ccenv.ini'
CCENV_ARG='--ccenv'
CCENV_OPT='ccenv'


def options(opt):
	'''Adds default command line options and tools for C/C++ projects.
	
	:param opt: options context 
	:type opt: waflib.Options.OptionsContext
	'''
	opt.add_option('--debug', dest='debug', default=False, action='store_true',
		help='build with debug information.')

	opt.add_option('--all', dest='all', default=False, action='store_true',
		help='execute command for build environments (host and variants)')

	opt.add_option('--cchost', dest='cchost', default=False, 
		action='store_true',
		help='use the default (native) C/C++ compiler for this platform')

	opt.add_option(CCENV_ARG, dest=CCENV_OPT, default=CCENV_INI, action='store',
		help='path to ccenv configuration file')

	opt.load('compiler_c')
	opt.load('compiler_cxx')
	opt.load('cmake', tooldir=waftools.location)
	opt.load('codeblocks', tooldir=waftools.location)
	opt.load('cppcheck', tooldir=waftools.location)
	opt.load('doxygen', tooldir=waftools.location)
	opt.load('eclipse', tooldir=waftools.location)
	opt.load('makefile', tooldir=waftools.location)
	opt.load('msdev', tooldir=waftools.location)
	opt.load('bdist', tooldir=waftools.location)
	opt.load('tree', tooldir=waftools.location)
	opt.load('indent', tooldir=waftools.location)


def configure(conf):
	'''Create and configure C/C++ build environment(s).
	
	:param conf: configuration context 
	:type conf: waflib.Configure.ConfigurationContext
	'''
	conf.check_waf_version(mini='1.7.6', maxi='1.9.0')
	conf.env.PREFIX = str(conf.env.PREFIX).replace('\\', '/')
	conf.env.CCENVINI = getattr(conf.options, CCENV_OPT)
	conf.env.CCENV = get_ccenv(conf.env.CCENVINI)
	conf.env.HOST = Utils.unversioned_sys_platform()
	c = c_compiler.copy()
	cxx = cxx_compiler.copy()
	host = Utils.unversioned_sys_platform()
	if host not in c_compiler:
		host = 'default'
	configure_variants(conf, host)
	configure_host(conf, host, c, cxx)


def configure_host(conf, host, c, cxx):
	'''Create and configure default C/C++ build environment.
	
	:param conf: configuration context 
	:type conf: waflib.Configure.ConfigurationContext
	:param host: name of the host machine (as used in waf)
	:type host: str
	:param c: contains default C compiler settings
	:type c: dict
	:param cxx: contains default C++ compiler settings
	:type cxx: dict
	'''
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


def configure_variants(conf, host):
	'''Create and configure variant C/C++ build environments.
	
	uses the the configuration data as specified in the *.ini*
	file using configparser.ExtendedInterpolation syntax.

	:param conf: configuration context 
	:type conf: waflib.Configure.ConfigurationContext
	:param host: name of the host machine (as used in waf)
	:type host: str
	'''
	prefix = conf.env.PREFIX
	ccenv = conf.env.CCENV
	env = conf.env

	for name, ini in ccenv.items():
		if 'host' in ini and ini['host'] != conf.env.HOST:
			conf.msg('Skip variant build environment', '%s (host=%s)' % (name, conf.env.HOST), color='YELLOW')
			continue

		conf.setenv(name, env.derive())
		conf.msg('Configure environment', '%s (variant)' % name, color='PINK')
		conf.env.CCENV = ccenv
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
	'''Generic tool and compiler configuration settings used
	 for both host and variant build environments.
	
	:param conf: configuration context 
	:type conf: waflib.Configure.ConfigurationContext
	'''
	conf.load('compiler_c')
	conf.load('compiler_cxx')

	if conf.env.CC_NAME is 'msvc':
		configure_msvc(conf)
	elif conf.env.CC_NAME is 'gcc':
		configure_gcc(conf)

	conf.load('cppcheck')
	conf.load('codeblocks')
	conf.load('eclipse')
	conf.load('makefile')
	conf.load('msdev')
	conf.load('tree')


def configure_msvc(conf):
	'''Configures general environment settings for MSVC compilers; e.g. set
	default C/C++ compiler flags and defines based on the value of the 
	command line --debug option.
	
	:param conf: configuration context 
	:type conf: waflib.Configure.ConfigurationContext
	'''
	if conf.options.debug:
		pdbname = str(getattr(Context.g_module, Context.APPNAME)).lower().replace(' ', '-')
		cflags = ('/Wall', '/Od', '/Zi', '/Fd%s.pdb' % pdbname)
		conf.env.append_unique('LINKFLAGS', '/DEBUG')
	else:
		cflags = ('/Wall', '/Ox')
		conf.env.append_unique('DEFINES', 'NDEBUG')

	for cc in ('CFLAGS', 'CXXFLAGS'):
		for cflag in cflags:
			conf.env.append_unique(cc, flag)
	

def configure_gcc(conf):
	'''Configures general environment settings for GNU compilers; e.g. set
	default C/C++ compiler flags and defines based on the value of the 
	command line --debug option.
	
	:param conf: configuration context 
	:type conf: waflib.Configure.ConfigurationContext
	'''
	flags = ['-Wall', '-pthread']

	if conf.options.debug:
		flags.extend(['-g', '-ggdb'])
		defines = []
	else:
		flags.extend(['-O3'])
		defines = ['NDEBUG']

	for cc in ('CFLAGS', 'CXXFLAGS'):
		for flag in flags:
			conf.env.append_unique(cc, flag)
	for define in defines:
		conf.env.append_unique('DEFINES', define)


def build(bld, trees=[]):
	'''Performs build context commands for one or more C/C++
	build environments using the trees argument as list of source
	directories.
	
	:param bld: build context 
	:type bld: waflib.Build.BuildContext
	:param trees: top level directories containing projects to build
	:type trees: list
	'''
	if bld.variant:
		libs = bld.env.CCENV[bld.variant]['shlib']
		for lib in libs:
			bld.read_shlib(lib, paths=bld.env.LIBPATH)

	if bld.options.all and not bld.variant:
		if bld.cmd in ('build', 'clean', 'install', 'uninstall', 'codeblocks', 'makefile', 'eclipse'):
			for variant in bld.env.CCENV.keys():
				Scripting.run_command('%s_%s' % (bld.cmd, variant))

	for tree in trees:
		for script in waftools.get_scripts(tree, 'wscript'):
			bld.recurse(script)


def get_ccenv(fname):
	'''Returns dictionary of variant C/C++ build environments. In which the keys
	are the name of the actual variant C/C++ build environments and its values the
	settings for that variant build environment.
	
	:param fname: Complete path to the configuration file.
	:type fname: str
	'''
	if not os.path.exists(fname):
		Logs.warn("CCENV: ini file '%s' not found!" % fname)
	ccenv = {}
	c = configparser.ConfigParser()
	c.read(fname)
	for s in c.sections():
		ccenv[s] = {'prefix' : None, 'shlib' : [], 'env' : [], 'c': ['gcc'], 'cxx': ['g++', 'cpp']}
		if c.has_option(s, 'c'):
			ccenv[s]['c'] = c.get(s,'c').split(',')
		if c.has_option(s, 'cxx'):
			ccenv[s]['cxx'] = c.get(s,'cxx').split(',')
		if c.has_option(s, 'prefix'):
			ccenv[s]['prefix'] = c.get(s,'prefix')
		if c.has_option(s, 'shlib'):
			ccenv[s]['shlib'] = [l for l in str(c.get(s,'shlib')).split(',') if len(l)]
		if c.has_option(s, 'env'):
			ccenv[s]['env'] = [l.split('\t') for l in c.get(s,'env').splitlines() if len(l)]
		if c.has_option(s, 'host'):
			ccenv[s]['host'] = c.get(s,'host')
	return ccenv


def variants(fname=None):
	'''Returns a list of variant names; i.e. a list of names for build environments 
	that have been defined in the 'ccenv.ini' configuration file.
	
	:param fname: Complete path to the configuration file
	:type fname: str
	'''
	if not fname:
		fname = CCENV_INI
		opt = '%s=' % CCENV_ARG
		for a in sys.argv:
			if a.startswith(opt):
				fname = a.replace(opt, '')

	ccenv = get_ccenv(fname)
	return list(ccenv.keys())


def contexts():
	'''Returns a list of build contexts to be used for variant build 
	environments.
	'''
	return [ BuildContext, CleanContext, InstallContext, UninstallContext, CodeblocksContext, MakeFileContext, EclipseContext ]

