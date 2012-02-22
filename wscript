import Options
from os import unlink, symlink, popen
from os.path import exists

srcdir = "."
blddir = "build"
VERSION = "0.0.1"

def set_options(opt):
	opt.tool_options("compiler_cxx")

def configure(conf):
	conf.check_tool("compiler_cxx")
	conf.check_tool("node_addon")
	conf.check_cfg(package='clutter-1.0', uselib_store='CLUTTER', args='--cflags --libs')

def build(bld):
	obj = bld.new_task_gen("cxx", "shlib", "node_addon")
	obj.cxxflags = ["-Wall", "-ansi", "-pedantic"]
	obj.target = "clutter"
	obj.source = """
		src/clutter.cpp
		src/actor.cpp
		src/container.cpp
		src/stage.cpp
		"""
	obj.cxxflags = ["-D_FILE_OFFSET_BITS=64", "-D_LARGEFILE_SOURCE"]
	obj.uselib = "CLUTTER"

def shutdown():
	if Options.commands['clean']:
		if exists('clutter.node'): unlink('clutter.node')
	else:
		if exists('build/default/clutter.node') and not exists('clutter.node'):
			symlink('build/default/clutter.node', 'clutter.node')
