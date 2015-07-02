#! /usr/bin/env python
# -*- encoding: utf-8 -*-
# Michel Mooij, michel.mooij7@gmail.com

'''
Summary
-------
Generate **GNU Make** files of all C/C++ programs, static- and shared libraries
that have been defined within a *waf* build environment.
Once exported to makefiles, all exported (C/C++) tasks can be build without 
any further need for, or dependency, to the *waf* build system itself.

Description
-----------
When exporting *waf* project data, a single top level **GNU Make** file will be
exported in the top level directory of your *WAF* build environment. This 
makefile file will contain references to all exported makefiles of each 
individual C/C++ build task, contains generic variables and settings (e.g 
compiler to use, global preprocessor defines, link options and so on).

Example below presents the content of the top level makefile that has been 
exported from the test directory::


		#------------------------------------------------------------------------------
		# WAFTOOLS generated makefile
		# version: 0.1.5
		# waf: 1.7.15
		#------------------------------------------------------------------------------

		SHELL=/bin/sh

		# commas, spaces and tabs:
		sp:= 
		sp+= 
		tab:=$(sp)$(sp)$(sp)$(sp)
		comma:=,

		# token for separating dictionary keys and values:
		dsep:=;

		# token for separating list elements:
		lsep:=,

		# token for joining command and component names (e.g. 'build.hello')
		csep:=.

		export APPNAME:=waftools-test
		export APPVERSION:=0.1.5
		export PREFIX:=$(CURDIR)/output
		export TOP:=$(CURDIR)
		export OUT:=$(TOP)/build
		export AR:=/usr/bin/ar
		export CC:=/usr/lib64/ccache/gcc
		export CXX:=/usr/lib64/ccache/g++
		export CFLAGS:=-Wall -g -ggdb
		export CXXFLAGS:=-Wall -g -ggdb
		export DEFINES:=
		export RPATH:=
		export BINDIR:=$(PREFIX)/bin
		export LIBDIR:=$(PREFIX)/lib

		SEARCHPATH=components/
		SEARCHFILE=Makefile

		#------------------------------------------------------------------------------
		# list of unique logical module names;
		modules= \
			cleaking \
			cxxshlib \
			ciambad \
			cstlib \
			chello \
			cprogram \
			cxxstlib \
			cxxhello \
			cmath \
			cxxprogram \
			cshlib

		# dictionary of modules names (key) and paths to modules;
		paths= \
			cleaking;components/cleaking \
			cxxshlib;components/cxxlib/shared \
			ciambad;components/ciambad \
			cstlib;components/clib/static \
			chello;components/chello \
			cprogram;components/clib/program \
			cxxstlib;components/cxxlib/static \
			cxxhello;components/cxxhello \
			cmath;components/cmath \
			cxxprogram;components/cxxlib/program \
			cshlib;components/clib/shared

		# dictionary of modules names (key) and module dependencies;
		deps= \
			cleaking; \
			cxxshlib; \
			ciambad;cleaking \
			cstlib; \
			chello; \
			cprogram;cstlib,cshlib \
			cxxstlib; \
			cxxhello; \
			cmath; \
			cxxprogram;cxxstlib,cxxshlib \
			cshlib;

		#------------------------------------------------------------------------------
		# define targets
		#------------------------------------------------------------------------------
		build_targets=$(addprefix build$(csep),$(modules))
		clean_targets=$(addprefix clean$(csep),$(modules))
		install_targets=$(addprefix install$(csep),$(modules))
		uninstall_targets=$(addprefix uninstall$(csep),$(modules))

		cmds=build clean install uninstall
		commands=$(sort $(cmds) all help find list modules $(foreach prefix,$(cmds),$($(prefix)_targets)))

		.DEFAULT_GOAL:=all

		#------------------------------------------------------------------------------
		# recursive wild card implementation
		#------------------------------------------------------------------------------
		define rwildcard
		$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
		endef

		#------------------------------------------------------------------------------
		# returns the value from a dictionary
		# $1 = key, where key is the functional name of the component.
		# $2 = dictionary
		#------------------------------------------------------------------------------
		define getdval
		$(subst $(lastword $(subst $(csep),$(sp),$1))$(dsep),$(sp),$(filter $(lastword $(subst $(csep),$(sp),$1))$(dsep)%,$2))
		endef

		#------------------------------------------------------------------------------
		# returns path to makefile
		# $1 = key, where key is the functional name of the component.
		#------------------------------------------------------------------------------
		define getpath
		$(call getdval, $1, $(paths))
		endef

		#------------------------------------------------------------------------------
		# returns component dependencies.
		# $1 = key, where key is the functional name of the component.
		#------------------------------------------------------------------------------
		define getdeps
		$(addprefix $(firstword $(subst $(csep),$(sp),$1))$(csep),$(subst $(lsep),$(sp),$(call getdval, $1, $(deps))))
		endef

		#------------------------------------------------------------------------------
		# creates a make recipe:
		#      'make -r -C <path> -f <name>.mk <command>'
		# where:
		#      <path>     is the relative path to the component
		#      <name>     is the name of the component
		#      <command>  is the make action to be executed, e.g. build, install, clean
		#
		# $1 = key, where key is the functional recipe name (e.g. build.a).
		#------------------------------------------------------------------------------
		define domake
		$1: $(call getdeps, $1)
			$(MAKE) -r -C $(call getpath,$1) -f $(lastword $(subst $(csep),$(sp),$1)).mk $(firstword $(subst $(csep),$(sp),$1))
		endef

		#------------------------------------------------------------------------------
		# return files found in given search path
		# $1 = search path
		# $2 = file name so search
		#------------------------------------------------------------------------------
		define dofind
		$(foreach path, $(dir $(call rwildcard,$1,$2)),echo "  $(path)";)
		endef

		#------------------------------------------------------------------------------
		# definitions of recipes (i.e. make targets)
		#------------------------------------------------------------------------------
		all: build

		build: $(build_targets)

		clean: $(clean_targets)

		install: build $(install_targets)

		uninstall: $(uninstall_targets)

		list:
			@echo ""
			@$(foreach cmd,$(commands),echo "  $(cmd)";)
			@echo ""

		modules:
			@echo ""
			@$(foreach module,$(modules),echo "  $(module)";)
			@echo ""

		find:
			@echo ""
			@echo "$@:"
			@echo "  path=$(SEARCHPATH) file=$(SEARCHFILE)"
			@echo ""
			@echo "result:"
			@$(call dofind,$(SEARCHPATH),$(SEARCHFILE))
			@echo ""

		help:
			@echo ""
			@echo "$(APPNAME) version $(APPVERSION)"
			@echo ""
			@echo "usage:"
			@echo "  make [-r] [-s] [--jobs=N] [command] [VARIABLE=VALUE]"
			@echo ""
			@echo "commands:"
			@echo "  all                                 builds all modules"
			@echo "  build                               builds all modules"
			@echo "  build$(csep)a                             builds module 'a' and it's dependencies"
			@echo "  clean                               removes all build intermediates and outputs"
			@echo "  clean$(csep)a                             cleans module 'a' and it's dependencies"
			@echo "  install                             installs files in $(PREFIX)"
			@echo "  install$(csep)a                           installs module 'a' and it's dependencies"
			@echo "  uninstall                           removes all installed files from $(PREFIX)"
			@echo "  uninstall$(csep)a                         removes module 'a' and it's dependencies"
			@echo "  list                                list available make commands (i.e. recipes)"
			@echo "  modules                             list logical names of all modules"
			@echo "  find [SEARCHPATH=] [SEARCHFILE=]    searches for files default(path=$(SEARCHPATH),file=$(SEARCHFILE))"
			@echo "  help                                displays this help message."
			@echo ""
			@echo "remarks:"
			@echo "  use options '-r' and '--jobs=N' in order to improve speed"
			@echo "  use options '-s' to decrease verbosity"
			@echo ""

		$(foreach t,$(build_targets),$(eval $(call domake,$t)))

		$(foreach t,$(clean_targets),$(eval $(call domake,$t)))

		$(foreach t,$(install_targets),$(eval $(call domake,$t)))

		$(foreach t,$(uninstall_targets),$(eval $(call domake,$t)))

		.PHONY: $(commands)

Note that only the first two segments within the exported makefile contains 
project specific data; the first contains global settings, the second contains
list of the functional names of the modules, path to those modules and finally
dependencies between modules (if any). The remainder of the makefile is generic
and will be the same for each project.


For each single task generator (*waflib.TaskGenerator*), for instance a 
*bld.program(...)* which has been defined within a *wscript* file somewhere in
the build environment, a single **GNU Make** file will be generated in the same
directory as where the task generator has been defined.
The name of this task generator will be used as name for the exported 
**GNU Make** file. If for instance the name of the task generator is *hello*, 
then a **GNU Make** file named *hello.mk* will be exported.

Example below presents the content of the makefile for the *chello* component
that has been exported from the test directory::


    #------------------------------------------------------------------------------
    # WAFTOOLS generated makefile
    # version: 0.1.5
    # waf: 1.7.15
    #------------------------------------------------------------------------------

    SHELL=/bin/sh

    # commas, spaces and tabs:
    sp:= 
    sp+= 
    tab:=$(sp)$(sp)$(sp)$(sp)
    comma:=,

    #------------------------------------------------------------------------------
    # definition of build and install locations
    #------------------------------------------------------------------------------
    ifeq ($(TOP),)
    TOP=$(CURDIR)
    OUT=$(TOP)/build
    else
    OUT=$(subst $(sp),/,$(call rptotop) build $(call rpofcomp))
    endif

    PREFIX?=$(HOME)
    BINDIR?=$(PREFIX)/bin
    LIBDIR?=$(PREFIX)/lib

    #------------------------------------------------------------------------------
    # component data
    #------------------------------------------------------------------------------
    BIN=chello
    OUTPUT=$(OUT)/$(BIN)

    # REMARK: use $(wildcard src/*.c) to include all sources.
    SOURCES= \
        src/hello.c

    OBJECTS=$(SOURCES:.c=.1.o)

    DEFINES+=HELLO_VERSION='"1.2.3"'
    DEFINES:=$(addprefix -D,$(DEFINES))

    INCLUDES+= \
        ./include

    HEADERS:=$(foreach inc,$(INCLUDES),$(wildcard $(inc)/*.h))
    INCLUDES:=$(addprefix -I,$(INCLUDES))

    CFLAGS+=

    LINKFLAGS+=

    RPATH+=
    RPATH:= $(addprefix -Wl$(comma)-rpath$(comma),$(RPATH))

    LIBPATH_ST+=
    LIBPATH_ST:= $(addprefix -L,$(LIBPATH_ST))

    LIB_ST+=
    LIB_ST:= $(addprefix -l,$(LIB_ST))

    LIBPATH_SH+=
    LIBPATH_SH:= $(addprefix -L,$(LIBPATH_SH))

    LINK_ST= -Wl,-Bstatic $(LIBPATH_ST) $(LIB_ST)

    LIB_SH+=
    LIB_SH:= $(addprefix -l,$(LIB_SH))

    LINK_SH= -Wl,-Bdynamic $(LIBPATH_SH) $(LIB_SH)

    #------------------------------------------------------------------------------
    # returns the relative path of this component from the top directory
    #------------------------------------------------------------------------------
    define rpofcomp
    $(subst $(subst ~,$(HOME),$(TOP))/,,$(CURDIR))
    endef

    #------------------------------------------------------------------------------
    # returns the relative path of this component to the top directory
    #------------------------------------------------------------------------------
    define rptotop
    $(foreach word,$(subst /,$(sp),$(call rpofcomp)),..)
    endef

    #------------------------------------------------------------------------------
    # define targets
    #------------------------------------------------------------------------------
    commands= build clean install uninstall all

    .DEFAULT_GOAL=all

    #------------------------------------------------------------------------------
    # definitions of recipes (i.e. make targets)
    #------------------------------------------------------------------------------
    all: build

    build: $(OBJECTS)
        $(CC) $(LINKFLAGS) $(addprefix $(OUT)/,$(OBJECTS)) -o $(OUTPUT) $(RPATH) $(LINK_ST) $(LINK_SH)

    clean:
        $(foreach obj,$(OBJECTS),rm -f $(OUT)/$(obj);)
        rm -f $(OUTPUT)

    install: build
        mkdir -p $(BINDIR)
        cp $(OUTPUT) $(BINDIR)

    uninstall:
        rm -f $(BINDIR)/$(BIN)

    $(OBJECTS): $(HEADERS)
        mkdir -p $(OUT)/$(dir $@)
        $(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) $(subst .1.o,.c,$@) -c -o $(OUT)/$@

    .PHONY: $(commands)


Note that only one segment of this file, the named *component* *data* contains 
project specific data; the remainder of this makefile is generic.


Example below presents an overview of an environment in which **GNU** 
**Makefiles** already have been exported::

        .
        ├── components
        │   └── clib
        │       ├── program
        │       │   ├── cprogram.mk
        │       │   └── wscript
        │       ├── shared
        │       │   ├── cshlib.mk
        │       │   └── wscript
        │       └── static
        │           ├── cstlib.mk
        │           └── wscript
        │
        ├── Makefile
        └── wscript

Usage
-----
Tasks can be exported to makefiles using the *makefile* command, as shown in the
example below::

        $ waf makefile

All exported makefiles can be removed in one go using the *makefile* *clean*
option::

        $ waf makefile --clean

'''

