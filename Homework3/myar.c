/**************************************************************************************************
 ** Author:         		Benjamin Hobbs
 ** Email:					hobbsbe@onid.oregonstate.edu
 ** Class:					CS344-400
 ** Assignment:		Homework #3
 **************************************************************************************************
 ** Date Created:		7/12/2014
 ** File Name:			myar.c
 ** Overview:			...
 ** Citations:				...
**************************************************************************************************/
#define _BSD_SOURCE
#define BLOCKSIZE 1
#define STR_SIZE sizeof("rwxrwxrwx")

#include <ar.h>
#include <assert.h>
#include <ctype.h>
#include <dirent.h>
#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/utsname.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
#include <utime.h>

char * convert_octal( int mode ) {
	static char str[STR_SIZE];
	snprintf(str, STR_SIZE, "%c%c%c%c%c%c%c%c%c", 
			(mode & S_IRUSR) ? 'r' : '-', (mode & S_IWUSR) ? 'w' : '-',
			(mode & S_IXUSR) ? 'x' : '-', (mode & S_IRGRP) ? 'r' : '-',
			(mode & S_IWGRP) ? 'w' : '-', (mode & S_IXGRP) ? 'x' : '-',
			(mode & S_IROTH) ? 'r' : '-', (mode & S_IWOTH) ? 'w' : '-',
			(mode & S_IXOTH) ? 'x' : '-');
	return str;
}

void format_input( char *input ) {
	int i = sizeof(input);
	while (i > 0) {	//loop though input
		if (input[i] == '/' ) {
			input[i] = 0;
			break;
		}
		i--;
	}
}

int table_of_contents_concise( int fd ) {	
	char name[16];	
	//int hdr_length = sizeof(struct ar_hdr);
	int hdr_length = 60;
	struct ar_hdr fh;
	int len;
	
	
	lseek( fd, SARMAG, SEEK_SET);
	while( read( fd, &fh, hdr_length) == hdr_length ) {	//read headers into struct
		sscanf(fh.ar_name, "%s", name);
		format_input(name);	//strip '/' char
		printf( "%s\n",name);
		len = atoi( fh.ar_size );
		len += ( len % 2 );
		lseek( fd, len, SEEK_CUR );
		//lseek( fd, atoi( fh.ar_size ), SEEK_CUR );
	}
	
	return 0;
}

int table_of_contents_verbose( int fd ) {
	char name[16];	
	char date[30];
	char *str;	//[STR_SIZE];
	int hdr_length = sizeof(struct ar_hdr);
	struct ar_hdr fh;
	struct tm* time_info;
	int mode, uid, gid, size;
	int len;
	
	lseek( fd, SARMAG, SEEK_SET);
	while( read( fd, &fh, hdr_length) == hdr_length ) {	//read header into struct
	    sscanf(fh.ar_mode, "%o", &mode);
        str = convert_octal(mode);
        sscanf(fh.ar_uid, "%d", &uid);
        sscanf(fh.ar_gid, "%d", &gid);
        sscanf(fh.ar_size, "%d", &size);
        time_t file_t = atoi(fh.ar_date);
        time_info = localtime(&file_t);
        strftime(date, 30, "%b %d %R %Y", time_info);
		sscanf(fh.ar_name, "%s", name);
		format_input(name);			//strip '/' char	
        printf("%s %-d/%-d %6d %s %s\n", str, uid, gid, size, date, name);
		len = atoi( fh.ar_size );
		len += ( len % 2 );
		lseek( fd, len, SEEK_CUR );
		//lseek( fd, atoi( fh.ar_size ), SEEK_CUR );
	}
	return 0;
}


int create_archive( char *name ) {
	int ar;
	ar = open(name, O_CREAT | O_WRONLY, 0666);
	if( ( ar = open(name, O_CREAT | O_WRONLY, 0666) ) == -1 ) {	//failed to create
		printf("myar - Failed to create new archive '%s'.\n", name );
		exit( -1 );
	}
	write(ar, ARMAG, SARMAG);
	printf("myar - Created archive '%s'.\n", name );
	return ar;
}


