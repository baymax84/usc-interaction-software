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

def tryExecuteCommand( command_str, simulate = False ):
	output = ""
	if simulate is False:
		try:
			printDebug( "Executing command: " + command_str )
			output = subprocess.check_output( command_str, shell=True )
			printDebugSuccess( "Got result: " + output )
		except subprocess.CalledProcessError as e:
			printWarn( "Command failed: " + str( e ) )

	return output

def executeCommand( command_str, simulate = False, strip_trailing = True ):
	output = ""
	if simulate is False:
		printDebug( "Executing command: " + command_str )
		output = subprocess.check_output( command_str, shell=True )
		printDebugSuccess( "Got result: " + output )

	return output.rstrip( "\n" )

def readChangelogTemplateFile():
	global deb_folder_path_
	global deb_changelog_template_file_path_
	global deb_changelog_template_file_str_

	if deb_changelog_template_file_str_ is None:
		try:
			if deb_folder_path_ is None:
				deb_folder_path_ = userargs_.package_path + "/debian"
			if deb_changelog_template_file_path_ is None:
				deb_changelog_template_file_path_ = deb_folder_path_ + "/changelog.in"
			printDebug( "Opening deb changelog.in file: " + deb_changelog_template_file_path_ + "..." )
			deb_control_file = open( deb_changelog_template_file_path_, "r" )
			deb_changelog_template_file_str_ = deb_control_file.read()
			deb_control_file.close()
			printDebugSuccess( "Read " + deb_changelog_template_file_str_ )
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
			printDebug( "Found commit: " + str( kv_pair[1] ) )
			entry_map[kv_pair[1].rstrip( '\n' )] = "".join( lines[line_id - 2:line_id + 3] )
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
			printDebugSuccess( "Read " + deb_control_file_str_ )
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

	userargs_parser = argparse.ArgumentParser()
	buildinfo_parser = argparse.ArgumentParser()

	userargs_parser.add_argument( "package_path", type=str, action="store", help="Path of package to use" )

	userargs_parser.add_argument( "--generate", dest="do_generate", action="store_true", default=False, help="Generate changelog from changelog.in" )
	userargs_parser.add_argument( "--update", dest="do_update", action="store_true", default=False, help="Update changelog.in entries" )
	userargs_parser.add_argument( "--generate-only", dest="do_generate_only", action="store_true", default=False, help="Only generate changelog, do not update changelog.in" )

	output_level_group = userargs_parser.add_mutually_exclusive_group()
	output_level_group.add_argument( "--output-level", dest="output_level", action="store", choices=["silent", "quiet", "normal", "noisy", "verbose"], default="normal", help="Set output verbosity level" )
	output_level_group.add_argument( "-v", "--verbose", dest="be_verbose", action="store_true", default=False, help="Be verbose" )

	userargs_parser.add_argument( "--simulate", dest="simulate", action="store_true", default=False, help="Only show commands to be executed, but don't execute them" )
	userargs_parser.add_argument( "--logfile", dest="logfile", action="store", default="ipa-clone.log", help="File to log command outputs to" )

	build_arg_lambdas = [
		lambda parser: parser.add_argument( "-p", "--platform", dest="platform", type=str, action="store", help="Platform to generate for" ),
		lambda parser: parser.add_argument( "--version", dest="set_version", action="store", default="auto", help="Override version info from VCS" ),
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

	if userargs.platform is None:
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

	package_name = getDebControlMap()['Source']

	changelog_template_str = ""

	if userargs.do_generate_only is True:
		userargs.do_generate = True
		userargs.go_update = False

	if userargs.do_update is False and userargs.do_generate is True:
		try:
			changelog_template_str = readChangelogTemplateFile()
		except IOError as e:
			printWarn( "Will update changelog template prior to generating changelog" )
			userargs.do_update = True

	# update first, if applicable
	if userargs.do_update is True:
		printInfo( "Updating changelog template for package: " + package_name )
		git_log_hashes_str = None
		git_log_hashes = None
		try:
			git_log_str = executeCommand( "git log --pretty=format:\"%H\" -- " + userargs.package_path )
			git_log_hashes = [ line.rstrip( "\n" ) for line in git_log_str.splitlines() ]
		except subprocess.CalledProcessError as e:
			printError( "Failed to get list of git hashes" )
			raise SystemExit 

		changelog_hash_map = {}
		try:
			changelog_hash_map = getDebChangelogHashes()
		except IOError as e:
			printWarn( "Generating new changelog template..." )

		changelog_entry_values = {
			'version': userargs.set_version,
			'platform': "{platform}",
			'package': package_name,
			'urgency': "low"
		}

		changelog_template_entry = "{package} ({version}-{release}) {platform}; {urgency}" + "\n" \
			+ "  * {commit}" + "\n" \
			+ "    + {subject}" + "\n" \
			+ " -- {author}  {date} \n\n"

		for git_log_hash in git_log_hashes:
			changelog_entry_values['commit'] = git_log_hash
			if git_log_hash in changelog_hash_map.keys():
				changelog_template_str += changelog_hash_map[git_log_hash]
			else:
				try:
					# generate changelog entry
					changelog_entry_values['author'] = executeCommand( "git log -n 1 --pretty=format:\"%cn <%ce>\" " + git_log_hash )
					changelog_entry_values['date'] = executeCommand( "git log -n 1 --pretty=format:\"%cD\" " + git_log_hash )
					changelog_entry_values['subject'] = executeCommand( "git log -n 1 --pretty=format:\"%s\" " + git_log_hash )
					changelog_entry_values['release'] = executeCommand( "git log -n 1 --pretty=format:\"%ct\" " + git_log_hash )
					changelog_template_str += changelog_template_entry.format( **changelog_entry_values )
				except subprocess.CalledProcessError as e:
					printWarn( "Failed to retrieve value for changelog entry; skipping: " + str( e ) )
					continue
		try:
			changelog_template_file = open( deb_changelog_template_file_path_, "w" )
			changelog_template_file.write( changelog_template_str )
			changelog_template_file.close()
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
		except IOError as e:
			printError( "Failed to open changelog file: " + changelog_file_path + "; " + str( e ) )

if __name__ == "__main__":
	main()
