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
#include <unistd.h>
#include <sys/stat.h>
#include <sys/ioctl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>

 
#define THIS_DEBUG 1
#define QRY_MAXPARAMS 64

char  copyQuery [4096];
const char *uri;
const char *method;
const char *query;

struct key_value {
        char *key;
        char *value;
};

struct key_value gparams[QRY_MAXPARAMS+1];

int unescape(char *, int);
int hexdigit(char);
char * paramValue(struct key_value *, char *);
int parseQuery(struct key_value *, char *);


int main(argc, argv)
    int argc;
    char *argv[];
{
  int i; //,j,k,a,l;
  char fileName[256];
  char confKey[256];
//  char confSection[256];
  char confValue[1024];
  char *confFile=NULL;
  char *confFileCopy=NULL;
  char * v;
  int  useEQ=1;
  FILE *  cfile;
  int  thisError=0;
  size_t fileSize;
  char * cp;
  char * ep;
  char * lineEnd;
  char * lineStart;
  char * keyStart;
  char * valueStart;
  
  
  fileName[0]=0;   fileName[sizeof(fileName)-1]=0;
  confKey[0]=0;    confKey[sizeof(confKey)-1]=0;
  confValue[0]=0;  confValue[sizeof(confValue)-1]=0;
//  short int hsz[2];
  
//  struct stat buf;
 
  method = getenv("REQUEST_METHOD");
  query = getenv("QUERY_STRING");
  uri = getenv("REQUEST_URI");
  if (argc > 1) method = NULL; // to fix a bug when a script is called as CGI and uses editconf as a command line app.
  if (method == NULL) {
    if (argc<3) {printf("This program should be run as a CGI from the web server or:\n" \
                        "editconf filename key         -  will return a key value if any\n" \
                        "editconf filename key value   -  will set a value to a key (using '=' as separator)" \
                        "editconf filename key value 0 -  will set a value to a key (using ' ' as separator)");
                        exit (0);}
    strncpy(fileName,argv[1], sizeof(fileName));     
    strncpy(confKey, argv[2], sizeof(confKey));
    if (argc>3) strncpy(confValue,argv[3], sizeof(confValue));
    if (argc>4) useEQ   =atoi(argv[4]);
  
  /* Not a CGI! */
//     printf("This program should be run as a CGI from the web server!\n");
//     exit(1);
  } else {

  i= (query)? strlen (query):0;
//  fprintf(stderr,"querry length=%d\n", i);

  if (i>(sizeof(copyQuery)-1)) i= sizeof(copyQuery)-1;
  if (i>0) strncpy(copyQuery,query, i);
  copyQuery[i]=0;
  unescape(copyQuery,sizeof(copyQuery));

  parseQuery(gparams, copyQuery);

  if((v = paramValue(gparams, "file"))  != NULL) strncpy(fileName,v, sizeof(fileName));
  if((v = paramValue(gparams, "key"))   != NULL) strncpy(confKey,v, sizeof(confKey));
  if((v = paramValue(gparams, "value")) != NULL) strncpy(confValue,v, sizeof(confValue));
  if((v = paramValue(gparams, "eq")) != NULL) useEQ=atoi (v);

//    int keyFound=0;
      printf("Content-Type: text/xml\n");
      printf("Pragma: no-cache\n\n"); // 2 new lines!
      printf("<?xml version=\"1.0\" ?>\n<editconf>\n");
//      printf("<argc>%d</argc>\n",argc);
/*      
   if (fileName[0])  printf("<fileName>%s</fileName>\n",fileName);     
   if (confKey[0])   printf("<confKey>%s</confKey>\n",confKey);     
   if (confValue[0]) printf("<confValue>%s</confValue>\n",confValue);     
                     printf("<useEQ>%d</useEQ>\n",useEQ);     
*/                     
  } // end of METHOD==NULL/else
#ifdef THIS_DEBUG
   fprintf (stderr,"editconf(file='%s'\n",fileName);
   fflush(stderr);
#endif

      unescape (confValue, sizeof(confValue));


//  printf("<file>%s</file>\n",fileName);     
      if ((cfile = fopen(fileName, "r"))==NULL) {thisError=-1;}
      else {
        fseek(cfile, 0L, SEEK_END);
        fileSize=  ftell(cfile);
        confFile= (char*) malloc(fileSize+1);
        fseek(cfile, 0L, SEEK_SET);
        i=fread(confFile, 1, fileSize, cfile);
        for (i=0; i<fileSize; i++) if (confFile[i]==0) confFile[i]=' ';
        confFileCopy= strdup(confFile);
        confFile[fileSize]=0;
        fclose (cfile);
        ep=confFile+fileSize;
        valueStart=ep;
        for (lineStart=confFile;(lineStart-confFile)<fileSize; lineStart=ep+1) {
          cp=lineStart;
          ep=strchr(cp,'\n'); if (!ep) ep=confFile+fileSize;
          ep[0]=0;
// now - just output everything as xml          
          for (lineEnd=cp;(lineEnd<ep) && !strchr("#\n\r",lineEnd[0]);lineEnd++);
          lineEnd[0]=0;
// now the line is truncated by any end of line or "#"          
// still left white spaces before "#"
          lineEnd--;
          while ((lineEnd>=cp) && strchr(" \t",lineEnd[0])) {
            lineEnd[0]=0;
            lineEnd--;
          }
          lineEnd++;
          for (;(strchr(" \t",cp[0])) && (cp<lineEnd);cp++);
          keyStart=cp;
          for (;!(strchr(" \t=",cp[0])) && (cp<lineEnd);cp++);
          cp[0]=0;
          valueStart=cp; // will point on empty string
          if (cp<lineEnd) {
            cp++;
            for (;(strchr(" \t",cp[0])) && (cp<lineEnd);cp++);
            valueStart=cp;
          }
          if (confKey[0]) { // key specified
              if (strncmp(confKey,keyStart, sizeof(confKey))==0) break;
          } else if(keyStart[0] && valueStart[0]) { // no jey specified - output all lines
          if (method == NULL)  printf ("%s\n",valueStart);        // will never be used
          else                 printf ("<%s>%s</%s>\n",keyStart,valueStart,keyStart);
          }    
        } // end of for (lineStart=confFile;(lineStart-confFile)<fileSize; lineStart=ep+1) // may break;
        
//printf ("<fileEnd>%d</fileEnd>\n",fileSize);        
//printf ("<lineStart>%d</lineStart>\n",(lineStart-confFile));        
//printf ("<lineEnd>%d</lineEnd>\n",(lineEnd-confFile));        
//printf ("<keyStart>%d</keyStart>\n",(keyStart-confFile));        
//printf ("<valueStart>%d</valueStart>\n",(keyStart-confFile));
        
        if (!confValue[0]) { // read value]
           if (method == NULL) {
             if ((lineStart-confFile)<fileSize)  printf ("%s\n",valueStart);
           }else   {
              if ((lineStart-confFile)<fileSize)  printf ("<value>%s</value>\n",valueStart);
              else if (confKey[0])                printf ("<error>%d</error>\n",-3); // key not found
            } 
        } else { // modify value
          if ((cfile = fopen(fileName, "w"))==NULL) {thisError=-2;}
          else { // file opened OK
            fwrite(confFileCopy, 1, (lineStart-confFile), cfile); // write everything before the line with a key (if any)
            if (confValue[0]!='#') { // replace the key
              if ((lineStart>confFile) && (confFileCopy[lineStart-confFile-1] != '\n'))  fprintf(cfile,"\n");
              if (useEQ) fprintf(cfile,"%s=%s\n",confKey,confValue);
              else       fprintf(cfile,"%s %s\n",confKey,confValue);
            }
            if ((ep-confFile) < fileSize)  fwrite(confFileCopy+(ep+1-confFile), 1, (confFile+ fileSize - ep-1), cfile); // write everything after the line with a key (if any)
          }
        }
      }
      if (method != NULL) {
        if ((confValue[0])&& !thisError)  printf("<error>0</error>\n");
        if (confFile) free (confFile);
        if (confFileCopy) free (confFileCopy);
        if (thisError)  printf("<error>%d</error>\n",thisError);
        printf("</editconf>\n");
      }  
  return 0; 
}

