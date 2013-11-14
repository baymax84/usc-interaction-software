#!/usr/bin/python

import subprocess
import pickle
import os
import argparse
from string import Formatter

log_file_ = None

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

def addToLog( msg ):
	global log_file_
	log_file_.write( msg + "\n" )

def printInfo( msg ):
	content = consolecolor.BLUE + '[INFO] ' + msg + consolecolor.ENDC
	print content
	addToLog( content )

def printWarn( msg ):
	content = consolecolor.YELLOW + '[WARN] ' + msg + consolecolor.ENDC
	print content
	addToLog( content )

def printError( msg ):
	content = consolecolor.RED + '[ERROR] ' + msg + consolecolor.ENDC
	print content
	addToLog( content )

def printSuccess( msg ):
	content = consolecolor.GREEN + '[SUCCESS] ' + msg + consolecolor.ENDC
	print content
	addToLog( content )

def printDebug( msg ):
	content = consolecolor.CYAN + '[DEBUG] ' + msg + consolecolor.ENDC
	print content
	addToLog( content )

def printQuiet( msg ):
	content = consolecolor.GRAY + '[QUIET] ' + msg + consolecolor.ENDC
	print content
	addToLog( content )

def printNormal( msg ):
	content = consolecolor.WHITE + msg + consolecolor.ENDC
	print content
	addToLog( content )

#def writeToFile( db_prefix, save_generic, pickle_plaintext ):
#	global simulate_
#
#	printInfo( "Pickling KVs..." )
#	kvs_db_file = None
#	try:
#		kvs_db_file = open( db_prefix + '-kvs.db', 'wb' )
#		if pickle_plaintext is True:
#			pickle.dump( kvs_, kvs_db_file, 0 )
#		else:
#			pickle.dump( kvs_, kvs_db_file, pickle.HIGHEST_PROTOCOL )
#		kvs_db_file.close()
#	except pickle.PickleError as e:
#		printError( "Failed to pickle kvs; " + e )
#	except IOError as e:
#		printError( "Failed to open kvs file for writing; " + e )
#
#	if kvs_db_file is None:
#		raise SystemExit
#	
#	printSuccess( "Done pickling KVs" )
#
#	printInfo( "Pickling outputs..." )
#	outputs_db_file = None
#	try:
#		outputs_db_file = open( db_prefix + '-outputs.db', 'wb' )
#		if pickle_plaintext is True:
#			pickle.dump( command_outputs_, outputs_db_file, 0 )
#		else:
#			pickle.dump( command_outputs_, outputs_db_file, pickle.HIGHEST_PROTOCOL )
#		outputs_db_file.close()
#	except pickle.PickleError as e:
#		printError( "Failed to pickle outputs; " + e )
#	except IOError as e:
#		printError( "Failed to open outputs file for writing; " + e )
#
#	if outputs_db_file is None:
#		raise SystemExit
#
#	printSuccess( "Done pickling outputs" )
#
#def loadFromFile( db_prefix ):
#	global simulate_
#
#	printInfo( "Un-pickling KVs..." )
#	kvs_db_file = None
#	try:
#		kvs_db_file = open( db_prefix + '-kvs.db', 'rb' )
#		kvs_ = pickle.load( kvs_db_file )
#		kvs_db_file.close()
#	except pickle.PickleError as e:
#		printError( "Failed to pickle kvs; " + str( e ) )
#	except IOError as e:
#		printError( "Failed to open kvs file for writing; " + str( e ) )
#
#	if kvs_db_file is None:
#		raise SystemExit
#
#	printSuccess( "Done un-pickling KVs" )
#
#	printInfo( "Un-pickling outputs..." )
#	outputs_db_file = None
#	try:
#		outputs_db_file = open( db_prefix + '-outputs.db', 'rb' )
#		command_outputs_ = pickle.load( outputs_db_file )
#		outputs_db_file.close()
#	except pickle.PickleError as e:
#		printError( "Failed to pickle outputs; " + str( e ) )
#	except IOError as e:
#		printError( "Failed to open outputs file for writing; " + str( e ) )
#
#	if outputs_db_file is None:
#		raise SystemExit
#
#	printSuccess( "Done un-pickling outputs" )

def main():
	parser = argparse.ArgumentParser()

	#parser.add_argument( "operating_mode", action="store", choices=["backup", "restore", "delete-all", "delete-in-db"], help="Export or import content from/into database" )
	#parser.add_argument( "--simulate", dest="simulate", action="store_true", default=False, help="Only show commands to be executed" )
	#parser.add_argument( "--resume", dest="resume", action="store_true", default=False, help="Read KVs and outputs from existing db files" )
	#parser.add_argument( "--logfile", dest="logfile", action="store", default="ipa-clone.log", help="File to log command outputs to" )
	#parser.add_argument( "--db-prefix", metavar="file", dest="db_prefix", action="store", default="ipa-backup", help="Prefix for pickled file URIs" )
	parser.add_argument( "-d", "--dists", dest="dists", action="store", nargs="+", help="List of dists to build for" )
	parser.add_argument( "-d", "--dists", dest="dists", action="store", nargs="+", help="List of dists to build for" )
	parser.add_argument( "-d", "--dists", dest="dists", action="store", nargs="+", help="List of dists to build for" )
	parser.add_argument( "-d", "--dists", dest="dists", action="store", nargs="+", help="List of dists to build for" )

	args = parser.parse_args()

	print( str( args.dists ) )

if __name__ == "__main__":
	main()
