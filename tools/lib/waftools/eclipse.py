#!/usr/bin/env python
# -*- encoding: utf-8 -*-
# Michel Mooij, michel.mooij7@gmail.com

'''
Summary
-------
Exports and converts *waf* project data, for C/C++ programs, static- and shared
libraries, into **Eclipse** *CDT* project files (.projects, .cproject).
**Eclipse** is an open source integrated development environment, which can be 
used, amongst others, for development of C/C++ programs. 

See https://www.eclipse.org and https://www.eclipse.org/cdt for a more detailed 
description on how to install and use it for your particular Desktop environment.

.. note::
    Export of C/C++ projects are supported for GCC/MinGW compilers only, 
    other compilers and/or environments (e.g. MSVC, CygWin) are NOT supported!

Description
-----------
For each single C/C++ task generator (*waflib.TaskGenerator*), for instance a 
*bld.program(...)* which has been defined within a *wscript* file somewhere in
the build environment, a single **Eclipse** *CDT* project file will be generated
in the same directory as where the task generator has been defined. Once generated
these projects can be imported into **Eclipse** using 'Import -> General ->
Existing Projects Into Workspace'.

Example below presents an overview of an environment in which **Eclipse** 
projects already have been exported::

        .
        ├── components
        │   └── clib
        │       ├── program
        │       │   ├── .project
        │       │   ├── .cproject
        │       │   └── wscript
        │       ├── shared
        │       │   ├── .project
        │       │   ├── .cproject
        │       │   └── wscript
        │       └── static
        │           ├── .project
        │           ├── .cproject
        │           └── wscript
        │
        └── wscript


.. warning:: 
    Export of multiple C/C++ build tasks (program, stlib and/or shlib) 
    which have been defined within the same directory is not supported.

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

Usage
-----
**Eclipse** project and workspace files can be exported using the *eclipse* 
command, as shown in the example below::

    $ waf eclipse

When needed, exported **Eclipse** project- and workspaces files can be 
removed using the *clean* command, as shown in the example below::

    $ waf eclipse --clean
        

Tasks generators to be excluded can be marked with the *skipme* option 
as shown below::

    def build(bld):
        bld.program(name='foo',	src='foobar.c',	eclipse_skip=True)

'''


import sys
import os
import codecs
import xml.etree.ElementTree as ElementTree
from xml.dom import minidom
from waflib import Utils, Logs, Errors, Context
from waflib.Build import BuildContext
import waftools

def options(opt):
	'''Adds command line options to the *waf* build environment 

	:param opt: Options context from the *waf* build environment.
	:type opt: waflib.Options.OptionsContext
	'''
	opt.add_option('--eclipse', dest='eclipse', default=False, action='store_true', help='select Eclipse for export/import actions')
	opt.add_option('--clean', dest='clean', default=False, action='store_true', help='delete exported files')


def configure(conf):
	'''Method that will be invoked by *waf* when configuring the build 
	environment.
	
	:param conf: Configuration context from the *waf* build environment.
	:type conf: waflib.Configure.ConfigurationContext
	'''
	pass


class EclipseContext(BuildContext):
	'''export C/C++ tasks to Eclipse CDT projects.'''
	cmd = 'eclipse'

	def execute(self):
		'''Will be invoked when issuing the *eclipse* command.'''
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
		
		self.eclipse = True
		if self.options.clean:
			cleanup(self)
		else:
			export(self)
		self.timer = Utils.Timer()


def export(bld):
	'''Generates Eclipse CDT projects for each C/C++ task.

	Also generates a top level Eclipse PyDev project
	for the WAF build environment itself.
	Warns when multiple task have been defined in the same,
	or top level, directory.

	:param bld: a *waf* build instance from the top level *wscript*.
	:type bld: waflib.Build.BuildContext
	'''
	if not bld.options.eclipse and not hasattr(bld, 'eclipse'):
		return

	targets = waftools.deps.get_targets(bld)
	detect_project_duplicates(bld, targets)

	for tgen in bld.task_gen_cache_names.values():
		if targets and tgen.get_name() not in targets:
			continue
		if getattr(tgen, 'eclipse_skipme', False):
			continue
		if set(('c', 'cxx')) & set(getattr(tgen, 'features', [])):
			Project(bld, tgen).export()
			CDTProject(bld, tgen).export()


def cleanup(bld):
	'''Removes all generated Eclipse project- and launcher files

	:param bld: a *waf* build instance from the top level *wscript*.
	:type bld: waflib.Build.BuildContext
	'''
	if not bld.options.eclipse and not hasattr(bld, 'eclipse'):
		return

	targets = waftools.deps.get_targets(bld)

	for tgen in bld.task_gen_cache_names.values():
		if targets and tgen.get_name() not in targets:
			continue
		if getattr(tgen, 'eclipse_skipme', False):
			continue
		if set(('c', 'cxx')) & set(getattr(tgen, 'features', [])):
			Project(bld, tgen).cleanup()
			CDTProject(bld, tgen).cleanup()


def detect_project_duplicates(bld, targets):
	'''Warns when multiple TaskGen's have been defined in the same directory.

	Since Eclipse works with static project filenames, only one project per
	directory can be created. If multiple task generators have been defined
	in the same directory (i.e. same wscript) one will overwrite the other(s).
	This problem can only e circumvented by changing the structure of the 
	build environment; i.e. place each single task generator in a seperate 
	directory.
	'''
	locations = { '.': 'waf (top level)' }
	anomalies = {}

	for tgen in bld.task_gen_cache_names.values():
		if targets and tgen.get_name() not in targets:
			continue
		
		name = tgen.get_name()
		location = str(tgen.path.relpath()).replace('\\', '/')
		
		if location in locations:
			anomalies[name] = location
		else:
			locations[location] = name

	cnt = len(anomalies.keys())
	if cnt != 0:
		Logs.info('')
		Logs.warn('WARNING ECLIPSE EXPORT: TASK LOCATION CONFLICTS(%s)' % cnt)
		Logs.info('Failed to create project files for:')
		s = ' {n:<15} {l:<40}'
		Logs.info(s.format(n='(name)', l='(location)'))
		for (name, location) in anomalies.items():
			Logs.info(s.format(n=name, l=location))
		Logs.info('')
		Logs.info('TIPS:')
		Logs.info('- use one task per directory/wscript.')
		Logs.info('- don\'t place tasks in the top level directory/wscript.')
		Logs.info('')