import os
import re
from waflib import Utils, Context, Logs, Errors
from waflib.Build import BuildContext
import waftools
from waftools import deps


def options(opt):
	'''Adds command line options to the *waf* build environment 

	:param opt: Options context from the *waf* build environment.
	:type opt: waflib.Options.OptionsContext
	'''
	opt.add_option('--makefile', dest='makefile', default=False, action='store_true', help='select makefile for export/import actions')
	opt.add_option('--clean', dest='clean', default=False, action='store_true', help='delete exported files')


def configure(conf):
	'''Method that will be invoked by *waf* when configuring the build 
	environment.
	
	:param conf: Configuration context from the *waf* build environment.
	:type conf: waflib.Configure.ConfigurationContext
	'''
	pass


class MakeFileContext(BuildContext):
	'''export C/C++ tasks to (GNU) make files.'''
	cmd = 'makefile'

	def execute(self):
		'''Will be invoked when issuing the *makefile* command.'''
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
			cleanup(self)
		else:
			export(self)
		self.timer = Utils.Timer()


def export(bld):
	'''Exports all C and C++ task generators to makefiles.
	
	:param bld: a *waf* build instance from the top level *wscript*.
	:type bld: waflib.Build.BuildContext
	'''
	if not bld.options.makefile and not hasattr(bld, 'makefile'):
		return

	root = MakeRoot(bld)
	targets = waftools.deps.get_targets(bld)
	
	for tgen in bld.task_gen_cache_names.values():
		if targets and tgen.get_name() not in targets:
			continue
		if set(('c', 'cxx')) & set(tgen.features):
			child = MakeChild(bld, tgen, tgen.tasks)
			child.export()
			root.add_child(child.get_data())
	root.export()


def cleanup(bld):
	'''Removes all generated makefiles from the *waf* build environment.
	
	:param bld: a *waf* build instance from the top level *wscript*.
	:type bld: waflib.Build.BuildContext
	'''
	if not bld.options.makefile and not hasattr(bld, 'makefile'):
		return

	root = MakeRoot(bld)
	targets = waftools.deps.get_targets(bld)
	
	for tgen in bld.task_gen_cache_names.values():
		if targets and tgen.get_name() not in targets:
			continue
		if set(('c', 'cxx')) & set(tgen.features):
			child = MakeChild(bld, tgen, tgen.tasks)
			child.cleanup()
	
	root.cleanup()


