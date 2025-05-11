/*
 * vfilter can understand version strings
 *
 * Copyright (C) 2006 Jean-Philippe Guillemin <jp.guillemin@free.fr> and Thibaud Guerin <guth@ploposor.com>
 *
 * This file is free software; the copyright holder gives unlimited
 * permission to copy and/or distribute it, with or without
 * modifications, as long as this notice is preserved.
 *
 * This file is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY, to the extent permitted by law; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A
 * PARTICULAR PURPOSE.
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <sysexits.h>
#include <linux/types.h>
#include <regex.h>


#define VERSION "0.2"
#define TRUE 1
#define FALSE 0
#define EQUAL 0
#define LESSER -1
#define GREATER 1

#ifdef DEBUG
#define dbg(fmt,args...) fprintf(stderr, fmt , ## args)
#else
#define dbg(fmt,args...) do { } while (0)
#endif

struct pkgfields
{
	char cat[129];
	char name[257];
	char softname[129];
	char version[33];
	char arch[33];
	char build[33];
	char vserial[129];
	char bserial[65];
};

regex_t regex;
regmatch_t pmatch[3];
size_t size = 0;

/**********************/
/*      PROTOTYPES    */
/**********************/

/* Get version and build fields from package name */
int getfields (struct pkgfields *pkg);

/* Transform homosapiens into Hal9000 style version string */
int vserialize (struct pkgfields *pkg);
int bserialize (struct pkgfields *pkg);

/* string version comparison function */
int strverscmp(const char *s1, const char *s2);

/* usage function */
int usage(void);


/***************** MAIN ********************/
int main(int argc, char *argv[]) {

	if (argc < 2) {
		usage();
		exit(EXIT_FAILURE);
	}

	int i,c; 
	char *filename = NULL;
	char *query = NULL;
	int showall = FALSE;
	int showupdates = FALSE;
	int showdowngrades = FALSE;
	int shownews = FALSE;
	FILE *fd;

	struct pkgfields *pkg1;
	pkg1=(struct pkgfields *) malloc (sizeof(struct pkgfields));
	struct pkgfields *pkg2;
	pkg2=(struct pkgfields *) malloc (sizeof(struct pkgfields));
	char buffer[256];
	int versioncmp,buildcmp;

	while ((i = getopt(argc, argv, "f:q:audnh")) != EOF){
		switch (i){
			case 'f':
				filename = optarg;
				break;
			case 'u':
				showupdates = TRUE;
				break;
			case 'd':
				showdowngrades = TRUE;
				break;
			case 'n':
				shownews = TRUE;
				break;
			case 'a':
				showall = TRUE;
				break;
			case 'q':
				query = optarg;
				break;
			case 'h': usage(); exit(EXIT_FAILURE);
		}
	}

	if (filename == NULL) {
		fd = stdin;
	}else{
		if((fd=fopen(filename, "rb")) == 0) {
			dbg("Error opening file %s !\n",filename);
			exit(EXIT_FAILURE);
		}
	}

	if ( regcomp(&regex, "^.*-([^-]*)-[^-]*-([^-]*)\\.t[glx]z$", REG_EXTENDED )) {
		dbg("getfields() : regex failed compilation\n");
		return(EXIT_FAILURE);
	}

	while (fgets (buffer, sizeof(buffer), fd) != NULL) {
		buffer[strlen(buffer) - 1 ] = '\0';
		// dbg("main() : buffer = %s\n", buffer);

		memset(pkg1, 0, sizeof(struct pkgfields));
		memset(pkg2, 0, sizeof(struct pkgfields));	
		
		c = sscanf (buffer, "%s |%s | %s",pkg1->cat ,pkg1->name , pkg2->name);

		if (showall == TRUE) {
			printf("%s\n", buffer);
			continue;
		}

		if (strlen(pkg1->name) == 0) continue;

		if (query != NULL) {
			if ( strncmp (query, pkg1->name, strlen(query) ) != 0){
				continue;
			}
		}

		if ( getfields (pkg1) != 0 ) {
			dbg("main() : can't process %s\n", pkg1->name);
			continue;
		}

		if (shownews == TRUE) {
			if (strlen(pkg2->name) == 0) printf("%s\n", buffer);
			continue;
		}

		if ( getfields (pkg2) != 0 ) {
			dbg("main() : can't process %s\n", pkg2->name);
			continue;
		}

		if ( vserialize (pkg1) != 0 ) {
			dbg("serialize() error while processing %s\n", pkg1->name);
		}
		if ( vserialize (pkg2) != 0 ) {
			dbg("serialize() error while processing %s\n", pkg1->name);
		}
		
		versioncmp=strcmp(pkg1->vserial, pkg2->vserial);
		dbg("%s versions %s <-> %s\n",pkg1->name, pkg1->vserial, pkg2->vserial);

		if ( versioncmp < 0){		// version1 < version2 <=> downgrade
			if (showdowngrades == TRUE) printf("%s\n", buffer);
			dbg("version1 < version2 (%d)\n", versioncmp);
			continue;
		}else if( versioncmp > 0){	// version1 > version2 <=> update
			if (showupdates == TRUE) printf("%s\n", buffer);
			dbg("version1 > version2 (%d)\n", versioncmp);
			continue;
		}

		/* if we are here then version1 == version2, so we need to
		take a look at the BUILD field */

		if ( bserialize (pkg1) != 0 ) {
			dbg("serialize() error while processing %s\n", pkg2->name);
		}
		
		if ( bserialize (pkg2) != 0 ) {
			dbg("serialize() error while processing %s\n", pkg2->name);
		}

		
		dbg("version1 == version2\n");
		buildcmp=strcmp(pkg1->bserial, pkg2->bserial);
		dbg("%s builds %s <-> %s\n",pkg1->name, pkg1->bserial, pkg2->bserial);
		
		if ( buildcmp == 0){		// build1 == build2 <=> same package
		    dbg("build1 == version2 (%d)\n", versioncmp);
			continue;
		}

		if ( buildcmp < 0) {		// build1 < build2 <=> downgrade
			if (showdowngrades == TRUE) printf("%s\n", buffer);
			dbg("build1 < build2 (%d)\n", buildcmp);
			continue;
		}else{						// build1 > build2 <=> update
			if (showupdates == TRUE) printf("%s\n", buffer);
			dbg("build1 > build2 (%d)\n", buildcmp);
			continue;
		}

	memset(buffer, 0, 256);
	
	}
	fclose (fd);
	regfree(&regex);
	return(EXIT_SUCCESS);
}

