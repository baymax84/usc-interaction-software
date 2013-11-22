#!/usr/bin/python

import subprocess
import pickle
import os
import argparse
import sys
import re
import shutil
import time
from datetime import datetime
from string import Formatter

log_file_ = None
userargs_ = None
changelog_hash_map_ = None
deb_changelog_template_file_str_ = None
deb_changelog_template_file_path_ = None
deb_control_map_ = None
deb_control_file_str_ = None
deb_control_file_path_ = None
deb_folder_path_ = None

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

def openLog( log_name ):
	global log_file_

	closeLog()

	if log_file_ is None:
		try:
			log_path = userargs_.build_system + "/logs/" + log_name + ".log"
			log_file_ = open( log_path, "w" )
		except IOError as e:
			printWarn( "Failed to open log file: " + str( e ) )

def addToLog( msg ):
	global log_file_
	if log_file_ is None:
		try:
			log_file_ = open( userargs_.logfile, "w" )
		except IOError as e:
			printWarn( "Failed to open log file: " + str( e ) )
	else:
		log_file_.write( msg + "\n" )

def closeLog():
	global log_file_

	if not log_file_ is None:
		log_file_.close()
		log_file_ = None

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

def readChangelogTemplateFile():
	global deb_changelog_template_file_str_

	if deb_changelog_template_file_str_ is None:
		try:
			printDebug( "Opening deb changelog.in file: " + deb_changelog_template_file_path_ + "..." )
			deb_control_file = open( deb_changelog_template_file_path_, "r" )
			deb_changelog_template_file_str_ = deb_control_file.read()
			deb_control_file.close()
			printDebugSuccess( "Read:\n" + deb_changelog_template_file_str_ )
		except IOError as e:
			printWarn( "Failed to open changelog file: " + deb_changelog_template_file_path_ + "; " + str( e ) )
			raise e

	return deb_changelog_template_file_str_

def parseEntries( raw_text ):
	entry_map = {}
	lines = raw_text.splitlines()
	for line_id in xrange( 0, len( lines ) ):
		line = lines[line_id]
		kv_pair = line.split( "* Commit " )
		if len( kv_pair ) == 2:
			printInfo( "Found commit: " + str( kv_pair[1] ) )
			entry_content = "\n".join( lines[line_id - 1:line_id + 3] )
			printDebug( "Found entry:\n" + entry_content )
			entry_map[kv_pair[1].rstrip( '\n' )] = entry_content
		# skip to what should be next "* Commit "
		line_id += 5

	return entry_map

def getDebChangelogHashes():
	global changelog_hash_map_
	if changelog_hash_map_ is None:
		changelog_hash_map_ = parseEntries( readChangelogTemplateFile() )

	return changelog_hash_map_

def readDebControlFile():
	global deb_folder_path_
	global deb_control_file_str_
	global deb_control_file_path_

	if deb_control_file_str_ is None:
		try:
			if deb_folder_path_ is None:
				deb_folder_path_ = userargs_.package_path + "/debian"
			if deb_control_file_path_ is None:
				deb_control_file_path_ = deb_folder_path_ + "/control"
			printDebug( "Opening deb control file: " + deb_control_file_path_ + "..." )
			deb_control_file = open( deb_control_file_path_, "r" )
			deb_control_file_str_ = deb_control_file.read()
			deb_control_file.close()
			printDebugSuccess( "Read:\n" + deb_control_file_str_ )
		except IOError as e:
			printError( "Failed to open control file: " + deb_control_file_path_ + "; " + str( e ) )
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
		deb_control_map_ = findValues( readDebControlFile(), [ "Source" ] )

	return deb_control_map_