class Make(object):
	def __init__(self, bld):
		self.bld = bld

	def export(self):
		content = self.get_content()
		if not content:
			return
		node = self.make_node()
		if not node:
			return
		node.write(content)
		Logs.pprint('YELLOW', 'exported: %s' % node.abspath())
			
	def cleanup(self):
		node = self.find_node()
		if node:
			node.delete()
			Logs.pprint('YELLOW', 'removed: %s' % node.abspath())

	def find_node(self):
		name = self.get_name()
		if not name:
			return None
		return self.bld.srcnode.find_node(name)

	def make_node(self):
		name = self.get_name()
		if not name:
			return None
		return self.bld.srcnode.make_node(name)
		
	def populate(self, content):
		s = content
		s = re.sub('==WAFVERSION==', Context.WAFVERSION, s)
		s = re.sub('==VERSION==', waftools.version, s)
		return s

	def get_name(self):
		'''abstract operation to be defined in child'''
		return None

	def get_content(self):
		'''abstract operation to be defined in child'''
		return None


class MakeRoot(Make):
	def __init__(self, bld):
		super(MakeRoot, self).__init__(bld)
		self.childs = []

	def get_name(self):
		name = self.bld.path.relpath().replace('\\', '/')
		if self.bld.variant:
			name += '/makefile_%s.mk' % (self.bld.variant)
		else:
			name += '/Makefile'
		return name

	def get_content(self):
		bld = self.bld
		cwd = str(os.getcwd()).replace('\\', '/')
		prefix = str(os.path.abspath(bld.env.PREFIX)).replace('\\', '/')
		if prefix.startswith(cwd):
			prefix = '$(CURDIR)%s' % prefix[len(cwd):]

		out = str(os.path.abspath(getattr(Context.g_module, Context.OUT))).replace('\\', '/')
		if out.startswith(cwd):
			out = '$(TOP)%s' % out[len(cwd):]
		if self.bld.variant:
			out = '%s/%s' % (out, self.bld.variant)

		ar = bld.env.AR
		if isinstance(ar, list):
			ar = ar[0]
		ar = ar.replace('\\', '/')
		try:
			cc = bld.env.CC[0]
		except IndexError:
			cc = 'gcc'
		cc = cc.replace('\\', '/')
		try:
			cxx = bld.env.CXX[0]
		except IndexError:
			cxx = 'g++'
		cxx = cxx.replace('\\', '/')
	
		var = self.bld.variant
		s = MAKEFILE_ROOT
		s = super(MakeRoot, self).populate(s)
		s = re.sub('APPNAME:=', 'APPNAME:=%s' % getattr(Context.g_module, Context.APPNAME), s)
		s = re.sub('APPVERSION:=', 'APPVERSION:=%s' % getattr(Context.g_module, Context.VERSION), s)
		s = re.sub('OUT:=', 'OUT:=%s' % out, s)
		s = re.sub('BINDIR:=', 'BINDIR:=%s' % str(bld.env.BINDIR).replace('\\', '/'), s)
		s = re.sub('LIBDIR:=', 'LIBDIR:=%s' % str(bld.env.LIBDIR).replace('\\', '/'), s)
		s = re.sub('AR:=', 'AR:=%s' % ar, s)
		s = re.sub('CC:=', 'CC:=%s' % cc, s)
		s = re.sub('CXX:=', 'CXX:=%s' % cxx, s)
		s = re.sub('RPATH:=', 'RPATH:=%s' % ' '.join(bld.env.RPATH), s)
		s = re.sub('CFLAGS:=', 'CFLAGS:=%s' % ' '.join(bld.env.CFLAGS), s)
		s = re.sub('CXXFLAGS:=', 'CXXFLAGS:=%s' % ' '.join(bld.env.CXXFLAGS), s)
		s = re.sub('DEFINES:=', 'DEFINES:=%s' % ' '.join(bld.env.DEFINES), s)
		s = re.sub('INCLUDES:=', 'INCLUDES:=%s' % ' '.join(bld.env.INCLUDES), s)
		s = re.sub('==MODULES==', self.get_modules(), s)
		s = re.sub('==MODPATHS==', self.get_modpaths(), s)
		s = re.sub('==MODDEPS==', self.get_moddeps(), s)
		s = re.sub('==VARIANT==', '_%s' % var if var else '', s)
		s = re.sub(bld.env.PREFIX, '$(PREFIX)', s)
		s = re.sub('PREFIX:=', 'PREFIX:=%s' % prefix, s)
		return s

	def add_child(self, child):
		self.childs.append(child)
	
	def get_modules(self):
		d = []
		for (name, _, _) in self.childs:
			d.append(name)
		s = ' \\\n\t'.join(d)
		return s

	def get_modpaths(self):
		d = []
		for (name, makefile, _) in self.childs:
			s = '%s;%s' %(name, os.path.dirname(makefile))
			d.append(s)
		s = ' \\\n\t'.join(d)
		return s

	def get_moddeps(self):
		d = []
		names = [c[0] for c in self.childs]
		for (name, _, deps) in self.childs:
			s = '%s;' %(name)
			for dep in deps:
				if dep in names:
					s += ',%s' % dep
			d.append(s)
		s = ' \\\n\t'.join(d)
		return s