int append(int fd, int num_files, char** file_names) {
	int i = 0;
	char name[16];
	struct stat fs;
	struct ar_hdr fh;
	int cur_file;
	int len;
	
	lseek( fd, 0, SEEK_END );	//move to end of archive
	while ( i < num_files ) {			//for each member specified in the command line args
		if( ( cur_file = open(file_names[ i ], O_RDONLY ) ) == -1) {	//if does not exist
			printf( "myar - Member '%s' does not exist.  File will be skipped.\n", file_names[ i ] );
			i++;
			continue;																			//move to next
		}
		
		if ( fstat( cur_file, &fs) == 0 ) {
			sscanf( file_names[ i ], "%s", name );
			len = strlen( name );
			name[ len ] = '/';
			name[ len +1 ] = '\0';
			sprintf( fh.ar_name, "%-16.16s", name );
            sprintf( fh.ar_date, "%-12u", ( uint32_t) fs.st_mtime );
            sprintf( fh.ar_uid, "%-6u", ( uint32_t) fs.st_uid );
            sprintf( fh.ar_gid, "%-6u", ( uint32_t) fs.st_gid );
            sprintf( fh.ar_mode, "%-8o", ( uint32_t ) fs.st_mode );
            sprintf( fh.ar_size, "%-10u", ( uint32_t ) fs.st_size );
            sprintf( fh.ar_fmag, "%s", ARFMAG );
            write( fd, ( char* ) &fh, sizeof( fh ) );
			//printf("%s", ( char* ) &fh);
		} else {
			printf( "myar - Could not get status of member '%s'.  File will be skipped.\n", file_names[ i ] );
			i++;
			continue;
		}
		
		char content[fs.st_size];
		
        read( cur_file, content, sizeof( content ) );
        write( fd, content, sizeof( content ) );
        if ( ( fs.st_size % 2 ) == 1 ) {		//append new line char if content size is odd
           write( fd, "\n", 1 );
        }
        close( cur_file );
        i++;
	}

	return 0;
}

int entry_not_found( int fd, char* file_name ) {
	char name[16];	
	int hdr_length = 60;
	struct ar_hdr fh;
	int len;
	
	lseek( fd, SARMAG, SEEK_SET);
	while( read( fd, &fh, hdr_length) == hdr_length ) {	//read headers into struct
		sscanf(fh.ar_name, "%s", name);
		format_input(name);	//strip '/' char
		if ( strcmp( name, file_name ) == 0 ) {
			return 1;
		}
		len = atoi( fh.ar_size );
		len += ( len % 2 );
		lseek( fd, len, SEEK_CUR );
	}
	return 0;
}

int delete(int fd, int num_files, char** file_names, char* archive) {	
	int len, flag, new_ar;	
	int i = 0, k = 0, j = 0;
	char name[16];
	char *files[20];
	struct ar_hdr fh;
	int hdr_length = sizeof(struct ar_hdr);

	
	while ( i < num_files ) {			//for each member specified in the command line args
		if( entry_not_found( fd, file_names[ i ] ) == 1 ) {
			files[ k ] = file_names[ i ];
			k++;
		}
		i++;
	}
	new_ar = create_archive( "temp" );
	
	lseek( fd, SARMAG, SEEK_SET);

	while( read( fd, &fh, hdr_length) == hdr_length ) {	//read header into struct
		sscanf(fh.ar_name, "%s", name);
		format_input(name);			//strip '/' char	
		flag = 0;
		for( j = 0 ; j < k ; j++ ) {
			if ( strcmp( files[ j ], name) == 0 ) { 
				flag = 1;
			}
			//printf("\n%s\n", files[ j ] );
		}		
		len = atoi( fh.ar_size );
		len += ( len % 2 );
		
		if ( flag == 0 ) {
			//printf("%s\n", ( char* ) &fh );
			write( new_ar, &fh, sizeof( fh ) );
			char buf[len];
			read( fd, buf, len );
			write( new_ar, buf, len );
			//printf("%s\n",buf);
			//lseek( fd, 0, SEEK_CUR );
			continue;
		}
		lseek( fd, len, SEEK_CUR );
	}
	unlink(archive);
    link("temp", archive);
    unlink("temp");
	
	return 0;
}

