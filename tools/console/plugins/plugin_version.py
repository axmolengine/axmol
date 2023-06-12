#!/usr/bin/python
# ----------------------------------------------------------------------------
# cocos2d "version" plugin
#
# Author: Ricardo Quesada
# Copyright 2013 (C) Zynga, Inc
#
# License: MIT
# ----------------------------------------------------------------------------
'''
"version" plugin for cocos2d command line tool
'''

__docformat__ = 'restructuredtext'

import re
import os
import cocos2d
import inspect


#
# Plugins should be a sublass of CCJSPlugin
#
class CCPluginVersion(cocos2d.CCPlugin):

    @staticmethod
    def plugin_name():
    	return "version"

    @staticmethod
    def brief_description():
        return "prints the version of the installed components"

    def _show_versions(self):
        path = os.path.join(self._src_dir, "axmol", "axmolver.h")
        if not os.path.exists(path):
            path = os.path.join(self._src_dir, "axmol", "2d", "axmolver.h")
            if not os.path.exists(path):
                raise cocos2d.CCPluginError("Couldn't find file with version information")

    	with open(path, 'r')  as f:
    		data = f.read()
    		match = re.search(r".*AX_VERSION_STR[ \t]+\"(.*)\"", data)
    		if match:
    			print 'axmol %s' % match.group(1)
    		else:
    			raise cocos2d.CCPluginError("Couldn't find version info")

    def run(self, argv, dependencies):
    	self.parse_args(argv)
        self._show_versions()