class MakeChild(Make):
	def __init__(self, bld, gen, targets):
		super(MakeChild, self).__init__(bld)
		self.gen = gen
		self.targets = targets
		self.process()

	def get_name(self):
		gen = self.gen
		if self.bld.variant:
			name = '%s/%s_%s.mk' % (gen.path.relpath(), gen.get_name(), self.bld.variant)
		else:
			name = '%s/%s.mk' % (gen.path.relpath(), gen.get_name())
		return name.replace('\\', '/')

	def get_content(self):
		features = list(self.gen.features)
		if features == ['cxx']:
			features.append('cxxstlib')
		elif features == ['c']:
			features.append('cstlib')

		s = ''
		for feature in features:
			if hasattr(self, feature):
				func = getattr(self, feature)
				s += func(self.bld, self.gen)
				break
		
		if s == '':
			self.bld.fatal('FAILED(%s) features=%s' % (self.gen.get_name(), self.gen.features))

		loc = 'build'
		if self.bld.variant:
			loc += '/%s' % self.bld.variant
		install = MAKEFILE_LOC % (loc, loc)
		return re.sub('==INSTALL==', install, s)
		
	def get_data(self):
		gen = self.gen
		name = gen.get_name()
		makefile = self.get_name()
		deps = Utils.to_list(getattr(gen, 'use', []))
		return (name, makefile, deps)
	
	def get_libs(self, target, nest):
		'''returns library information for this target and any target it depends on

		returns a list of tuples;
			(nest, name, paths, kind)

			nest: nesting level (int)
			name: library name (str)
			paths: list of search paths or None
			kind: library type ('shared' OR 'static')
		'''
		try:
			tg = self.bld.get_tgen_by_name(target)
		except Errors.WafError:
			return []
		name = tg.get_name()
		features = Utils.to_list(getattr(tg, 'features', []))
		if 'fake_lib' in features:
			paths = [p.replace('\\', '/') for p in tg.lib_paths]
			return [(nest, name, paths, 'shared')]
		if not set(('c','cxx')) & set(features):
			return []
		paths = [tg.path.relpath().replace('\\','/')]
		if set(('cshlib', 'cxxshlib')) & set(features):
			return [(nest, name, paths, 'shared')]
		libs = [(nest, name, paths, 'static')]
		for lib in Utils.to_list(getattr(tg, 'lib', [])):
			libs.append((nest, lib, None, 'shared'))
		for use in getattr(tg, 'use', []):
			libs.extend(self.get_libs(use, nest+1))
		return libs

	def get_includes(self, gen):
		'''returns the include paths for the given task generator.
		'''
		includes = self.get_genlist(gen, 'includes')
		for use in getattr(gen, 'use', []):
			key = 'INCLUDES_%s' % use
			try:
				tg = self.bld.get_tgen_by_name(use)
				if 'fake_lib' in tg.features:
					if key in gen.env:
						includes.extend(gen.env[key])
			except Errors.WafError:
				if key in gen.env:
					includes.extend(gen.env[key])
		return includes

	def process(self):
		self.lib = {}
		self.lib['static'] = { 'name' : [], 'path' : [] }
		self.lib['shared'] = { 'name' : [], 'path' : [] }

		# construct list of (lib) dependencies and sort by task
		# with highest nesting
		uses = []
		for use in Utils.to_list(getattr(self.gen, 'use', [])):
			uses.extend(self.get_libs(use, 0))
		uses = sorted(uses, key=lambda use: use[0])
		uses = uses[::-1]

		# insert tasks at start of list, lowest nesting at start
		for (_, lib, paths, kind) in uses:
			self.lib[kind]['name'].insert(0, lib)
			if paths:
				self.lib[kind]['path'].extend(paths)
		for lib in Utils.to_list(getattr(self.gen, 'lib', [])):
			self.lib['shared']['name'].append(lib)

	def cprogram(self, bld, gen):
		'''returns the makefile content for a C program.
		'''
		source = self.get_genlist(gen, 'source')
		includes = self.get_includes(gen)
		defines = self.get_defines(gen)
		defines = [d for d in defines]
		s = MAKEFILE_CPROGRAM
		s = super(MakeChild, self).populate(s)
		name = bld.env.cprogram_PATTERN % gen.get_name()
		s = re.sub('BIN=', 'BIN=%s' % name, s)
		s = re.sub('SOURCES=', 'SOURCES= \\\n\t%s' % ' \\\n\t'.join(source), s)
		s = re.sub('INCLUDES\+=', 'INCLUDES+= \\\n\t%s' % ' \\\n\t'.join(includes),s)
		s = re.sub('DEFINES\+=', 'DEFINES+=%s' % ' '.join(defines),s)
		s = re.sub('CFLAGS\+=', 'CFLAGS+=%s' % self.get_cflags(gen),s)
		s = re.sub('LINKFLAGS\+=', 'LINKFLAGS+=%s' % self.get_linkflags(gen),s)
		s = re.sub('LIBPATH_ST\+=', 'LIBPATH_ST+=%s' % self.get_libpath('static'),s)
		s = re.sub('LIB_ST\+=', 'LIB_ST+=%s' % self.get_lib('static'),s)
		s = re.sub('LIBPATH_SH\+=', 'LIBPATH_SH+=%s' % self.get_libpath('shared'),s)
		s = re.sub('LIB_SH\+=', 'LIB_SH+=%s' % self.get_lib('shared'),s)
		return s

	def cxxprogram(self, bld, gen):
		'''returns the makefile content for a C++ program.
		'''
		source = self.get_genlist(gen, 'source')
		includes = self.get_includes(gen)
		defines = self.get_defines(gen)
		defines = [d for d in defines]
		s = MAKEFILE_CXXPROGRAM
		s = super(MakeChild, self).populate(s)
		name = bld.env.cxxprogram_PATTERN % gen.get_name()
		s = re.sub('BIN=', 'BIN=%s' % name, s)
		s = re.sub('SOURCES=', 'SOURCES= \\\n\t%s' % ' \\\n\t'.join(source), s)
		s = re.sub('INCLUDES\+=', 'INCLUDES+= \\\n\t%s' % ' \\\n\t'.join(includes),s)
		s = re.sub('DEFINES\+=', 'DEFINES+=%s' % ' '.join(defines),s)
		s = re.sub('CXXFLAGS\+=', 'CXXFLAGS+=%s' % self.get_cxxflags(gen),s)
		s = re.sub('LINKFLAGS\+=', 'LINKFLAGS+=%s' % self.get_linkflags(gen),s)
		s = re.sub('LIBPATH_ST\+=', 'LIBPATH_ST+=%s' % self.get_libpath('static'),s)
		s = re.sub('LIB_ST\+=', 'LIB_ST+=%s' % self.get_lib('static'),s)
		s = re.sub('LIBPATH_SH\+=', 'LIBPATH_SH+=%s' % self.get_libpath('shared'),s)
		s = re.sub('LIB_SH\+=', 'LIB_SH+=%s' % self.get_lib('shared'),s)
		return s

	def cstlib(self, bld, gen):
		'''returns the makefile content for a C static library.
		'''
		source = self.get_genlist(gen, 'source')
		includes = self.get_genlist(gen, 'includes')
		defines = self.get_defines(gen)
		defines = [d for d in defines]
		s = MAKEFILE_CSTLIB
		s = super(MakeChild, self).populate(s)
		name = bld.env.cstlib_PATTERN % gen.get_name()
		s = re.sub('LIB=', 'LIB=%s' % name, s)
		s = re.sub('SOURCES=', 'SOURCES= \\\n\t%s' % ' \\\n\t'.join(source), s)
		s = re.sub('INCLUDES\+=', 'INCLUDES+= \\\n\t%s' % ' \\\n\t'.join(includes),s)
		s = re.sub('DEFINES\+=', 'DEFINES+=%s' % ' '.join(defines),s)
		s = re.sub('CFLAGS\+=', 'CFLAGS+=%s' % self.get_cflags(gen),s)
		s = re.sub('ARFLAGS=', 'ARFLAGS=%s' % self.get_arflags(gen), s)
		return s

	def cshlib(self, bld, gen):
		'''returns the makefile content for a C shared library.
		'''
		source = self.get_genlist(gen, 'source')
		includes = self.get_genlist(gen, 'includes')
		defines = self.get_defines(gen)
		defines = [d for d in defines]
		s = MAKEFILE_CSHLIB
		s = super(MakeChild, self).populate(s)
		name = bld.env.cshlib_PATTERN % gen.get_name()
		vnum = getattr(gen, 'vnum', '')
		s = re.sub('LIB=', 'LIB=%s' % name, s)
		s = re.sub('VNUM=', 'VNUM=%s' % vnum, s)
		s = re.sub('SOURCES=', 'SOURCES= \\\n\t%s' % ' \\\n\t'.join(source), s)
		s = re.sub('INCLUDES\+=', 'INCLUDES+= \\\n\t%s' % ' \\\n\t'.join(includes),s)
		s = re.sub('DEFINES\+=', 'DEFINES+=%s' % ' '.join(defines),s)
		s = re.sub('CFLAGS\+=', 'CFLAGS+=%s' % self.get_cflags(gen),s)
		s = re.sub('LINKFLAGS\+=', 'LINKFLAGS+=%s' % self.get_linkflags(gen),s)
		s = re.sub('LIBPATH_ST\+=', 'LIBPATH_ST+=%s' % self.get_libpath('static'),s)
		s = re.sub('LIB_ST\+=', 'LIB_ST+=%s' % self.get_lib('static'),s)
		s = re.sub('LIBPATH_SH\+=', 'LIBPATH_SH+=%s' % self.get_libpath('shared'),s)
		s = re.sub('LIB_SH\+=', 'LIB_SH+=%s' % self.get_lib('shared'),s)
		return s

	def cxxstlib(self, bld, gen):
		'''returns the makefile content for a C++ static library.
		'''
		source = self.get_genlist(gen, 'source')
		includes = self.get_genlist(gen, 'includes')
		defines = self.get_defines(gen)
		defines = [d for d in defines]
		s = MAKEFILE_CXXSTLIB
		s = super(MakeChild, self).populate(s)
		name = bld.env.cxxstlib_PATTERN % gen.get_name()
		s = re.sub('LIB=', 'LIB=%s' % name, s)
		s = re.sub('SOURCES=', 'SOURCES= \\\n\t%s' % ' \\\n\t'.join(source), s)
		s = re.sub('INCLUDES\+=', 'INCLUDES+= \\\n\t%s' % ' \\\n\t'.join(includes),s)
		s = re.sub('DEFINES\+=', 'DEFINES+=%s' % ' '.join(defines),s)
		s = re.sub('CXXFLAGS\+=', 'CXXFLAGS+=%s' % self.get_cxxflags(gen),s)
		s = re.sub('ARFLAGS=', 'ARFLAGS=%s' % self.get_arflags(gen), s)
		return s

	def cxxshlib(self, bld, gen):
		'''returns the makefile content for a C++ shared library.
		'''
		source = self.get_genlist(gen, 'source')
		includes = self.get_genlist(gen, 'includes')
		defines = self.get_defines(gen)
		defines = [d for d in defines]
		s = MAKEFILE_CXXSHLIB
		s = super(MakeChild, self).populate(s)
		name = bld.env.cxxshlib_PATTERN % gen.get_name()
		vnum = getattr(gen, 'vnum', '')
		s = re.sub('LIB=', 'LIB=%s' % name, s)
		s = re.sub('VNUM=', 'VNUM=%s' % vnum, s)
		s = re.sub('SOURCES=', 'SOURCES= \\\n\t%s' % ' \\\n\t'.join(source), s)
		s = re.sub('INCLUDES\+=', 'INCLUDES+= \\\n\t%s' % ' \\\n\t'.join(includes),s)
		s = re.sub('DEFINES\+=', 'DEFINES+=%s' % ' '.join(defines),s)
		s = re.sub('CXXFLAGS\+=', 'CXXFLAGS+=%s' % self.get_cxxflags(gen),s)
		s = re.sub('LINKFLAGS\+=', 'LINKFLAGS+=%s' % self.get_linkflags(gen),s)
		s = re.sub('LIBPATH_ST\+=', 'LIBPATH_ST+=%s' % self.get_libpath('static'),s)
		s = re.sub('LIB_ST\+=', 'LIB_ST+=%s' % self.get_lib('static'),s)
		s = re.sub('LIBPATH_SH\+=', 'LIBPATH_SH+=%s' % self.get_libpath('shared'),s)
		s = re.sub('LIB_SH\+=', 'LIB_SH+=%s' % self.get_lib('shared'),s)
		return s

	def get_genlist(self, gen, name):
		lst = Utils.to_list(getattr(gen, name, []))
		lst = [str(l.path_from(gen.path)) if hasattr(l, 'path_from') else l for l in lst]
		return [l.replace('\\', '/') for l in lst]

	def get_defines(self, gen):
		defines = []
		defs = self.get_genlist(gen, 'defines')
		for d in defs:
			if d.count('"') == 2:
				(pre, val, post) = d.split('"')
				d = '%s\'"%s"\'%s' % (pre, val, post) 
			defines.append(d)
		return defines

	def get_cflags(self, gen):
		cflags = getattr(gen, 'cflags', [])
		if isinstance(cflags, str):
			cflags = cflags.split()
		if 'cshlib' in gen.features:
			cflags.extend(self.bld.env.CFLAGS_cshlib)
		return ' '.join(cflags)

	def get_cxxflags(self, gen):
		cxxflags = getattr(gen, 'cxxflags', [])
		if isinstance(cxxflags, str):
			cxxflags = cxxflags.split()
		if 'cxxshlib' in gen.features:
			cxxflags.extend(self.bld.env.CXXFLAGS_cxxshlib)
		return ' '.join(cxxflags)

	def get_linkflags(self, gen):
		linkflags = getattr(gen, 'linkflags', [])
		if 'cshlib' in gen.features:
			linkflags.extend(self.bld.env.LINKFLAGS_cshlib)
		if 'cxxshlib' in gen.features:
			linkflags.extend(self.bld.env.LINKFLAGS_cxxshlib)
		return ' '.join(linkflags)

	def get_arflags(self, gen):
		flags = Utils.to_list(self.bld.env.ARFLAGS)
		return ' '.join(flags)

	def get_libpath(self, kind):
		paths = self.lib[kind]['path']
		top = self.bld.path.abspath().replace('\\', '/')
		libpath = []
		for p in paths:
			if os.path.abspath(p) and p.startswith(top):
				libpath.append(p.replace(top, '$(TOP)', 1))
			elif self.bld.variant:
				libpath.append('$(TOP)/build/%s/%s' % (self.bld.variant, p))
			else:
				libpath.append('$(TOP)/build/%s' % p)
		return ' \\\n\t'.join(libpath)

	def get_lib(self, kind):
		lib = self.lib[kind]['name']
		return ' '.join(lib)


