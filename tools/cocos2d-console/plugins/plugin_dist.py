#!/usr/bin/python
# ----------------------------------------------------------------------------
# cocos2d "dist" plugin
#
# Copyright 2014 (C) Luis Parravicini
#
# License: MIT
# ----------------------------------------------------------------------------
'''
"dist" plugin for cocos2d command line tool
'''

__docformat__ = 'restructuredtext'

import re
import os
import cocos2d

class CCPluginDist(cocos2d.CCPlugin):
    """
    builds a project for distribution
    """

    @staticmethod
    def plugin_name():
      return "dist"

    @staticmethod
    def brief_description():
        return "builds a project for distribution"

    def init(self, options, working_dir):
        super(CCPluginDist, self).init(options, working_dir)

        if self._platforms.is_ios_active():
            if not options.provisioning:
                raise cocos2d.CCPluginError("Provisioning profile is needed")
            else:
                self._provisioning = options.provisioning

    def _add_custom_options(self, parser):
        parser.add_option("-f", "--provisioning",
                          dest="provisioning",
                          help="provisioning profile to use (needed for iOS distribution)")
        
    def dist_android(self):
        if not self._platforms.is_android_active():
            return
        project_dir = self._platforms.project_path()

        raise cocos2d.CCPluginError("unimplemented")

    def _find_ios_scheme(self, project_dir):
        out = self._output_for("cd \"%s\" && xcodebuild -list" % project_dir)

        match = re.search('Schemes:(.*)', out, re.DOTALL)
        if match is None:
            raise cocos2d.CCPluginError("Couldn't find the schemes list")

        schemes = match.group(1).split()
        if len(schemes) == 0:
            raise cocos2d.CCPluginError("Couldn't find a scheme")
        
        return schemes[0]


    @staticmethod
    def target_path(project_dir):
        return os.path.join(project_dir, '..', 'target')
        
    def dist_ios(self):
        if not self._platforms.is_ios_active():
            return
        project_dir = self._platforms.project_path()

        scheme = self._find_ios_scheme(project_dir)
        cocos2d.Logging.info("using scheme %s" % scheme)

        archive_path = os.path.join(CCPluginDist.target_path(project_dir), scheme + '.xcarchive')
        cocos2d.Logging.info("archiving")
        self._run_cmd("cd '%s' && xcodebuild -scheme '%s' -archivePath '%s' archive" % (project_dir, scheme, archive_path))

        cocos2d.Logging.info("exporting archive")
        ipa_path = os.path.join(os.path.dirname(archive_path), scheme + '.ipa')
        if os.path.exists(ipa_path):
            os.remove(ipa_path)
        self._run_cmd("cd '%s' && xcodebuild -exportArchive -exportFormat IPA -archivePath '%s' -exportPath '%s' -exportProvisioningProfile '%s'" % (project_dir, archive_path, ipa_path, self._provisioning))
        cocos2d.Logging.info("\nThe ipa was created at:\n%s" % os.path.abspath(ipa_path))
        cocos2d.Logging.info("\nNow you can use 'Application Loader' to submit the .ipa\n")

    def run(self, argv, dependencies):
        self.parse_args(argv)
        self.dist_android()
        self.dist_ios()