class EclipseProject(object):
	'''Abstract class for exporting *Eclipse* project files.

	:param bld: a *waf* build instance from the top level *wscript*.
	:type bld: waflib.Build.BuildContext

	:param tgen: Task generator that contains all information of the task to be
	             converted and exported to the *Eclipse* project.
	:type tgen: waflib.Task.TaskGen
	'''
	def __init__(self, bld, tgen, fname, template):
		self.bld = bld
		self.tgen = tgen
		self.fname = fname
		self.template = template
		self.appname = getattr(Context.g_module, Context.APPNAME)

	def export(self):
		content = self.xml_clean(self.get_content())
		node = self.make_node()
		node.write(content)
		Logs.pprint('YELLOW', 'exported: %s' % node.abspath())

	def cleanup(self):
		node = self.find_node()
		if node:
			node.delete()
			Logs.pprint('YELLOW', 'removed: %s' % node.abspath())

	def find_node(self):
		name = self.get_fname()   
		return self.bld.srcnode.find_node(name)

	def make_node(self):
		name = self.get_fname()   
		return self.bld.srcnode.make_node(name)

	def get_fname(self):
		'''returns file name including relative path.'''
		return '%s/%s' % (self.tgen.path.relpath().replace('\\', '/'), self.fname)

	def get_name(self):
		'''returns functional name of task generator.'''
		return self.tgen.get_name()

	def xml_clean(self, content):
		s = minidom.parseString(content).toprettyxml(indent="\t")
		lines = [l for l in s.splitlines() if not l.isspace() and len(l)]
		lines = self.comments + lines[1:] + ['']
		return '\n'.join(lines)

	def get_root(self):
		'''get XML root from template or file.'''
		fname = self.get_fname()
		if os.path.exists(fname):
			tree = ElementTree.parse(fname)
			root = tree.getroot()
		else:
			root = ElementTree.fromstring(self.template)
		return root

	def get_content(self):
		'''Abstract, to be defined in concrete classes:
		returns XML file contents as string.
		'''
		return None


class Project(EclipseProject):
	'''Class for exporting *Eclipse* '.project' files.

	:param bld: a *waf* build instance from the top level *wscript*.
	:type bld: waflib.Build.BuildContext

	:param tgen: Task generator that contains all information of the task to be
	             converted and exported to the *Eclipse* project.
	:type tgen: waflib.Task.TaskGen
	'''
	def __init__(self, bld, tgen):
		super(Project, self).__init__(bld, tgen, '.project', ECLIPSE_PROJECT)
		self.comments = ['<?xml version="1.0" encoding="UTF-8"?>']

	def get_content(self):
		root = self.get_root()
		root.find('name').text = self.get_name()
		self.add_projects(root)
		self.add_natures(root)
		return ElementTree.tostring(root)

	def add_projects(self, root):
		projects = root.find('projects')
		uses = list(Utils.to_list(getattr(self.tgen, 'use', [])))
		for project in projects.findall('project'):
			if project.text in uses: uses.remove(project.text)
		for use in uses:
			ElementTree.SubElement(projects, 'project').text = use

	def add_natures(self, root):
		if 'cxx' not in self.tgen.features:
			return
		natures = root.find('natures')
		ccnature = 'org.eclipse.cdt.core.ccnature'
		for nature in natures.findall('nature'):
			if nature.text == ccnature:
				return
		element = ElementTree.SubElement(natures, 'nature')
		element.text = ccnature


