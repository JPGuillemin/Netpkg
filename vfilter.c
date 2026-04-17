/*
 * vfilter - compare two package names
 *
 * Modified from vfilter
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
#include <regex.h>

#define VERSION "1.0"
#define TRUE 1
#define FALSE 0

#ifdef DEBUG
#define dbg(fmt,args...) fprintf(stderr, fmt , ## args)
#else
#define dbg(fmt,args...) do { } while (0)
#endif

struct pkgfields
{
  char name[257];
  char version[33];
  char build[33];
  char vserial[129];
  char bserial[65];
};

/**********************/
/*      PROTOTYPES    */
/**********************/

/* Get version and build fields from package name */
int getfields (struct pkgfields *pkg);

/* Transform homosapiens into Hal9000 style version string */
int vserialize (struct pkgfields *pkg);
int bserialize (struct pkgfields *pkg);

/* usage function */
int usage(void);

/***************** MAIN ********************/
int main(int argc, char *argv[]) {

  if (argc != 3) {
    usage();
    exit(EXIT_FAILURE);
  }

  struct pkgfields *pkg1;
  pkg1 = (struct pkgfields *) malloc(sizeof(struct pkgfields));
  struct pkgfields *pkg2;
  pkg2 = (struct pkgfields *) malloc(sizeof(struct pkgfields));

  int versioncmp, buildcmp;
  regex_t regex;

  // Copy package names from arguments
  strncpy(pkg1->name, argv[1], sizeof(pkg1->name) - 1);
  pkg1->name[sizeof(pkg1->name) - 1] = '\0';

  strncpy(pkg2->name, argv[2], sizeof(pkg2->name) - 1);
  pkg2->name[sizeof(pkg2->name) - 1] = '\0';

  // Compile regex for package name parsing
  if (regcomp(&regex, "^.*-([^-]*)-[^-]*-([^-]*)\\.t[glx]z$", REG_EXTENDED)) {
    fprintf(stderr, "Error: regex compilation failed\n");
    exit(EXIT_FAILURE);
  }

  // Extract version and build from package names
  if (getfields(pkg1) != 0) {
    fprintf(stderr, "Error: cannot process package name: %s\n", pkg1->name);
    regfree(&regex);
    exit(EXIT_FAILURE);
  }

  if (getfields(pkg2) != 0) {
    fprintf(stderr, "Error: cannot process package name: %s\n", pkg2->name);
    regfree(&regex);
    exit(EXIT_FAILURE);
  }

  // Serialize versions for comparison
  if (vserialize(pkg1) != 0) {
    fprintf(stderr, "Error: serializing version for %s\n", pkg1->name);
    regfree(&regex);
    exit(EXIT_FAILURE);
  }

  if (vserialize(pkg2) != 0) {
    fprintf(stderr, "Error: serializing version for %s\n", pkg2->name);
    regfree(&regex);
    exit(EXIT_FAILURE);
  }

  // Compare versions
  versioncmp = strcmp(pkg1->vserial, pkg2->vserial);
  dbg("%s versions %s <-> %s\n", pkg1->name, pkg1->vserial, pkg2->vserial);

  if (versioncmp < 0) {
    // version1 < version2
    dbg("version1 < version2 (%d)\n", versioncmp);
    regfree(&regex);
    free(pkg1);
    free(pkg2);
    return 2;
  } else if (versioncmp > 0) {
    // version1 > version2
    dbg("version1 > version2 (%d)\n", versioncmp);
    regfree(&regex);
    free(pkg1);
    free(pkg2);
    return 1;
  }

  /* If we are here then version1 == version2,
     so we need to look at the BUILD field */

  if (bserialize(pkg1) != 0) {
    fprintf(stderr, "Error: serializing build for %s\n", pkg1->name);
    regfree(&regex);
    exit(EXIT_FAILURE);
  }

  if (bserialize(pkg2) != 0) {
    fprintf(stderr, "Error: serializing build for %s\n", pkg2->name);
    regfree(&regex);
    exit(EXIT_FAILURE);
  }

  dbg("version1 == version2\n");
  buildcmp = strcmp(pkg1->bserial, pkg2->bserial);
  dbg("%s builds %s <-> %s\n", pkg1->name, pkg1->bserial, pkg2->bserial);

  regfree(&regex);
  free(pkg1);
  free(pkg2);

  if (buildcmp == 0) {
    // build1 == build2 => same package
    dbg("build1 == build2 (%d)\n", buildcmp);
    return 0;
  }

  if (buildcmp < 0) {
    // build1 < build2
    dbg("build1 < build2 (%d)\n", buildcmp);
    return 2;
  } else {
    // build1 > build2
    dbg("build1 > build2 (%d)\n", buildcmp);
    return 1;
  }
}

