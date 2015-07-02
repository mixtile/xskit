#! /usr/bin/env python
# -*- coding: utf-8 -*-
# Michel Mooij, michel.mooij7@gmail.com


'''
Summary
-------
Generates C/C++ source code documentation using Doxygen.


Description
-----------
This module contains a wrapper around doxygen; the de facto standard 
tool for generating documentation from annotated C/C++ sources. Doxygen
is part of many linux distributions (e.g. Ubuntu, Fedora, ..) but can 
also be found at http://www.doxygen.org/

In order to start generating documentation of C/C++ source code simply 
issue the following command::

        $ waf doxygen

When needed the generation of documentation for a specific task can be
skipped by adding the feature 'doxygen_skipme' to the task, as presented
in the example below::

        bld.shlib(..., doxygen_skipme=True)

For each C/C++ static- and shared library task doxygen will store the
documentation results in::

        reports/doxygen/<task-name>

Usage
-----
In order to use this waftool simply add it to the 'options' and 'configure' 
functions of your main *waf* script as shown in the example below::

	import waftools

	def options(opt):
		opt.load('doxygen', tooldir=waftools.location)

	def configure(conf):
		conf.load('doxygen')

When configured as shown in the example above, **doxygen** will generate 
source code documentation for all C/C++ tasks that have been defined in your
*waf* build environment when issuing the 'doxygen' build command::

	waf doxygen --targets=cprogram
'''

# TODO: create index page containing links to generated components.


import os, re, datetime
from waflib.Build import BuildContext
from waflib import Utils, TaskGen, Logs, Scripting, Context


def options(opt):
	opt.add_option('--doxygen-output', dest='doxygen_output', default='reports/doxygen',
		action='store', help='defines destination path for generated DoxyGen files')

	opt.add_option('--doxygen-config', dest='doxygen_config', default='resources/doxy.config',
		action='store', help='complete path to doxygen configuration file')


def configure(conf):
	conf.find_program('doxygen', var='DOXYGEN', mandatory=False)
	conf.env.DOXYGEN_OUTPUT = conf.options.doxygen_output
	conf.env.DOXYGEN_CONFIG = conf.options.doxygen_config


class DoxygenContext(BuildContext):
	'''generates C/C++ source code documentation using doxygen.'''
	cmd = 'doxygen'
	fun = Scripting.default_cmd

	def execute(self):
		'''Enrty point for the doxygen source code document generator.
		
		iterate through all groups(g) and task generators (t) and generate
		doxygen report for C and C++ tasks.
		'''
		self.restore()
		if not self.all_envs:
			self.load_envs()
		self.recurse([self.run_dir])

		if self.env.DOXYGEN == []:
			self.fatal('doxygen not available; please install it and reconfigure.')

		for group in self.groups:
			for tgen in group:
				if self.targets == '':
					skipme = getattr(tgen, 'doxygen_skipme', False)
					if skipme:
						continue

				elif tgen.name not in self.targets.split(','):
					continue

				doxygen = self._get_doxygen_conf(tgen)
				if doxygen is not None:
					self._exec_doxygen(tgen, doxygen)

	def _get_doxygen_conf(self, tgen):
		'''Returns a dictionary containing files data for the doxygen
		source code documentation tool
		
		Returns None if no documentation should be generated for the task 
		generator.
		'''
		if not isinstance(tgen, TaskGen.task_gen):
			return None

		tgen.post()
		targets = self.options.targets
		if targets != '' and tgen.name not in targets:
			return None

		features = Utils.to_list(getattr(tgen, 'features', ''))
		if not set(['c', 'cxx']) & set(features):
			return None

		# create list of files for the documentation (paths to files)
		files = []
		sources = Utils.to_list(getattr(tgen, 'source', ''))
		for source in sources:
			src = './%s' % os.path.dirname(source.relpath()).replace('\\','/')
			files.append(src)   

		# create list of include paths
		tgen_path = './%s' % tgen.path.relpath().replace('\\','/')
		include_path = []
		includes = tgen.to_incnodes(tgen.to_list(getattr(tgen, 'includes', [])) + tgen.env['INCLUDES'])
		for include in includes:
			# add include files from component itself to files
			inc = './%s' % include.relpath().replace('\\', '/')
			if inc.startswith(tgen_path):
				files.append(inc)
			else:
				include_path.append(inc)

		# remove duplicates and replace '\' with '/'
		files = list(set(files[:]))
		include_path = list(set(include_path[:]))

		# create a list of defines
		defines = Utils.to_list(getattr(tgen, 'defines', ''))

		# get the application name and version from the main script
		appname = getattr(Context.g_module, Context.APPNAME, os.path.basename(self.srcnode.abspath()))
		version = getattr(Context.g_module, Context.VERSION, os.path.basename(self.srcnode.abspath()))
		now = datetime.datetime.now()
		
		conf = {}
		conf['PROJECT_NAME']     = tgen.name.upper()
		conf['PROJECT_NUMBER']   = '"%s v%s / %s"' % (appname.upper(), version, now.strftime('%Y-%m-%d'))
		conf['PROJECT_BRIEF']    = '"features: %s"' % ', '.join(features)
		conf['OUTPUT_DIRECTORY'] = '%s/%s' % (self.env.DOXYGEN_OUTPUT, tgen.name)
		conf['INPUT']            = ' '.join(files)
		conf['INCLUDE_PATH']     = ' '.join(include_path)
		conf['PREDEFINED']       = ' '.join(defines)
		return conf

	def _exec_doxygen(self, tgen, conf):
		'''Generate source code documentation for the given task generator.'''
		Logs.info("Generating documentation for '%s'" % tgen.name)

		# open template configuration and read as string
		name = self.env.DOXYGEN_CONFIG
		if not os.path.exists(name):
			name = '%s/doxy.config' % os.path.dirname(__file__)
		f = open(name, 'r')
		s = f.read()
		f.close()

		# write configuration key,value pairs into template string
		for key,value in conf.items():
			s = re.sub('%s\s+=.*' % key, '%s = %s' % (key, value), s)

		# create base directory for storing reports
		doxygen_path = self.env.DOXYGEN_OUTPUT
		if not os.path.exists(doxygen_path):
			os.makedirs(doxygen_path)

		# write component configuration to file and doxygen on it
		config = '%s/doxy-%s.config' % (doxygen_path, tgen.name)
		f = open(config, 'w+')
		f.write(s)
		f.close()
		cmd = '%s %s' % (Utils.to_list(self.env.DOXYGEN)[0], config)
		self.cmd_and_log(cmd)

