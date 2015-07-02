#!/usr/bin/env python
# -*- encoding: utf-8 -*-
# Michel Mooij, michel.mooij7@gmail.com


'''
Summary
-------
Clean up and format C/C++ source code using GNU indent.

Usage
-----
Source code from C/C++ tasks can cleaned and formatted according to the
specified rules (default is *GNU* style) using the following command::

        $ waf indent --targets=<task>

When cleaning only those task specified using the '--targets=' option
will be cleaned. The default is to clean all task within the entire build 
environment. Specific C/C++ task can be permanently excluded from cleaning
using the 'indent_skipme' as attribute for those tasks.

formatting rules can be specified the using a 'indent.pro' file using the
options as described for GNU indent. These global rules can used by this
module when using the '--indent-pro=<path-to-indent-pro' option at 
configuration time.

''' 


import os
from waflib import Scripting, Logs, Utils
from waflib.Build import BuildContext


def options(opt):
	opt.add_option('--indent-pro', dest='indent_pro', 
		default=None, action='store', help='GNU indent configuration')
	opt.add_option('--indent-cleanup', dest='indent_cleanup', 
		default=False, action='store_true', help='cleanup GNU indent backup files')


def configure(conf):
	conf.find_program('indent', var='INDENT', mandatory=False)
	if conf.options.indent_pro:
		conf.env.INDENT_PRO = conf.options.indent_pro
	if conf.options.indent_cleanup:
		conf.env.INDENT_CLEAN = [True]


class GnuIndentContext(BuildContext):
	'''format C/C++ source code using GNU indent.'''
	cmd = 'indent'
	fun = Scripting.default_cmd

	def execute(self):
		self.restore()
		if not self.all_envs:
			self.load_envs()
		self.recurse([self.run_dir])

		targets = self.targets.split(',') if self.targets!='' else None

		for group in self.groups:
			for tgen in group:
				if targets and tgen.name not in targets:
					continue
				if getattr(tgen, 'indent_skipme', False):
					continue
				if not set(('c', 'cxx')) & set(getattr(tgen, 'features', [])):
					continue
				(sources, headers) = self.get_files(tgen)
				self.exec_indent(tgen, sources, headers)

	def get_files(self, tgen):
		'''returns a tuple containing a list of source and header filenames 
		defined for the given task generator.

		:param opt: Task for which the input file names should be returned.
		:type opt: waflib.Task.TaskGenerator
		'''
		sources = tgen.to_list(getattr(tgen, 'source', []))
		sources = [tgen.path.find_node(s) if isinstance(s, str) else s for s in sources]
		sources = [s.abspath() for s in sources]

		headers = []
		for include in tgen.to_list(getattr(tgen, 'includes', [])):
			node = tgen.path.find_node(include)
			if not node:
				Logs.warn("WARNING task(%s): include directory '%s' does not exist" % (tgen.name, include))
			else:
				for header in node.ant_glob('*.h'):
					headers.append(header.abspath())
		return (list(set(sources)), list(set(headers)))

	def indent(self, tgen, files, env, cleanup=False):
		command = '%s' % Utils.to_list(self.env.INDENT)[0]
		for f in files:
			cmd = '%s %s' % (command, os.path.basename(f))
			cwd = os.path.dirname(f)
			Logs.info("--> indent(%s)" % (f))
			err = self.exec_command(cmd, cwd=cwd, env=env)
			if err:
				self.fatal("indent(%s): failure on '%r', err=%s" % (tgen.name, f, err))
			if cleanup:
				os.remove('%r~' % f)

	def exec_indent(self, tgen, sources, headers):
		'''execute GNU indent on the source and include files of task generator

		:param tgen:        task of which the source code should be beautified
		:type tgen:         waflib.Task.TaskGenerator
		:param sources:     list of source file names
		:type sources:      list(str, str, ..)
		:param headers:     list of include file names
		:type headers:      list(str, str, ..)
		'''
		cleanup = False if self.env.INDENT_CLEAN==[] else True
		command = self.env.INDENT
		if command == []:
			self.fatal('GNU indent not found; please install it and reconfigure')
			return

		env = dict(os.environ)
		pro = self.env.INDENT_PRO
		if pro == []:
			pro = None
		else:
			if not os.path.isabs(pro):
				pro = self.path.find_node(pro)
				if not pro:
					self.fatal('GNU indent profile file not found; please correct it and reconfigure')
				pro = pro.abspath()
			env['INDENT_PROFILE'] = pro

		Logs.info("indent(%s): formatting code" % tgen.name)
		Logs.info("$INDENT_PROFILE = %s" % pro)
		self.indent(tgen, sources, env, cleanup)
		self.indent(tgen, headers, env, cleanup)
		Logs.info("indent(%s): finished" % tgen.name)