MAKEFILE_ROOT = \
'''#------------------------------------------------------------------------------
# WAFTOOLS generated makefile
# version: ==VERSION==
# waf: ==WAFVERSION==
#------------------------------------------------------------------------------

SHELL=/bin/sh

# commas, spaces and tabs:
sp:= 
sp+= 
tab:=$(sp)$(sp)$(sp)$(sp)
comma:=,

# token for separating dictionary keys and values:
dsep:=;

# token for separating list elements:
lsep:=,

# token for joining command and component names (e.g. 'build.hello')
csep:=.

export APPNAME:=
export APPVERSION:=
export PREFIX:=
export TOP:=$(CURDIR)
export OUT:=
export AR:=
export CC:=
export CXX:=
export CFLAGS:=
export CXXFLAGS:=
export DEFINES:=
export RPATH:=
export BINDIR:=
export LIBDIR:=
export INCLUDES:=

SEARCHPATH=components/
SEARCHFILE=Makefile

#------------------------------------------------------------------------------
# list of unique logical module names;
modules= \\
	==MODULES==

# dictionary of modules names (key) and paths to modules;
paths= \\
	==MODPATHS==

# dictionary of modules names (key) and module dependencies;
deps= \\
	==MODDEPS==

#------------------------------------------------------------------------------
# define targets
#------------------------------------------------------------------------------
build_targets=$(addprefix build$(csep),$(modules))
clean_targets=$(addprefix clean$(csep),$(modules))
install_targets=$(addprefix install$(csep),$(modules))
uninstall_targets=$(addprefix uninstall$(csep),$(modules))

cmds=build clean install uninstall
commands=$(sort $(cmds) all help find list modules $(foreach prefix,$(cmds),$($(prefix)_targets)))

.DEFAULT_GOAL:=all

#------------------------------------------------------------------------------
# recursive wild card implementation
#------------------------------------------------------------------------------
define rwildcard
$(wildcard $1$2) $(foreach d,$(wildcard $1*),$(call rwildcard,$d/,$2))
endef

#------------------------------------------------------------------------------
# returns the value from a dictionary
# $1 = key, where key is the functional name of the component.
# $2 = dictionary
#------------------------------------------------------------------------------
define getdval
$(subst $(lastword $(subst $(csep),$(sp),$1))$(dsep),$(sp),$(filter $(lastword $(subst $(csep),$(sp),$1))$(dsep)%,$2))
endef

#------------------------------------------------------------------------------
# returns path to makefile
# $1 = key, where key is the functional name of the component.
#------------------------------------------------------------------------------
define getpath
$(call getdval, $1, $(paths))
endef

#------------------------------------------------------------------------------
# returns component dependencies.
# $1 = key, where key is the functional name of the component.
#------------------------------------------------------------------------------
define getdeps
$(addprefix $(firstword $(subst $(csep),$(sp),$1))$(csep),$(subst $(lsep),$(sp),$(call getdval, $1, $(deps))))
endef

#------------------------------------------------------------------------------
# creates a make recipe:
#      'make -r -C <path> -f <name>.mk <command>'
# where:
#      <path>     is the relative path to the component
#      <name>     is the name of the component
#      <command>  is the make action to be executed, e.g. build, install, clean
#
# $1 = key, where key is the functional recipe name (e.g. build.a).
#------------------------------------------------------------------------------
define domake
$1: $(call getdeps, $1)
	$(MAKE) -r -C $(call getpath,$1) -f $(lastword $(subst $(csep),$(sp),$1))==VARIANT==.mk $(firstword $(subst $(csep),$(sp),$1))
endef

#------------------------------------------------------------------------------
# return files found in given search path
# $1 = search path
# $2 = file name so search
#------------------------------------------------------------------------------
define dofind
$(foreach path, $(dir $(call rwildcard,$1,$2)),echo "  $(path)";)
endef

#------------------------------------------------------------------------------
# definitions of recipes (i.e. make targets)
#------------------------------------------------------------------------------
all: build

build: $(build_targets)

clean: $(clean_targets)

install: build $(install_targets)

uninstall: $(uninstall_targets)

list:
	@echo ""
	@$(foreach cmd,$(commands),echo "  $(cmd)";)
	@echo ""

modules:
	@echo ""
	@$(foreach module,$(modules),echo "  $(module)";)
	@echo ""

find:
	@echo ""
	@echo "$@:"
	@echo "  path=$(SEARCHPATH) file=$(SEARCHFILE)"
	@echo ""
	@echo "result:"
	@$(call dofind,$(SEARCHPATH),$(SEARCHFILE))
	@echo ""

help:
	@echo ""
	@echo "$(APPNAME) version $(APPVERSION)"
	@echo ""
	@echo "usage:"
	@echo "  make [-r] [-s] [--jobs=N] [command] [VARIABLE=VALUE]"
	@echo ""
	@echo "commands:"
	@echo "  all                                 builds all modules"
	@echo "  build                               builds all modules"
	@echo "  build$(csep)a                       builds module 'a' and it's dependencies"
	@echo "  clean                               removes all build intermediates and outputs"
	@echo "  clean$(csep)a                       cleans module 'a' and it's dependencies"
	@echo "  install                             installs files in $(PREFIX)"
	@echo "  install$(csep)a                     installs module 'a' and it's dependencies"
	@echo "  uninstall                           removes all installed files from $(PREFIX)"
	@echo "  uninstall$(csep)a                   removes module 'a' and it's dependencies"
	@echo "  list                                list available make commands (i.e. recipes)"
	@echo "  modules                             list logical names of all modules"
	@echo "  find [SEARCHPATH=] [SEARCHFILE=]    searches for files default(path=$(SEARCHPATH),file=$(SEARCHFILE))"
	@echo "  help                                displays this help message."
	@echo ""
	@echo "remarks:"
	@echo "  use options '-r' and '--jobs=N' in order to improve speed"
	@echo "  use options '-s' to decrease verbosity"
	@echo ""

$(foreach t,$(build_targets),$(eval $(call domake,$t)))

$(foreach t,$(clean_targets),$(eval $(call domake,$t)))

$(foreach t,$(install_targets),$(eval $(call domake,$t)))

$(foreach t,$(uninstall_targets),$(eval $(call domake,$t)))

.PHONY: $(commands)

'''

