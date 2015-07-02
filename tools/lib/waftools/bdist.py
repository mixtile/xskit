#!/usr/bin/env python
# -*- encoding: utf-8 -*-
# Michel Mooij, michel.mooij7@gmail.com

'''
Summary
-------
Creates archives (.tar.gz) and/or installers (e.g. NSIS) for end users 
containing all build results (i.e. artifacts).

Description
-----------
Using this tool, it is possible to create archives and installers containing
all files, libraries and binaries that are the results of the build and 
install process. The structure and prefix will be the same as has been defined
in the *waf* build environment.
At present the following distribution formats are supported:
- windows installers using NSIS 
- compressed tar.bz2 archives
- list; displays all files (including complete path) to be packaged

When selecting NSIS as distribution method, a *.nsi* script may be provided, if not 
a default *.nsi* script will be generated.


Usage
-----
For windows targets platforms installers can be created using the NullSoft
Installable Scripting system (*NSIS*). If no user defined .nsi script is 
provided a default one will be created in the top level directory of the 
build system.

The code snippet below presents a *wscript* that provides support for
creating installers using **NSIS**::

	import waftools

	def options(opt):
		opt.load('compiler_c')
		opt.load('bdist', tooldir=waftools.location)
	
	def configure(conf):
		conf.load('compiler_c')
		conf.load('bdist')

	def build(bld):
		bld.program(target='hello', source='hello.c')

Using this code snippet, a Windows installer can be created using
the following command::

	waf bdist --formats=nsis

'''



import shutil, os, sys, platform
from waflib import Build, Context, Scripting, Logs

BDIST_TYPES_DEFAULT='all'

def options(opt):
	'''Adds command line options to the *waf* build environment 

	:param opt: Options context from the *waf* build environment.
	:type opt: waflib.Options.OptionsContext
	'''
	opt.add_option('--bdist', 
		dest='bdist_types',
		default=BDIST_TYPES_DEFAULT,
		action='store',
		help='bdist types to create (default=all)')

	opt.add_option('--bdist_cleanup', 
		dest='bdist_cleanup',
		default=False,
		action='store_true',
		help='cleanup bdist staging area')

	opt.add_option('--nsis_script', 
		dest='nsis_script',
		default='install.nsi',
		action='store',
		help='nsis install script (default=install.nsi)')
		

def configure(conf):
	'''Method that will be invoked by *waf* when configuring the build 
	environment.
	
	:param conf: Configuration context from the *waf* build environment.
	:type conf: waflib.Configure.ConfigurationContext
	'''	
	conf.env.BDIST_TYPES = conf.options.bdist_types.split(',')
	conf.env.NSIS_SCRIPT = conf.options.nsis_script
	conf.find_program('makensis', var='NSIS', mandatory=False)


