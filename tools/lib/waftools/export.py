#! /usr/bin/env python
# -*- encoding: utf-8 -*-
# Michel Mooij, michel.mooij7@gmail.com

'''
Summary
-------
Provides conversion and export of C/C++ programs, static- and shared libraries
to foreign build system formats (e.g. make) as well as projects and workspaces
for C/C++ integrated development environments (e.g. Eclipse).

Description
-----------
This module can be used to convert and export *waf* project data of C/C++ 
programs, static- and shared libraries into one or more of the following 
formats:
- Makefiles (GNU/MinGW/CygWin),
- CMake makefiles,
- Code::Blocks projects and workspaces,
- Eclipse CDT projects
- Microsoft Visual Studio

Once exported to Make and/or CMake makefiles, all exported (C/C++) tasks can be
build without any further need for, or dependency, to the *waf* build system 
itself. Exporting to these formats can be beneficial when you need to tie your
build environment to some external system, a CI build system for instance, that
is unable to interact with *waf*, or just does a poor job at it. In this case 
you can use *waf* as a kind of templating system, make use of its versatility 
and export new makefiles whenever needed. Note that in such a case the exported
makefiles will merely act as intermediate files that shouldn't be altered
manually; any changes to the build environment needed should made to *wscripts*
within *waf* build system from they have been generated.
Of course one could also use the export as last resort in order to stop using
*waf* as build system altogether and just convert all C/C++ tasks from the *waf*
build environment into Make and/or CMake makefiles.

When exporting C/C++ tasks to integrated developments environments (e.g. 
Eclipse), data will be converted and exported such, that it will reflect the 
structure, relations (dependencies) and environment variables as defined within
the *waf* build system as much as possible. This however will be done such that
the generated project files and workspaces will have the same structure and 
content as one would expect when using these files. When exporting to Eclipse,
for instance, all project files will contain CDT project data; compilation and 
linking will be carried out by the CDT engine itself. In most cases the
exported project files and workspaces for the integrated development 
environements will also contain some special build targets that will allow you
to execute *waf* commands from within those IDE's.

Usage
-----
Tasks can be exported to codeblocks using the *export* command, as shown in the 
example below::

        $ waf export --codeblocks

Exported project files, workspaces and makefiles can be removed in one go using 
the *clean* option::

        $ waf export --clean --codeblocks

Note that only the formats that have been selected will be cleaned; i.e. 
exported files from formats not selected will not be removed.
'''


from waflib import Build, Utils
from waftools import makefile, codeblocks, eclipse, cmake, msdev


def options(opt):
	'''Adds command line options to the *waf* build environment 

	:param opt: Options context from the *waf* build environment.
	:type opt: waflib.Options.OptionsContext
	'''
	opt.add_option('--clean', dest='clean', default=False, action='store_true', help='delete exported files')

	codeblocks.options(opt)
	eclipse.options(opt)
	makefile.options(opt)
	cmake.options(opt)
	msdev.options(opt)


def configure(conf):
	'''Method that will be invoked by *waf* when configuring the build 
	environment.
	
	:param conf: Configuration context from the *waf* build environment.
	:type conf: waflib.Configure.ConfigurationContext
	'''
	codeblocks.configure(conf)
	eclipse.configure(conf)
	makefile.configure(conf)
	cmake.configure(conf)
	msdev.configure(conf)


class ExportContext(Build.BuildContext):
	'''exports and converts C/C++ tasks to external formats (e.g. makefiles, 
	codeblocks, msdev, ...).'''
	cmd = 'export'

	def execute(self, *k, **kw):
		'''Executes the *export* command.

		The export command installs a special task process method
		which enables the collection of tasks being executed (i.e.
		the actual command line being executed). Furthermore it 
		installs a special *post_process* methods that will be called
		when the build has been completed (see build_postfun).

		Note that before executing the *export* command, a *clean* command
		will forced by the *export* command. This is needed in order to
		(re)start the task processing sequence.
		'''
		self.restore()
		if not self.all_envs:
			self.load_envs()
		self.recurse([self.run_dir])
		self.pre_build()

		for group in self.groups:
			for tgen in group:
				try:
					f = tgen.post
				except AttributeError:
					pass
				else:
					f()
		try:
			self.get_tgen_by_name('')
		except Exception:
			pass
		
		self.makefile = True
		if self.options.clean:
			self.do_clean()
		else:
			self.do_export()
		self.timer = Utils.Timer()

	def do_export(self):
		'''Export waf C/C++ build tasks to selected formats (e.g. make).
		'''
		codeblocks.export(self)
		eclipse.export(self)
		makefile.export(self)
		cmake.export(self)
		msdev.export(self)

	def do_clean(self):
		'''Delete exported C/C++ build task for the selected formats.
		'''
		codeblocks.cleanup(self)
		eclipse.cleanup(self)
		makefile.cleanup(self)
		cmake.cleanup(self)
		msdev.cleanup(self)


