#!/usr/bin/python

import subprocess
import pickle
import os
import argparse
import sys
import re
import shutil
from string import Formatter

log_file_ = None
userargs_ = None
deb_control_map_ = None
deb_control_file_str_ = None
deb_folder_path_ = None
package_db_ = {}

class consolecolor:
	RED = '\033[91m'
	YELLOW = '\033[93m'
	GREEN = '\033[92m'
	CYAN = '\033[96m'
	BLUE = '\033[94m'
	PURPLE = '\033[95m'
	BLACK = '\033[90m'
	GRAY = '\033[98m'
	WHITE = '\033[97m'
	ENDC = '\033[0m'

class buildstates:
	NOT_BUILT = 0
	FAILED = 1
	CHANGELOG_UPDATED = 2
	BUILD_DIR_CREATED = 3
	CHANGELOG_GENERATED = 4
	CONFIGURED = 5
	BUILT = 6
	UPLOADED = 7
	CLEANED = 8

	str_map = {
		FAILED: "FAILED",
		NOT_BUILT: "NOT_BUILT",
		CHANGELOG_UPDATED: "CHANGELOG_UPDATED",
		BUILD_DIR_CREATED: "BUILD_DIR_CREATED",
		CHANGELOG_GENERATED: "CHANGELOG_GENERATED",
		CONFIGURED: "CONFIGURED",
		BUILT: "BUILT",
		UPLOADED: "UPLOADED",
		CLEANED: "CLEANED"
	}

	@staticmethod
	def getStateStr( state ):
		return buildstates.str_map[state]

output_levels_ = {
	'silent': 0,
	'quiet': 1,
	'normal': 2,
	'noisy': 3,
	'verbose': 4
}

def addToLog( msg ):
	global log_file_
	if not log_file_ is None:
		log_file_.write( msg + "\n" )

def printInfo( msg ):
	content = consolecolor.BLUE + '[INFO] ' + msg + consolecolor.ENDC
	if userargs_.output_level >= output_levels_['normal']:
		print content
	addToLog( content )

def printWarn( msg ):
	content = consolecolor.YELLOW + '[WARN] ' + msg + consolecolor.ENDC
	if userargs_.output_level >= output_levels_['quiet']:
		print content
	addToLog( content )

def printError( msg ):
	content = consolecolor.RED + '[ERROR] ' + msg + consolecolor.ENDC
	if userargs_.output_level >= output_levels_['silent']:
		print content
	addToLog( content )

def printSuccess( msg ):
	content = consolecolor.GREEN + '[SUCCESS] ' + msg + consolecolor.ENDC
	if userargs_.output_level >= output_levels_['quiet']:
		print content
	addToLog( content )

def printDebugSuccess( msg ):
	content = consolecolor.GREEN + '[DEBUG][SUCCESS] ' + msg + consolecolor.ENDC
	if userargs_.output_level >= output_levels_['noisy']:
		print content
	addToLog( content )

def printDebug( msg ):
	content = consolecolor.CYAN + '[DEBUG] ' + msg + consolecolor.ENDC
	if userargs_.output_level >= output_levels_['noisy']:
		print content
	addToLog( content )

def printQuiet( msg ):
	content = consolecolor.GRAY + '[QUIET] ' + msg + consolecolor.ENDC
	if userargs_.output_level >= output_levels_['quiet']:
		print content
	addToLog( content )

def printNormal( msg ):
	content = consolecolor.WHITE + msg + consolecolor.ENDC
	if userargs_.output_level >= output_levels_['normal']:
		print content
	addToLog( content )

def writeToFile( db_prefix, pickle_plaintext ):
	printInfo( "Pickling package database..." )
	package_db_file = None
	try:
		package_db_file = open( db_prefix + '.db', 'wb' )
		if pickle_plaintext is True:
			pickle.dump( package_db_, package_db_file, 0 )
		else:
			pickle.dump( package_db_, package_db_file, pickle.HIGHEST_PROTOCOL )
		package_db_file.close()
	except pickle.PickleError as e:
		printError( "Failed to pickle package database; " + str( e ) )
	except IOError as e:
		printError( "Failed to open package database file for writing; " + str( e ) )

	if package_db_file is None:
		raise SystemExit
	
	printSuccess( "Done pickling package database" )

def loadFromFile( db_prefix ):
	global package_db_

	printInfo( "Un-pickling package database..." )
	package_db_file = None
	try:
		package_db_file = open( db_prefix + '.db', 'rb' )
		package_db_ = pickle.load( package_db_file )
		package_db_file.close()
	except pickle.PickleError as e:
		printError( "Failed to un-pickle package database; " + str( e ) )
	except IOError as e:
		printError( "Failed to open package database file for writing; " + str( e ) )

	if package_db_file is None:
		raise SystemExit

	printSuccess( "Done un-pickling package database" )