class BDistContext(Build.InstallContext):
	'''creates installers (e.g. NSIS) for end users.'''
	cmd = 'bdist'
	fun = 'build'

	def init_dirs(self, *k, **kw):
		super(BDistContext, self).init_dirs(*k, **kw)
		self._pkgroot = self.bldnode.make_node('.bdist')
		try:
			shutil.rmtree(self._pkgroot.abspath())
		except:
			pass
		if os.path.exists(self._pkgroot.abspath()):
			self.fatal('Could not remove temporary directory %r' % self._pkgroot)
		self._pkgroot.mkdir()
		self.options.destdir = self._pkgroot.abspath()

	def execute(self, *k, **kw):
		'''Executes normal 'install' into a special, temporary, bdist directory
		and creates a bdist (e.g. tar.bz2 or rpm) file containing all installed
		files into the bdist directory.
		'''
		super(BDistContext, self).execute(*k, **kw)

		prefix = str(self.env.PREFIX)
		i = prefix.find(':')
		if i >= 0 and (i+1) < len(prefix):
			prefix = prefix[i+1:]
		self._bdist = self._pkgroot.find_or_declare(prefix)

		version = getattr(Context.g_module, Context.VERSION, self.top_dir)
		appname = getattr(Context.g_module, Context.APPNAME, self.top_dir)
		variant = self.variant if self.variant else ''
		pkgtype = self.env.BDIST_TYPES
		files = self.get_files()

		pkg = self.options.bdist_types
		if pkg != BDIST_TYPES_DEFAULT:
			pkgtype = pkg.split(',')

		if set(pkgtype) & set(['all', 'ls']):
			self.bdist_ls(appname, variant, version, files)

		if set(pkgtype) & set(['all', 'tar.bz2']):
			self.bdist_tar_bz2(appname, variant, version)

		if set(pkgtype) & set(['all', 'nsis']):
			self.bdist_nsis(appname, version)

		if self.options.bdist_cleanup:
			shutil.rmtree(self._bdist.abspath())

	def get_files(self):
		'''returns a list of file names to be packaged from which the PREFIX
		path has been stripped.		
		'''
		prefix = self._bdist.relpath().replace('\\','/')
		files = []
		for node in self._bdist.ant_glob('**'):
			f = node.relpath().replace('\\','/')
			f = f[len(prefix):]
			files.append(f)
		return files

	def bdist_ls(self, appname, variant, version, files):
		'''Lists all files that will be packaged.

		:param appname: Functional application and bdist name
		:type appname: str
		:param variant: Name of the build variant (if any)
		:type variant: str or None
		:param version: Current version of the application being packaged
		:type version: str
		:param files: List of file nodes
		:type files: list
		'''
		p = Logs.info
		p('')
		p('=======================')
		p('BDIST   = ls')
		p('APPNAME = %s' % appname)
		p('VERSION = %s' % version)
		p('VARIANT = %s' % variant)
		p('PREFIX  = %s' % self.env.PREFIX)
		p('=======================')
		p('')
		for f in files:
			p('$PREFIX%s' % f)
		p('-----------------------')

	def bdist_tar_bz2(self, appname, variant, version):
		'''Creates a tar.gz archive.

		:param appname: Functional application and bdist name
		:type appname: str
		:param variant: Name of the build variant (if any)
		:type variant: str or None
		:param version: Current version of the application being packaged
		:type version: str
		'''	
	
		name = '%s%s-%s' % (appname, variant, version)
		p = Logs.info
		p('')
		p('=======================')
		p('BDIST (tar.bz2)')
		p('=======================')
		p('PREFIX=%s' % self.env.PREFIX)	
		ctx = Scripting.Dist()
		ctx.arch_name = '%s.tar.bz2' % (name)
		ctx.files = self._bdist.ant_glob('**')
		ctx.tar_prefix = ''
		ctx.base_path = self._pkgroot
		ctx.archive()
		p('-----------------------')

	def bdist_nsis(self, appname, version):
		'''Creates an installer for Windows hosts using NSIS.
		
		If the install script does not exist, a default install
		script will be created by this module.

		:param appname: Functional application and bdist name
		:type appname: str
		:param version: Current version of the application being packaged
		:type version: str
		'''	
		nsis = self.env.NSIS
		if isinstance(nsis, list):
			if not len(nsis):
				Logs.warn('NSIS not available, skipping')
				return
			nsis = nsis[0]
			
		fname = self.env.NSIS_SCRIPT
		script = self.path.find_node(fname)
		if not script:
			script = self.nsis_create_script(fname, appname)
		
		args = []
		args.append('/V4')
		args.append('/NOCD')
		args.append('/DVERSION=%s' % version)
		v = version.split('.')
		try:
			v_major = int(v[0])
			args.append('/DVER_MAJOR=%s' % v_major)
		except:
			pass
		try:
			v_minor = int(v[1])
			args.append('/DVER_MINOR=%s' % v_minor)
		except:
			pass
		try:
			v_patch = int(v[2])
			args.append('/DVER_PATCH=%s' % v_patch)
		except:
			pass

		arch = platform.architecture()			
		if sys.platform == 'win32' and arch[0] == '32bit':
			outfile = '%s-%s-win32-setup.exe' % (appname, version)
		else:
			outfile = '%s-%s-win64-setup.exe' % (appname, version)
		outfile = os.path.join(self.path.abspath(), outfile)
		args.append('/DINSTALLER=%s' % outfile)

		if sys.platform != 'win32':
			args = [a.replace('/','-',1) for a in args]

		cmd = '%s %s %s' % (nsis, ' '.join(args), script.abspath())
		cwd = self._bdist.abspath()
		stdout = self.cmd_and_log(cmd, output=Context.STDOUT, quiet=Context.STDOUT, cwd=cwd)
		Logs.info(stdout)

	def nsis_create_script(self, fname, appname):
		'''Creates and returns a NSIS installer script using the buildin template.

		:param fname: Name of the script file (including path).
		:type fname: str
		:param appname: Functional application and bdist name
		:type appname: str
		'''
		script = self.path.make_node(fname)
		s = _nsis_template % appname
		script.write(s)		
		return script

		