class CDTProject(EclipseProject):
	'''Class for exporting C/C++ task generators to an *Eclipse* *CDT* 
	project.
	When exporting this class exports three files associated with C/C++
	projects::
	
		.project
		.cproject
		target_name.launch

	The first file mostly contains perspective, the second contains the actual
	C/C++ project while the latter is a launcher which can be import into
	*Eclipse* and used to run and/or debug C/C++ programs. 
	
	:param bld: a *waf* build instance from the top level *wscript*.
	:type bld: waflib.Build.BuildContext

	:param tgen: Task generator that contains all information of the task to be
	             converted and exported to the *Eclipse* project.
	:type tgen: waflib.Task.TaskGen
	
	:param project: Reference to *Eclipse* project (which will export the 
	                *.project* file.
	:param project: Project
	'''
	def __init__(self, bld, tgen):
		super(CDTProject, self).__init__(bld, tgen, '.cproject', ECLIPSE_CDT_PROJECT)
		
		if not set(('c', 'cxx')) & set(tgen.features):
			bld.fatal("ERROR: '%s' is not a C/C++ build task" % tgen.get_name())
		
		d = tgen.env.DEST_OS
		c = tgen.env.DEST_CPU
		l = 'cpp' if 'cxx' in tgen.features else 'c'
		b = 'debug' if '-g' in tgen.env.CFLAGS else 'release'

		self.cross = not sys.platform.startswith(d) or c not in ('x86_64', 'x86', 'ia') # TODO: assuming host is intel based only!!!
		self.comments = ['<?xml version="1.0" encoding="UTF-8" standalone="no"?>','<?fileVersion 4.0.0?>']
		self.language = l
		self.cdt = {}
		if set(('cprogram', 'cxxprogram')) & set(tgen.features):
			self.cdt['ext'] = 'exe'
			self.cdt['kind'] = 'Executable'
			self.cdt['buildArtefactType'] = 'org.eclipse.cdt.build.core.buildArtefactType.exe'
			self.cdt['artifactExtension'] = 'exe' if d=='win32' else ''

		elif set(('cshlib', 'cxxshlib')) & set(tgen.features):
			self.cdt['ext'] = 'so'
			self.cdt['kind'] = 'Shared Library'
			self.cdt['buildArtefactType'] = 'org.eclipse.cdt.build.core.buildArtefactType.sharedLib'
			self.cdt['artifactExtension'] = 'dll' if d=='win32' else 'so'

		else: # cstlib, cxxstlib or plain objects
			self.cdt['ext'] = 'lib'
			self.cdt['kind'] = 'Static Library'
			self.cdt['buildArtefactType'] = 'org.eclipse.cdt.build.core.buildArtefactType.staticLib'
			self.cdt['artifactExtension'] = 'a'

		e = self.cdt['ext']
		ar = Utils.to_list(tgen.env.AR)[0]
		ar = os.path.splitext(os.path.basename(ar))[0]
		cc = tgen.env.CC[0]
		self.cdt['c'] = os.path.splitext(os.path.basename(cc))[0]
		self.cdt['cpp'] = os.path.splitext(os.path.basename(tgen.env.CXX[0]))[0]
		self.cdt['ar'] = ar
		self.cdt['as'] = ar.replace('ar', 'as')
		self.cdt['cc_path'] = os.path.dirname(cc).replace('\\', '/')

		if sys.platform=='win32':
			gnu = 'gnu.mingw'
			gnut = 'mingw'
		elif self.cross:
			gnu = 'gnu.cross'
			gnut = 'cross'
		else:
			gnu = 'gnu'
			gnut = None

		self.cdt['gnut'] = gnut
		self.cdt['gnu'] = gnu
		self.cdt['build'] = b
		self.cdt['parent'] = 'cdt.managedbuild.config.%s.%s.%s' % (gnu, self.cdt['ext'], b)
		self.cdt['instance'] = '%s.%s' % (self.cdt['parent'], self.get_uuid())
		self.cdt['name'] = '%s%s' % (b, '-%s' % bld.variant if bld.variant else '')

		s = 'cdt.managedbuild.tool.gnu.%s.compiler.%s%s.%s' % (l, '%s.' % gnut if gnut else '', e, b)
		self.cdt['compiler'] = '%s.%s' % (s, self.get_uuid())
		self.cdt['input'] = 'cdt.managedbuild.tool.gnu.%s.compiler.input.%s' % (l, self.get_uuid())

		t = '%s.%s' % (e, b)
		tt = '%s.%s' % ('.%s' % gnut if gnut else '', t)
		self.cdt['toolchain'] = 'cdt.managedbuild.toolchain.%s.%s' % (gnu, t)
		self.cdt['platform'] = 'cdt.managedbuild.target.gnu.platform%s' % (tt)
		self.cdt['compiler_option'] = 'gnu.{0}.compiler.option.{1}.level'
		self.cdt['archiver'] = 'cdt.managedbuild.tool.gnu.archiver%s.base' % ('.mingw' if sys.platform=='win32' else '')
		
		if self.cross and sys.platform!='win32':
			self.cdt['c_compiler'] = 'cdt.managedbuild.tool.gnu.cross.c.compiler'
			self.cdt['cpp_compiler'] = 'cdt.managedbuild.tool.gnu.cross.cpp.compiler'
			self.cdt['c_linker'] = 'cdt.managedbuild.tool.gnu.cross.c.linker'
			self.cdt['cpp_linker'] = 'cdt.managedbuild.tool.gnu.cross.cpp.linker'
			self.cdt['assembler'] = 'cdt.managedbuild.tool.gnu.cross.assembler'
		else:
			self.cdt['c_compiler'] = 'cdt.managedbuild.tool.gnu.c.compiler%s' % (tt)
			self.cdt['cpp_compiler'] = 'cdt.managedbuild.tool.gnu.cpp.compiler%s' % (tt)
			self.cdt['c_linker'] = 'cdt.managedbuild.tool.gnu.c.linker%s' % (tt)
			self.cdt['cpp_linker'] = 'cdt.managedbuild.tool.gnu.cpp.linker%s' % (tt)
			self.cdt['assembler'] = 'cdt.managedbuild.tool.gnu.assembler%s' % (tt)

		if self.cdt['kind'] == 'Executable':
			self.launch = CDTLaunch(bld, tgen, self.cdt['instance'], self.cdt['build'], self.cdt['artifactExtension'])
			
	def get_uuid(self):
		uuid = codecs.encode(os.urandom(4), 'hex_codec')
		return int(uuid, 16)

	def get_content(self):
		root = self.get_root()

		for module in root.findall('storageModule'):
			if module.get('moduleId') == 'org.eclipse.cdt.core.settings':
				self.update_cdt_core_settings(module)
			if module.get('moduleId') == 'cdtBuildSystem':
				self.update_buildsystem(module)
			if module.get('moduleId') == 'refreshScope':
				self.update_refreshscope(module)

		for module in root.findall('storageModule'):
			if module.get('moduleId') == 'scannerConfiguration':
				self.update_scanner_configuration(module)
		return ElementTree.tostring(root)

	def export(self):
		super(CDTProject, self).export()
		if hasattr(self, 'launch'):
			self.launch.export()

	def cleanup(self):
		super(CDTProject, self).cleanup()
		if hasattr(self, 'launch'):
			self.launch.cleanup()

	def update_cdt_core_settings(self, module):
		cconfig = self.cconfig_get(module)
		if cconfig is None:
			cconfig = ElementTree.fromstring(ECLIPSE_CDT_CCONFIGURATION)
			module.append(cconfig)
		self.cconfig_update(cconfig)

	def update_buildsystem(self, module):
		project = module.find('project')
		if project == None:
			project = ElementTree.SubElement(module, 'project')
		ptype = 'cdt.managedbuild.target.%s.%s' % (self.cdt['gnu'], self.cdt['ext'])

		project.set('id', '%s.%s.%s' % (self.tgen.get_name(), ptype, self.get_uuid()))
		project.set('name', self.cdt['kind'])
		project.set('projectType', ptype)

	def update_refreshscope(self, module):
		name = self.cdt['name']
		for configuration in module.findall('configuration'):
			if configuration.get('configurationName') == name:
				return
		configuration = ElementTree.SubElement(module, 'configuration')
		configuration.set('configurationName', name)
		resource = ElementTree.SubElement(configuration, 'resource')
		resource.set('resourceType', 'PROJECT')
		resource.set('workspacePath', '/%s' % self.tgen.get_name())

	def update_scanner_configuration(self, module):
		i = self.cdt['instance']
		c = self.cdt['compiler']
		f = self.cdt['input']
		scanner = ElementTree.SubElement(module, 'scannerConfigBuildInfo')
		scanner.set('instanceId', '%s;%s.;%s;%s' % (i, i, c, f))
		ElementTree.SubElement(scanner, 'autodiscovery', {'enabled': 'true', 'problemReportingEnabled' : 'true', 'selectedProfileId' : ''})

	def cconfig_get(self, module):
		'''Returns configuration module'''
		for cconfig in module.findall('cconfiguration'):
			if cconfig.get('id') and cconfig.get('id').startswith(self.cdt['parent']):
				for storage in cconfig.findall('storageModule'):
					if storage.get('moduleId') == 'org.eclipse.cdt.core.settings':
						if storage.get('name') == self.cdt['name']:
							return cconfig
		return None

	def cconfig_update(self, cconfig):
		'''Update configuration module.'''
		cconfig.set('id', self.cdt['instance'])
		for storage in cconfig.findall('storageModule'):
			if storage.get('moduleId') == 'org.eclipse.cdt.core.settings':
				self.cconfig_settings_update(storage)
			if storage.get('moduleId') == 'cdtBuildSystem':
				self.cconfig_buildsystem_update(storage)				
			if storage.get('moduleId') == 'org.eclipse.cdt.core.externalSettings':
				self.cconfig_external_settings_update(storage)

	def cconfig_settings_update(self, storage):
		storage.set('name', self.cdt['name'])
		storage.set('id', self.cdt['instance'])

		settings = storage.find('externalSettings')
		if self.cdt['ext'] == 'exe':
			settings.clear()
		else:
			name = self.tgen.get_name()
			for entry in settings.iter('entry'):
				if entry.get('kind') == 'includePath':
					entry.set('name', '/%s' % name)
				if entry.get('kind') == 'libraryPath':
					entry.set('name', '/%s/%s' % (name, self.cdt['name']))
				if entry.get('kind') == 'libraryFile':
					entry.set('name', '%s' % name)

	def cconfig_buildsystem_update(self, storage):
		config = storage.find('configuration')
		config.set('name', self.cdt['name'])
		config.set('buildArtefactType', self.cdt['buildArtefactType'])
		if 'artifactExtension' in self.cdt:
			config.set('artifactExtension', self.cdt['artifactExtension'])
		config.set('parent', self.cdt['parent'])
		config.set('id', self.cdt['instance'])

		prop = '{0}.{1}={0}.{1}.{3},{0}.{2}={0}.{2}.{4}'.format( \
			'org.eclipse.cdt.build.core', 'buildType', 'buildArtefactType', self.cdt['build'], self.cdt['ext'])
		config.set('buildProperties', prop)
		folder = config.find('folderInfo')
		folder.set('id','%s.' % (self.cdt['instance']))
		self.cconfig_toolchain_update(folder)


	def _external_settings_update(self, settings, name):
		settings.set('containerId', '%s;' % name)
		settings.set('factoryId', 'org.eclipse.cdt.core.cfg.export.settings.sipplier')
		eset = ElementTree.SubElement(settings, 'externalSetting')
		e = ElementTree.SubElement(eset, 'entry', {'flags':'VALUE_WORKSPACE_PATH', 'kind':'includePath'})
		e.set('name', '/%s' % name)
		e = ElementTree.SubElement(eset, 'entry', {'flags':'VALUE_WORKSPACE_PATH', 'kind':'libraryPath'})
		e.set('name', '/%s/%s' % (name, self.cdt['name']))
		e = ElementTree.SubElement(eset, 'entry', {'flags':'RESOLVED', 'kind':'libraryFile', 'srcPrefixMapping':'', 'srcRootPath': ''})
		e.set('name', '%s' % name)
	
	def cconfig_external_settings_update(self, module):
		deps = waftools.deps.get_deps(self.bld, self.tgen.get_name())
		tgens = waftools.deps.get_tgens(self.bld, deps)
		for tg in tgens:
			if not set(('c', 'cxx')) & set(tg.features):			
				continue
			if set(('cprogram', 'cxxprogram')) & set(tg.features):
				continue
			name = tg.get_name()
			settings = None
			for s in module.findall('externalSettings'):
				cid = s.get('containerId').rstrip(';')
				if cid == name:
					settings = s
					break
			if settings is None:
				settings = ElementTree.SubElement(module, 'externalSettings')
			self._external_settings_update(settings, name)

	def cconfig_toolchain_update(self, folder):
		toolchain = folder.find('toolChain')
		toolchain.set('superClass', self.cdt['toolchain'])
		toolchain.set('id', '%s.%s' % (self.cdt['toolchain'], self.get_uuid()))
		name = 'MinGW GCC' if sys.platform=='win32' else 'Linux GCC'
		if self.cross and sys.platform!='win32':
			name = 'Cross GCC'
		toolchain.set('name', name)

		self.toolchain_cross_update(toolchain)
		self.toolchain_target_update(toolchain)
		self.toolchain_builder_update(toolchain)
		self.toolchain_compiler_update(toolchain, 'c')
		self.toolchain_compiler_update(toolchain, 'cpp')
		self.toolchain_linker_update(toolchain, 'c')
		self.toolchain_linker_update(toolchain, 'cpp')
		self.toolchain_archiver_update(toolchain)
		self.toolchain_assembler_update(toolchain)

	def toolchain_cross_update(self, toolchain):
		options = toolchain.findall('option')
		for option in options:
			toolchain.remove(option)
		if not self.cross or sys.platform=='win32':
			return

		option = ElementTree.Element('option', {'valueType':'string', 'name':'Path'})
		option.set('superClass', 'cdt.managedbuild.option.gnu.cross.path')
		option.set('id', 'cdt.managedbuild.option.gnu.cross.path.%s' % self.get_uuid())
		option.set('value', self.cdt['cc_path'])
		toolchain.insert(0, option)

		option = ElementTree.Element('option', {'valueType':'string', 'name':'Prefix'})
		option.set('superClass', 'cdt.managedbuild.option.gnu.cross.prefix')
		option.set('id', 'cdt.managedbuild.option.gnu.cross.prefix.%s' % self.get_uuid())
		option.set('value', str(self.cdt['c']).rstrip('gcc'))
		toolchain.insert(0, option)

	def toolchain_target_update(self, toolchain):
		target = toolchain.find('targetPlatform')
		target.set('name', self.cdt['name'])
		target.set('superClass', self.cdt['platform'])
		target.set('id', '%s.%s' % (self.cdt['platform'], self.get_uuid()))
		if self.cross:
			target.set('archList', 'all')
			target.set('osList', 'all')
		target.set('binaryParser', 'org.eclipse.cdt.core.PE;org.eclipse.cdt.core.ELF')

	def toolchain_builder_update(self, toolchain):
		builder = toolchain.find('builder')
		builder.set('buildPath', '${workspace_loc:/%s}/%s' % (self.tgen.get_name(), self.cdt['name']))
		builder.set('superClass', 'cdt.managedbuild.target.gnu.builder.base')
		builder.set('id', '%s.%s' % (builder.get('superClass'), self.get_uuid()))
		builder.set('keepEnvironmentInBuildfile', 'false')
		builder.set('name', 'Gnu Make Builder')

	def toolchain_archiver_get(self, toolchain):
		for tool in toolchain.findall('tool'):
			if tool.get('superClass').count('.gnu.archiver.'):
				tool.clear()
				return tool
		return ElementTree.SubElement(toolchain, 'tool', {'id':'', 'name':'', 'superClass':''})

	def toolchain_archiver_update(self, toolchain):
		archiver = self.toolchain_archiver_get(toolchain)
		archiver.set('superClass', self.cdt['archiver'])
		archiver.set('id', '%s.%s' % (self.cdt['archiver'], self.get_uuid()))
		if self.cross and sys.platform=='win32':
			archiver.set('command', self.cdt['ar'])
		archiver.set('name', 'GCC Archiver')

	def toolchain_assembler_get(self, toolchain):
		for tool in toolchain.findall('tool'):
			if tool.get('superClass').count('.gnu.assembler.'):
				tool.clear()
				return tool
		return ElementTree.SubElement(toolchain, 'tool', {'id':'', 'name':'', 'superClass':''})

	def toolchain_assembler_update(self, toolchain):
		assembler = self.toolchain_assembler_get(toolchain)
		assembler.set('superClass', self.cdt['assembler'])
		assembler.set('id', '%s.%s' % (self.cdt['assembler'], self.get_uuid()))
		name = 'MinGW Assembler' if sys.platform=='win32' else 'GCC Assembler'
		if self.cross:
			name = 'Cross GCC Assembler'
		assembler.set('name', name)
		if self.cross and sys.platform=='win32':
			assembler.set('command', self.cdt['as'])

		inputtype = assembler.find('inputType')
		if inputtype is None:
			inputtype = ElementTree.SubElement(assembler, 'inputType')
		inputtype.set('superClass', 'cdt.managedbuild.tool.gnu.assembler.input')
		inputtype.set('id', '%s.%s' % (inputtype.get('superClass'), self.get_uuid()))

	def toolchain_compiler_get(self, toolchain, language):
		for tool in toolchain.findall('tool'):
			if tool.get('superClass').count('.gnu.%s.compiler.' % language):
				tool.clear()
				return tool
		return ElementTree.SubElement(toolchain, 'tool', {'id':'', 'name':'', 'superClass':''})

	def toolchain_compiler_update(self, toolchain, language):
		compiler = self.toolchain_compiler_get(toolchain, language)
		compiler.set('superClass', self.cdt['%s_compiler' % language])
		compiler.set('id', '%s.%s' % (compiler.get('superClass'), self.get_uuid()))
		name = 'G++ Compiler' if language=='cpp' else 'GCC Compiler'
		if self.cross:
			name = 'Cross ' + name
		compiler.set('name', name)
		if self.cross and sys.platform=='win32':
			compiler.set('command', self.cdt[language])
		self.compiler_add_includes(compiler, language)
		self.compiler_add_defines(compiler, language)
		self.compiler_add_options(compiler, language)
		self.compiler_add_input(compiler, language)

	def compiler_add_options(self, compiler, language):
		if self.cdt['build'] == 'debug':
			optimization_level = 'none'
			debug_level = 'max'
		else:
			optimization_level = 'most'
			debug_level = 'none'

		if language == 'c':
			options = ['-c']
			flags = self.tgen.env.CFLAGS
		else:
			flags = self.tgen.env.CXXFLAGS
			options = ['-c++']

		if self.tgen.env.CC_VERSION[0] not in ('1', '2'):
			options.append('-fmessage-length=0') # only when not using ancient compilers

		for f in flags:
			if f.startswith('-W') or f.startswith('-O') or f == '-g': # handled elsewhere
				continue
			options.append(f)
		options = ' '.join(options)

		gnut = self.cdt['gnut']
		t = 'gnu.%s.compiler%s' % (language, '.%s' % gnut if gnut else '')

		option = ElementTree.SubElement(compiler, 'option', {'name':'Optimization Level', 'valueType':'enumerated'})
		option.set('superClass', '%s.%s.%s.option.optimization.level' % (t, self.cdt['ext'], self.cdt['build']))
		option.set('id', '%s.%s' % (option.get('superClass'), self.get_uuid()))
		option.set('defaultValue', 'gnu.%s.optimization.level.%s' % (language, optimization_level))

		option = ElementTree.SubElement(compiler, 'option', {'name':'Debug Level', 'valueType':'enumerated'})
		option.set('superClass', '%s.%s.%s.option.debugging.level' % (t, self.cdt['ext'], self.cdt['build']))
		option.set('id', '%s.%s' % (option.get('superClass'), self.get_uuid()))
		option.set('value', 'gnu.%s.debugging.level.%s' % (language, debug_level))

		option = ElementTree.SubElement(compiler, 'option', {'value':options,'valueType':'string'})
		option.set('superClass', 'gnu.c.compiler.option.misc.other')
		option.set('id', '%s.%s' % (option.get('superClass'), self.get_uuid()))

		if self.cdt['ext'] == 'so' and self.language == language:
			option = ElementTree.SubElement(compiler, 'option', {'value':'true','valueType':'boolean'})
			option.set('superClass', 'gnu.%s.compiler.option.misc.pic' % language)
			option.set('id', '%s.%s' % (option.get('superClass'), self.get_uuid()))

	def compiler_add_includes(self, compiler, language):
		if self.language != language:
			return
		uses = waftools.deps.get_deps(self.bld, self.tgen.get_name())
		includes = Utils.to_list(getattr(self.tgen, 'includes', []))
		
		if not len(uses) and not len(includes):
			return

		option = ElementTree.SubElement(compiler, 'option', {'name':'Include paths (-I)', 'valueType':'includePath'})
		option.set('superClass', 'gnu.%s.compiler.option.include.paths' % (language))
		option.set('id', '%s.%s' % (option.get('superClass'), self.get_uuid()))

		includes = list(set([str(i).lstrip('./') for i in includes]))
		for include in includes:
			pth = os.path.join(self.tgen.path.abspath(), include)
			if os.path.exists(pth):
				listoption = ElementTree.SubElement(option, 'listOptionValue', {'builtIn':'false'})
				listoption.set('value', '"${workspace_loc:/${ProjName}/%s}"' % (include))

		includes = list(self.bld.env.INCLUDES)
		includes = [i.abspath() if hasattr(i, 'abspath') else i for i in includes]
		includes = [i.replace('\\', '/') for i in includes]
		for include in includes:
			if os.path.exists(include):
				listoption = ElementTree.SubElement(option, 'listOptionValue', {'builtIn':'false'})
				listoption.set('value', '"%s"' % (include))
				
		for use in uses:
			try:
				tg = self.bld.get_tgen_by_name(use)
			except Errors.WafError:
				includes = self.tgen.env['INCLUDES_%s' % use]
			else:
				includes = Utils.to_list(getattr(tg, 'export_includes', []))
				sources = Utils.to_list(getattr(tg, 'source', []))
				if not len(sources):
					top = tg.path.abspath().replace('\\', '/')
					includes = ['%s/%s' % (top, i) for i in includes if len(i)]
				else:
					includes = list(set([str(i) for i in includes]))
					includes = [i.lstrip('./') if i.startswith('./') else i for i in includes]
					includes = ['"${workspace_loc:/%s/%s}"' % (use, i) for i in includes if len(i)]
					
			for include in includes:
				listoption = ElementTree.SubElement(option, 'listOptionValue', {'builtIn':'false'})
				listoption.set('value', include)

	def compiler_add_defines(self, compiler, language):
		if self.language != language:
			return
		
		defines = list(self.tgen.env.DEFINES)
		if not len(defines):
			return

		if language == 'cpp':
			superclass = 'gnu.cpp.compiler.option.preprocessor.def'
		else:
			superclass = 'gnu.c.compiler.option.preprocessor.def.symbols'

		option = ElementTree.SubElement(compiler, 'option', {'name':'Defined symbols (-D)', 'valueType':'definedSymbols'})
		option.set('superClass', superclass)
		option.set('id', '%s.%s' % (superclass, self.get_uuid()))

		for define in defines:
			listoption = ElementTree.SubElement(option, 'listOptionValue', {'builtIn':'false'})
			listoption.set('value', '%s' % define)

	def compiler_add_input(self, compiler, language):
		if self.language != language:
			return

		ci = self.cdt['input']
		inputtype = ElementTree.SubElement(compiler, 'inputType')
		inputtype.set('superClass', '.'.join(ci.split('.')[:-1]))
		inputtype.set('id', ci)
		
		if self.cdt['ext'] == 'so':
			ElementTree.SubElement(inputtype, 'additionalInput', {'kind':'additionalinputdependency', 'paths':'$(USER_OBJS)'})
			ElementTree.SubElement(inputtype, 'additionalInput', {'kind':'additionalinput', 'paths':'$(LIBS)'})

	def toolchain_linker_get(self, toolchain, language):
		for tool in toolchain.findall('tool'):
			if tool.get('superClass').count('.gnu.%s.linker.' % language):
				tool.clear()
				return tool
		return ElementTree.SubElement(toolchain, 'tool', {'id':'', 'name':'', 'superClass':''})

	def toolchain_linker_update(self, toolchain, language):
		linker = self.toolchain_linker_get(toolchain, language)
		linker.set('superClass', self.cdt['%s_linker' % language])
		linker.set('id', '%s.%s' % (linker.get('superClass'), self.get_uuid()))
		name = 'G++ Linker' if language=='cpp' else 'GCC Linker'
		if self.cross:
			name = 'Cross ' + name
		linker.set('name', name)
		if self.cross and sys.platform=='win32':
			linker.set('command', self.cdt[language])		
		if self.cdt['ext'] == 'so':
			option = ElementTree.SubElement(linker, 'option', {'defaultValue':'true', 'valueType':'boolean'})
			option.set('name', 'Shared (-shared)')
			option.set('superClass', 'gnu.%s.link.so.%s.option.shared' % (language, self.cdt['build']))
			option.set('id', '%s.%s' % (option.get('superClass'), self.get_uuid()))

		self.toolchain_linker_add_libs(linker, language)
		self.toolchain_linker_add_libpaths(linker, language)
		self.toolchain_linker_add_input(linker, language)

	def _get_libs(self, target, nest):
		try:
			tgen = self.bld.get_tgen_by_name(target)
		except Errors.WafError:
			return []
		name = tgen.get_name()
		features = Utils.to_list(getattr(tgen, 'features', []))
		
		if 'fake_lib' in features:
			paths = [p.replace('\\', '/') for p in tgen.lib_paths]
			return [(nest, name, None, paths)]
		
		if not set(('c','cxx')) & set(features):
			return []
		
		libs = [(nest, name, name, None)]		
		
		if set(('cshlib', 'cxxshlib')) & set(features):
			return libs
		
		for lib in Utils.to_list(getattr(tgen, 'lib', [])):
			libs.append((nest, lib, None, None))
			
		for use in getattr(tgen, 'use', []):
			libs.extend(self._get_libs(use, nest+1))
		
		return libs

	def toolchain_linker_get_libs(self):
		'''returns a list of tuples containing lib name, task name and list of 
		search paths.
		
		libs = [(str, str, []), (str, str, []), ... ]
		
		'''
		tgen = self.tgen
		name = tgen.get_name()
		libs = []
		uses = []
		for use in Utils.to_list(getattr(tgen, 'use', [])):
			uses.extend(self._get_libs(use, 0))
			
		# sort by task with highest nesting
		uses = sorted(uses, key=lambda use: use[0])
		uses = uses[::-1]

		# insert tasks at start of list
		# lowest nesting at start
		for (_, lib, name, paths) in uses:
			if name:
				entry = (lib, name, paths)
				if entry not in libs:
					libs.insert(0, entry)

		# append system libraries at end of list
		for lib in Utils.to_list(getattr(tgen, 'lib', [])):
			libs.append((lib, None, None))
		for (_, lib, name, paths) in uses:
			if not name:
				entry = (lib, name, paths)
				if entry not in libs:
					libs.append(entry)
		return libs

	def toolchain_linker_add_libs(self, linker, language):
		if self.language != language:
			return

		option = ElementTree.SubElement(linker, 'option', {'name':'Libraries (-l)', 'valueType':'libs'})
		option.set('superClass', 'gnu.%s.link.option.libs' % (language))
		option.set('id', '%s.%s' % (option.get('superClass'), self.get_uuid()))

		libs = self.toolchain_linker_get_libs()
		for (lib, _, _) in libs:
			listoption = ElementTree.SubElement(option, 'listOptionValue', {'builtIn':'false'})
			listoption.set('value', lib)

	def toolchain_linker_add_libpaths(self, linker, language):
		if self.language != language:
			return

		option = ElementTree.SubElement(linker, 'option', {'name':'Library search path (-L)', 'valueType':'libPaths'})
		option.set('superClass', 'gnu.%s.link.option.paths' % (language))
		option.set('id', '%s.%s' % (option.get('superClass'), self.get_uuid()))

		libs = self.toolchain_linker_get_libs()
		for (_, name, paths) in libs:
			if name:
				listoption = ElementTree.SubElement(option, 'listOptionValue', {'builtIn':'false'})
				listoption.set('value', '"${workspace_loc:/%s/%s}"' % (name, self.cdt['name']))
			elif paths:
				listoption = ElementTree.SubElement(option, 'listOptionValue', {'builtIn':'false'})
				for path in paths:
					listoption.set('value', path)

	def toolchain_linker_add_input(self, linker, language):
		if self.language != language or self.cdt['ext'] == 'lib':
			return
		inputtype = ElementTree.SubElement(linker, 'inputType')
		inputtype.set('superClass', 'cdt.managedbuild.tool.gnu.%s.linker.input' % (language))
		inputtype.set('id', '%s.%s' % (inputtype.get('superClass'), self.get_uuid()))
		ElementTree.SubElement(inputtype, 'additionalInput', {'kind':'additionalinputdependency', 'paths':'$(USER_OBJS)'})
		ElementTree.SubElement(inputtype, 'additionalInput', {'kind':'additionalinput', 'paths':'$(LIBS)'})


