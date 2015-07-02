#!/usr/bin/env python
# -*- encoding: utf-8 -*-
# Michel Mooij, michel.mooij7@gmail.com


'''
Summary
-------
Installs the WAF meta build system.

Description
-----------
Downloads the waf-x.y.z.tar.bz2 archive, extracts it, builds the
waf executable and installs it (e.g. in ~/.local/bin). Depending on 
the platform and python version the PATH environment variable will 
be updated as well.

Usage
-----
In order to install waf call:
    python wafinstall.py [options]

Available options:
    -h | --help     prints this help message.

    -v | --version  waf package version to install, e.g.
                    -v1.8.5

    -t | --tools    comma seperated list of waf tools to be used
                    default=None

    -u | --user     install in user directory (i.e. $HOME)
					
    -s | --skip-env do not update environment variables (i.e. $PATH,
					$WAFDIR)
'''


import os
import sys
import stat
import subprocess
import shutil
import tarfile
import getopt
import tempfile
import logging
import site
import distutils.sysconfig
try:
	from urllib.request import urlopen
except ImportError:
	from urllib2 import urlopen


WAF_URL = "https://github.com/waf-project/waf/archive"
WAF_VERSION = "1.8.10"
WAF_TOOLS = None

HOME = os.path.expanduser('~')
if sys.platform == "win32":
	BINDIR = os.path.join(sys.prefix, 'Scripts')
	LIBDIR = distutils.sysconfig.get_python_lib()

else:
	BINDIR = '%s/bin' % (sys.prefix)
	LIBDIR = distutils.sysconfig.get_python_lib()


def usage():
	print(__doc__)


def download(url, saveto):
	'''downloads the waf package.'''
	logging.info("wget %s" % url)
	u = None
	try:
		u = urlopen(url)
		with open(saveto, 'wb') as f: f.write(u.read())
	finally:
		if u: u.close()
	return os.path.realpath(saveto)


def deflate(archive, path='.'):
	'''deflates the waf archive.'''
	c = 'gz' if os.path.splitext(archive)[1] in ('.gz', '.tgz') else 'bz2'
	logging.info("tar %s %s" % ('zxvf' if c=='gz' else 'jxvf', archive))
	with tarfile.open(archive, 'r:%s' % c) as tar:
		for member in tar.getmembers():
			tar.extract(member, path=path)
			logging.debug('\t%s' % member.name)


def create(release, tools):
	'''creates the waf binary.'''
	cmd = "python waf-light --make-waf"
	if tools:
		cmd += ' --tools=%s' % tools
	top = os.getcwd()
	cwd = os.path.join(".", release)
	env = os.environ.copy()
	env['WAFDIR'] = os.path.abspath(cwd)
	try:
		logging.info(cmd)
		subprocess.call(cmd.split(), cwd=cwd, env=env)
	finally:
		os.chdir(top)


def install_waflib(waf, extras=[], libdir=LIBDIR):
	'''installs waflib.'''
	rm(os.path.join(libdir, 'waflib'))

	if len(extras):
		extras.append('__init__')
	extras = ['%s.py' % e for e in extras]

	mkdirs(os.path.join(libdir, 'waflib', 'extras'))
	top = os.getcwd()
	try:
		os.chdir(waf)
		for (path, dirs, files) in os.walk('waflib'):
			if path.count('__pycache__'):
				continue
			for file in files:
				if path.endswith('extras') and file not in extras:
					continue
				dst = os.path.join(libdir,path)
				mkdirs(dst)
				cp(os.path.join(path,file), dst)
	finally:
		os.chdir(top)


def install(release, bindir, libdir, tools, set_env):
	'''installs waf at the given location.'''
	mkdirs(bindir)
	dst = os.path.join(bindir, 'waf').replace('~', HOME)
	src = os.path.join('.', release, 'waf')
	cp(src, dst)
	os.chmod(dst, stat.S_IRWXU)
	install_waflib(release, libdir=libdir, extras=tools.split(',') if tools else [])
	if sys.platform == "win32":
		cp("%s.bat" % src, "%s.bat" % dst)
	if set_env:
		env_set('PATH', bindir, extend=True)
		env_set('WAFDIR', libdir)


def env_set(variable, value, extend=False):
	'''sets an environment variable.'''
	variable = variable.upper()
	if variable in os.environ:
		val = os.environ[variable]
		if extend:
			values = val.split(';' if sys.platform=='win32' else ':')
			if value in values:
				return
		elif val == value:
			return

	if sys.platform == "win32":
		win32_env_set(variable, value, extend)
	else:
		if value.startswith(HOME):
			value = value.replace(HOME, '~', 1)
		linux_env_set(variable, value, extend)


