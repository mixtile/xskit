#! /usr/bin/env python
# -*- encoding: utf-8 -*-
# Michel Mooij, michel.mooij7@gmail.com


'''
Summary
-------
Provides a *waf* wrapper (i.e. waftool) around the static C/C++ source code
checking tool **cppcheck**.

See http://cppcheck.sourceforge.net/ for more information on **cppcheck** 
itself; how you can obtain and install it for your particular desktop 
environment. Note that many linux distributions already provide a ready to 
install version of **cppcheck**. On *Fedora*, for instance, it can be installed
using *yum*::

    $ sudo yum install cppcheck


Description
-----------
Each time a C/C++ task generator within your *waf* build environment is being 
build or rebuild, its source code can be checked using cppcheck. This module 
will gather and extract all the required information from the C/C++ task
generator (e.g. *bld.program* defined somewhere in a *wscript* file) and will 
use it to perform a source code analysis using cppcheck on command line. The 
command line results from **cppcheck** (in XML format) will be used as input in 
order to create a highlighted and colorful HTML report pinpointing all
(possible) problems. 
For each single C/C++ task defined within your *waf* build environment such a 
separate HTML report will be created. Furthermore a single HTML index page will
be created containing references to all individual HTML reports of components. 
All these reports will be stored in the sub directory *reports/cppcheck* in the
top level directory of your build environment. When needed this location can
also be changed to, see command line options.

Example below present an example of the reports generated in a build environment
in which three *C* components have been defined::

    .
    ├── components
    │   ├── chello
    │   │   ├── include
    │   │   │   └── hello.h
    │   │   ├── src
    │   │   │   └── hello.c
    │   │   └── wscript
    │   ├── ciambad
    │   │   ├── cppcheck.suppress
    │   │   ├── include
    │   │   ├── src
    │   │   │   └── iambad.c
    │   │   └── wscript
    │   └── cleaking
    │       ├── include
    │       │   └── leaking.h
    │       ├── src
    │       │   └── leaking.c
    │       └── wscript
    ├── reports
    │   └── cppcheck
    │       ├── components
    │       │   ├── chello
    │       │   │   ├── chello.html
    │       │   │   ├── index.html
    │       │   │   ├── style.css
    │       │   │   └── chello.xml
    │       │   ├── ciambad
    │       │   │   ├── ciambad.html
    │       │   │   ├── index.html
    │       │   │   ├── style.css
    │       │   │   └── ciambad.xml
    │       │   └── cleaking
    │       │       ├── cleaking.html
    │       │       ├── index.html
    │       │       ├── style.css
    │       │       └── cleaking.xml
    │       ├── index.html
    │       └── style.css
    └── wscript

Note that each report for a task generator from the components directory 
contains an extra indent in the reports directory; cppchecks reports are stored
in a sub directory using the name of the unique task generator as name for that
sub directory. This allows for the creation of multiple reports at the same
location in case a single *wscript* file contains multiple task generators in
the components directory.  

Under normal conditions no additional parameters or definitions are needed in
the definition of a C/C++ task generator itself; simply defining it as 
*program*, *stlib* or *shlib* and adding this module to the top level *wscript*
of your *waf* build environment will suffice. However in some cases 
**cppcheck** might detect problems that are either not true, or you just want
to suppress them. In these cases you can either use global suppression options
(using command line options) but you can also add special rules to the 
definition of the C/C++ task generators in question (more on this the next 
section Usage).


Usage
-----
In order to use this waftool simply add it to the 'options' and 'configure' 
functions of your main *waf* script as shown in the example below::

    import waftools

    def options(opt):
        opt.load('cppcheck', tooldir=waftools.location)

    def configure(conf):
        conf.load('cppcheck')

When configured as shown in the example above, **cppcheck** will perform a 
source code analysis on all C/C++ tasks that have been defined in your *waf* 
build environment when using the '--cppcheck' build option::

    waf build --cppcheck

The example shown below for a C program will be used as input for **cppcheck** 
when building the task::

    def build(bld):
        vbld.program(name='foo', src='foobar.c')

The result of the source code analysis will be stored both as XML and HTML 
files in the build location for the task. Should any error be detected by
**cppcheck**, then the build process will be aborted and a link to the HTML 
report will be presented. When desired you also choose to resume with checking
other components after a fatal error has been detected using the following command
line option::

    $ waf build --cppcheck --cppcheck-err-resume 

When needed source code checking by **cppcheck** can be disabled per task or even 
for each specific error and/or warning within a particular task.

In order to exclude a task from source code checking add the skip option to the
task as shown below::

    def build(bld):
        bld.program(name='foo', src='foobar.c', cppcheck_skip=True)

When needed problems detected by cppcheck may be suppressed using a file 
containing a list of suppression rules. The relative or absolute path to this 
file can be added to the build task as shown in the example below::

    bld.program(name='bar', src='foobar.c', cppcheck_suppress='bar.suppress')

A **cppcheck** suppress file should contain one suppress rule per line. Each of 
these rules will be passed as an '--suppress=<rule>' argument to **cppcheck**.

'''