_nsis_template = r'''
!include "MUI2.nsh"
!include "x64.nsh"

!ifndef VER_MAJOR
!define VER_MAJOR			0
!endif

!ifndef VER_MINOR
!define VER_MINOR			0
!endif

!ifndef VER_PATCH
!define VER_PATCH			0
!endif

!ifndef VERSION
!define VERSION     		"${VER_MAJOR}.${VER_MINOR}.${VER_PATCH}"
!endif

!ifndef APPNAME
!define APPNAME				"%s"
!endif

!ifndef INSTALLER
!ifdef RunningX64
!define INSTALLER			"${APPNAME}-${VERSION}-win64-setup.exe"
!else
!define INSTALLER			"${APPNAME}-${VERSION}-win32-setup.exe"
!endif
!endif

!define REGKEY      		"Software\${APPNAME}"
!define UNINSTALL_REGKEY	"Software\Microsoft\Windows\CurrentVersion\Uninstall\${APPNAME}"

Name                    	"${APPNAME} ${VERSION}"
OutFile                 	"${INSTALLER}"
InstallDir              	"$PROGRAMFILES\${APPNAME}"
InstallDirRegKey        	HKCU "${REGKEY}" ""
RequestExecutionLevel   	admin
AutoCloseWindow         	false
ShowInstDetails         	show
ShowUnInstDetails       	show
CRCCheck                	On

!define MUI_ABORTWARNING
!define MUI_STARTMENUPAGE_REGISTRY_ROOT         HKCU
!define MUI_STARTMENUPAGE_REGISTRY_KEY          "${REGKEY}"
!define MUI_STARTMENUPAGE_REGISTRY_VALUENAME    "Start Menu Folder"  
!define MUI_VERSION                             "${VERSION}"
!define MUI_PRODUCT                             "${APPNAME} ${MUI_VERSION}"
!define MUI_BRANDINGTEXT                        ""

Var StartMenuFolder

!insertmacro MUI_PAGE_WELCOME
!insertmacro MUI_PAGE_DIRECTORY
!insertmacro MUI_PAGE_STARTMENU Application $StartMenuFolder
!insertmacro MUI_PAGE_INSTFILES
!insertmacro MUI_PAGE_FINISH
!insertmacro MUI_UNPAGE_WELCOME
!insertmacro MUI_UNPAGE_CONFIRM
!insertmacro MUI_UNPAGE_COMPONENTS
!insertmacro MUI_UNPAGE_INSTFILES
!insertmacro MUI_UNPAGE_FINISH
!insertmacro MUI_LANGUAGE "English"


Section "-Install" Section0
	SetOutPath "$INSTDIR"
	File /r *.*
SectionEnd


Section "-Post install" Section1
	${If} ${RunningX64}
		SetRegView 64
	${EndIf}
    WriteRegStr HKCU "${REGKEY}" 				"" 					$INSTDIR
    WriteRegStr HKCU "${UNINSTALL_REGKEY}" 		"DisplayName"		"${APPNAME}"
    WriteRegStr HKCU "${UNINSTALL_REGKEY}" 		"DisplayVersion"	"${VERSION}"
    WriteRegStr HKCU "${UNINSTALL_REGKEY}" 		"InstallLocation"	"$INSTDIR"
    WriteRegStr HKCU "${UNINSTALL_REGKEY}" 		"Publisher"			""
    WriteRegStr HKCU "${UNINSTALL_REGKEY}" 		"UninstallString"	"$INSTDIR\Uninstall.exe"
	WriteRegDWORD HKCU "${UNINSTALL_REGKEY}"	"VersionMajor"		${VER_MAJOR}
	WriteRegDWORD HKCU "${UNINSTALL_REGKEY}"	"VersionMinor"		${VER_MINOR}
    WriteUninstaller "$INSTDIR\Uninstall.exe"
    
    !insertmacro MUI_STARTMENU_WRITE_BEGIN Application
        CreateDirectory "$SMPROGRAMS\$StartMenuFolder"
        CreateShortCut "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk" "$INSTDIR\Uninstall.exe"
    !insertmacro MUI_STARTMENU_WRITE_END
SectionEnd


Section Uninstall
    Delete "$INSTDIR\Uninstall.exe"
    RMDir /r /REBOOTOK "$INSTDIR"    
    !insertmacro MUI_STARTMENU_GETFOLDER Application $StartMenuFolder
    Delete "$SMPROGRAMS\$StartMenuFolder\Uninstall.lnk"
    RMDir "$SMPROGRAMS\$StartMenuFolder"    
    DeleteRegKey /ifempty HKCU "${REGKEY}"
	DeleteRegKey HKCU "${UNINSTALL_REGKEY}"
SectionEnd


Function .onInit
    ${IfNot} ${RunningX64}
        MessageBox MB_ICONSTOP "This $(^Name) installer is suitable for 64-bit Windows only!"
        Abort
  ${EndIf}
FunctionEnd


Function un.onInit
    MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "Are you sure you want to completely remove $(^Name) and all of its components?" IDYES +2
    Abort
FunctionEnd


Function un.onUninstSuccess
    HideWindow
    MessageBox MB_ICONINFORMATION|MB_OK "$(^Name) was successfully removed from your computer."
FunctionEnd

'''
