#!/usr/bin/env python
# -*- encoding: utf-8 -*-
# Michel Mooij, michel.mooij7@gmail.com

'''
Summary
-------
Exports and converts *waf* project data, for C/C++ programs, static- and shared
libraries, into **Code::Blocks** project files (.cbp) and workspaces 
(codeblock.workspace).
**Code::Blocks** is an open source integrated development environment for C
and C++. It is available for all major Desktop Operating Systems (MS Windows,
all major Linux distributions and Macintosh OS-X).
See http://www.codeblocks.org for a more detailed description on how to install
and use it for your particular Desktop environment.

Description
-----------
When exporting *waf* project data, a single **Code::Blocks** workspace will be
exported in the top level directory of your *WAF* build environment. This 
workspace file will contain references to all exported **Code::Blocks** 
projects and will include dependencies between those projects.

For each single task generator (*waflib.TaskGenerator*), for instance a 
*bld.program(...)* which has been defined within a *wscript* file somewhere in
the build environment, a single **Code::Blocks** project file will be generated
in the same directory as where the task generator has been defined.
The name of this task generator will be used as name for the exported 
**Code::Blocks** project file. If for instance the name of the task generator
is *hello*, then a **Code::Blocks** project file named *hello.cbp* will be 
exported.

Example below presents an overview of an environment in which **Code::Blocks** 
files already have been exported::

        .
        ├── components
        │   └── clib
        │       ├── program
        │       │   ├── cprogram.cbp
        │       │   └── wscript
        │       ├── shared
        │       │   ├── cshlib.cbp
        │       │   └── wscript
        │       └── static
        │           ├── cstlib.cbp
        │           └── wscript
        │
        ├── waf.cbp
        ├── codeblocks.workspace
        └── wscript


Projects will be exported such that they will use the same settings and 
structure as has been defined for that build task within the *waf* build 
environment. Projects will contain exactly **one** build target per build 
variant that has been defined in the *waf* build environment, as explained in
the example below;

    **input**:
    In a *waf* build environment three variants have been defined, one 
    default (without name) build used for normal compiling and linking for the 
    current host, and two variants used for cross compiling and linking for
    embedded systems; one is named *arm5* the other *arm7*.
    Also the *complete* environment has been configured to be build with
    debugging information (i.e. the CFLAGS and CXXFLAGS both contain the 
    compiler option`-g`).
    
    **output**:
    Each exported project will contain the following build targets:
    - The first named **debug**, for the current host platform,
    - The second named **arm5-debug**, for the ARM5 target, and
    - The third named **arm7-debug**, for the ARM7 target. 

Please note that in contrast to a *normal* IDE setup the exported projects 
will contain either a *debug* **or** a *release* build target but not both at
the same time. By doing so exported projects will always use the same settings
(e.g. compiler options, installation paths) as when building the same task in
the *waf* build environment from command line.

Besides these normal projects that will be exported based on the task 
generators that have been defined within the *waf* build environment, a special
**Code::Blocks** project named *waf.cbp* will also be exported. This project 
will be stored in the top level directory of the build environment and will
contain the following build targets per build variant;

* build
* clean
* install
* uninstall

If, for instance, an additional build variant named *arm5* has been defined in 
the *waf* build environment, then the following build targets will be added as
well;

* build_arm5
* clean_arm5
* install_arm5
* uninstall_arm5

Usage
-----
**Code::Blocks** project and workspace files can be exported using the 
*codeblocks* command, as shown in the example below::

    $ waf codeblocks

When needed, exported **Code::Blocks** project- and workspaces files can be 
removed using the *clean* command, as shown in the example below::

    $ waf codeblocks --clean

Once exported simple open the *codeblocks.workspace* using **Code::Blocks**.
This will automatically open all exported projects as well.

Tasks generators to be excluded can be marked with the *skipme* option 
as shown below::

    def build(bld):
        bld.program(name='foo', src='foobar.c', codeblocks_skip=True)

'''

# TODO: contains errors for cross-compilers (WIN32 specific?)

import os
import sys
import copy
import platform
import xml.etree.ElementTree as ElementTree
from xml.dom import minidom
from waflib import Utils, Logs, Errors, Context
from waflib.Build import BuildContext
import waftools
from waftools import deps