import os
import sys
import xml.etree.ElementTree as ElementTree
from jinja2 import Template
import chardet
import pygments
from pygments import formatters, lexers
from pygments.formatters import HtmlFormatter
from waflib import TaskGen, Context, Logs, Utils

CPPCHECK_WARNINGS = ['error', 'warning', 'performance', 'portability', 'unusedFunction']


CPPCHECK_TOP_HTML = \
'''<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
    <head>
        <title>{{ title }}</title>
        <link href="style.css" rel="stylesheet" type="text/css" />
		<style type="text/css"></style>
    </head>
    <body class="body">
        <div id="page-header">&#160;</div>
        <div id="page">
            <div id="header">
                <h1>{{ header }}</h1>
            </div>
            <div id="menu">
                <a href="index.html">Home</a>
            </div>
            <div id="content">
				<table>
					<tr>
						<th colspan="3">Component</th>
						<th>Severity</th>
					</tr>
					{% for component in components %}
					<tr>
						<td colspan="3"><a href={{ component.url }}>{{ component.name }}</a></td>
						<td>{{ component.severity }}</td>
					</tr>
					{% endfor %}
				</table>
			</div>
			<div id="footer">
				<div>cppcheck - a tool for static C/C++ code analysis</div>
				<div>
				Internet: <a href="http://cppcheck.sourceforge.net">http://cppcheck.sourceforge.net</a><br>
				Forum: <a href="http://apps.sourceforge.net/phpbb/cppcheck/">http://apps.sourceforge.net/phpbb/cppcheck/</a><br>
				IRC: #cppcheck at irc.freenode.net
				</div>
				&#160;
			</div>
			&#160;
		</div>
		<div id="page-footer">&#160;</div>
	</body>
</html>
'''


CPPCHECK_INDEX_HTML = \
'''<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
	<head>
		<title>{{ title }}</title>
		<link href="style.css" rel="stylesheet" type="text/css" />
		<style type="text/css"></style>
	</head>
	<body class="body">
		<div id="page-header">&#160;</div>
		<div id="page">
			<div id="header">
				<h1>{{ header }}</h1>
			</div>
			<div id="menu">
				<a href={{ home }}>Home</a>
			</div>
			<div id="content">
				<table>
					<tr>
						<th colspan="5">File</th>
						<th>Line</th>
						<th>Type</th>
						<th>Severity</th>
						<th>Description</th>
					</tr>
					{% for defect in defects %}
					<tr>
						<td colspan="5"><a href={{ defect.url }}>{{ defect.file }}</a></td>
						<td><a href={{ defect.url }}#line-{{ defect.line }}>{{ defect.line }}</a></td>
						<td>{{ defect.kind }}</td>
						<td>{{ defect.severity }}</td>
						<td>{{ defect.description }}</td>
					</tr>
					{% endfor %}
				</table>
			</div>
			<div id="footer">
				<div>cppcheck - a tool for static C/C++ code analysis</div>
				<div>
				Internet: <a href="http://cppcheck.sourceforge.net">http://cppcheck.sourceforge.net</a><br>
				Forum: <a href="http://apps.sourceforge.net/phpbb/cppcheck/">http://apps.sourceforge.net/phpbb/cppcheck/</a><br>
				IRC: #cppcheck at irc.freenode.net
				</div>
				&#160;
			</div>
			&#160;
		</div>
		<div id="page-footer">&#160;</div>
	</body>
</html>
'''