/* Get fields from package */
int getfields(struct pkgfields *pkg) {

  int start, end;
  regex_t regex;
  regmatch_t pmatch[3];
  size_t size = 0;

  if (regcomp(&regex, "^.*-([^-]*)-[^-]*-([^-]*)\\.t[glx]z$", REG_EXTENDED)) {
    dbg("getfields() : regex failed compilation\n");
    return EXIT_FAILURE;
  }

  if (regexec(&regex, pkg->name, 3, pmatch, 0)) {
    dbg("getfields() : regex doesn't match\n");
    regfree(&regex);
    return EXIT_FAILURE;
  } else {
    // Retrieving the version
    start = pmatch[1].rm_so;
    end = pmatch[1].rm_eo;
    size = end - start;
    strncpy(pkg->version, pkg->name + start, size);
    pkg->version[size] = '\0';

    dbg("getfields() : package version = %s\n", pkg->version);

    // Retrieving the build
    start = pmatch[2].rm_so;
    end = pmatch[2].rm_eo;
    size = end - start;
    strncpy(pkg->build, pkg->name + start, size);
    pkg->build[size] = '\0';

    dbg("getfields() : package build = %s\n", pkg->build);
  }

  regfree(&regex);
  return EXIT_SUCCESS;
}

char *strings_tab[] =
  { "cvs","svn","git","alpha1","alpha2","alpha3","alpha4","alpha",
    "beta1","beta2","beta3","beta4","beta","pre1","pre2","pre3","pre4",
    "pre","rc1","rc2","rc3","rc4","rc",
    "a","b","c","d","e","f","g","h","i","j","k","l",0 };
char high_flags_tab[] = "000111112222233333444446666777788880";
char low_flags_tab[] = "555123451234512345123451234123412340";

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
      if (!strncasecmp(offset, strings_tab[index], strlen(strings_tab[index]))) {
        flag[0] = high_flags_tab[index];
        flag[1] = low_flags_tab[index];
        offset += strlen(strings_tab[index]) - 1; // -1 as we offset++ before next global for loop
        found=TRUE;
        break;
      }
    }
    if (found == FALSE) buffer[i++] = *(offset);
  }

  strncpy (pkg->vserial + (++dots * 8) - i, buffer, i);

  strncpy(pkg->vserial + 126, flag, 2);

  pkg->vserial[128] = '\0';

  dbg("serialize() : vserial = %s\n", pkg->vserial);

  return 0;
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

  return EXIT_SUCCESS;
}

/* usage */
int usage(void) {
  printf("\nvfilter v%s : compare two package names\n", VERSION);
  printf("Usage: vfilter <package1> <package2>\n\n");
  printf("Compares package versions and builds extracted from package names.\n");
  printf("Package format: name-version-arch-build.t[glx]z\n\n");
  printf("Returns:\n");
  printf("  1  if package1 > package2\n");
  printf("  0  if package1 == package2\n");
  printf("  2  if package1 < package2\n");
  return EXIT_SUCCESS;
}
