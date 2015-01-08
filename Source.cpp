#include <stdio.h>
#include <string>
#include <fstream>
#include <iostream>
#include "cstdlib"

extern "C"
{
#include "xxtea.h"
}

#include "FileSystem.h"

#define LOG_FILE_EXT "ord"
#define DECODE_FILE_EXT "txt"

#define PRIVATE_KEY "JbUG4N"

void DecodeFile( const std::string& fileName )
{
	std::ifstream file;
	file.open( fileName, std::ifstream::binary );
	if( !file.is_open( ) )
	{
		std::cout << "[Error!!]Decode file failed! File Name: " << fileName << std::endl;
		return;
	}

	std::string decodeFileName = FileSystem::GetInstance( ).GetFileName( fileName, true, false );
	decodeFileName.append( "." );
	decodeFileName.append( DECODE_FILE_EXT );

	std::ofstream decodeFile;
	decodeFile.open( decodeFileName, std::ifstream::binary );
	if( !decodeFile.is_open( ) )
	{
		std::cout << "[Error!!]Decode file failed! File Name: " << fileName << std::endl;
		file.close( );
		return;
	}

	std::string key = PRIVATE_KEY;


	file.seekg( 0, file.end );
	long size = (long) file.tellg( );
	file.seekg( 0, file.beg );

	if( size == 0 )
	{
		file.close( );
		decodeFile.close( );
	}

	char* buffer = new char[size];
	file.read( buffer, size );
	
	xxtea_long outputSize;
	char* outputBuffer = (char*) xxtea_decrypt( (unsigned char*) buffer, size, (unsigned char*) key.c_str( ), (long) key.length( ), &outputSize );

	decodeFile.write( outputBuffer, outputSize );

	delete[] buffer;
	file.close( );
	decodeFile.close( );
	std::cout << "Decode " << fileName << " succeeded." << std::endl;
}

bool OnEnumFile( const std::string& fileName, int param1 = -1, int param2 = -1 )
{
	std::string ext = FileSystem::GetInstance( ).GetExtName( fileName );
	if ( ext != LOG_FILE_EXT )
		return true;

	DecodeFile( fileName );
	return true;
}



int main( int argc, char* argv[] )
{
	FileSystem& fileSystem = FileSystem::GetInstance( );

	std::string path;
	if( argc > 1 )
	{
		if ( fileSystem.IsFile( argv[1] ) )
			DecodeFile( argv[1] );
		else if( fileSystem.IsFolder( argv[1] ) )
			fileSystem.EnumFiles( argv[1], true, OnEnumFile );
	}
	else
	{
		path = fileSystem.GetPathName( argv[0] );
		fileSystem.EnumFiles( path, true, OnEnumFile );
	}


    #ifdef _WIN32
	system( "pause" );
    #endif
}