CPPCHECK_HTML = \
'''<!DOCTYPE HTML PUBLIC "-//W3C//DTD HTML 4.01//EN" "http://www.w3.org/TR/html4/strict.dtd">
<html>
	<head>
		<meta http-equiv="Content-Type" content="text/html; charset=UTF-8">
		<title>{{ title }}</title>
		<link href="style.css" rel="stylesheet" type="text/css">
		<style type="text/css"></style>
	</head>
	<body class="body">
		<div id="page-header">&#160;</div>
		<div id="page">
			<div id="header">
				<h1>{{ header }}</h1>
			</div>
			<div id="menu">
				<a href={{ home }}>Home</a>
				<a href={{ back }}>Back</a>
			</div>
			<div id="content">
				{{ table }}
			</div>
			<div id="footer">
				<div>cppcheck - a tool for static C/C++ code analysis</div>
				<div>
				Internet: <a href="http://cppcheck.sourceforge.net">http://cppcheck.sourceforge.net</a><br>
				Forum: <a href="http://apps.sourceforge.net/phpbb/cppcheck/">http://apps.sourceforge.net/phpbb/cppcheck/</a><br>
				IRC: #cppcheck at irc.freenode.net
				</div>
				&#160;
			</div>
			&#160;
		</div>
		<div id="page-footer">&#160;</div>
	</body>
</html>
'''