//http://stackoverflow.com/questions/612097/how-can-i-get-a-list-of-files-in-a-directory-using-c-or-c
int full_dir_append( int fd, char** files, char* ar_name ) {
	char cwd[255];
	int count = 0;
	struct stat fs;
	struct dirent *ent;	

	DIR *dir;
	getcwd(cwd, 255);
	
	if( ( dir = opendir ( cwd ) ) != NULL ) {
		while ( ( ent = readdir ( dir ) ) != NULL ) {
			char *temp = ent->d_name;
			stat(ent->d_name, &fs);
			if( ( fs.st_mode & S_IFMT ) == S_IFREG ) {
				//printf("%s", temp);
				if( ( strcmp( temp, "." ) == 0 ) || ( strcmp( temp, ".." ) ) == 0 ) {
					break;
				} else if (strcmp( temp, ar_name) == 0 ) {
					break;
				} else if (strcmp( temp, "myar") == 0 ) {
					break;
				} else {
					//printf ("%s\n", ent->d_name);
					files[ count ] = ent->d_name;
					count++;
				}
			}
			
		}
		closedir (dir);
	} else {
		printf ("myar - Directory '%s' could not be opened.\n", ( char * ) dir);
		exit( -1 );
	}
	
	return count;
}


int check_position(int argc, char *argv[], int *fdArchive, struct ar_hdr *fh) {
   int i , j;
   for ( i = 0 ; i < 16 ; i++ ) {
       if( fh->ar_name[i] == '/' ) {
           fh->ar_name[i] = '\0';
       }
   }
   for ( j = 0 ; j < argc ; j++ ) {
       if( ( strcmp( fh->ar_name, argv[j]) == 0 ) ) {
           return 0;
       }
   }
   return -1;
}

void extract_file(int argc, char *argv[] ) {
    int fd, fd_file, num_read;
    struct ar_hdr fh;
    char buf[1024];
    struct stat file_stat;
    struct utimbuf time_object;
 
    if( ( fd = open( argv[2], O_RDONLY ) ) != -1){
        lseek( fd, SARMAG, SEEK_SET);
        while( ( read(fd, &fh, 60 ) ) != 0 ) {
            if( check_position( argc, argv, &fd, &fh) == 0 ) {
                if( ( fd_file = open(fh.ar_name, O_CREAT | O_WRONLY | O_EXCL, strtol( fh.ar_mode, NULL, 8) ) ) == -1) {
                    if( EEXIST ) {
                        printf( "myar - File '%s' already exists. Cannot extract.\n", fh.ar_name );
                        exit( -1 );
                    }
                } else {
                    fstat(fd_file, &file_stat);
                    time_object.actime = atoi(fh.ar_date);
                    time_object.modtime = atoi(fh.ar_date);
                    utime(fh.ar_name, &time_object);
 
                    int runTotal = 0;
					
                    while( runTotal < atoi( fh.ar_size ) ) {
                        if( ( runTotal + 1024 ) > atoi( fh.ar_size ) ) {
                            num_read = read( fd, buf, ( atoi( fh.ar_size ) - runTotal ) );
                            runTotal += num_read;
                            write(fd_file, buf, num_read);
                        }
                        else{
                            num_read = read(fd, buf, 1024);
                            runTotal += num_read;
                            write(fd_file, buf, 1024);
                        }
                    }
                }
            }
            else{
                lseek(fd, atoi(fh.ar_size), SEEK_CUR);
            }
 
        }
    close(fd_file);
    close(fd);
    }
    else{
        printf("myar - Could not open the file.\n");
        exit( -1 );
    }
}