def tryExecuteCommand( command_str, simulate = False ):
	output = ""
	if simulate is False:
		try:
			printDebug( "Executing command: " + command_str )
			output = subprocess.check_output( command_str, shell=True )
			printDebugSuccess( "Got result:\n" + output )
		except subprocess.CalledProcessError as e:
			printWarn( "Command failed: " + str( e ) )

	return output

def executeCommand( command_str, simulate = False, strip_trailing = True ):
	output = ""
	if simulate is False:
		printDebug( "Executing command: " + command_str )
#		output = subprocess.check_output( command_str, shell=True )
		process = subprocess.Popen( command_str, shell=True, stdout=subprocess.PIPE, stderr=subprocess.PIPE )
		all_outputs = process.communicate()

		if len( all_outputs[1] ) > 0:
			printWarn( "Command gave warning:\n" + all_outputs[1] )

		output = all_outputs[0]

		printDebugSuccess( "Got result:\n" + output )

	if strip_trailing is True:
		output = output.rstrip( "\n" )

	return output

def readDebControlFile():
	global deb_folder_path_
	global deb_control_file_str_

	if deb_control_file_str_ is None:
		try:
			deb_folder_path_ = userargs_.package_path + "/debian"
			deb_control_file_path = deb_folder_path_ + "/control"
			printDebug( "Opening deb control file: " + deb_control_file_path + "..." )
			deb_control_file = open( deb_control_file_path, "r" )
			deb_control_file_str_ = deb_control_file.read()
			deb_control_file.close()
			printDebugSuccess( "Read:\n" + deb_control_file_str_ )
		except IOError as e:
			printError( "Failed to open control file: " + deb_control_file_path + "; " + str( e ) )
			raise SystemExit

	return deb_control_file_str_

def findValues( raw_text, keys ):
	values_map = {}
	for line in raw_text.splitlines():
		kv_pair = line.split( ": " )
		if len( kv_pair ) >= 2:
			if kv_pair[0] in keys:
				printDebug( "Found kv: " + str( kv_pair ) )
				values_map[kv_pair[0]] = "".join( kv_pair[1:] ).rstrip( '\n' )
				# if we've found all the keys we're looking for
				if len( values_map.keys() ) == len( keys ):
					break

	if len( values_map.keys() ) < len( keys ):
		printWarn( "Only found " + str( len( values_map.keys() ) ) + "/" + str( len( keys ) ) + " keys" )

	return values_map

def getDebControlMap():
	global deb_control_map_
	if deb_control_map_ is None:
		deb_control_map_ = findValues( readDebControlFile(), [ "Architecture", "Source" ] )

	return deb_control_map_

def cleanupBuildDir( package_build_dir ):
	printDebug( "Cleaning build space for package: " + package_build_dir )
	if os.path.exists( package_build_dir ):
		try:
			shutil.rmtree( package_build_dir )
			printDebugSuccess( "Removed dir: " + package_build_dir )
		except OSError as e:
			printWarn( "Failed to remove directory: " + package_build_dir + "; " + str( e ) )
			raise subprocess.CalledProcessError( 1, "rm -r " + package_build_dir, str( e ) )
	else:
		printDebug( "Build space not found" )

