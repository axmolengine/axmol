import sys
if(sys.version_info.major >= 3):
    from .project_new import CCPluginNew
else:
    from project_new import CCPluginNew
