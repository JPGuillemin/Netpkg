/*
 * listbuilder can merge 2 package lists
 *
 * Copyright (C) 2006 Jean-Philippe Guillemin <jp.guillemin@free.fr>
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


#define VERSION "0.1"
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


/**********************/
/*      PROTOTYPES    */
/**********************/



regex_t regex;
regmatch_t pmatch[2];
size_t size = 0;

/* usage function */
int usage(void);



/***************** MAIN ********************/
int main(int argc, char *argv[]) {
	
	if (argc < 2) { 
		usage(); 
		exit(EXIT_FAILURE); 
	}
	
	int i,c;
	char *filename1 = NULL;
	char *filename2 = NULL;
	FILE *fd1;
	FILE *fd2;	
	
	char pkg1[256];
	char softname1[128];
	char pkg2[10000][256];
	char softname2[10000][128];
	
	char category[128];
	char buffer[256];
	
	int start,end;	
	
	while ((i = getopt(argc, argv, "a:i:h")) != EOF){
		switch (i){
			case 'a': 
				filename1 = optarg;
				break;		
			case 'i': 
				filename2 = optarg;
				break;									
			case 'h': usage(); exit(EXIT_FAILURE); 
		}
	}
	

	if((fd1=fopen(filename1, "rb")) == 0) {
		dbg("Error opening file %s !\n",filename1);
		exit(EXIT_FAILURE);
	}
	if((fd2=fopen(filename2, "rb")) == 0) {
		dbg("Error opening file %s !\n",filename2);
		exit(EXIT_FAILURE);
	}

	if ( regcomp(&regex, "^(.*)-[^-]*-[^-]*-[^-]*\\.t[glx]z$", REG_EXTENDED )) {
		dbg("main() : regex failed compilation\n");
		return(EXIT_FAILURE);
	}
	
	int id=0;
	while (fgets (buffer, sizeof(buffer), fd2) != NULL) {
		buffer[strlen(buffer) - 1 ] = '\0';
		dbg("main() : buffer = %s\n", buffer);
		c = sscanf (buffer, "%s", pkg2[id]);
		dbg("main() : pkg1 = %s\n", pkg2[id]);
		
		if ( regexec(&regex, pkg2[id], 2, pmatch, 0) ) {
			dbg("main() : regex doesn't match\n");
			continue;
		} else {

			// Retrieving unique name
			start = pmatch[1].rm_so;
			end = pmatch[1].rm_eo;
			size = end - start;
			strncpy (softname2[id], pkg2[id] + start, size);
			softname2[id][size] = '\0';
			
			dbg("main() : softname2 = %s\n", softname2[id]);
		}

		id++;
	}
	
	
	while (fgets (buffer, sizeof(buffer), fd1) != NULL) {
		buffer[strlen(buffer) - 1 ] = '\0';
		dbg("main() : buffer = %s\n", buffer);
		
		pkg1[0] = '\0';
		c = sscanf (buffer, "%s %s",category,pkg1);
		dbg("main() : category = %s\n", category);
		dbg("main() : pkg1 = %s\n", pkg1);
		
		if ( regexec(&regex, pkg1, 2, pmatch, 0) ) {
			dbg("main() : regex doesn't match\n");
			continue;
		} else {
			
			// Retrieving unique name
			start = pmatch[1].rm_so;
			end = pmatch[1].rm_eo;
			size = end - start;
			strncpy (softname1, pkg1 + start, size);
			softname1[size] = '\0';
			
			dbg("main() : softname1 = %s\n", softname1);
		}
		for (i=0; i< id; i++) {
			if ( strncasecmp (softname1, softname2[i], 128) == 0){
				break;
			}
		}
		printf("%s | %s | %s\n",category, pkg1, pkg2[i]);	
	}
	fclose (fd1);
	fclose (fd2);
	regfree(&regex);
	return(EXIT_SUCCESS);
}


/* usage */
int usage(void) {
	printf("\nlistbuilder v%s : build 1 main list from 2 package lists\n", VERSION);
	printf("Usage : listbuilder -a filename1 -i filename2\n");
	printf("\"filename1\" must contain lines in format \"category package\"\n");	
	printf("\"filename2\" must contain lines in format \"package\"\n");	
	return(EXIT_SUCCESS);
}
