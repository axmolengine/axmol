#!/usr/bin/python
# ----------------------------------------------------------------------------
# cocos2d "gui" plugin
#
# ----------------------------------------------------------------------------
'''
"gui" plugin for cocos2d command line tool
'''
from __future__ import print_function
from __future__ import unicode_literals

__docformat__ = 'restructuredtext'

import cocos
import subprocess


class CCPluginGUI(cocos.CCPlugin):
    """
    launches the cocos2d console gui
    """

    @staticmethod
    def plugin_name():
        return "gui"

    @staticmethod
    def brief_description():
        return "shows the GUI"

    def run(self, argv, dependencies):
        subprocess.Popen(['open', '-a', self.get_console_path() + '/' + 'Cocos2d-Console-GUI.app'])