class CDTLaunch(Project):
	'''Class for exporting a *CDT* launcher for C/C++ programs.

	:param bld: a *waf* build instance from the top level *wscript*.
	:type bld: waflib.Build.BuildContext
	
	:param gen: The C/C++ task generator for which the launcher should be 
				created.
	:type gen: waflib.Task.TaskGen

	:param instance: Identifier from the *Eclipse* *CDT* project of the program
				to be started by this launcher
	:type instance: str
	
	:param btype: Build type and build directory name (debug|release)
	:type btype: str
    
	:param ext: Program name extension
	:type ext: str
	'''
	def __init__(self, bld, tgen, instance, btype, ext):
		super(CDTLaunch, self).__init__(bld, tgen)
		self.comments = ['<?xml version="1.0" encoding="UTF-8" standalone="no"?>']
		self.template = ECLIPSE_CDT_LAUNCH
		self.build_dir = btype
		self.build_config_id = instance
		self.program = '%s%s' % (tgen.get_name(), '.%s' % ext if len(ext) else '')

	def get_fname(self):
		name = '%s(%s).launch' % (self.tgen.get_name(), self.build_dir)
		if self.tgen:
			name = '%s/%s' % (self.tgen.path.relpath(), name)
		return name.replace('\\', '/')

	def get_content(self):
		root = ElementTree.fromstring(self.template)
		for attrib in root.iter('stringAttribute'):
			if attrib.get('key') == 'org.eclipse.cdt.launch.PROGRAM_NAME':
				attrib.set('value', '%s/%s' % (self.build_dir, self.program))
			if attrib.get('key') == 'org.eclipse.cdt.launch.PROJECT_ATTR':
				attrib.set('value', self.tgen.get_name())
			if attrib.get('key') == 'org.eclipse.cdt.launch.PROJECT_BUILD_CONFIG_ID_ATTR':
				attrib.set('value', self.build_config_id)

		for attrib in root.iter('listAttribute'):
			if attrib.get('key') == 'org.eclipse.debug.core.MAPPED_RESOURCE_PATHS':
				attrib.find('listEntry').set('value', '/%s' % self.tgen.get_name())

		attrib = root.find('mapAttribute')
		
		deps = waftools.deps.get_deps(self.bld, self.tgen.get_name())
		for tg in waftools.deps.get_tgens(self.bld, deps):
			if set(('cshlib', 'cxxshlib')) & set(tg.features):
				mapentry = ElementTree.SubElement(attrib, 'mapEntry', {'key':'PATH' if sys.platform=='win32' else 'LD_LIBRARY_PATH'})
				mapentry.set('value', '${workspace_loc:/%s/%s}' % (tg.get_name(), self.build_dir))
		return ElementTree.tostring(root)