MAKEFILE_CHILD = \
'''#------------------------------------------------------------------------------
# WAFTOOLS generated makefile
# version: ==VERSION==
# waf: ==WAFVERSION==
#------------------------------------------------------------------------------

commands:= build clean install uninstall all

all: build

build:
	@echo BUILD $(abspath $(@D))

clean:
	@echo CLEAN $(abspath $(@D))

install:
	@echo INSTALL $(abspath $(@D))
	
uninstall:
	@echo UNINSTALL $(abspath $(@D))

.PHONY: $(commands)

'''

MAKEFILE_LOC = \
'''
ifeq ($(TOP),)
TOP=$(CURDIR)
OUT=$(TOP)/%s
else
OUT=$(subst $(sp),/,$(call rptotop) %s $(call rpofcomp))
endif
'''

MAKEFILE_CPROGRAM = \
'''#------------------------------------------------------------------------------
# WAFTOOLS generated makefile
# version: ==VERSION==
# waf: ==WAFVERSION==
#------------------------------------------------------------------------------

SHELL=/bin/sh

# commas, spaces and tabs:
sp:= 
sp+= 
tab:=$(sp)$(sp)$(sp)$(sp)
comma:=,

#------------------------------------------------------------------------------
# definition of build and install locations
#------------------------------------------------------------------------------
==INSTALL==

PREFIX?=$(HOME)
BINDIR?=$(PREFIX)/bin
LIBDIR?=$(PREFIX)/lib

#------------------------------------------------------------------------------
# component data
#------------------------------------------------------------------------------
BIN=
OUTPUT=$(OUT)/$(BIN)

# REMARK: use $(wildcard src/*.c) to include all sources.
SOURCES=

OBJECTS=$(SOURCES:.c=.1.o)

DEFINES+=
DEFINES:=$(addprefix -D,$(DEFINES))

INCLUDES+=

HEADERS:=$(foreach inc,$(INCLUDES),$(wildcard $(inc)/*.h))
INCLUDES:=$(addprefix -I,$(INCLUDES))

CFLAGS+=

LINKFLAGS+=

RPATH+=
RPATH:= $(addprefix -Wl$(comma)-rpath$(comma),$(RPATH))

LIBPATH_ST+=
LIBPATH_ST:= $(addprefix -L,$(LIBPATH_ST))

LIB_ST+=
LIB_ST:= $(addprefix -l,$(LIB_ST))

LIBPATH_SH+=
LIBPATH_SH:= $(addprefix -L,$(LIBPATH_SH))

LINK_ST= -Wl,-Bstatic $(LIBPATH_ST) $(LIB_ST)

LIB_SH+=
LIB_SH:= $(addprefix -l,$(LIB_SH))

LINK_SH= -Wl,-Bdynamic $(LIBPATH_SH) $(LIB_SH)

#------------------------------------------------------------------------------
# returns the relative path of this component from the top directory
#------------------------------------------------------------------------------
define rpofcomp
$(subst $(subst ~,$(HOME),$(TOP))/,,$(CURDIR))
endef

#------------------------------------------------------------------------------
# returns the relative path of this component to the top directory
#------------------------------------------------------------------------------
define rptotop
$(foreach word,$(subst /,$(sp),$(call rpofcomp)),..)
endef

#------------------------------------------------------------------------------
# define targets
#------------------------------------------------------------------------------
commands= build clean install uninstall all

.DEFAULT_GOAL=all

#------------------------------------------------------------------------------
# definitions of recipes (i.e. make targets)
#------------------------------------------------------------------------------
all: build

build: $(OBJECTS)
	$(CC) $(LINKFLAGS) $(addprefix $(OUT)/,$(OBJECTS)) -o $(OUTPUT) $(RPATH) $(LINK_ST) $(LINK_SH)

clean:
	$(foreach obj,$(OBJECTS),rm -f $(OUT)/$(obj);)
	rm -f $(OUTPUT)

install: build
	mkdir -p $(BINDIR)
	cp $(OUTPUT) $(BINDIR)

uninstall:
	rm -f $(BINDIR)/$(BIN)

$(OBJECTS): $(HEADERS)
	mkdir -p $(OUT)/$(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) $(subst .1.o,.c,$@) -c -o $(OUT)/$@

.PHONY: $(commands)

'''

