#include "FileSystem.h"
#include "sstream"

#ifdef _WIN32

#include <windows.h>

#else

#include <dirent.h>
#include <sys/stat.h>

#endif

// Helper function.
void EnumFileHelper( const std::string& root, int& fileNum, FileSystem::OnEnumFile funcptr, int param1, int param2 )
{
	std::ostringstream stream;
	stream << root << "\\*.*";

	#ifdef _WIN32

	std::string filter = stream.str( );

	WIN32_FIND_DATA finddata;
	HANDLE finder = ::FindFirstFile( filter.c_str(), &finddata );
	if( finder == NULL || finder == INVALID_HANDLE_VALUE )
		return;

	#else

	DIR* finder = ::opendir( root.c_str() );
	if( finder == NULL )
		return;

	dirent* finddata = ::readdir( finder );
	if( finddata == NULL )
		return;
	
	#endif

	do
	{
		#ifdef _WIN32

		if( finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY )
			continue;

		std::ostringstream stream;
		stream << root << "\\" << finddata.cFileName;
		std::string fileName = stream.str( );

		#else

		std::string fileName = finddata->d_name;

		#endif

		if( fileName == "." || fileName == ".." )
			continue;

		fileNum++;
		funcptr( fileName, param1, param2 );
	}
	#ifdef _WIN32
	while( ::FindNextFile( finder, &finddata ) );
	#else
	while( (finddata = ::readdir( finder )) != NULL );
	#endif
}

void EnumFolderHelper( const std::string& root, int& fileNum, FileSystem::OnEnumFile funcptr, int param1, int param2, bool enumFile, bool recursive )
{
	if ( enumFile )
		EnumFileHelper( root, fileNum, funcptr, param1, param2 );

	#ifdef _WIN32

	std::ostringstream filter;
	filter << root << "\\*.*";

	WIN32_FIND_DATA finddata;
	HANDLE finder = ::FindFirstFile( filter.str( ).c_str( ), &finddata );
	if( finder == NULL || finder == INVALID_HANDLE_VALUE )
		return;

	#else

	DIR* finder = ::opendir( root.c_str() );
	if( finder == NULL )
		return;

	dirent* finddata = ::readdir( finder );
	if( finddata == NULL )
		return;


	#endif

	do
	{
		#ifdef _WIN32

		if( ( finddata.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY ) == 0 )
			continue;

		std::string folderName = finddata.cFileName;

		#else

		std::string folderName = finddata->d_name;

		#endif

		if( folderName == "." || folderName == ".." )
			continue;

		std::ostringstream childPath;
		childPath << root << "\\" << folderName;

		if( recursive )
			EnumFolderHelper( childPath.str( ), fileNum, funcptr, param1, param2, enumFile, recursive );


		if( !enumFile )
			funcptr( childPath.str( ), param1, param2 );
	}
	#ifdef _WIN32
	while( ::FindNextFile( finder, &finddata ) );
	#else
	while( (finddata = ::readdir( finder )) != NULL );
	#endif
}

FileSystem& FileSystem::GetInstance( )
{
	static FileSystem fileSystem;
	return fileSystem;
}

FileSystem::FileSystem( )
{
}

FileSystem::~FileSystem( )
{

}

bool FileSystem::IsFile( const std::string& path )
{
	#ifdef _WIN32

	int attr = ::GetFileAttributes( path.c_str() );
	return attr != -1 && (attr & FILE_ATTRIBUTE_DIRECTORY) == 0;

	#else

	struct stat st;
	if ( ::stat( path.c_str(), &st ) != 0 )
		return false;

	return S_ISREG( st.st_mode ) != 0;

	#endif

}

bool FileSystem::IsFolder( const std::string& path )
{
	#ifdef _WIN32

	int attr = ::GetFileAttributes( path.c_str() );
	return attr != -1 && (attr & FILE_ATTRIBUTE_DIRECTORY) != 0;

	#else

	struct stat st;
	if( ::stat( path.c_str(), &st ) != 0 )
		return false;

	return S_ISDIR( st.st_mode ) != 0;

	#endif
}

std::string FileSystem::GetPathName( const std::string& str )
{
	if( IsFolder( str ) )
		return str;

	if( IsFile( str ) )
	{
		std::string path = "";
		unsigned long backslash = str.find_last_of( "\\" );
		unsigned long slash = str.find_last_of( "/" );

		unsigned long cutPos = backslash <= slash ? backslash : slash;
		if ( cutPos != (unsigned long) -1 )
			path = str.substr( 0, cutPos );

		return path;
	}

	return "";
}

std::string FileSystem::GetFileName( const std::string& str, bool fullPath, bool ext )
{
	if( !IsFile( str ) )
		return "";

	if( fullPath && ext )
		return str;

	std::string filePath = GetPathName( str );
	unsigned long lastDotPos = str.find_last_of( '.' );
	bool hasExt = lastDotPos > filePath.length( );

	std::string fileName;
	if( fullPath )
	{
		if( ext && hasExt )
			fileName = str.substr( 0 ); 
		else
			fileName = str.substr( 0, lastDotPos );
	}
	else
	{
		if( ext && hasExt )
			fileName = str.substr( filePath.length( ) + 1 );
		else
			fileName = str.substr( filePath.length( ) + 1, lastDotPos - filePath.length( ) - 1 );
	}

	return fileName;
}

std::string FileSystem::GetExtName( const std::string& str )
{
	if( !IsFile( str ) )
		return "";

	std::string filePath = GetPathName( str );
	unsigned long lastDotPos = str.find_last_of( '.' );
	bool hasExt = lastDotPos > filePath.length( );
	if(!hasExt)
		return "";

	return str.substr( lastDotPos + 1 );
}

int FileSystem::EnumFiles( const std::string& path, bool recursive, OnEnumFile funcPtr, int param1, int param2 )
{
	int fileNum = 0;
	if( recursive )
		EnumFolderHelper( path, fileNum, funcPtr, param1, param2, true, true );
	else
		EnumFileHelper( path, fileNum, funcPtr, param1, param2 );

	return fileNum;
}