CPPCHECK_STYLE_CSS = \
'''
body.body {
    font-family: Arial;
    font-size: 13px;
    background-color: black;
    padding: 0px;
    margin: 0px;
}

.error {
    font-family: Arial;
    font-size: 13px;
    background-color: #ffb7b7;
    padding: 0px;
    margin: 0px;
}

th, td {
    min-width: 100px;
    text-align: left;
}

#page-header {
    clear: both;
    width: 1200px;
    margin: 20px auto 0px auto;
    height: 10px;
    border-bottom-width: 2px;
    border-bottom-style: solid;
    border-bottom-color: #aaaaaa;
}

#page {
    width: 1160px;
    margin: auto;
    border-left-width: 2px;
    border-left-style: solid;
    border-left-color: #aaaaaa;
    border-right-width: 2px;
    border-right-style: solid;
    border-right-color: #aaaaaa;
    background-color: White;
    padding: 20px;
}

#page-footer {
    clear: both;
    width: 1200px;
    margin: auto;
    height: 10px;
    border-top-width: 2px;
    border-top-style: solid;
    border-top-color: #aaaaaa;
}

#header {
    width: 100%;
    height: 70px;
    background-image: url(logo.png);
    background-repeat: no-repeat;
    background-position: left top;
    border-bottom-style: solid;
    border-bottom-width: thin;
    border-bottom-color: #aaaaaa;
}

#menu {
    margin-top: 5px;
    text-align: left;
    float: left;
    width: 100px;
    height: 300px;
}

#menu > a {
    margin-left: 10px;
    display: block;
}

#content {
    float: left;
    width: 1020px;
    margin: 5px;
    padding: 0px 10px 10px 10px;
    border-left-style: solid;
    border-left-width: thin;
    border-left-color: #aaaaaa;
}

#footer {
    padding-bottom: 5px;
    padding-top: 5px;
    border-top-style: solid;
    border-top-width: thin;
    border-top-color: #aaaaaa;
    clear: both;
    font-size: 10px;
}

#footer > div {
    float: left;
    width: 33%;
}

.highlight .hll { background-color: #ffffcc }
.highlight  { background: #ffffff; }
.highlight .c { color: #888888 } /* Comment */
.highlight .err { color: #FF0000; background-color: #FFAAAA } /* Error */
.highlight .k { color: #008800; font-weight: bold } /* Keyword */
.highlight .o { color: #333333 } /* Operator */
.highlight .cm { color: #888888 } /* Comment.Multiline */
.highlight .cp { color: #557799 } /* Comment.Preproc */
.highlight .c1 { color: #888888 } /* Comment.Single */
.highlight .cs { color: #cc0000; font-weight: bold } /* Comment.Special */
.highlight .gd { color: #A00000 } /* Generic.Deleted */
.highlight .ge { font-style: italic } /* Generic.Emph */
.highlight .gr { color: #FF0000 } /* Generic.Error */
.highlight .gh { color: #000080; font-weight: bold } /* Generic.Heading */
.highlight .gi { color: #00A000 } /* Generic.Inserted */
.highlight .go { color: #888888 } /* Generic.Output */
.highlight .gp { color: #c65d09; font-weight: bold } /* Generic.Prompt */
.highlight .gs { font-weight: bold } /* Generic.Strong */
.highlight .gu { color: #800080; font-weight: bold } /* Generic.Subheading */
.highlight .gt { color: #0044DD } /* Generic.Traceback */
.highlight .kc { color: #008800; font-weight: bold } /* Keyword.Constant */
.highlight .kd { color: #008800; font-weight: bold } /* Keyword.Declaration */
.highlight .kn { color: #008800; font-weight: bold } /* Keyword.Namespace */
.highlight .kp { color: #003388; font-weight: bold } /* Keyword.Pseudo */
.highlight .kr { color: #008800; font-weight: bold } /* Keyword.Reserved */
.highlight .kt { color: #333399; font-weight: bold } /* Keyword.Type */
.highlight .m { color: #6600EE; font-weight: bold } /* Literal.Number */
.highlight .s { background-color: #fff0f0 } /* Literal.String */
.highlight .na { color: #0000CC } /* Name.Attribute */
.highlight .nb { color: #007020 } /* Name.Builtin */
.highlight .nc { color: #BB0066; font-weight: bold } /* Name.Class */
.highlight .no { color: #003366; font-weight: bold } /* Name.Constant */
.highlight .nd { color: #555555; font-weight: bold } /* Name.Decorator */
.highlight .ni { color: #880000; font-weight: bold } /* Name.Entity */
.highlight .ne { color: #FF0000; font-weight: bold } /* Name.Exception */
.highlight .nf { color: #0066BB; font-weight: bold } /* Name.Function */
.highlight .nl { color: #997700; font-weight: bold } /* Name.Label */
.highlight .nn { color: #0e84b5; font-weight: bold } /* Name.Namespace */
.highlight .nt { color: #007700 } /* Name.Tag */
.highlight .nv { color: #996633 } /* Name.Variable */
.highlight .ow { color: #000000; font-weight: bold } /* Operator.Word */
.highlight .w { color: #bbbbbb } /* Text.Whitespace */
.highlight .mb { color: #6600EE; font-weight: bold } /* Literal.Number.Bin */
.highlight .mf { color: #6600EE; font-weight: bold } /* Literal.Number.Float */
.highlight .mh { color: #005588; font-weight: bold } /* Literal.Number.Hex */
.highlight .mi { color: #0000DD; font-weight: bold } /* Literal.Number.Integer */
.highlight .mo { color: #4400EE; font-weight: bold } /* Literal.Number.Oct */
.highlight .sb { background-color: #fff0f0 } /* Literal.String.Backtick */
.highlight .sc { color: #0044DD } /* Literal.String.Char */
.highlight .sd { color: #DD4422 } /* Literal.String.Doc */
.highlight .s2 { background-color: #fff0f0 } /* Literal.String.Double */
.highlight .se { color: #666666; font-weight: bold; background-color: #fff0f0 } /* Literal.String.Escape */
.highlight .sh { background-color: #fff0f0 } /* Literal.String.Heredoc */
.highlight .si { background-color: #eeeeee } /* Literal.String.Interpol */
.highlight .sx { color: #DD2200; background-color: #fff0f0 } /* Literal.String.Other */
.highlight .sr { color: #000000; background-color: #fff0ff } /* Literal.String.Regex */
.highlight .s1 { background-color: #fff0f0 } /* Literal.String.Single */
.highlight .ss { color: #AA6600 } /* Literal.String.Symbol */
.highlight .bp { color: #007020 } /* Name.Builtin.Pseudo */
.highlight .vc { color: #336699 } /* Name.Variable.Class */
.highlight .vg { color: #dd7700; font-weight: bold } /* Name.Variable.Global */
.highlight .vi { color: #3333BB } /* Name.Variable.Instance */
.highlight .il { color: #0000DD; font-weight: bold } /* Literal.Number.Integer.Long */
'''