int main( int argc, char **argv )
{
	char buf[SARMAG];	//set to length of ARMAG
	char *msg, *ar_file;
	int fd, fe_flag, num_read, option;	//file exists flag:  0 file does not exist, 1 file does exist
		msg = "myar - Correct Usage:\n\t\tmyar -q [ quick append ]\n\t\t-x [ extract ]\n\t\t-t [ print concise table of contents ]\n\t\t-v [ print verbose table of contents ]\n\t\t-d [ delete from archive ]\n\t\t-A [ append entire directory ]\n";
	
	int number_of_files;
	char *files[20];
	
	
	/*  Test to make sure enough arguments were provided, if not exit program  */
	if( argc < 3 ) {
		printf( "%s", msg );
		exit( -1 );
	}

	/*  Check if file exists  */
	ar_file = argv[ 2 ];	//set archive name
	if( ( fd = open( ar_file, O_RDWR, 0666 ) ) == -1 ) {	//file does not exist
		printf("myar - Archive '%s' does not exist.\n",ar_file );
		fe_flag = 0;
	} else {																			//file exists
		fe_flag = 1;
		
		/* Check if file is archive, if not exit */
		lseek( fd, 0, SEEK_SET);
		num_read = read( fd, buf, SARMAG );
		if ( num_read < 0 ) {
			printf( "myar - Error while reading archive '%s'.\n", ar_file );
			exit( -1 );
		} else if( num_read > 0 ) {
			if( strncmp( buf, ARMAG, SARMAG ) != 0 ) {
				printf( "File format not recognized.  '%s'\n", buf );
				exit( -1 );
			}
		} else {
			printf( "myar - File '%s' is empty.\n", ar_file );
			exit( -1 );
		}
	}
	
	//deal with tv problem
	
	while (( option = getopt(argc, argv, "qxt:v:dA")) != -1) {
		switch (option) {
			/**************************************************************************************
			 *	KEY 'q'
			 *	“Quickly” append named files (members) to archive. 40 points.
			 *	Check the meaning of append in the notes below (section 3.6.3).
			 *	If the key –q is used and no file members are on the command line, an
			 *	archive file with no members will be created, just like ar does.
			 *************************************************************************************/
			case 'q':
				if( fe_flag == 0 ) {
					fd = create_archive( ar_file );
				}
				append( fd, (argc - 3), (argv + 3));
				break;
			/**************************************************************************************
			 *	KEY 'x'
			 *	Extract named members. 40 points.
			 *	Just as the regular ar command, if no member is named on the command
			 *	line when extracting files, all files are extracted from the archive. The 
			 *	permissions on the extracted files should match permissions on the files 
			 *	before archiving (as described in the notes below). To “extract” a file is not
			 *	to remove the file from the archive, it is to make a copy of the file outside of
			 *	the archive file.
			 *************************************************************************************/
			case 'x':
				extract_file(argc, argv);
				break;
			/**************************************************************************************
			 *	KEY 't'
			 *	Print a concise table of contents of the archive. 15 points. The concise
			 *	table of contents for your application (myar) should match exactly the
			 *	output from the “ar t” command on the same archive file.
			 *************************************************************************************/			
			case 't':
				if( fe_flag == 0 ) {
					exit( -1 );
				} else {		 
					table_of_contents_concise( fd );
				}
				break;
			/**************************************************************************************
			 *	KEY 'v'
			 *	Print a verbose table of contents of the archive. 30 points.
			 *	The verbose table of contents for your application (myar) should match
			 *	exactly the output from the “ar tv” command on the same archive file. See
			 *	the man page on ar.
			 *************************************************************************************/
			case 'v':
				if( fe_flag == 0 ) {
					exit( -1 );
				} else {		 
					table_of_contents_verbose( fd );
				}
				break;
			/**************************************************************************************
			 *	KEY 'd'
			 *	Delete named files from archive. 70 points.
			 *	Make sure you read the note below about the –d option and creation of a
			 *	new file. To delete a file from the archive does remove it from the archive.
			 *	If the –d key is used on the command line without any members, the
			 *	archive file is unaltered.
			 *************************************************************************************/
			case 'd':
				if( fe_flag == 0 ) {
					exit( -1 );
				}
				delete( fd, (argc - 3), (argv + 3), ar_file );
				break;
			/**************************************************************************************
			 *	KEY 'A'
			 *	Quickly append all “regular” files in the current directory (Except the 
			 * 		archive itself). 50 points.
			 *	There is not an option for the Unix ar command that does this. The -A key
			 *	is used without any members listed on the command line. If a member is
			 * on the command line with the -A key, issue a warning that it is ignored.
			 *************************************************************************************/
			case 'A':
				if( fe_flag == 0 ) {
					fd = create_archive( ar_file );
				}
				if( argc > 3 ) {
					printf( "myar - The included member(s) will be ignored.\n" );
				}
				number_of_files = full_dir_append( fd, files, ar_file);
				append( fd, number_of_files , files );
				break;
			default:
				return 0;
		}
	}
	close(fd);
	
	return 0;
}