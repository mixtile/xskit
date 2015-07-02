#!/usr/bin/env python
# -*- encoding: utf-8 -*-
# Michel Mooij, michel.mooij7@gmail.com


'''
Configures common release and debug settings for C/C++ compilers.

.. warning::
	This module has been marked as deprecated and will be removed in 
	release version 0.5.0; use the *ccenv* module instead.
'''


def options(opt):
	'''Add default command line options for C/C++ compilers
	
	:param opt: options context 
	:type opt: waflib.Options.OptionsContext
	'''
	opt.add_option('--debug', dest='debug', default=False, action='store_true', help='build with debug information.')


def configure(conf):
	cc = conf.env.CC_NAME
	if cc is 'msvc':
		configure_msvc(conf)	
	else:
		configure_gcc(conf)


def configure_msvc(conf):
	'''Configures general environment settings for MSVC compilers; e.g. set
	default C/C++ compiler flags and defines based on the value of the 
	command line --debug option.
	
	:param conf: configuration context 
	:type conf: waflib.Configure.ConfigurationContext
	'''
	flags = ['/Wall']
	
	if conf.options.debug:
		flags.extend(['/Od', '/Zi'])
		defines = []
	else:
		flags.extend(['/Ox'])
		defines = ['NDEBUG']

	for cc in ('CFLAGS', 'CXXFLAGS'):
		for flag in flags:
			conf.env.append_unique(cc, flag)
	for define in defines:
		conf.env.append_unique('DEFINES', define)
		
	
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