def options(opt):
	'''Adds command line options to the *waf* build environment 

	:param opt: Options context from the *waf* build environment.
	:type opt: waflib.Options.OptionsContext
	'''
	opt.add_option('--cppcheck', dest='cppcheck', default=False,
		action='store_true', help='check C/C++ sources (default=False)')

	opt.add_option('--cppcheck-path', dest='cppcheck_path', default='reports/cppcheck',
		action='store', help='location to save cppcheck reports to.')
	
	opt.add_option('--cppcheck-fatals', dest='cppcheck_fatals', default='error',
		action='store', help='comma separated list of fatal severities')
	
	opt.add_option('--cppcheck-err-resume', dest='cppcheck_err_resume',
		default=False, action='store_true',
		help='continue in case of errors (default=False)')

	opt.add_option('--cppcheck-bin-enable', dest='cppcheck_bin_enable',
		default='warning,performance,portability,style,unusedFunction',
		action='store',
		help="cppcheck option '--enable=' for binaries (default=warning,performance,portability,style,unusedFunction)")

	opt.add_option('--cppcheck-lib-enable', dest='cppcheck_lib_enable',
		default='warning,performance,portability,style', action='store',
		help="cppcheck option '--enable=' for libraries (default=warning,performance,portability,style)")

	opt.add_option('--cppcheck-std-c', dest='cppcheck_std_c',
		default='c99', action='store',
		help='cppcheck standard to use when checking C (default=c99)')

	opt.add_option('--cppcheck-std-cxx', dest='cppcheck_std_cxx',
		default='c++03', action='store',
		help='cppcheck standard to use when checking C++ (default=c++03)')

	opt.add_option('--cppcheck-check-config', dest='cppcheck_check_config',
		default=False, action='store_true',
		help='forced check for missing buildin include files, e.g. stdio.h (default=False)')

	opt.add_option('--cppcheck-max-configs', dest='cppcheck_max_configs',
		default='10', action='store',
		help='maximum preprocessor (--max-configs) define iterations (default=20)')


def configure(conf):
	'''Method that will be invoked by *waf* when configuring the build 
	environment.
	
	:param conf: Configuration context from the *waf* build environment.
	:type conf: waflib.Configure.ConfigurationContext
	'''
	if conf.options.cppcheck:
		conf.env.CPPCHECK_EXECUTE = [1]
	conf.env.CPPCHECK_PATH = conf.options.cppcheck_path
	conf.env.CPPCHECK_FATALS = conf.options.cppcheck_fatals.split(',')
	conf.env.CPPCHECK_STD_C = conf.options.cppcheck_std_c
	conf.env.CPPCHECK_STD_CXX = conf.options.cppcheck_std_cxx
	conf.env.CPPCHECK_MAX_CONFIGS = conf.options.cppcheck_max_configs
	conf.env.CPPCHECK_BIN_ENABLE = conf.options.cppcheck_bin_enable
	conf.env.CPPCHECK_LIB_ENABLE = conf.options.cppcheck_lib_enable
	conf.find_program('cppcheck', var='CPPCHECK', mandatory=False)