def options(opt):
	'''Adds command line options to the *waf* build environment 

	:param opt: Options context from the *waf* build environment.
	:type opt: waflib.Options.OptionsContext
	'''
	opt.add_option('--codeblocks', dest='codeblocks', default=False, action='store_true', help='select codeblocks for export/import actions')
	opt.add_option('--clean', dest='clean', default=False, action='store_true', help='delete exported files')


def configure(conf):
	'''Method that will be invoked by *waf* when configuring the build 
	environment.
	
	:param conf: Configuration context from the *waf* build environment.
	:type conf: waflib.Configure.ConfigurationContext
	'''
	pass


class CodeblocksContext(BuildContext):
	'''export C/C++ tasks to Code::Blocks projects and workspaces.'''
	cmd = 'codeblocks'

	def execute(self):
		'''Will be invoked when issuing the *codeblocks* command.'''
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
		
		self.codeblocks = True
		if self.options.clean:
			cleanup(self)
		else:
			export(self)
		self.timer = Utils.Timer()


def export(bld):
	'''Exports all C and C++ task generators as **Code::Blocks** projects
	and creates a **Code::Blocks** workspace containing references to 
	those project.
	
	:param bld: a *waf* build instance from the top level *wscript*.
	:type bld: waflib.Build.BuildContext
	'''
	if not bld.options.codeblocks and not hasattr(bld, 'codeblocks'):
		return

	cc_name = bld.env.CC_NAME
	workspace = CBWorkspace(bld)
	targets = waftools.deps.get_targets(bld)

	for tgen in bld.task_gen_cache_names.values():
		if targets and tgen.get_name() not in targets:
			continue
		if getattr(tgen, 'codeblocks_skipme', False):
			continue
		if set(('c', 'cxx')) & set(getattr(tgen, 'features', [])):
			project = CBProject(bld, tgen)
			project.export()
			
			(name, fname, deps) = project.get_metadata()
			workspace.add_project(name, fname, deps)
			
			cc_name = project.get_toolchain()

	if cc_name != 'gcc':
		Logs.info("")
		Logs.warn("EXPORT NOTIFICATION")
		Logs.warn("Remember to setup your toolchain in Code::Blocks for these projects!")
		Logs.info("")
		Logs.info("Toolchain name: '%s'" % cc_name)
		Logs.info("CC            : '%s'" % bld.env.CC[0])
		Logs.info("CXX           : '%s'" % bld.env.CXX[0])
		Logs.info("AR            : '%s'" % bld.env.AR)
		Logs.info("")

	project = WafCBProject(bld)
	project.export()
	
	(name, fname, deps) = project.get_metadata()
	workspace.add_project(name, fname, deps)
	workspace.export()


def cleanup(bld):
	'''Removes all **Code::Blocks** projects and workspaces from the 
	*waf* build environment.
	
	:param bld: a *waf* build instance from the top level *wscript*.
	:type bld: waflib.Build.BuildContext
	'''
	if not bld.options.codeblocks and not hasattr(bld, 'codeblocks'):
		return

	targets = waftools.deps.get_targets(bld)

	for tgen in bld.task_gen_cache_names.values():
		if targets and tgen.get_name() not in targets:
			continue
		if getattr(tgen, 'codeblocks_skipme', False):
			continue
		if set(('c', 'cxx')) & set(getattr(tgen, 'features', [])):
			project = CBProject(bld, tgen)
			project.cleanup()

	project = WafCBProject(bld)
	project.cleanup()

	workspace = CBWorkspace(bld)
	workspace.cleanup()