def win32_env_set(variable, value, extend=False):
	'''sets environment variable.'''
	try:
		import winreg
	except ImportError:
		logging.error("failed to set environment variable '%s'. please add it manually" % variable)
		return

	loc = r"SYSTEM\CurrentControlSet\Control\Session Manager\Environment"
	reg = winreg.ConnectRegistry(None, winreg.HKEY_LOCAL_MACHINE)
	key = winreg.OpenKey(reg, loc, 0, winreg.KEY_ALL_ACCESS)
	try:
		(values, _) = winreg.QueryValueEx(key, variable)
	except FileNotFoundError:
		values = None
	
	if values:
		if value in values.split(';'): return
		if extend: value = values + ';' + value

	try:
		winreg.SetValueEx(key, variable, 0, winreg.REG_SZ, value)
	finally:
		winreg.CloseKey(key)
	logging.info("WINREG(%s)" % loc)
	logging.info("\t%s=%s" % (variable,value))


def linux_env_set(variable, value, extend=False):
	'''sets environment variable.'''
	name = os.path.join(HOME, '.bashrc')
	variable = variable.upper()

	with open(name, 'r') as f:
		for line in list(f):
			if line.startswith('export %s=' % variable) and line.count(value):
				return
	export = 'export %s={0}%s\n' % (variable, value)
	export = export.format('$%s:' % variable if extend else '')

	with open(name, 'r+') as f:
		f.seek(-2, 2)
		s = f.read(2) 
		if s == '\n\n': f.seek(-1, 1) # remove double newline
		if s[1] != '\n': f.write('\n') # add missing newline
		f.write(export)
	logging.info("%s %s" % (name,export))


def getopts(argv):
	'''returns command line options as tuple.'''
	user = False
	set_env = True
	version = WAF_VERSION
	tools = WAF_TOOLS
	bindir = BINDIR
	libdir = LIBDIR

	opts, args = getopt.getopt(argv[1:], 'hv:t:us', ['help', 'version=', 'tools=', 'user', 'skip-env'])
	for opt, arg in opts:
		if opt in ('-h', '--help'):
			usage()
			sys.exit()
		elif opt in ('-v', '--version'):
			version = arg
		elif opt in ('-t', '--tools'):
			tools = arg
		elif opt in ('-u', '--user'):
			user = True
		elif opt in ('-s', '--skip-env'):
			set_env = False
	
	if user:
		# install in home directory but not on windows or virtualenv
		if sys.platform != "win32" and not hasattr(sys, 'real_prefix'):
			bindir = "~/.local/bin"
			libdir = site.getusersitepackages()

	return (version, tools, bindir, libdir, set_env)


def main(argv=sys.argv, level=logging.DEBUG):
	'''downloads, unpacks, creates and installs waf package.'''
	logging.basicConfig(level=level, format=' %(message)s')
	try:
		(version, tools, bindir, libdir, set_env) = getopts(argv)
	except getopt.GetoptError as err:
		print(str(err))
		usage()
		sys.exit(2)

	release = "waf-waf-%s" % version
	package = "waf-%s.tar.gz" % version
	url = "%s/%s" % (WAF_URL, package)
	logging.info("WAF version=%s, tools=%s, url=%s, bin=%s, lib=%s" % (version, tools, url, bindir, libdir))

	top = os.getcwd()
	tmp = tempfile.mkdtemp()
	try:
		cd(tmp)
		download(url, package)
		deflate(package)
		create(release, tools)
		install(release, bindir, libdir, tools, set_env)
	finally:
		cd(top)
		rm(tmp)
	logging.info("COMPLETE")


def cp(src, dst):
	'''copies files or directories.'''
	logging.debug('cp %s %s' % (src, dst))
	shutil.copy(src, dst)


def rm(path):
	'''delete directory, including sub-directories and files it contains.'''
	if os.path.exists(path):
		logging.debug("rm -rf %s" % (path))
		if os.path.isfile(path):
			os.remove(path)
		else:
			shutil.rmtree(path)


def mkdirs(path):
	'''create directory including missing parent directories.'''
	if not os.path.exists(path):
		logging.debug("mkdirs -p %s" % (path))
		os.makedirs(path)


def cd(path):
	'''changes current working directory.'''
	logging.debug("cd %s" % path)
	os.chdir(path)


if __name__ == "__main__":
	main(argv=sys.argv, level=logging.DEBUG)