def postfun(bld):
	'''Method that will be invoked by the *waf* build environment once the 
	build has been completed.
	
	It will use the result of the source code checking stored within the given
	build context and use it to create a global HTML index. This global index
	page contains a reference to all reports on C/C++ components that have been
	checked.
	
	:param bld: Build context from the *waf* build environment.
	:type bld: waflib.Build.BuildContext
	'''
	root = str(bld.env.CPPCHECK_PATH).replace('\\', '/')
	index = Index(bld, root, bld.catalog)
	index.save_css()
	index.save_index_html()
	index.report()
	

@TaskGen.feature('c')
@TaskGen.feature('cxx')
def cppcheck_execute(self):
	'''Method that will be invoked by *waf* for each task generator for the 
	C/C++ language.
	
	:param self: A task generator that contains all information of the C/C++
				 program, shared- or static library to be exported.
	:type self: waflib.Task.TaskGen
	'''
	bld = self.bld
	if not hasattr(bld, 'options') or bld.env.CPPCHECK == []:
		return
	check = bld.env.CPPCHECK_EXECUTE
	root = str(bld.env.CPPCHECK_PATH).replace('\\', '/')
	if not bool(check):
		if not bld.options.cppcheck and not bld.options.cppcheck_err_resume:
			return
	if getattr(self, 'cppcheck_skip', False):
		return

	if not hasattr(bld, 'catalog'):
		bld.catalog = []
		bld.add_post_fun(postfun)

	fatals = bld.env.CPPCHECK_FATALS
	if bld.options.cppcheck_err_resume:
		fatals = []

	(index, severity) = CppCheck(self, root, fatals).execute()
	bld.catalog.append( (self.get_name(), index.replace('\\', '/'), severity) )


class Defect(object):
	def __init__(self, url, kind, severity, description, verbose, file, line):
		self.url = url
		self.kind = kind
		self.severity = severity
		self.description = description
		self.verbose = verbose
		self.file = file
		self.line = line


class Component(object):
	def __init__(self, name, url, severity):
		self.name = name
		self.url = '"%s"' % url
		self.severity = ', '.join([s for s in severity if s!=''])


class Index(object):
	def __init__(self, bld, root, catalog):
		self.bld = bld
		self.root = root
		self.home = '%s/%s/index.html' % (bld.path.abspath().replace('\\', '/'), root)
		self.components = []
		for (name, url, severity) in catalog:
			self.components.append(Component(name, url, severity))

	def save(self, fname, content):
		fname = '%s/%s' % (self.root, fname)
		path = os.path.dirname(fname)
		if not os.path.exists(path):
			os.makedirs(path)
		node = self.bld.path.make_node(fname)
		node.write(content)
		return node.abspath().replace('\\', '/')

	def save_css(self):
		return self.save('style.css', CPPCHECK_STYLE_CSS)
	
	def save_index_html(self):
		template = Template(CPPCHECK_TOP_HTML)
		context = {}
		name = '%s %s' % (getattr(Context.g_module, Context.APPNAME), getattr(Context.g_module, Context.VERSION))
		context['title'] = name
		context['header'] = name
		context['components'] = self.components
		fname = self.save('index.html', template.render(context))
		Logs.info('html index created: file:///%s' % fname)
		return fname
	
	def report(self):
		Logs.pprint('PINK', '\ncppcheck complete, html report can be found at:')
		Logs.pprint('PINK', '\tfile://%s\n' % self.home)


