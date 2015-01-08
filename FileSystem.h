#pragma once

#include "string"

class FileSystem
{
public:
	typedef bool (*OnEnumFile)( const std::string& name, int param1, int param2 );
	
public:
	~FileSystem( );
	
	static FileSystem& GetInstance( );

	bool IsFile( const std::string& path );
	bool IsFolder( const std::string& path );
	std::string GetPathName( const std::string& path );
	std::string GetFileName( const std::string& str, bool fullPath = true, bool ext = true );
	std::string GetExtName( const std::string& str );

	int EnumFiles( const std::string& path, bool recursize, OnEnumFile funcPtr, int param1 = -1, int param2 = -1 );
	
private:
	FileSystem( );
};