int unescape (char * s, int l) {
  int i=0;
  int j=0;
  while ((i<l) && s[i]) {
    if ((s[i]=='%') && (i<(l-2)) && s[i+1]){ // behavior from Mozilla
      s[j++]=(hexdigit(s[i+1])<<4) | hexdigit(s[i+2]);
      i+=3;
    } else s[j++]=s[i++];
  }
  if (i<l) s[j]=0;
  return j;
}

int hexdigit (char c) {
  int i;
  i=c-'0';
  if ((i>=0) && (i<10)) return i;
  i=c-'a'+10;
  if ((i>=10) && (i<16)) return i;
  i=c-'A'+10;
  if ((i>=10) && (i<16)) return i;
  return 0; // could be -1??
}

char * paramValue(struct key_value * params, char * skey) { // returns pointer to parameter value, NULL if not defined
  int i=0;
  if (skey)
   while ((i<QRY_MAXPARAMS) && (params[i].key)) {
    if (strcmp(skey,params[i].key)==0) return   params[i].value;

    i++;
   }
  return NULL;
}

int parseQuery(struct key_value * params, char * qry) {
  char * cp;
  int l=0;
  cp=strtok(qry,"=");
  while ((cp) && (l<QRY_MAXPARAMS)) {
    params[l].key=cp;
    cp=strtok(NULL,"&");
    params[l++].value=cp;
    if (cp) cp=strtok(NULL,"=");
  }
 params[l].key=NULL;
 return l;
}
