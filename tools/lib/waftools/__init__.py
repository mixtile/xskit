#!/usr/bin/env python
# -*- encoding: utf-8 -*-
# Michel Mooij, michel.mooij7@gmail.com

import os

version = "0.4.11"
location = os.path.abspath(os.path.dirname(__file__))


def build(bld, trees=[]):
	'''Perform build command on all given source trees.
	
	:param: bld: build context
	:type bld: waflib.Build.BuildContext
	:param: trees: list of source tree names
	:type trees: list
	'''
	recurse(bld, trees)


def recurse(ctx, trees=[]):
	'''Recurse on all given source trees.
	
	:param: ctx: waf context
	:type ctx: waflib.Context.Context
	:param: trees: list of source tree names
	:type trees: list
	'''
	for tree in trees:
		for script in get_scripts(tree, 'wscript'):
			ctx.recurse(script)


def get_scripts(top, name):
	'''Returns a list of top level paths containing the specified file name.
	
	:param: top: root where the search should be started.
	:type top: str
	:param: name: name of the file to be found.
	:type name: str
	'''
	locations = []
	for cwd, _, files in os.walk(top):
		for f in files:
			if os.path.basename(f) != name:
				continue
			locations.append(cwd)
	scripts = []
	for loc in locations:
		if any(os.path.dirname(loc).endswith(t) for t in locations):
			continue
		scripts.append(loc.replace('\\', '/'))
	return scripts