class CodeBlocks(object):
	'''Abstract base class used for exporting *waf* project data to 
	**Code::Blocks** projects and workspaces.

	:param bld: Build context as used in *wscript* files of your *waf* build
	            environment.
	:type bld:  waflib.Build.BuildContext
	'''

	PROGRAM = '1'
	'''Identifier for projects containing an executable'''

	STLIB   = '2'
	'''Identifier for projects containing a static library'''

	SHLIB   = '3'
	'''Identifier for projects containing a shared library'''
	
	OBJECT  = '4'
	'''Identifier for projects for building objects only'''

	def __init__(self, bld):
		self.bld = bld

	def export(self):
		'''Exports a **Code::Blocks** workspace or project.'''
		content = self._get_content()
		if not content:
			return
		content = self._xml_clean(content)

		node = self._make_node()
		if not node:
			return
		node.write(content)
		Logs.pprint('YELLOW', 'exported: %s' % node.abspath())

	def cleanup(self):
		'''Deletes a **Code::Blocks** workspace or project file including 
		.layout and .depend files.
		'''
		cwd = self._get_cwd()
		for node in cwd.ant_glob('*.layout'):
			node.delete()
		for node in cwd.ant_glob('*.depend'):
			node.delete()
		node = self._find_node()
		if node:
			node.delete()
			Logs.pprint('YELLOW', 'removed: %s' % node.abspath())

	def _get_cwd(self):
		cwd = os.path.dirname(self._get_fname())
		if cwd == "":
			cwd = "."
		return self.bld.srcnode.find_node(cwd)

	def _find_node(self):
		name = self._get_fname()
		if not name:
			return None    
		return self.bld.srcnode.find_node(name)

	def _make_node(self):
		name = self._get_fname()
		if not name:
			return None    
		return self.bld.srcnode.make_node(name)

	def _get_fname(self):
		'''<abstract> Returns file name.'''
		return None

	def _get_content(self):
		'''<abstract> Returns file content.'''
		return None

	def _xml_clean(self, content):
		s = minidom.parseString(content).toprettyxml(indent="\t")
		lines = [l for l in s.splitlines() if not l.isspace() and len(l)]
		lines[0] = '<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>'
		return '\n'.join(lines)


class CBWorkspace(CodeBlocks):
	'''Class used for exporting *waf* project data to a **Code::Blocks** 
	workspace located in the lop level directory of the *waf* build
	environment.

	:param bld: Build context as used in *wscript* files of your *waf* build
	            environment.
	:type bld:  waflib.Build.BuildContext
	'''
	
	def __init__(self, bld):
		super(CBWorkspace, self).__init__(bld)
		self.projects = {}

	def _get_fname(self):
		'''Returns the workspace's file name.'''
		return 'codeblocks.workspace'

	def _get_content(self):
		'''returns the content of a code::blocks workspace file containing references to all 
		projects and project dependencies.
		'''
		root = ElementTree.fromstring(CODEBLOCKS_WORKSPACE)
		workspace = root.find('Workspace')
		workspace.set('title', getattr(Context.g_module, Context.APPNAME))

		for name in sorted(self.projects.keys()):
			(fname, deps) = self.projects[name]
			project = ElementTree.SubElement(workspace, 'Project', attrib={'filename':fname})
			for dep in deps:
				try:
					(fname, _) = self.projects[dep]
				except KeyError:
					pass
				else:
					ElementTree.SubElement(project, 'Depends', attrib={'filename':fname})
		return ElementTree.tostring(root)

	def add_project(self, name, fname, deps):
		'''Adds a project to the workspace.
		
		:param name:    Name of the project.
		:type name:     str
		:param fname:   Complete path to the project file
		:type fname:    str
		:param deps:    List of names on which this project depends
		:type deps:     list of str
		'''
		self.projects[name] = (fname, deps)