def main():
	global userargs_
	global deb_folder_path_
	global deb_changelog_template_file_path_

	userargs_parser = argparse.ArgumentParser()
	buildinfo_parser = argparse.ArgumentParser()

	userargs_parser.add_argument( "package_path", type=str, action="store", help="Path of package to use" )

	userargs_parser.add_argument( "--generate", dest="do_generate", action="store_true", default=False, help="Generate changelog from changelog.in" )
	userargs_parser.add_argument( "--update", dest="do_update", action="store_true", default=False, help="Update changelog.in entries" )
	userargs_parser.add_argument( "--generate-only", dest="do_generate_only", action="store_true", default=False, help="Only generate changelog, do not update changelog.in" )
	userargs_parser.add_argument( "--regenerate", dest="regenerate", action="store_true", default=False, help="Re-generate changelog template" )

	output_level_group = userargs_parser.add_mutually_exclusive_group()
	output_level_group.add_argument( "--output-level", dest="output_level", action="store", choices=["silent", "quiet", "normal", "noisy", "verbose"], default="normal", help="Set output verbosity level" )
	output_level_group.add_argument( "-v", "--verbose", dest="output_level", action="store_const", const="verbose", help="Be verbose" )
	output_level_group.add_argument( "-q", "--quiet", dest="output_level", action="store_const", const="quiet", help="Be quiet" )
	output_level_group.add_argument( "-s", "--silent", dest="output_level", action="store_const", const="silent", help="Be silent" )

	userargs_parser.add_argument( "--simulate", dest="simulate", action="store_true", default=False, help="Only show commands to be executed, but don't execute them" )
	userargs_parser.add_argument( "--logfile", dest="logfile", action="store", default="auto", help="File to log command outputs to" )
	userargs_parser.add_argument( "--build-system", dest="build_system", action="store", default="/home/buildmaster/build-system", help="Place where build system files are stored" )
	userargs_parser.add_argument( "-p", "--platform", dest="platform", type=str, action="store", default="auto", help="Platform to generate for" ),

	build_arg_lambdas = [
		lambda parser: parser.add_argument( "--version", dest="set_version", action="store", default="auto", help="Override version info from VCS" ),
	]
	
	for arg_lambda in build_arg_lambdas:
		arg_lambda( userargs_parser )
		arg_lambda( buildinfo_parser )

	userargs,unknown = userargs_parser.parse_known_args()

	userargs.output_level = output_levels_[userargs.output_level]

	userargs.package_path = userargs.package_path.rstrip( "/" )

	userargs_ = userargs

	if userargs.logfile == "auto":
		timestamp = datetime.fromtimestamp( time.time() )
		timestamp_str = timestamp.strftime( "%Y-%m-%d_%H-%M-%S_%f" )
		log_name = "init-changelog-" + userargs.package_path.split( "/" )[-1] + "-" + userargs.platform + "-" + timestamp_str
		if userargs.do_generate is True:
			log_name += "-generate"
		if userargs.do_update is True:
			log_name += "-update"
		if userargs.do_generate_only is True:
			log_name += "-generateonly"
		if userargs.regenerate is True:
			log_name += "-regenerate"
		openLog( log_name )

	if userargs.package_path == "none":
		raise SystemExit

	try:
		buildinfo_path = userargs.package_path + "/buildinfo"
		printInfo( "Opening " + buildinfo_path + "..." )
		buildinfo_file = open( buildinfo_path, "r" )
		buildinfo_str = buildinfo_file.read().rstrip( '\n' )
		buildinfo_file.close()
		printSuccess( "Found extra args: " + buildinfo_str )

		buildinfo_args,unknown = buildinfo_parser.parse_known_args( buildinfo_str.split() )

		for key,val in buildinfo_args.__dict__.iteritems():
			if not val is None and ( not val in userargs.__dict__.keys() or userargs.__dict__[key] is None ):
				userargs.__dict__[key] = val
	except IOError as e:
		printWarn( "Failed to open buildinfo file for package path: " + userargs.package_path + "; " + str( e ) )

	if userargs.platform is "auto":
		printDebug( "Retrieving default platform..." )
		# get system distname
		userargs.platform = executeCommand( "cat /etc/*-release | grep CODENAME | sed 's:DISTRIB_CODENAME=::g'" )

	if userargs.set_version is "auto":
		printInfo( "Auto-detecting package version..." )
		git_branchname = executeCommand( "cd " + userargs.package_path + " && git branch -l | grep \* | sed 's:\* ::g'" )
		userargs.set_version = re.sub( r"[a-zA-Z\-]*-?([0-9\.]+)-", r"\1", git_branchname )
		printSuccess( "Detected version: " + userargs.set_version )
	else:
		printDebug( "Forcing version to " + userargs.set_version )

	userargs_ = userargs

	deb_folder_path_ = userargs.package_path + "/debian"
	deb_changelog_template_file_path_ = deb_folder_path_ + "/changelog.in"

	package_name = getDebControlMap()['Source']

	changelog_template_str = ""

	if userargs.regenerate is False:
		if userargs.do_generate is True:
			userargs.do_update = True
		if userargs.do_generate_only is True:
			userargs.do_generate = True
			userargs.do_update = False

		if userargs.do_update is False and userargs.do_generate is True:
			try:
				changelog_template_str = readChangelogTemplateFile()
			except IOError as e:
				printWarn( "Will update changelog template prior to generating changelog" )
				userargs.do_update = True
	else:
		userargs.do_update = True

	# update first, if applicable
	if userargs.do_update is True:
		printInfo( "Updating changelog template for package: " + package_name )
		git_log_hashes_str = None
		git_log_hashes = None
		try:
			git_log_str = executeCommand( "cd " + userargs.package_path + " && git log --pretty=format:\"%H\" -- ." )
			git_log_hashes = [ line.rstrip( "\n" ) for line in git_log_str.splitlines() ]
		except subprocess.CalledProcessError as e:
			printError( "Failed to get list of git hashes" )
			raise SystemExit 

		changelog_hash_map = {}

		if userargs.regenerate is False:
			try:
				printInfo( "Looking for existing changelog template..." )
				changelog_hash_map = getDebChangelogHashes()
				num_entries_found = len( changelog_hash_map.keys() )
				if num_entries_found > 0:
					printSuccess( "Found template with " + str( num_entries_found ) + " entries" )
				else:
					printWarn( "Found empty changelog template" )
			except IOError as e:
				printWarn( "Failed to find existing changelog template: " + str( e ) )

		if len( changelog_hash_map.keys() ) == 0:
			printInfo( "Generating new changelog template..." )

		changelog_entry_values = {
			'version': userargs.set_version,
			'platform': "{platform}",
			'package': package_name,
			'urgency': "low"
		}

		changelog_template_entry = "{package} ({version}-{release}) {platform}; urgency={urgency}" + "\n" \
			+ "  * Commit {commit}" + "\n" \
			+ "    + {subject}" + "\n" \
			+ " -- {author}  {date} \n\n"

		for git_log_hash in git_log_hashes:
			changelog_entry_values['commit'] = git_log_hash
			if git_log_hash in changelog_hash_map.keys():
				printInfo( "Using existing changelog entry for commit hash: " + git_log_hash )
				changelog_template_str += changelog_hash_map[git_log_hash] + "\n\n"
			else:
				printInfo( "Generating changelog entry for commit hash: " + git_log_hash )
				try:
					# generate changelog entry
					changelog_entry_values['author'] = executeCommand( "cd " + userargs.package_path + " && git log -n 1 --pretty=format:\"%cn <%ce>\" " + git_log_hash + " ." )
					changelog_entry_values['date'] = executeCommand( "cd " + userargs.package_path + " && git log -n 1 --pretty=format:\"%cD\" " + git_log_hash + " ." )
					changelog_entry_values['subject'] = executeCommand( "cd " + userargs.package_path + " && git log -n 1 --pretty=format:\"%s\" " + git_log_hash + " ." )
					changelog_entry_values['release'] = executeCommand( "cd " + userargs.package_path + " && git log -n 1 --pretty=format:\"%ct\" " + git_log_hash + " ." )
					changelog_template_str += changelog_template_entry.format( **changelog_entry_values )
				except subprocess.CalledProcessError as e:
					printWarn( "Failed to retrieve value for changelog entry; skipping: " + str( e ) )
					continue
		try:
			changelog_template_file = open( deb_changelog_template_file_path_, "w" )
			changelog_template_file.write( changelog_template_str )
			changelog_template_file.close()
			printSuccess( "Changelog template updated" )
		except IOError as e:
			printError( "Failed to open changelog file: " + deb_changelog_template_file_path_ + "; " + str( e ) )
			raise SystemExit

	if userargs.do_generate is True:
		printInfo( "Generating changelog for package: " + package_name )
		printInfo( "platform: " + userargs.platform )
		printInfo( "version: " + userargs.set_version )
		changelog_global_values = {
			'platform': userargs.platform
		}

		try:
			changelog_file_path = deb_folder_path_ + "/changelog"
			changelog_file = open( changelog_file_path, "w" )
			changelog_file.write( changelog_template_str.format( **changelog_global_values ) )
			printSuccess( "Changelog generated" )
		except IOError as e:
			printError( "Failed to open changelog file: " + changelog_file_path + "; " + str( e ) )

	closeLog()

if __name__ == "__main__":
	main()