class CppCheck(object):
	'''Class used for creating colorfull HTML reports based on the source code 
	check results from **cppcheck**.
	
	Excutes source code checking on each C/C++ source file defined in the 
	task generator.

	Performs following steps per source file:
	- check source using cppcheck, use xml output
	- save the result from stderr as xml file
	- process and convert the results from stderr and save as html report
	- report defects, if any, to stout and including a link to the report
	
	:param tgen: Contains all input information for the C/C++ component
	:type tgen: waflib.Task.TaskGen
	:param root: top level directory for storing the reports
	:type root: str
	:param fatals: list of severities that should be treated as fatal when encountered
	:type fatals: list
	'''
	def __init__(self, tgen, root, fatals):
		self.tgen = tgen
		self.bld = tgen.bld
		self.root = root
		self.home = str('%s/%s/index.html' % (tgen.bld.path.abspath(), root)).replace('\\', '/')
		self.index = str('%s/%s/%s/index.html' % (tgen.bld.path.abspath(), root, tgen.path.relpath())).replace('\\', '/')
		self.fatals = fatals
		self.warnings = CPPCHECK_WARNINGS

	def save(self, fname, content, encoding='utf-8'):
		fname = '%s/%s/%s' % (self.root, self.tgen.path.relpath(), fname)
		path = os.path.dirname(fname)
		if not os.path.exists(path):
			os.makedirs(path)

		node = self.bld.path.make_node(fname)
		if sys.version_info[0] > 2:
			if isinstance(content, bytes):
				content = content.decode(encoding)
		else:
			content = content.encode(encoding)
		node.write(content)
		return node.abspath().replace('\\', '/')

	def save_xml(self, fname, stderr, cmd):
		root = ElementTree.fromstring(stderr)
		element = ElementTree.SubElement(root.find('cppcheck'), 'cmd')
		element.text = cmd
		s = ElementTree.tostring(root)
		return self.save(fname, s.decode('utf-8'))

	def save_css(self):
		return self.save('style.css', CPPCHECK_STYLE_CSS)
	
	def save_index_html(self, defects):
		template = Template(CPPCHECK_INDEX_HTML)
		context = {}
		context['title'] = self.tgen.get_name()
		context['header'] = self.tgen.get_name()
		context['home'] = '"%s"' % self.home
		context['defects'] = defects
		return self.save('index.html', template.render(context))

	def save_html(self, url, source, defects):
		hl_lines = [d.line for d in defects if d.file!='']
		formatter = CppCheckFormatter(linenos=True, style='colorful', hl_lines=hl_lines, lineanchors='line')
		formatter.errors = [d for d in defects if d.file!='']
		css_style_defs = formatter.get_style_defs('.highlight')
		lexer = pygments.lexers.guess_lexer_for_filename(source, "")
		node = self.bld.root.find_node(source)
		s = node.read()
		if sys.version_info[0] > 2:
			s = s.encode()
		encoding = chardet.detect(s)['encoding']
		template = Template(CPPCHECK_HTML)
		context = {}
		context['title'] = self.tgen.get_name()
		context['header'] = self.tgen.get_name()
		context['home'] = '"%s"' % self.home
		context['back'] = '"%s"' % self.index
		context['table'] = pygments.highlight(s.decode(encoding), lexer, formatter)
		content = template.render(context)
		return self.save(url, content, encoding)

	def defects(self, source, stderr, url):
		defects = []
		defects.append(Defect(url, '', '', '', '', os.path.basename(source), 0))
				
		for error in ElementTree.fromstring(stderr).iter('error'):
			file = ''
			line = 0
			for location in error.findall('location'):
				file = os.path.basename(location.get('file'))
				line = str(int(location.get('line')))
			kind = error.get('id')
			description = str(error.get('msg')).replace('<','&lt;')
			severity = error.get('severity')
			verbose = error.get('verbose')
			defects.append(Defect(url, kind, severity, description, verbose, file, line))
		return defects
	
	def report(self, url, source, defects):
		name = self.tgen.get_name()
		log = "\ndetected (possible) problem(s) in task '%s', see report for details:\n\tfile://%s\n" % (name, url)
		clog = 'GREEN'
		Logs.info(os.path.basename(source))
		for d in defects:
			if d.severity == 'error': color = 'RED'
			elif d.severity in self.warnings: clog = color = 'YELLOW'
			else: color = 'GREEN'
			Logs.pprint(color, '\t%s %s %s %s' % (d.line, d.severity, d.kind, d.description))
			if d.severity in self.fatals:
				self.bld.fatal(log)
		if len(defects):
			Logs.pprint(clog, log)

	def execute(self):
		severity = []
		summary = []
		self.save_css()
		Logs.pprint('PINK', 'cppcheck: %s' % self.tgen.get_name())		
		for (name, source, cmd) in self.commands():
			stderr = self.bld.cmd_and_log(cmd.split(), quiet=Context.BOTH, output=Context.STDERR)
			xml = self.save_xml('%s.xml' % name, stderr, cmd)
			url = xml.replace('.xml', '.html')
			defects = self.defects(source, stderr, url)
			self.save_html(os.path.basename(url), source, defects)
			self.report(url, source, defects)
			severity.extend([defect.severity for defect in defects])
			summary.extend(defects)
		index = self.save_index_html(summary)
		return (index, list(set(severity)))

	def commands(self):
		'''returns a list of the commands to be executed, one per source file'''
		bld = self.bld
		gen = self.tgen
		env = self.tgen.env
		features = getattr(gen, 'features', [])
		commands = []

		if 'cxx' in features:
			language = 'c++ --std=%s' % env.CPPCHECK_STD_CXX
		else:
			language = 'c --std=%s' % env.CPPCHECK_STD_C
		configs = env.CPPCHECK_MAX_CONFIGS

		cmd = Utils.to_list(env.CPPCHECK)[0].replace('\\', '/')
		cmd += ' -v --xml --xml-version=2 --inconclusive --report-progress --max-configs=%s --language=%s' % (configs, language)

		if bld.options.cppcheck_check_config:
			cmd.append('--check-config')

		if set(['cprogram','cxxprogram']) & set(features):
			cmd += ' --enable=%s' % env.CPPCHECK_BIN_ENABLE
		elif set(['cstlib','cshlib','cxxstlib','cxxshlib']) & set(features):
			cmd += ' --enable=%s' % env.CPPCHECK_LIB_ENABLE

		inc = ''
		for i in gen.to_incnodes(gen.to_list(getattr(gen, 'includes', []))):
			inc += ' -I%r' % i
		for i in gen.to_incnodes(gen.to_list(gen.env.INCLUDES)):
			inc += ' -I%r' % i

		for src in gen.to_list(gen.source):
			name = os.path.splitext(str(src))[0]
			commands.append((name, src.abspath().replace('\\', '/'), '%s %r %s' % (cmd, src, inc)))
		return commands


class CppCheckFormatter(HtmlFormatter):
	'''Formatter used for adding error messages to HTML report containing
	syntax highlighted source code.
	'''
	errors = []
	'''List of error messages. Contains the error message and line number.'''
	
	_fmt = '<span style="background: #ffaaaa;padding: 3px;">&lt;--- %s</span>\n'

	def wrap(self, source, outfile):
		'''Adds the error messages to the highlighted source code at the correct
		location.
		'''
		line_no = 1
		for i, t in super(CppCheckFormatter, self).wrap(source, outfile):
			# If this is a source code line we want to add a span tag at the end.
			if i == 1:
				for error in self.errors:
					if int(error.line) == line_no:
						t = t.replace('\n', self._fmt % error.description)
				line_no = line_no + 1
			yield i, t