MAKEFILE_CXXPROGRAM = \
'''#------------------------------------------------------------------------------
# WAFTOOLS generated makefile
# version: ==VERSION==
# waf: ==WAFVERSION==
#------------------------------------------------------------------------------

SHELL=/bin/sh

# commas, spaces and tabs:
sp:= 
sp+= 
tab:=$(sp)$(sp)$(sp)$(sp)
comma:=,

#------------------------------------------------------------------------------
# definition of build and install locations
#------------------------------------------------------------------------------
==INSTALL==

PREFIX?=$(HOME)
BINDIR?=$(PREFIX)/bin
LIBDIR?=$(PREFIX)/lib

#------------------------------------------------------------------------------
# component data
#------------------------------------------------------------------------------
BIN=
OUTPUT=$(OUT)/$(BIN)

SOURCES=

OBJECTS=$(SOURCES:.cpp=.1.o)

DEFINES+=
DEFINES:=$(addprefix -D,$(DEFINES))

INCLUDES+=

HEADERS:=$(foreach inc,$(INCLUDES),$(wildcard $(inc)/*.h))
INCLUDES:=$(addprefix -I,$(INCLUDES))

CXXFLAGS+=

LINKFLAGS+=

RPATH+=
RPATH:= $(addprefix -Wl$(comma)-rpath$(comma),$(RPATH))

LIBPATH_ST+=
LIBPATH_ST:= $(addprefix -L,$(LIBPATH_ST))

LIB_ST+=
LIB_ST:= $(addprefix -l,$(LIB_ST))

LIBPATH_SH+=
LIBPATH_SH:= $(addprefix -L,$(LIBPATH_SH))

LINK_ST= -Wl,-Bstatic $(LIBPATH_ST) $(LIB_ST)

LIB_SH+=
LIB_SH:= $(addprefix -l,$(LIB_SH))

LINK_SH= -Wl,-Bdynamic $(LIBPATH_SH) $(LIB_SH)

#------------------------------------------------------------------------------
# returns the relative path of this component from the top directory
#------------------------------------------------------------------------------
define rpofcomp
$(subst $(subst ~,$(HOME),$(TOP))/,,$(CURDIR))
endef

#------------------------------------------------------------------------------
# returns the relative path of this component to the top directory
#------------------------------------------------------------------------------
define rptotop
$(foreach word,$(subst /,$(sp),$(call rpofcomp)),..)
endef

#------------------------------------------------------------------------------
# define targets
#------------------------------------------------------------------------------
commands= build clean install uninstall all

.DEFAULT_GOAL=all

#------------------------------------------------------------------------------
# definitions of recipes (i.e. make targets)
#------------------------------------------------------------------------------
all: build

build: $(OBJECTS)
	$(CXX) $(LINKFLAGS) $(addprefix $(OUT)/,$(OBJECTS)) -o $(OUTPUT) $(RPATH) $(LINK_ST) $(LINK_SH)

clean:
	$(foreach obj,$(OBJECTS),rm -f $(OUT)/$(obj);)
	rm -f $(OUTPUT)

install: build
	mkdir -p $(BINDIR)
	cp $(OUTPUT) $(BINDIR)
	
uninstall:
	rm -f $(BINDIR)/$(BIN)

$(OBJECTS): $(HEADERS)
	mkdir -p $(OUT)/$(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) $(subst .1.o,.cpp,$@) -c -o $(OUT)/$@

.PHONY: $(commands)

'''

MAKEFILE_CSTLIB = \
'''#------------------------------------------------------------------------------
# WAFTOOLS generated makefile
# version: ==VERSION==
# waf: ==WAFVERSION==
#------------------------------------------------------------------------------

SHELL=/bin/sh

# commas, spaces and tabs:
sp:= 
sp+= 
tab:=$(sp)$(sp)$(sp)$(sp)
comma:=,

#------------------------------------------------------------------------------
# definition of build and install locations
#------------------------------------------------------------------------------
==INSTALL==

#------------------------------------------------------------------------------
# component data
#------------------------------------------------------------------------------
LIB=
OUTPUT=$(OUT)/$(LIB)

# REMARK: use $(wildcard src/*.c) to include all sources.
SOURCES= 

OBJECTS=$(SOURCES:.c=.1.o)

DEFINES+=
DEFINES:=$(addprefix -D,$(DEFINES))

INCLUDES+=

HEADERS:=$(foreach inc,$(INCLUDES),$(wildcard $(inc)/*.h))
INCLUDES:=$(addprefix -I,$(INCLUDES))

CFLAGS+=

ARFLAGS=

#------------------------------------------------------------------------------
# returns the relative path of this component from the top directory
#------------------------------------------------------------------------------
define rpofcomp
$(subst $(subst ~,$(HOME),$(TOP))/,,$(CURDIR))
endef

#------------------------------------------------------------------------------
# returns the relative path of this component to the top directory
#------------------------------------------------------------------------------
define rptotop
$(foreach word,$(subst /,$(sp),$(call rpofcomp)),..)
endef

#------------------------------------------------------------------------------
# define targets
#------------------------------------------------------------------------------
commands= build clean install uninstall all

.DEFAULT_GOAL=all

#------------------------------------------------------------------------------
# definitions of recipes (i.e. make targets)
#------------------------------------------------------------------------------
all: build

build: $(OBJECTS)
	$(AR) $(ARFLAGS) $(OUTPUT) $(addprefix $(OUT)/,$(OBJECTS))

clean:
	$(foreach obj,$(OBJECTS),rm -f $(OUT)/$(obj);)
	rm -f $(OUTPUT)

install:
	
uninstall:

$(OBJECTS): $(HEADERS)
	mkdir -p $(OUT)/$(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) $(subst .1.o,.c,$@) -c -o $(OUT)/$@

.PHONY: $(commands)

'''

MAKEFILE_CSHLIB = \
'''#------------------------------------------------------------------------------
# WAFTOOLS generated makefile
# version: ==VERSION==
# waf: ==WAFVERSION==
#------------------------------------------------------------------------------

SHELL=/bin/sh

# commas, spaces and tabs:
sp:= 
sp+= 
tab:=$(sp)$(sp)$(sp)$(sp)
comma:=,

#------------------------------------------------------------------------------
# definition of build and install locations
#------------------------------------------------------------------------------
==INSTALL==

PREFIX?=$(HOME)
LIBDIR?=$(PREFIX)/lib

#------------------------------------------------------------------------------
# component data
#------------------------------------------------------------------------------
LIB=
OUTPUT=$(OUT)/$(LIB)

VNUM=

# REMARK: use $(wildcard src/*.c) to include all sources.
SOURCES= 

OBJECTS=$(SOURCES:.c=.1.o)

DEFINES+=
DEFINES:=$(addprefix -D,$(DEFINES))

INCLUDES+=

HEADERS:=$(foreach inc,$(INCLUDES),$(wildcard $(inc)/*.h))
INCLUDES:=$(addprefix -I,$(INCLUDES))

CFLAGS+= 

LINKFLAGS+= 

RPATH+=
RPATH:= $(addprefix -Wl$(comma)-rpath$(comma),$(RPATH))

LIBPATH_ST+=
LIBPATH_ST:= $(addprefix -L,$(LIBPATH_ST))

LIB_ST+=
LIB_ST:= $(addprefix -l,$(LIB_ST))

LIBPATH_SH+=
LIBPATH_SH:= $(addprefix -L,$(LIBPATH_SH))

LINK_ST= -Wl,-Bstatic $(LIBPATH_ST) $(LIB_ST)

LIB_SH+=
LIB_SH:= $(addprefix -l,$(LIB_SH))

LINK_SH= -Wl,-Bdynamic $(LIBPATH_SH) $(LIB_SH)

#------------------------------------------------------------------------------
# returns the relative path of this component from the top directory
#------------------------------------------------------------------------------
define rpofcomp
$(subst $(subst ~,$(HOME),$(TOP))/,,$(CURDIR))
endef

#------------------------------------------------------------------------------
# returns the relative path of this component to the top directory
#------------------------------------------------------------------------------
define rptotop
$(foreach word,$(subst /,$(sp),$(call rpofcomp)),..)
endef

#------------------------------------------------------------------------------
# define targets
#------------------------------------------------------------------------------
commands= build clean install uninstall all

.DEFAULT_GOAL=all

#------------------------------------------------------------------------------
# definitions of recipes (i.e. make targets)
#------------------------------------------------------------------------------
all: build

build: $(OBJECTS)
	$(CC) $(LINKFLAGS) $(addprefix $(OUT)/,$(OBJECTS)) -o $(OUTPUT) $(RPATH) $(LINK_ST) $(LINK_SH)

clean:
	$(foreach obj,$(OBJECTS),rm -f $(OUT)/$(obj);)
	rm -f $(OUTPUT)

install: build
	mkdir -p $(LIBDIR)
	cp $(OUTPUT) $(LIBDIR)
ifneq ($(VNUM),)
	ln -s -f $(LIBDIR)/$(LIB) $(LIBDIR)/$(LIB).$(VNUM)
endif

uninstall:
ifneq ($(VNUM),)
	rm -f $(LIBDIR)/$(LIB).$(VNUM)
endif
	rm -f $(LIBDIR)/$(LIB)

$(OBJECTS): $(HEADERS)
	mkdir -p $(OUT)/$(dir $@)
	$(CC) $(CFLAGS) $(INCLUDES) $(DEFINES) $(subst .1.o,.c,$@) -c -o $(OUT)/$@

.PHONY: $(commands)

'''

