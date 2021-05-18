import sys
if(sys.version_info.major >= 3):
    from .project_compile import CCPluginCompile
else:
    from project_compile import CCPluginCompile
