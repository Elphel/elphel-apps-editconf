/*
** -----------------------------------------------------------------------------**
** editconf.c
**
** Copyright (C) 2006 Elphel, Inc.
**
** -----------------------------------------------------------------------------**
**  This program is free software: you can redistribute it and/or modify
**  it under the terms of the GNU General Public License as published by
**  the Free Software Foundation, either version 3 of the License, or
**  (at your option) any later version.
**
**  This program is distributed in the hope that it will be useful,
**  but WITHOUT ANY WARRANTY; without even the implied warranty of
**  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
**  GNU General Public License for more details.
**
**  You should have received a copy of the GNU General Public License
**  along with this program.  If not, see <http://www.gnu.org/licenses/>.
** -----------------------------------------------------------------------------**
*/

#include <stdlib.h>
#include <stdio.h>
 int hexdigit (char c) {
  int i;
  i=c-'0';
  if ((i>=0) && (i<10)) return i;
  i=c-'a'+10;
  if ((i>=10) && (i<16)) return i;
  i=c-'A'+10;
  if ((i>=10) && (i<16)) return i;
  return 0;	// could be -1??
}

int main(argc, argv)
    int argc;
    char *argv[];
{
    if (argc<2) {printf("unescape returnes unescaped string (argument) to stdout\n");  return 0; }
  int i=0;
  while (argv[1][i]) {
    if ((argv[1][i]=='%') &&  argv[1][i+1]) { // behavior from Mozilla
      printf("%c", (char) (hexdigit(argv[1][i+1])<<4) | hexdigit(argv[1][i+2]));
      i+=3;
    } else printf("%c",argv[1][i++]);
  }
  return 0;
}