ECLIPSE_PROJECT = \
'''<?xml version="1.0" encoding="UTF-8"?>
<projectDescription>
	<name></name>
	<comment></comment>
	<projects/>
	<buildSpec>
		<buildCommand>
			<name>org.eclipse.cdt.managedbuilder.core.genmakebuilder</name>
			<triggers>clean,full,incremental,</triggers>
			<arguments>
			</arguments>
		</buildCommand>
		<buildCommand>
			<name>org.eclipse.cdt.managedbuilder.core.ScannerConfigBuilder</name>
			<triggers>full,incremental,</triggers>
			<arguments>
			</arguments>
		</buildCommand>
	</buildSpec>
	<natures>
		<nature>org.eclipse.cdt.core.cnature</nature>
		<nature>org.eclipse.cdt.managedbuilder.core.managedBuildNature</nature>
		<nature>org.eclipse.cdt.managedbuilder.core.ScannerConfigNature</nature>
	</natures>
</projectDescription>
'''


ECLIPSE_CDT_PROJECT = \
'''<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<?fileVersion 4.0.0?>
<cproject storage_type_id="org.eclipse.cdt.core.XmlProjectDescriptionStorage">
	<storageModule moduleId="org.eclipse.cdt.core.settings">
	</storageModule>
	<storageModule moduleId="cdtBuildSystem" version="4.0.0">
		<project id="" name="" projectType=""/>
	</storageModule>
	<storageModule moduleId="scannerConfiguration">
		<autodiscovery enabled="true" problemReportingEnabled="true" selectedProfileId=""/>
	</storageModule>
	<storageModule moduleId="org.eclipse.cdt.core.LanguageSettingsProviders"/>
	<storageModule moduleId="org.eclipse.cdt.make.core.buildtargets"/>
	<storageModule moduleId="refreshScope" versionNumber="2">
	</storageModule>
</cproject>
'''


