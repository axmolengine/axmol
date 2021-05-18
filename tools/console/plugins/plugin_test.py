#!/usr/bin/python
# ----------------------------------------------------------------------------
# cocos2d "test" plugin
#
# Author: Ricardo Quesada
# Copyright 2014 (C) Chukong Technologies
#
# License: MIT
# ----------------------------------------------------------------------------
'''
"test" plugin for cocos2d command line tool
'''

__docformat__ = 'restructuredtext'

import cocos


#
# Plugins should be a sublass of CCPlugin
#
class CCPluginTest(cocos.CCPlugin):

    @staticmethod
    def plugin_name():
        return "test"

    @staticmethod
    def brief_description():
        return "useful to test the plugin framework"

    def run(self, argv, dependencies):
        print("cocos2d path: %s" % self.get_cocos2d_path())
        print("console path: %s" % self.get_console_path())
        print("templates paths: %s" % self.get_templates_paths())

        parser = cocos.Cocos2dIniParser()
        print("plugins path: %s" % parser.get_plugins_path())

        print("cocos2d mode: %s" % self.get_cocos2d_mode())
