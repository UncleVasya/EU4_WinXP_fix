"""
This script generates a function forwarding header
for proxy DLL generation.

It is expected that DUMPBIN.EXE is in the path.
"""
import logging as l
import optparse
import os
import os.path
import re
import popen2
import string
import sys

#~ int main( int, char ** )
if __name__=="__main__":
    #~ Check Python version.
    version = string.split( string.split( sys.version )[0], "." )
    if map(int, version) > [ 2, 3 ]:
        #~ Set logging configuration.
        l.basicConfig( level = l.DEBUG, format='%(asctime)s %(levelname)-8s %(message)s', datefmt='%d-%b-%y %H:%M:%S' )
    else:
        l.basicConfig()

    #~ Usage string.
    usageStr = """
    make_pragmas.py -o [output dir] [source DLL name]

Where:
    source DLL name is the name of the DLL
    where the exports are extracted from


Example:
    C:\\>python make_pragmas.py gdi32

    Generates a 'gdi32_fwd.h' in the current directory.
"""

    #~ Create options.
    parser = optparse.OptionParser( usage=usageStr, version="%prog v1.1")
    parser.add_option( "-o",
                       "--output-dir",
                       dest="odir",
                       default=".",
                       metavar="DIR",
                       help="Specify output directory." )
    ( options, args ) = parser.parse_args()

    #~ Sanity check.
    if len( args ) < 1:
        parser.parse_args( ['-h'] )

    #~ Process input.
    for inputDLL in args:
        #~ Generate basename for output file.
        baseName = os.path.splitext( os.path.split( inputDLL )[1] )[0]

        #~ Generate output file's name.
        outputName = "%s/%s_fwd.h"%( options.odir, baseName )

        #~ Print some info.
        l.info( "Processing '%s'."%inputDLL )
        l.info( "Generating '%s'."%outputName )

        #~ Open output file for writing.
        try:
            fOut = open( outputName, "w" )
        except IOError, ( err, str ):
            l.error( "Error: %s ( errno %d )."%( str, err ) )
            continue

        #~ Set initial state flag.
        permission = False

        #~ Run dumpbin.
        ( coe, cin ) = popen2.popen4( "dumpbin -exports " + inputDLL )
        loopBaseName = ""
        for line in coe:
            loopBaseName = baseName
            line = line.strip()
            if len( line ) is 0:
                continue
            if line.lower().find( "fatal error" ) > -1:
                l.error( line )
            if line.lower().startswith( "ordinal" ):
                permission = True
                continue
            if line.lower().startswith( "summary" ):
                permission = False
            if permission:
                srcExport = ""
                srcOrdinal = ""
                #~ Already forwarded?
                if line.lower().find( "forwarded to " ) > -1:
                    try:
                        srcExport = line.split()[2]
                    except:
                        l.warning( "Line '%s...' failed."%line.strip()[0:60] )
                        continue
                else:
                    #~ Export it.
                    try:
                        srcExport = line.split()[-1].replace( ")", "" )
                        srcOrdinal = line.split()[0]
                    except:
                        l.warning( "Line '%s' failed."%line.strip() )
                        continue
                fOut.write( '#pragma comment(linker, "/export:%s=%s.%s,@%s")\n'%( srcExport, loopBaseName, srcExport, srcOrdinal ) )
        fOut.close()

    #~ All done.
    l.info( "All done, TTFN." )