class CBProject(CodeBlocks):
	'''Class used for exporting *waf* project data to **Code::Blocks** 
	projects.

	:param bld: Build context as used in *wscript* files of your *waf* build
	            environment.
	:type bld:  waflib.Build.BuildContext
	
	:param gen: Task generator that contains all information of the task to be
	            converted and exported to the **Code::Blocks** project.
	:type gen:  waflib.Task.TaskGen
	'''

	def __init__(self, bld, gen):
		super(CBProject, self).__init__(bld)
		self.gen = gen

	def _get_fname(self):
		'''Returns the project's file name.'''
		gen = self.gen
		return '%s/%s.cbp' % (gen.path.relpath().replace('\\', '/'), gen.get_name())

	def _get_root(self):
		'''Returns a document root, either from an existing file, or from template.
		'''
		fname = self._get_fname()
		if os.path.exists(fname):
			tree = ElementTree.parse(fname)
			root = tree.getroot()
		else:
			root = ElementTree.fromstring(CODEBLOCKS_PROJECT)
		return root

	def _get_target(self, project, toolchain):
		'''Returns a targets for the requested toolchain name.

		If the target doesn't exist in the project it will be added.
		'''
		build = project.find('Build')
		for target in build.iter('Target'):
			for option in target.iter('Option'):
				if option.get('compiler') in [toolchain, 'XXX']:
					return target

		target = copy.deepcopy(build.find('Target'))
		build.append(target)
		return target

	def _get_content(self):
		'''Returns the content of a project file.'''
		root = self._get_root()
		project = root.find('Project')
		for option in project.iter('Option'):
			if option.get('title'):
				option.set('title', self.gen.get_name())
		
		toolchain = self.get_toolchain()
		target = self._get_target(project, toolchain)
		target.set('title', self._get_target_title())
		target_type = self._get_target_type()

		for option in target.iter('Option'):
			if option.get('output'):
				option.set('output', self._get_output())
			elif option.get('object_output'):
				option.set('object_output', self._get_object_output())
			elif option.get('type'):
				option.set('type', target_type)
			elif option.get('compiler'):
				option.set('compiler', toolchain)
			elif option.get('working_dir'):
				if target_type == CodeBlocks.PROGRAM:
					option.set('working_dir', self._get_working_directory())

		compiler = target.find('Compiler')
		compiler.clear()
		for option in self._get_compiler_options():
			ElementTree.SubElement(compiler, 'Add', attrib={'option':option})
		for define in self._get_compiler_defines():
			ElementTree.SubElement(compiler, 'Add', attrib={'option':'-D%s' % define})
		for include in self._get_compiler_includes():
			ElementTree.SubElement(compiler, 'Add', attrib={'directory':include})

		linker = target.find('Linker')
		linker.clear()
		for option in self._get_link_options():
			ElementTree.SubElement(linker, 'Add', attrib={'option':option})
		for library in self._get_link_libs():
			ElementTree.SubElement(linker, 'Add', attrib={'library':library})
		for directory in self._get_link_paths():
			ElementTree.SubElement(linker, 'Add', attrib={'directory':directory})
		
		sources = self._get_genlist(self.gen, 'source')
		for unit in project.iter('Unit'):
			source = unit.get('filename')
			if source in sources:
				sources.remove(source)
		for source in sources:
			ElementTree.SubElement(project, 'Unit', attrib={'filename':source})
		
		includes = self._get_includes_files()
		for unit in project.iter('Unit'):
			include = unit.get('filename')
			if include in includes:
				includes.remove(include)
		for include in includes:
			ElementTree.SubElement(project, 'Unit', attrib={'filename':include})
		
		return ElementTree.tostring(root)

	def get_metadata(self):
		'''Returns a tuple containing project information (name, file name and 
		dependencies).
		'''
		gen = self.gen
		name = gen.get_name()
		fname = self._get_fname()
		deps = Utils.to_list(getattr(gen, 'use', []))
		return (name, fname, deps)

	def get_toolchain(self):
		'''Returns the name of the compiler toolchain.
		'''
		if self.gen.env.DEST_OS == 'win32':
			if sys.platform == 'win32':
				toolchain = 'gcc'
			else:
				toolchain = 'mingw32'

		elif self.gen.env.DEST_CPU != platform.processor():
			bld = self.gen.bld
			if bld.variant:
				toolchain = 'gcc_%s' % bld.variant
			else:
				toolchain = 'gcc_%s' % self.gen.env.DEST_CPU

		else:
			toolchain = 'gcc'

		return toolchain

	def _get_target_title(self):
		bld = self.gen.bld
		env = self.gen.env

		if bld.variant:
			title = '%s ' % (bld.variant)
		elif env.DEST_OS in sys.platform \
				and env.DEST_CPU == platform.processor():
			title = ''
		else:
			title = '%s-%s' % (env.DEST_OS, env.DEST_CPU)

		if '-g' in env.CFLAGS or '-g' in env.CXXFLAGS:
			title += 'debug'
		else:
			title += 'release'

		return title.title()

	def _get_buildpath(self):
		bld = self.bld
		gen = self.gen
		pth = '%s/%s' % (bld.path.get_bld().path_from(gen.path), gen.path.relpath())
		return pth.replace('\\', '/')

	def _get_output(self):
		return '%s/%s' % (self._get_buildpath(), self.gen.target)

	def _get_object_output(self):
		return self._get_buildpath()

	def _get_working_directory(self):
		gen = self.gen
		bld = self.bld

		sdir = gen.bld.env.BINDIR
		if sdir.startswith(bld.path.abspath()):
			sdir = os.path.relpath(sdir, gen.path.abspath())

		return sdir.replace('\\', '/')

	def _get_target_type(self):
		gen = self.gen
		if set(('cprogram', 'cxxprogram')) & set(gen.features):
			return '1'
		elif set(('cstlib', 'cxxstlib')) & set(gen.features):
			return '2'
		elif set(('cshlib', 'cxxshlib')) & set(gen.features):
			return '3'
		else:
			return '4'

	def _get_genlist(self, gen, name):
		lst = Utils.to_list(getattr(gen, name, []))
		lst = [str(l.path_from(gen.path)) if hasattr(l, 'path_from') else l for l in lst]
		return [l.replace('\\', '/') for l in lst]

	def _get_compiler_options(self):
		bld = self.bld
		gen = self.gen
		if 'cxx' in gen.features:
			flags = getattr(gen, 'cxxflags', []) + bld.env.CXXFLAGS
		else:
			flags = getattr(gen, 'cflags', []) + bld.env.CFLAGS

		if 'cshlib' in gen.features:
			flags.extend(bld.env.CFLAGS_cshlib)
		elif 'cxxshlib' in gen.features:
			flags.extend(bld.env.CXXFLAGS_cxxshlib)
		return list(set(flags))

	def _get_deps(self, gen):
		bld = self.bld
		deps = Utils.to_list(getattr(gen, 'use', []))
		names = deps[:]
		for name in names:
			try:
				child = bld.get_tgen_by_name(name)
			except Errors.WafError:
				pass
			else:
				deps += self._get_deps(child)
		return deps
		
	def _get_compiler_includes(self):
		bld = self.bld
		gen = self.gen
		includes = self._get_genlist(gen, 'includes')
		deps = self._get_deps(gen)
		for dep in deps:
			key = 'INCLUDES_%s' % dep
			for path in gen.env[key]:
				includes.append(bld.root.find_node(path).path_from(gen.path).replace('\\', '/'))
		return includes

	def _get_compiler_defines(self):
		gen = self.gen
		defines = self._get_genlist(gen, 'defines') + gen.bld.env.DEFINES
		if 'win32' in sys.platform:
			defines = [d.replace('"', '\\"') for d in defines]
		else:
			defines = [d.replace('"', '\\\\"') for d in defines]
		return defines

	def _get_link_options(self):
		bld = self.bld
		gen = self.gen
		flags = getattr(gen, 'linkflags', []) + bld.env.LINKFLAGS

		libs = self._get_link_libs()
		if 'pthread' in libs:
			flags.append('-pthread')

		if 'cshlib' in gen.features:
			flags.extend(bld.env.LINKFLAGS_cshlib)
		elif 'cxxshlib' in gen.features:
			flags.extend(bld.env.LINKFLAGS_cxxshlib)
		return list(set(flags))

	def _get_link_libs(self):
		bld = self.bld
		gen = self.gen
		libs = Utils.to_list(getattr(gen, 'lib', []))
		deps = Utils.to_list(getattr(gen, 'use', []))
		for dep in deps:
			try:
				tgen = bld.get_tgen_by_name(dep)
			except Errors.WafError:
				pass
			else:
				if set(('cstlib', 'cshlib', 'cxxstlib', 'cxxshlib')) & set(tgen.features):
					libs.append(dep)
		return libs
	
	def _get_link_paths(self):
		bld = self.bld
		gen = self.gen
		dirs = []
		deps = Utils.to_list(getattr(gen, 'use', []))
		for dep in deps:
			try:
				tgen = bld.get_tgen_by_name(dep)
			except Errors.WafError:
				key = 'LIBPATH_%s' % dep
				for path in gen.env[key]:
					dirs.append(bld.root.find_node(path).path_from(gen.path).replace('\\', '/'))
			else:
				if set(('cstlib', 'cshlib', 'cxxstlib', 'cxxshlib')) & set(tgen.features):
					directory = tgen.path.get_bld().path_from(gen.path)
					dirs.append(directory.replace('\\', '/'))
		return dirs

	def _get_includes_files(self):
		gen = self.gen
		includes = []
		for include in self._get_genlist(gen, 'includes'):
			node = gen.path.find_dir(include)
			if node:
				for include in node.ant_glob('*.h*'):
					includes.append(include.path_from(gen.path).replace('\\', '/'))
		return includes