/* Get fields from package */
int getfields (struct pkgfields *pkg) {

	int start,end;


	if ( regexec(&regex, pkg->name, 3, pmatch, 0) ) {
		dbg("getfields() : regex doesn't match\n");
		return(EXIT_FAILURE);
	}else{

		// Retrieving the version
		start = pmatch[1].rm_so;
		end = pmatch[1].rm_eo;
		size = end - start;
		strncpy (pkg->version, pkg->name + start, size);
		pkg->version[size] = '\0';

		dbg("getfields() : package version = %s\n", pkg->version);

		// Retrieving the build
		start = pmatch[2].rm_so;
		end = pmatch[2].rm_eo;
		size = end - start;
		strncpy (pkg->build, pkg->name + start, size);
		pkg->build[size] = '\0';

		dbg("getfields() : package build = %s\n", pkg->build);

	}

	return(EXIT_SUCCESS);
}


char *strings_tab[]=
  { "cvs","svn","git","alpha1","alpha2","alpha3","alpha4","alpha",
  	"beta1","beta2","beta3","beta4","beta","pre1","pre2","pre3","pre4",
  	"pre","rc1","rc2","rc3","rc4","rc",
  	"a","b","c","d","e","f","g","h","i","j","k","l",0 };
char high_flags_tab[]="000111112222233333444446666777788880";
 char low_flags_tab[]="555123451234512345123451234123412340";

/* Transform homosapiens into Hal9000 style version string */
int vserialize (struct pkgfields *pkg) {

	unsigned int i, dots, found, index;
	char flag[2];
	char *offset;
	char buffer[9];
	strncpy(flag,"55",2);

	// Processing version *****************

	memset(pkg->vserial, '0', 128);
	
	// We remove leading '0'
	offset = pkg->version;
	while ((*offset) == '0') offset++;
	
	i=0; dots=0;

	for (; *offset; offset++) {
		
		/* check numbers first as they are the most common char */
		if (*offset >= '0' && *offset <= '9'){
			buffer[i++] = *(offset);
			continue;
		}
		
		/* check "." or "_" */
		if (*offset == '.' || *offset == '_'){
			strncpy (pkg->vserial + (++dots * 8) - i, buffer, i);
			i=0; 
			continue;
		}
		
		/* check "cvs", "svn", "git", "alpha", "beta", "pre" and "rc" ... */
		found=FALSE;
		for (index=0; strings_tab[index]; index++) {
			if ( !strncasecmp(offset, strings_tab[index], strlen(strings_tab[index])) ){
				flag[0] = high_flags_tab[index];
				flag[1] = low_flags_tab[index];
				offset += strlen(strings_tab[index]) - 1; // -1 as we offset++ before next global for loop
				found=TRUE;
				break;
			}			
		}
		if ( found == FALSE ) buffer[i++] = *(offset);
	}
	
	strncpy (pkg->vserial + (++dots * 8) - i, buffer, i);
	
	strncpy(pkg->vserial + 126, flag,2);
	
	pkg->vserial[128] = '\0';
	
	dbg("serialize() : vserial = %s\n", pkg->vserial);


	return(EXIT_SUCCESS);
}

/* Transform homosapiens into Hal9000 style build string */
int bserialize (struct pkgfields *pkg) {

	unsigned int i, dots;
	char flag[2];
	char *offset;
	char buffer[9];
	strncpy(flag,"55",2);


	memset(pkg->bserial, '0', 64);

	// We remove leading '0'
	offset = pkg->build;
	while ((*offset) == '0') offset++;
	
	i=0; dots=0;

	for (; *offset; offset++) {
		
		/* check numbers first as they are the most common char */
		if (*offset >= '0' && *offset <= '9'){
			buffer[i++] = *(offset);
			continue;
		}
		
		/* check "." or "_" or "z" */
		if (*offset == '.' || *offset == '_' || *offset == 'z'){
			strncpy (pkg->bserial + (++dots * 8) - i, buffer, i);
			i=0; 
			continue;
		}
		
		buffer[i++] = *(offset);
	}
	
	strncpy (pkg->bserial + (++dots * 8) - i, buffer, i);
	
	pkg->bserial[64] = '\0';

	dbg("serialize() : bserial = %s\n", pkg->bserial);

	return(EXIT_SUCCESS);
}

/* usage */
int usage(void) {
	printf("\nvfilter v%s : compare versions and returns updated packages\n", VERSION);
	printf("Usage : vfilter [-a] [-u] [-d] [-n] -q query -f filename\n");
	printf("[-a] : show all\n");
	printf("[-u] : show updates\n");
	printf("[-d] : show downgrades\n");
	printf("[-n] : show news\n");
	printf("\"filename\" is in format \"package1 | package2\"\n");
	printf("\"query\" is a filter matching on package1\n");
	printf("no \"filename\" <=> stdin is used\n");
	return(EXIT_SUCCESS);
}