def main():
	global userargs_

	userargs_parser = argparse.ArgumentParser()
	buildinfo_parser = argparse.ArgumentParser()

	userargs_parser.add_argument( "package_path", type=str, action="store", help="Path of package to build" )
	userargs_parser.add_argument( "--resume", dest="resume", action="store_true", default=True, help="Read packages from existing db file" )
	userargs_parser.add_argument( "--db-prefix", metavar="file", dest="db_prefix", action="store", default="/home/buildmaster/build-space/make-deb", help="Prefix for pickled database file URI" )
	userargs_parser.add_argument( "--pickle-plaintext", dest="pickle_plaintext", action="store_true", default=False, help="Save data structures in plaintext instead of binary" )
	userargs_parser.add_argument( "--summary", "--status", dest="show_summary", action="store_true", default=False, help="Show summary of package build states" )

	output_level_group = userargs_parser.add_mutually_exclusive_group()
	output_level_group.add_argument( "--output-level", dest="output_level", action="store", choices=["silent", "quiet", "normal", "noisy", "verbose"], default="normal", help="Set output verbosity level" )
	output_level_group.add_argument( "-v", "--verbose", dest="be_verbose", action="store_true", default=False, help="Be verbose" )

	userargs_parser.add_argument( "--simulate", dest="simulate", action="store_true", default=False, help="Only show commands to be executed, but don't execute them" )
	userargs_parser.add_argument( "--logfile", dest="logfile", action="store", default="ipa-clone.log", help="File to log command outputs to" )
	userargs_parser.add_argument( "--force", dest="force", action="store_true", default=False, help="Build package even if the PPA already has a record for it" )
	userargs_parser.add_argument( "--no-cleanup", dest="no_cleanup", action="store_true", default=False, help="Don't clean up build dir after building" )

	build_arg_lambdas = [
		lambda parser: parser.add_argument( "-d", "--dists", dest="dists", type=str, action="store", nargs="+", help="List of distros to build for" ),
		# here, all means "compile for all known archs", and any means "compile for the most convenient arch"
		# in contrast, in the architecture field in the debian control file, all means "this package runs on all archs (no need to specify one)", and any means "this package can be compiled for any known arch"
		# if set to "auto", first check the debian/control file for the right arch, then expand as needed
		lambda parser: parser.add_argument( "-a", "--archs", dest="archs", type=str, action="store", choices=["i386","amd64","auto"], default="auto", nargs="+", help="List of architectures to build for" ),
		lambda parser: parser.add_argument( "-m", "--mods", dest="mods", type=str, action="store", nargs="+", help="List of mods to build for" ),
		lambda parser: parser.add_argument( "--version", dest="set_version", action="store", default="auto", help="Override version info from VCS" ),
		lambda parser: parser.add_argument( "--release", dest="set_release", action="store", default="auto", help="Override release info from VCS" )
	]
	
	for arg_lambda in build_arg_lambdas:
		arg_lambda( userargs_parser )
		arg_lambda( buildinfo_parser )

	userargs,unknown = userargs_parser.parse_known_args()

	if userargs.be_verbose:
		userargs.output_level = output_levels_['verbose']
	else:
		userargs.output_level = output_levels_[userargs.output_level]

	userargs.package_path = userargs.package_path.rstrip( "/" )
	userargs_ = userargs

	if userargs.resume is True or userargs.show_summary is True:
		loadFromFile( userargs.db_prefix )

	if userargs.show_summary is True:
		printInfo( "Package: [version-string][dist][arch][mod]: state" )
		printInfo( "-------------------------------------------------" )
		for package,package_vals in package_db_.iteritems():
			for vr_string,vr_string_vals in sorted( package_vals.iteritems(), key=lambda x: x[0], reverse=True ):
				printInfo( package + ": [" + vr_string + "]: " + buildstates.getStateStr( vr_string_vals['build_state'] ) )
				if 'build_result' in vr_string_vals.keys():
					printInfo( "--> " + vr_string_vals['build_result'] )
				for dist,dist_vals in vr_string_vals.iteritems():
					if not dist in [ 'build_state', 'build_result' ]:
						printInfo( package + ": [" + vr_string + "][" + dist + "]: " + buildstates.getStateStr( dist_vals['build_state'] ) )
						if 'build_result' in dist_vals.keys():
							printInfo( "--> " + dist_vals['build_result'] )
						for arch,arch_vals in dist_vals.iteritems():
							if not arch in [ 'build_state', 'build_result' ]:
								for mod,mod_vals in arch_vals.iteritems():
									printInfo( package + ": [" + vr_string + "][" + dist + "][" + arch + "][" + mod + "]: " + buildstates.getStateStr( mod_vals['build_state'] ) )
									if 'build_result' in mod_vals.keys() and len( mod_vals['build_result'] ) > 0:
										printInfo( "--> " + mod_vals['build_result'] )
				printInfo( "--------------------" )
			printInfo( "==============================" )
	
	if userargs.package_path == "none":
		raise SystemExit

	try:
		buildinfo_path = userargs.package_path + "/buildinfo"
		printDebug( "Opening " + buildinfo_path + "..." )
		buildinfo_file = open( buildinfo_path, "r" )
		buildinfo_str = buildinfo_file.read().rstrip( '\n' )
		buildinfo_file.close()
		printSuccess( "Found extra args: " + buildinfo_str )

		buildinfo_args,unknown = buildinfo_parser.parse_known_args( buildinfo_str.split() )

		for key,val in buildinfo_args.__dict__.iteritems():
			if not val is None:
				userargs.__dict__[key] = val
	except IOError as e:
		printWarn( "Failed to open buildinfo file for package path: " + userargs.package_path + "; " + str( e ) )

	if "auto" in userargs.archs:
		printInfo( "Auto-detecting build architecture..." )
		if getDebControlMap()['Architecture'] == "any":
			userargs.archs = [ "i386", "amd64" ]
			printDebug( "Replaced arch 'any' with: " + str( userargs.archs ) )
		else:
			printDebug( "Retrieving default arch..." )
			userargs.archs = [ executeCommand( "dpkg --print-architecture" ) ]
		printSuccess( "Found archs: " + str( userargs.archs ) )

	if userargs.dists is None:
		printDebug( "Retrieving default dist..." )
		# get system distname
		userargs.dists = [ executeCommand( "cat /etc/*-release | grep CODENAME | sed 's:DISTRIB_CODENAME=::g'" ) ]

	if userargs.mods is None:
		printDebug( "Retrieving default mod..." )
		userargs.mods = [ "interaction" ]

	if userargs.set_version is "auto":
		printInfo( "Auto-detecting package version..." )
		git_branchname = executeCommand( "cd " + userargs.package_path + " && git branch -l | grep \* | sed 's:\* ::g'" )
		userargs.set_version = re.sub( r"[a-zA-Z\-]*-?([0-9\.]+)-", r"\1", git_branchname )
		printSuccess( "Detected version: " + userargs.set_version )
	else:
		printDebug( "Forcing version to " + userargs.set_version )

	if userargs.set_release is "auto":
		printInfo( "Auto-detecting package release..." )
		latest_timestamp = executeCommand( "git log -n 1 --pretty=format:\"%ct\" " + userargs.package_path )
		userargs.set_release = latest_timestamp
		printSuccess( "Detected release: " + userargs.set_release )
	else:
		printDebug( "Forcing release to " + userargs.set_release )

	userargs_ = userargs

	package_name = getDebControlMap()['Source']

	deb_file_basename = package_name + "_" + userargs.set_version + "-" + userargs.set_release

	printInfo( "Building package: " + str( package_name ) )
	printInfo( "Building for dists: " + str( userargs.dists ) )
	printInfo( "Building for archs: " + str( userargs.archs ) )
	printInfo( "Building for mods: " + str( userargs.mods ) )

	vr_string = userargs.set_version+"-"+userargs.set_release

	if not package_name in package_db_.keys():
		package_db_[package_name] = {}
	if not vr_string in package_db_[package_name].keys():
		package_db_[package_name][vr_string] = { 'build_state': buildstates.NOT_BUILT }

	try:
		if package_db_[package_name][vr_string]['build_state'] < buildstates.CHANGELOG_UPDATED:
			executeCommand( "interaction-init-changelog.py " + userargs.package_path + " --update", userargs.simulate )
			package_db_[package_name][vr_string]['build_state'] = buildstates.CHANGELOG_UPDATED

	except subprocess.CalledProcessError as e:
		# failed
		printError( "Command failed: " + str( e ) )
		package_db_[package_name][vr_string]['build_state'] = buildstates.FAILED
		package_db_[package_name][vr_string]['build_result'] = str( e )
		raise SystemExit

	for dist in userargs.dists:
		if not dist in package_db_[package_name][vr_string].keys():
			package_db_[package_name][vr_string][dist] = { 'build_state': buildstates.NOT_BUILT }

		package_build_space = userargs.package_path + "-" + vr_string + "-debuild~" + dist
		package_build_dir = package_build_space + "/" + package_name + "_" + vr_string
		package_orig_dir = package_build_dir + ".orig"

		try:
			if package_db_[package_name][vr_string][dist]['build_state'] < buildstates.BUILD_DIR_CREATED:
				try:
					os.makedirs( package_build_space )
				except OSError as e:
					printWarn( "Failed to create directory: " + package_build_space + "; " + str( e ) )

				if os.path.exists( package_build_dir ):
					printWarn( "Build dir already exists; removing..." )
					cleanupBuildDir( package_build_dir )

				shutil.copytree( userargs.package_path, package_build_dir )
				printDebug( "Package will be built in: " + package_build_dir )
				package_db_[package_name][vr_string][dist]['build_state'] = buildstates.BUILD_DIR_CREATED

			if package_db_[package_name][vr_string][dist]['build_state'] < buildstates.CHANGELOG_GENERATED:
				executeCommand( "interaction-init-changelog.py " + package_build_dir + " --generate-only -p " + dist, userargs.simulate )
				package_db_[package_name][vr_string][dist]['build_state'] = buildstates.CHANGELOG_GENERATED
			
			if package_db_[package_name][vr_string][dist]['build_state'] < buildstates.CONFIGURED:
				if os.path.exists( package_orig_dir ):
					printWarn( "Orig dir already exists; removing..." )
					cleanupBuildDir( package_orig_dir )
				shutil.copytree( package_build_dir, package_orig_dir )
				try:
					executeCommand( "cd " + package_build_dir + " && debuild -S", userargs.simulate )
					package_db_[package_name][vr_string][dist]['build_state'] = buildstates.CONFIGURED
				except subprocess.CalledProcessError as e:
					printWarn( "Command failed: " + str( e ) + "; trying again..." )
					executeCommand( "cd " + package_build_dir + " && debuild -S", userargs.simulate )
					package_db_[package_name][vr_string][dist]['build_state'] = buildstates.CONFIGURED

		except subprocess.CalledProcessError as e:
			# failed
			printError( "Command failed: " + str( e ) )
			package_db_[package_name][vr_string][dist]['build_state'] = buildstates.FAILED
			package_db_[package_name][vr_string][dist]['build_result'] = str( e )

		for arch in userargs.archs:
			if not arch in package_db_[package_name][vr_string][dist].keys():
				package_db_[package_name][vr_string][dist][arch] = {}

			for mod in userargs.mods:
				printInfo( "Preparing package: " + package_name + "[" + vr_string + "][" + dist + "][" + arch + "] for mod: " + mod )
				build_package = False
				if userargs.force is True:
					printDebug( "Skipping check for existing package due to --force" )
					build_package = True
				else:
					if not mod in package_db_[package_name][vr_string][dist][arch].keys():
						package_db_[package_name][vr_string][dist][arch][mod] = { 'build_state': buildstates.NOT_BUILT }

					build_package = package_db_[package_name][vr_string][dist][arch][mod]['build_state'] < buildstates.BUILT

				if build_package is True:
					# build package
					printInfo( "Building: " + package_name + "[" + vr_string + "][" + dist + "][" + arch + "] for mod: " + mod )

					try:
						if package_db_[package_name][vr_string][dist]['build_state'] == buildstates.CONFIGURED and package_db_[package_name][vr_string][dist][arch][mod]['build_state'] < buildstates.BUILT:
							executeCommand( "sudo DIST=" + dist + " ARCH=" + arch + " MOD=" + mod + " cowbuilder --build " + package_build_space + "/*.dsc", userargs.simulate )
							# done building
							package_db_[package_name][vr_string][dist][arch][mod]['build_state'] = buildstates.BUILT
							printSuccess( "Finished building: " + package_name + "[" + vr_string + "][" + dist + "][" + arch + "] for mod: " + mod )
					except subprocess.CalledProcessError as e:
						# failed
						printError( "Command failed: " + str( e ) )
						package_db_[package_name][vr_string][dist][arch][mod]['build_state'] = buildstates.FAILED
						package_db_[package_name][vr_string][dist][arch][mod]['build_result'] = str( e )

				else:
					printSuccess( "Package already built: " + package_name + "[" + vr_string + "][" + dist + "][" + arch + "] for mod: " + mod )

				if package_db_[package_name][vr_string][dist][arch][mod]['build_state'] == buildstates.BUILT:
					try:
						changes_file_path = "/var/cache/pbuilder/" + dist + "-" + arch + "-" + mod + "/result/" + package_name + "_" + vr_string + "*.changes"
						printDebug( "Uploading content in " + changes_file_path )
						executeCommand( "dput -U packages.robotics.usc.edu " + changes_file_path )
					except subprocess.CalledProcessError as e:
						printWarn( "Command failed: " + str( e ) )
						if not 'Error getting file info' in e.output:
							continue

					package_db_[package_name][vr_string][dist][arch][mod]['build_state'] = buildstates.UPLOADED
					printSuccess( "Package uploaded: " + package_name + "[" + vr_string + "][" + dist + "][" + arch + "] for mod: " + mod )

				elif package_db_[package_name][vr_string][dist][arch][mod]['build_state'] == buildstates.UPLOADED:
					printSuccess( "Package already uploaded: " + package_name + "[" + vr_string + "][" + dist + "][" + arch + "] for mod: " + mod )
			
		#clean up
		if userargs.no_cleanup is False and package_db_[package_name][vr_string][dist]['build_state'] > buildstates.NOT_BUILT:
			cleanupBuildDir( package_build_space )
			package_db_[package_name][vr_string][dist]['build_state'] = buildstates.CLEANED

	writeToFile( userargs.db_prefix, userargs.pickle_plaintext )

if __name__ == "__main__":
	main()
