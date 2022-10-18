#!/usr/bin/python
# ----------------------------------------------------------------------------
# axmol "dist" plugin
#
# Copyright 2014 (C) Luis Parravicini
#
# License: MIT
# ----------------------------------------------------------------------------
'''
"dist" plugin for axmol command line tool
'''

__docformat__ = 'restructuredtext'

import re
import os
import axmol

class CCPluginDist(axmol.CCPlugin):
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
                raise axmol.CCPluginError("Provisioning profile is needed")
            else:
                self._provisioning = options.provisioning

        if self._platforms.is_tvos_active():
            if not options.provisioning:
                raise axmol.CCPluginError("Provisioning profile is needed")
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

        raise axmol.CCPluginError("unimplemented")

    def _find_ios_scheme(self, project_dir):
        out = self._output_for("cd \"%s\" && xcodebuild -list" % project_dir)

        match = re.search('Schemes:(.*)', out, re.DOTALL)
        if match is None:
            raise axmol.CCPluginError("Couldn't find the schemes list")

        schemes = match.group(1).split()
        if len(schemes) == 0:
            raise axmol.CCPluginError("Couldn't find a scheme")

        return schemes[0]


    @staticmethod
    def target_path(project_dir):
        return os.path.join(project_dir, '..', 'target')

    def dist_ios(self):
        if not self._platforms.is_ios_active():
            return
        project_dir = self._platforms.project_path()

        scheme = self._find_ios_scheme(project_dir)
        axmol.Logging.info("using scheme %s" % scheme)

        archive_path = os.path.join(CCPluginDist.target_path(project_dir), scheme + '.xcarchive')
        axmol.Logging.info("archiving")
        self._run_cmd("cd '%s' && xcodebuild -scheme '%s' -archivePath '%s' archive" % (project_dir, scheme, archive_path))

        axmol.Logging.info("exporting archive")
        ipa_path = os.path.join(os.path.dirname(archive_path), scheme + '.ipa')
        if os.path.exists(ipa_path):
            os.remove(ipa_path)
        self._run_cmd("cd '%s' && xcodebuild -exportArchive -exportFormat IPA -archivePath '%s' -exportPath '%s' -exportProvisioningProfile '%s'" % (project_dir, archive_path, ipa_path, self._provisioning))
        axmol.Logging.info("\nThe ipa was created at:\n%s" % os.path.abspath(ipa_path))
        axmol.Logging.info("\nNow you can use 'Application Loader' to submit the .ipa\n")

    def dist_tvos(self):
        if not self._platforms.is_tvos_active():
            return
        project_dir = self._platforms.project_path()

        scheme = self._find_ios_scheme(project_dir)
        axmol.Logging.info("using scheme %s" % scheme)

        archive_path = os.path.join(CCPluginDist.target_path(project_dir), scheme + '.xcarchive')
        axmol.Logging.info("archiving")
        self._run_cmd("cd '%s' && xcodebuild -scheme '%s' -archivePath '%s' archive" % (project_dir, scheme, archive_path))

        axmol.Logging.info("exporting archive")
        ipa_path = os.path.join(os.path.dirname(archive_path), scheme + '.ipa')
        if os.path.exists(ipa_path):
            os.remove(ipa_path)
        self._run_cmd("cd '%s' && xcodebuild -exportArchive -exportFormat IPA -archivePath '%s' -exportPath '%s' -exportProvisioningProfile '%s'" % (project_dir, archive_path, ipa_path, self._provisioning))
        axmol.Logging.info("\nThe ipa was created at:\n%s" % os.path.abspath(ipa_path))
        axmol.Logging.info("\nNow you can use 'Application Loader' to submit the .ipa\n")

    def run(self, argv, dependencies):
        self.parse_args(argv)
        self.dist_android()
        self.dist_ios()
        self.dist_tvos()