MAKEFILE_CXXSHLIB = \
'''#------------------------------------------------------------------------------
# WAFTOOLS generated makefile
# version: ==VERSION==
# waf: ==WAFVERSION==
#------------------------------------------------------------------------------

SHELL=/bin/sh

# commas, spaces and tabs:
sp:= 
sp+= 
tab:=$(sp)$(sp)$(sp)$(sp)
comma:=,

#------------------------------------------------------------------------------
# definition of build and install locations
#------------------------------------------------------------------------------
==INSTALL==

PREFIX?=$(HOME)
LIBDIR?=$(PREFIX)/lib

#------------------------------------------------------------------------------
# component data
#------------------------------------------------------------------------------
LIB=
OUTPUT=$(OUT)/$(LIB)

VNUM=

# REMARK: use $(wildcard src/*.cpp) to include all sources.
SOURCES= 

OBJECTS=$(SOURCES:.cpp=.1.o)

DEFINES+=
DEFINES:=$(addprefix -D,$(DEFINES))

INCLUDES+=

HEADERS:=$(foreach inc,$(INCLUDES),$(wildcard $(inc)/*.h))
INCLUDES:=$(addprefix -I,$(INCLUDES))

CXXFLAGS+= 

LINKFLAGS+= 

RPATH+=
RPATH:= $(addprefix -Wl$(comma)-rpath$(comma),$(RPATH))

LIBPATH_ST+=
LIBPATH_ST:= $(addprefix -L,$(LIBPATH_ST))

LIB_ST+=
LIB_ST:= $(addprefix -l,$(LIB_ST))

LIBPATH_SH+=
LIBPATH_SH:= $(addprefix -L,$(LIBPATH_SH))

LINK_ST= -Wl,-Bstatic $(LIBPATH_ST) $(LIB_ST)

LIB_SH+=
LIB_SH:= $(addprefix -l,$(LIB_SH))

LINK_SH= -Wl,-Bdynamic $(LIBPATH_SH) $(LIB_SH)

#------------------------------------------------------------------------------
# returns the relative path of this component from the top directory
#------------------------------------------------------------------------------
define rpofcomp
$(subst $(subst ~,$(HOME),$(TOP))/,,$(CURDIR))
endef

#------------------------------------------------------------------------------
# returns the relative path of this component to the top directory
#------------------------------------------------------------------------------
define rptotop
$(foreach word,$(subst /,$(sp),$(call rpofcomp)),..)
endef

#------------------------------------------------------------------------------
# define targets
#------------------------------------------------------------------------------
commands= build clean install uninstall all

.DEFAULT_GOAL=all

#------------------------------------------------------------------------------
# definitions of recipes (i.e. make targets)
#------------------------------------------------------------------------------
all: build

build: $(OBJECTS)
	$(CXX) $(LINKFLAGS) $(addprefix $(OUT)/,$(OBJECTS)) -o $(OUTPUT) $(RPATH) $(LINK_ST) $(LINK_SH)

clean:
	$(foreach obj,$(OBJECTS),rm -f $(OUT)/$(obj);)
	rm -f $(OUTPUT)

install: build
	mkdir -p $(LIBDIR)
	cp $(OUTPUT) $(LIBDIR)
ifneq ($(VNUM),)
	ln -s -f $(LIBDIR)/$(LIB) $(LIBDIR)/$(LIB).$(VNUM)
endif

uninstall:
ifneq ($(VNUM),)
	rm -f $(LIBDIR)/$(LIB).$(VNUM)
endif
	rm -f $(LIBDIR)/$(LIB)

$(OBJECTS): $(HEADERS)
	mkdir -p $(OUT)/$(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) $(subst .1.o,.cpp,$@) -c -o $(OUT)/$@

.PHONY: $(commands)

'''

MAKEFILE_CXXSTLIB = \
'''#------------------------------------------------------------------------------
# WAFTOOLS generated makefile
# version: ==VERSION==
# waf: ==WAFVERSION==
#------------------------------------------------------------------------------

SHELL=/bin/sh

# commas, spaces and tabs:
sp:= 
sp+= 
tab:=$(sp)$(sp)$(sp)$(sp)
comma:=,

#------------------------------------------------------------------------------
# definition of build and install locations
#------------------------------------------------------------------------------
==INSTALL==

#------------------------------------------------------------------------------
# component data
#------------------------------------------------------------------------------
LIB=
OUTPUT=$(OUT)/$(LIB)

# REMARK: use $(wildcard src/*.cpp) to include all sources.
SOURCES= 

OBJECTS=$(SOURCES:.cpp=.1.o)

DEFINES+=
DEFINES:=$(addprefix -D,$(DEFINES))

INCLUDES+=

HEADERS:=$(foreach inc,$(INCLUDES),$(wildcard $(inc)/*.h))
INCLUDES:=$(addprefix -I,$(INCLUDES))

CXXFLAGS+=

ARFLAGS=

#------------------------------------------------------------------------------
# returns the relative path of this component from the top directory
#------------------------------------------------------------------------------
define rpofcomp
$(subst $(subst ~,$(HOME),$(TOP))/,,$(CURDIR))
endef

#------------------------------------------------------------------------------
# returns the relative path of this component to the top directory
#------------------------------------------------------------------------------
define rptotop
$(foreach word,$(subst /,$(sp),$(call rpofcomp)),..)
endef

#------------------------------------------------------------------------------
# define targets
#------------------------------------------------------------------------------
commands= build clean install uninstall all

.DEFAULT_GOAL=all

#------------------------------------------------------------------------------
# definitions of recipes (i.e. make targets)
#------------------------------------------------------------------------------
all: build

build: $(OBJECTS)
	$(AR) $(ARFLAGS) $(OUTPUT) $(addprefix $(OUT)/,$(OBJECTS))

clean:
	$(foreach obj,$(OBJECTS),rm -f $(OUT)/$(obj);)
	rm -f $(OUTPUT)

install:

uninstall:

$(OBJECTS): $(HEADERS)
	mkdir -p $(OUT)/$(dir $@)
	$(CXX) $(CXXFLAGS) $(INCLUDES) $(DEFINES) $(subst .1.o,.cpp,$@) -c -o $(OUT)/$@

.PHONY: $(commands)

'''