ECLIPSE_CDT_CCONFIGURATION = '''
<cconfiguration>
	<storageModule buildSystemId="org.eclipse.cdt.managedbuilder.core.configurationDataProvider" id="" moduleId="org.eclipse.cdt.core.settings" name="">
		<externalSettings>
			<externalSetting>
				<entry flags="VALUE_WORKSPACE_PATH" kind="includePath" name=""/>
				<entry flags="VALUE_WORKSPACE_PATH" kind="libraryPath" name=""/>
				<entry flags="RESOLVED" kind="libraryFile" name="" srcPrefixMapping="" srcRootPath=""/>
			</externalSetting>
		</externalSettings>
		<extensions>
			<extension id="org.eclipse.cdt.core.ELF" point="org.eclipse.cdt.core.BinaryParser"/>
			<extension id="org.eclipse.cdt.core.PE" point="org.eclipse.cdt.core.BinaryParser"/>
			<extension id="org.eclipse.cdt.core.GNU_ELF" point="org.eclipse.cdt.core.BinaryParser"/>
			<extension id="org.eclipse.cdt.core.GCCErrorParser" point="org.eclipse.cdt.core.ErrorParser"/>
			<extension id="org.eclipse.cdt.core.GASErrorParser" point="org.eclipse.cdt.core.ErrorParser"/>
			<extension id="org.eclipse.cdt.core.GLDErrorParser" point="org.eclipse.cdt.core.ErrorParser"/>
			<extension id="org.eclipse.cdt.core.GmakeErrorParser" point="org.eclipse.cdt.core.ErrorParser"/>
			<extension id="org.eclipse.cdt.core.CWDLocator" point="org.eclipse.cdt.core.ErrorParser"/>
		</extensions>
	</storageModule>
	<storageModule moduleId="cdtBuildSystem" version="4.0.0">
		<configuration artifactName="${ProjName}" buildArtefactType="" buildProperties="" cleanCommand="rm -rf" description="" id="" name="" parent="">
			<folderInfo id="" name="/" resourcePath="">
				<toolChain id="" name="" superClass="">
					<targetPlatform id="" name="" superClass=""/>
					<builder buildPath="" id="" name="" superClass=""/>
				</toolChain>
			</folderInfo>
		</configuration>
	</storageModule>
	<storageModule moduleId="org.eclipse.cdt.core.externalSettings">
	</storageModule>
</cconfiguration>
'''


