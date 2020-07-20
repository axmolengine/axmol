#!/usr/bin/python
# ----------------------------------------------------------------------------
# cocos2d "clean" plugin
#
# Author: Luis Parravicini
#
# License: MIT
# ----------------------------------------------------------------------------
'''
"clean" plugin for cocos2d command line tool
'''

__docformat__ = 'restructuredtext'

import os
import shutil

import cocos2d
from plugin_dist import CCPluginDist

class CCPluginClean(cocos2d.CCPlugin):
    """
    cleans a project
    """

    @staticmethod
    def plugin_name():
      return "clean"

    @staticmethod
    def brief_description():
        return "removes files produced by compilation"

    def clean_android(self):
        if not self._platforms.is_android_active():
            return
        project_dir = self._platforms.project_path()

        cocos2d.Logging.info("cleaning native")
        obj_path = os.path.join(project_dir, 'obj')
        self._rmdir(obj_path)
        cocos2d.Logging.info("cleaning java")
        self._run_cmd("cd \"%s\" && ant clean" % project_dir)

    def clean_ios(self):
        if not self._platforms.is_ios_active():
            return
        project_dir = self._platforms.project_path()

        cocos2d.Logging.info("removing intermediate files")
        self._run_cmd("cd \"%s\" && xcodebuild clean" % project_dir)
        self._rmdir(CCPluginDist.target_path(project_dir))

    def _rmdir(self, path):
        if os.path.exists(path):
            try:
                shutil.rmtree(path)
            except OSError as e:
                raise cocos2d.CCPluginError("Error removing directory: " + str(e.args))


    def run(self, argv, dependencies):
        self.parse_args(argv)
        self.clean_android()
        self.clean_ios()