class WafCBProject(CodeBlocks):
	'''Class used for creating a dummy **Code::Blocks** project containing
	only *waf* commands as pre build steps.

	:param bld: Build context as used in *wscript* files of your *waf* build
				environment.
	:type bld:	waflib.Build.BuildContext
	'''

	def __init__(self, bld):
		super(WafCBProject, self).__init__(bld)
		self.title = 'waf'

	def _get_fname(self):
		'''Returns the file name.'''
		return 'waf.cbp'

	def _get_root(self):
		'''Returns a document root, either from an existing file, or from template.
		'''
		fname = self._get_fname()
		if os.path.exists(fname):
			tree = ElementTree.parse(fname)
			root = tree.getroot()
		else:
			root = ElementTree.fromstring(CODEBLOCKS_PROJECT)
		return root

	def _get_cmd(self, name):
		'''Returns a string containing command and arguments to be executed.
		'''
		if 'win32' in sys.platform:
			cmd = 'python %s %s' % (str(sys.argv[0]).replace('\\', '/'), name)
		else:
			cmd = 'waf %s' % name
		return cmd
		
	def _init_target(self, target, name):
		'''Initializes a WAF build target.'''
		target.set('title', name)

		for option in target.iter('Option'):
			if option.get('output'):
				option.set('output', '')
			elif option.get('object_output'):
				option.set('object_output', '')
			elif option.get('compiler'):
				option.set('compiler', 'gcc')

		cmd = target.find('ExtraCommands/Add')
		cmd.set('before', self._get_cmd(name))
		return target

	def _add_target(self, project, name):
		'''Adds a WAF build target with given name.

		Will only be added if target does not exist yet.
		'''
		build = project.find('Build')
		for target in build.iter('Target'):
			if target.get('title') == 'XXX':
				commands = ElementTree.SubElement(target, 'ExtraCommands')
				ElementTree.SubElement(commands, 'Add', {'before': 'XXX'})
				return self._init_target(target, name)

			if target.get('title') == name:
				return self._init_target(target, name)

		target = copy.deepcopy(build.find('Target'))
		build.append(target)
		return self._init_target(target, name)

	def _get_content(self):
		'''Returns the content of a code::blocks project file.
		'''
		root = self._get_root()
		project = root.find('Project')
		for option in project.iter('Option'):
			if option.get('title'):
				option.set('title', self.title)

		bld = self.bld
		targets = ['build', 'clean', 'install', 'uninstall']
		if bld.variant:
			targets = ['%s_%s' % (t, bld.variant) for t in targets]

		for target in project.iter('Build/Target'):
			name = target.get('title')
			if name in targets:
				targets.remove(name)

		for target in targets:
			self._add_target(project, target)

		return ElementTree.tostring(root)

	def get_metadata(self):
		'''Returns a tuple containing project information (name, file name and 
		dependencies).
		'''
		name = self.title
		fname = self._get_fname()
		deps = []
		return (name, fname, deps)


CODEBLOCKS_WORKSPACE = \
'''<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>
<CodeBlocks_workspace_file>
    <Workspace title="Workspace">
    </Workspace>
</CodeBlocks_workspace_file>
'''

CODEBLOCKS_PROJECT = \
'''<?xml version="1.0" encoding="UTF-8" standalone="yes" ?>
<CodeBlocks_project_file>
    <FileVersion major="1" minor="6" />
    <Project>
        <Option title="XXX" />
        <Option pch_mode="2" />
        <Option compiler="gcc" />
        <Build>
            <Target title="XXX">
                <Option output="XXX" prefix_auto="1" extension_auto="1" />
                <Option object_output="XXX" />
                <Option working_dir="."/>
                <Option type="2" />
                <Option compiler="XXX" />
                <Compiler/>
                <Linker/>
            </Target>
        </Build>
        <Extensions>
            <code_completion />
            <envvars />
            <debugger />
            <lib_finder disable_auto="1" />
        </Extensions>
    </Project>
</CodeBlocks_project_file>
'''