ECLIPSE_CDT_LAUNCH = \
'''<?xml version="1.0" encoding="UTF-8" standalone="no"?>
<launchConfiguration type="org.eclipse.cdt.launch.applicationLaunchType">
	<booleanAttribute key="org.eclipse.cdt.dsf.gdb.AUTO_SOLIB" value="true"/>
	<listAttribute key="org.eclipse.cdt.dsf.gdb.AUTO_SOLIB_LIST"/>
	<stringAttribute key="org.eclipse.cdt.dsf.gdb.DEBUG_NAME" value="gdb"/>
	<booleanAttribute key="org.eclipse.cdt.dsf.gdb.DEBUG_ON_FORK" value="false"/>
	<stringAttribute key="org.eclipse.cdt.dsf.gdb.GDB_INIT" value=".gdbinit"/>
	<booleanAttribute key="org.eclipse.cdt.dsf.gdb.NON_STOP" value="false"/>
	<booleanAttribute key="org.eclipse.cdt.dsf.gdb.REVERSE" value="false"/>
	<listAttribute key="org.eclipse.cdt.dsf.gdb.SOLIB_PATH"/>
	<stringAttribute key="org.eclipse.cdt.dsf.gdb.TRACEPOINT_MODE" value="TP_NORMAL_ONLY"/>
	<booleanAttribute key="org.eclipse.cdt.dsf.gdb.UPDATE_THREADLIST_ON_SUSPEND" value="false"/>
	<booleanAttribute key="org.eclipse.cdt.dsf.gdb.internal.ui.launching.LocalApplicationCDebuggerTab.DEFAULTS_SET" value="true"/>
	<intAttribute key="org.eclipse.cdt.launch.ATTR_BUILD_BEFORE_LAUNCH_ATTR" value="2"/>
	<stringAttribute key="org.eclipse.cdt.launch.COREFILE_PATH" value=""/>
	<stringAttribute key="org.eclipse.cdt.launch.DEBUGGER_ID" value="gdb"/>
	<stringAttribute key="org.eclipse.cdt.launch.DEBUGGER_START_MODE" value="run"/>
	<booleanAttribute key="org.eclipse.cdt.launch.DEBUGGER_STOP_AT_MAIN" value="true"/>
	<stringAttribute key="org.eclipse.cdt.launch.DEBUGGER_STOP_AT_MAIN_SYMBOL" value="main"/>
	<stringAttribute key="org.eclipse.cdt.launch.PROGRAM_NAME" value=""/>
	<stringAttribute key="org.eclipse.cdt.launch.PROJECT_ATTR" value=""/>
	<booleanAttribute key="org.eclipse.cdt.launch.PROJECT_BUILD_CONFIG_AUTO_ATTR" value="true"/>
	<stringAttribute key="org.eclipse.cdt.launch.PROJECT_BUILD_CONFIG_ID_ATTR" value=""/>
	<booleanAttribute key="org.eclipse.cdt.launch.use_terminal" value="true"/>
	<listAttribute key="org.eclipse.debug.core.MAPPED_RESOURCE_PATHS">
		<listEntry value=""/>
	</listAttribute>
	<listAttribute key="org.eclipse.debug.core.MAPPED_RESOURCE_TYPES">
		<listEntry value="4"/>
	</listAttribute>
	<mapAttribute key="org.eclipse.debug.core.environmentVariables">
	</mapAttribute>
	<stringAttribute key="org.eclipse.dsf.launch.MEMORY_BLOCKS" value="&lt;?xml version=&quot;1.0&quot; encoding=&quot;UTF-8&quot; standalone=&quot;no&quot;?&gt;&#13;&#10;&lt;memoryBlockExpressionList context=&quot;reserved-for-future-use&quot;/&gt;&#13;&#10;"/>
	<stringAttribute key="process_factory_id" value="org.eclipse.cdt.dsf.gdb.GdbProcessFactory"/>
</launchConfiguration>
'''
