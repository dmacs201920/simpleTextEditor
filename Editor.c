#include<ncurses.h>
#include<string.h>
#include<stdlib.h>

////////////////////////////////////////////
void screen(WINDOW *w,int y,int x)
{ int dx,opt;
 char menu[6][8]={"  OPEN "," CLOSE ","SEARCH ","REPLACE","  EDIT ","  EXIT "};
 wborder(w, '|', '|', '-','-','+','+','*','*');
 /////////top line/////////
 for(dx=1;dx<x-1;dx++)
 { mvwprintw(w,0,dx,"%c",'-');
  wrefresh(w);
 }
 mvwprintw(w,0,x-1,"%c",'+');
 wrefresh(w);
 wrefresh(w);
 ////////bottom line///////
 for(dx=1;dx<x-1;dx++)
 { mvwprintw(w,y-1,dx,"%c",'-');
  wrefresh(w);
 }
 mvwprintw(w,y-1,x-1,"%c",'*');
 wrefresh(w);
 wmove(w,1,1);
 ///////middle line///////
 for(dx=1;dx<x-1;dx++)
 { mvwprintw(w,2,dx,"%c",'-');
  wrefresh(w);
 }
 ///////////menu/////////
 for(opt=0;opt<6;opt++)
 {
  mvwprintw(w,1,10*(opt)+1,"%s",menu[opt]);
  wrefresh(w);
 }
 ////////////////////////
 wrefresh(w);
}
///////////edit mode/////////////////////
typedef struct linenode
{
 char *str;
 int strleng,size;
 struct linenode *prev,*next;
}linenode;

linenode *createlinenode()
{
 linenode *temp;
 temp=(linenode*)calloc(sizeof(linenode),1);
 temp->str=NULL;
 temp->prev = temp->next=NULL;
 temp->size=1;
 return temp;
}

///////////////////////////////////////////////////////////////////
void update(WINDOW *w,int sy,int ey,int sx,int ex);
/////////////////////////////////////////////////////
void  printeditmode(WINDOW *w,int y,int x,linenode *head)
{
 linenode *trav=head;
 char *string;
 int dx=0,dy=3,len,pos;
 y-=2;
 //update(w,3,40,1,163);
 
 while(trav!=NULL)
 { 
  len=trav->strleng;
  string=trav->str;
  for(pos=0;pos<len;pos++)
  {
   if(dx<x)
    dx++;
   else
   { dx=1; dy++; }
   mvwprintw(w,dy,dx,"%c",string[pos]);
  // wrefresh(w);
  // wgetch(w);
  }
//  wgetch(w);
  wrefresh(w);
  if(dy<y)
  { dy++; dx=0; 
    trav=trav->next;
  }
  else
   break;
 }
}



void editmode(WINDOW *w,int y,int x,char file[100])
{
 FILE *fp=fopen(file,"r");
 linenode *head=NULL,*trav=NULL,*temp=NULL,*top=NULL;
 char ch,*str1,*str2,*string;
 int pos,len,dx,dy,i,ychange,xchange;

 string=(char*)calloc(sizeof(char),1);
 trav=head=createlinenode();
 trav->str=string;
 trav->strleng=0;
 trav->size=len=1;                //including an extra space in the end
 pos=-1;

 update(w,3,40,1,163);
 wmove(w,3,1);

 while(!feof(fp))
 { 
  if((ch=fgetc(fp))!='\n')
  { len++;  pos++;
    string=(char*)realloc(string,len);
    string[pos]=ch;
    //wprintw(w,"%c",trav->str[pos]);
    //wgetch(w);
    (trav->strleng)++;
  }
  else
  { //wprintw(w,"\n");
    trav->str=string;
    trav->size=len;

    temp=createlinenode();
    temp->prev=trav;
    trav->next=temp;
    trav=temp;
    string=(char*)calloc(sizeof(char),1);
    trav->str=string;
    len=1;
    pos=-1;
  }
 }
 top=head;
 printeditmode(w,y,x,top);
 ///////////////////
 wmove(w,3,1);
 trav=top;
 string=trav->str;
 len=trav->size;
 i=0; ///indexing
 dx=1; dy=3;
 ch=wgetch(w);
 while(ch!=27)////escape
 {
  switch(ch)
  {case 4://left   <<--
        if(i>0) ///same line
	{  i--;
	   if(dx>1)
	     dx--;
	   else
	   {dy--; dx=163;  }
	}
	else  /////previous line
	{ if(dy!=3)
	  { trav=trav->prev;
	    len=trav->strleng;
	    i=trav->strleng-1;
           
	    if(trav->strleng==0)      //if empty string
	     dx=1;
	    else
	     dx=(trav->strleng)%163;

	    if(dy>1)
	     dy--;
	  }
	  else if(trav->prev!=NULL)//move up by 1 line   
	  { trav=trav->prev;
	    len=trav->strleng;
	    string=trav->str;
	    i=trav->strleng-1;

	    if(trav->strleng==0)      //if empty string
	     dx=1;
	    else
	     dx=(trav->strleng)%163;
           
	    dy=(trav->strleng/163)+3;//last character of previous line 
            top=trav;
	    printeditmode(w,y,x,top);
          }
	}
        mvwprintw(w,41,100,"                                                                                  "); 
	mvwprintw(w,41,100,"i=%d  ,  x=%d  ,  y=%d   , strlen=%d  , size=%d  ",i,dx,dy,trav->strleng,trav->size);
        wmove(w,dy,dx); 
    break;
   case 5://right  -->>                 /////remember you have not done scroll down
      // if(i<(trav->strleng-1))  //same line
       if(i<(trav->size-1)&&trav->size>1)
       { i++;
         if(dx<163)
          dx++;
         else
         { dx=1; dy++; }
       }
       else if(trav->next!=NULL)
       { trav=trav->next;
         len=trav->strleng;
	 string=trav->str;
        if(trav->strleng==0)
	 i=-1;
	else
	  i=0;
         dx=1;
         dy++;
       }
        mvwprintw(w,41,100,"                                                                                  ");
        mvwprintw(w,41,100,"i=%d  ,  x=%d  ,  y=%d   , strlen=%d  , size=%d  ",i,dx,dy,trav->strleng,trav->size);
       wmove(w,dy,dx);
    break;
   case 3://up     ^^^^
       if(i<163)
       { if(trav->prev!=NULL)
         { trav=trav->prev;
           string=trav->str;
	   len=trav->strleng;
           dy--;
           if(trav->strleng!=0)
           { i=(trav->strleng-1) ;
             if(dx>(trav->strleng%163))
             { if(trav->strleng==0)
                dx=1;
	       else
 	        dx=(trav->strleng%163);
             }
             else
              i=i-((trav->strleng%163)-dx)  ;
           }
	   else
	   {i=-1;dx=1; }
         }
       }
       else
       {  dy--;   i=i-163; }
       mvwprintw(w,41,100,"                                                                                  ");
       mvwprintw(w,41,100,"i=%d  ,  x=%d  ,  y=%d   , strlen=%d  , size=%d  ",i,dx,dy,trav->strleng,trav->size);
       wmove(w,dy,dx);    
    break;
   case 2://down   vvvv   ////remember you have not written to scroll down 
         ychange=trav->strleng/163+1;
         
	 if( ((i+163)<trav->strleng)  &&(ychange > 2) )   ///more than 2 lies i.e. 3rd line or more no.of lines
         { dy++; i=i+163; }
         else if( ((i/163)+1)==(ychange-1) )  ///last second line
         { if(i+163<trav->strleng )
           { dy++;  i=i+163;   }
           else
           { i=trav->strleng-1;
             dx=(trav->strleng%163);
             dy++;
           }
         }
         else                                 //last line
         { if(trav->next!=NULL)
           { trav=trav->next;
	     string=trav->str;
             len=trav->strleng;
             dy++;
             if(dx>trav->strleng)
             { if(trav->strleng==0)
                 dx=1;
               else
	         dx=(trav->strleng%163);
                
	       i=trav->strleng-1;
             }
             else
              i=dx-1;
           }
         }
         mvwprintw(w,41,100,"                                                                                  ");
        mvwprintw(w,41,100,"i=%d  ,  x=%d  ,  y=%d   , strlen=%d  , size=%d  ",i,dx,dy,trav->strleng,trav->size);

         wmove(w,dy,dx);
     break;
   case '\n'://newline
     if(i==0)
     {
      temp=createlinenode();
      temp->str=trav->str;
      temp->strleng=trav->strleng;
      temp->size=trav->size;
      temp->next=trav->next;
      temp->prev=trav;
     
      if(trav->next!=NULL)
        trav->next->prev=temp;

      str1=(char*)calloc(sizeof(char),1);
      trav->str=str1;
      trav->strleng=0;
      trav->size=1;
      trav->next=temp;

      trav=temp; 

      dy++; dx=1; i=0;
        mvwprintw(w,41,100,"                                                                                  ");
        mvwprintw(w,41,100,"i=%d  ,  x=%d  ,  y=%d   , strlen=%d  , size=%d  ",i,dx,dy,trav->strleng,trav->size);
	 wmove(w,dy,dx);

     }

     else if(i<(trav->strleng)&&i>0)             
     {
     // mvwprintw(w,10,60,"string::%s",string);

      str1=(char*)calloc(sizeof(char),i+1);
      strncpy(str1,string,i);
      //mvwprintw(w,10,60,"size=%d,,%s",sizeof(str1),str1);
      //wgetch(w);
      
      str2=(char*)calloc(sizeof(char),((trav->strleng)-1)-i +2);
      strncpy(str2,string+i,( (trav->strleng)-1 )-i +1);
    //  mvwprintw(w,11,60,"size=%d,,%s",sizeof(str2),str2);
     // wgetch(w);

      free(string);
      trav->str=str1;
      trav->strleng=strlen(str1);
      trav->size=strlen(str1)+1; 

      temp=createlinenode();
      temp->prev=trav;
      temp->next=trav->next;
      
      if(trav->next!=NULL)
       trav->next->prev=temp;
   
      trav->next=temp;
      trav=temp;
      temp->str=str2;
      string=str2;
      temp->strleng=strlen(str2);
      temp->size=strlen(str2)+1;
      
      dy++; dx=1; i=0;
         mvwprintw(w,41,100,"                                                                                  ");
         mvwprintw(w,41,100,"i=%d  ,  x=%d  ,  y=%d   , strlen=%d  , size=%d  ",i,dx,dy,trav->strleng,trav->size);
	 wmove(w,dy,dx);
     }
     /*else if(i==trav->strleng)
     { temp=createlinenode();
       if(trav->next!=NULL)
       { temp->next=trav->next;
         temp->prev=trav;
         trav->next->prev=temp; 
       }
       else
       {
	trav->next=temp;
        temp->prev=trav;
       }

     }*/

     else
     {
       temp=createlinenode();
       if(trav->next!=NULL)
       { temp->next=trav->next;
         temp->prev=trav;
         trav->next->prev=temp; 
         trav->next=temp;
         
       }
       else
       {
	trav->next=temp;
        temp->prev=trav;
       }
      
       trav=temp;
       string=(char*)calloc(sizeof(char),1);
       trav->str=string;
       trav->strleng=0;
       trav->size=1;
       len=0;
       
       dy++; dx=1; i=-1;
       
       mvwprintw(w,41,100,"                                                                                  ");
	        mvwprintw(w,41,100,"i=%d  ,  x=%d  ,  y=%d   , strlen=%d  , size=%d  ",i,dx,dy,trav->strleng,trav->size);
      
      wmove(w,dy,dx);
     }
      update(w,3,40,1,163);
      //wgetch(w);
      printeditmode(w,y,x,top);
      wrefresh(w);
      wmove(w,dy,dx);
    break;
   default://///character
    (trav->strleng)++;
    (trav->size)++;
    len++;
    
    if( (i>=0) && i<(trav->strleng) )     ////  0 <= i < strleng-1
    {
     str1=(char*)calloc(sizeof(char),len);
     if(i==0)
     { str1[0]=ch;
      strcat(str1,string);
     }
     else
     {strncpy(str1,string,i);
      str1[i]=ch;
      strncpy(str1+i+1,string+i,( (trav->strleng)-1 )-i );
     }
     free(string);
     string=str1;
     trav->str=string;
     pos=i;
     for(int tx=x-1,ty=y ; pos < (trav->strleng) ; pos++ )
     { if(tx<163)
      tx++;
      else
      { tx=1; ty++; }
      mvwprintw(w,ty,tx,"%c",string[pos]);
      wrefresh(w);
     }
     printeditmode(w,y,x,top);
     //wrefresh(wtext);
     i++;
     if(dx<163)
      dx++;
     else
     { dx=1; dy++; }
    }

    else  //i==trav->strleng
    { 
     //string[i]= ch;
     //string=(char*)realloc(string,sizestr);
     str1=(char*)calloc(sizeof(char),len);
     
     if(i==-1)
      i++;
     else
      strcpy(str1,string);
     
     str1[i]=ch;
     free(string);
     string=str1;
     trav->str=string;
     i++;
     mvwprintw(w,dy,dx,"%c",ch);
     if(dx<163)
      dx++;
     else
     { dx=1; dy++; }
     //mvwprintw(w,dy,dx,"%c",ch);
     wrefresh(w);
    }
     mvwprintw(w,41,100,"                                                                                  ");
     mvwprintw(w,41,100,"i=%d  ,  x=%d  ,  y=%d   , strlen=%d  , size=%d  ",i,dx,dy,trav->strleng,trav->size);

    wmove(w,dy,dx);
    break;
  }
  ch=wgetch(w);
 }
 ///////////end while///////////////
 //////////save or not///////
     char dfile[100];
     FILE *np=NULL;
      mvwprintw(w,15,60,"*---------------------------------------------------------------------------------------*");
      mvwprintw(w,16,60,"|                                                                                       |");
      mvwprintw(w,17,60,"|          Do you want to save the file(y,n):                                           |");
      mvwprintw(w,18,60,"|                                                                                       |");
      mvwprintw(w,19,60,"|                                                                                       |");
      mvwprintw(w,20,60,"|                                                                                       |");
      mvwprintw(w,21,60,"|                                                                                       |");
      mvwprintw(w,22,60,"*---------------------------------------------------------------------------------------*");
      wrefresh(w);
      wmove(w,17,105);
     echo();
     ///mvwscanw(w,17,105,"%c",choice);
     ch=wgetch(w);
     noecho();
     wgetch(w);

     if((ch=='y')||(ch=='Y'))
     {  
      mvwprintw(w,17,60,"|        Do you want to rename the file with another name(y,n):                           |");
      wrefresh(w);
      wmove(w,17,123);
      echo();
     /// mvwscanw(w,17,121,"%c",choice);
      ch=wgetch(w);
      noecho();
      wgetch(w);

      if((ch=='y')||(ch=='Y'))
      {
       mvwprintw(w,19,70,"Enter the  new file name( renaming '%s' )",file);
       wrefresh(w);
       echo();
       mvwscanw(w,20,105,"%s",dfile);
       noecho();
       wrefresh(w);
       fclose(fp);
       rename(file,dfile); 
      }
      else
      {
       while(np==NULL)
       {
	mvwprintw(w,19,70,"Enter a new file name( creating)");
        echo();
        mvwscanw(w,20,80,"%s",dfile);
        noecho();
        np=fopen(dfile,"r");
        if(np!=NULL)
        {
         mvwprintw(w,19,70,"File name(%s) already exits",dfile);
	 wrefresh(w);
         wgetch(w);
	 mvwprintw(w,19,60,"|                                                                                       |");
	 mvwprintw(w,20,60,"|                                                                                       |");
	 wrefresh(w);
	 fclose(np);
	 np=NULL;
	}
	else
	{
	  np=fopen(dfile,"w+");
	  trav=head;
	  while(trav!=NULL)
	  {string=trav->str;
	   for(i=0;i<(trav->strleng);i++)
	    fputc(string[i],np);
	   fputc('\n',np);
	   trav=trav->next;
          }
	  fclose(np);
	}
       }
      }
      mvwprintw(w,21,70,"saving the file ( %s ) ",dfile);
      wrefresh(w);
      wgetch(w);
     
     }
     else
      {  mvwprintw(w,20,70,"Closing the edit mode ");
         wgetch(w);
         fclose(fp);
      }
     
 
// fclose(fp);
}


///*/
////////////////////////////////////

void update(WINDOW *w,int sy,int ey,int sx,int ex)
{  int dx=1,dy=3;
   
   while(dy<=40)
   {
  mvwprintw(w,dy,dx,"                                                                                                                                                                   |   ");
  wrefresh(w);
  wrefresh(w);
   dy++;
   }
   wborder(w, '|', '|', '-','-','+','+','*','*');

   wrefresh(w);
}
/*

void update(WINDOW *w,int sy,int ey,int sx,int ex)
{wclear(w);
 screen(w,42,165);
 wrefresh(w);
}
*/
/////////////////////////////////
/*
   FILE *printfile(WINDOW *w,int y,int x,FILE *sp)
   {
   int px=1,py=3;
   y-=1;
   char ch,string[165];
   while(!feof(sp) && py<y)
   {
   fgets(string,165,sp);
   mvwprintw(w,py,px,"%s",string);
   py++;
   }
   wrefresh(w);
   return sp;
   }
 */
/////////////////////////////////////
FILE *printfile(WINDOW *w,int y,int x,FILE *fp)
{
 int px=1,py=3;
 y-=1;
 char a,s[100];
 wmove(w,py,px);
 while(fscanf(fp,"%s",s)!=EOF&&py<y)
 {
  mvwprintw(w,py,px,"%s",s);
  wrefresh(w);
  px+=strlen(s);  if(px>164)   { px=1;    py++; }

  while((a=fgetc(fp))==' '||a=='\n'||a=='\t')
  {  if(a=='\n')
   { py++; px=1;
    wmove(w,py,px);
   }
   else
   {
    mvwprintw(w,py,px,"%c",a);
    if(a==' ') px++;
    else    px+=4;
   }
  }
  fseek(fp,-1,1);
  if(px>164)   { px=1;    py++; }

  wrefresh(w);
 }
 wrefresh(w);
 return fp;
}
/////////////////////////////////////////////
FILE * printsearchfile(WINDOW *w,int y,int x,FILE *fp,char search[100])
{
 int px=1,py=3;
 y-=1;
 char a,s[100];
 wmove(w,py,px);
 while(fscanf(fp,"%s",s)!=EOF && py<y)
 {
  if (strcmp(s,search)==0)
  {
   wattron(w,A_STANDOUT);
   mvwprintw(w,py,px,"%s",s);
   px+=strlen(s);
   wrefresh(w);
   wattroff(w,A_STANDOUT);
  }
  else
   mvwprintw(w,py,px,"%s",s);
  wrefresh(w);

  px+=strlen(s);  if(px>164)   { px=1;    py++; }

  while((a=fgetc(fp))==' '||a=='\n'||a=='\t')
  {  if(a=='\n')
   { py++; px=1;
    wmove(w,py,px);
   }     
   else
   {
    mvwprintw(w,py,px,"%c",a);
    if(a==' ') px++;
    else    px+=4;
   }
  }
  fseek(fp,-1,1);
  if(px>164)   { px=1;    py++; }

  wrefresh(w);
 }
 return fp;
}
////////////////////////////////////////
void replacemode(char file[100],char search[100],char replace[100])
{ FILE *fp=fopen(file,"r");
 FILE *dp=fopen("temp.txt","w");
 char s[100],a;


 while(fscanf(fp,"%s",s)!=EOF)
 {
  if(strcmp(s,search)==0)
   fprintf(dp,"%s",replace);
  else
   fprintf(dp,"%s",s);

  while((a=fgetc(fp))==' '||a=='\n'||a=='\t')
   fprintf(dp,"%c",a);
 
  fseek(fp,-1,1);
 }
 fclose(fp);
 remove(file);
 fclose(dp);
 rename("temp.txt",file);
 // fclose(dp);

}

////////////////////////////////////////
typedef struct line { int pos; }line;

FILE *lp;///global variable

int  readmode(WINDOW *w,int wy,int wx,FILE *fp,char file[100],char search[100],int mode)
{
 FILE *rp,*sp=fp,*ep;
 int i=0,x=1,y=3,lpos;
 char ch,string[165];
 line lnode;
 if(mode!=2)
  mode=0;

 if(ftell(fp)==0)
 { rp=fopen(file,"r");
  lp=fopen("line.txt","w+");
  while(!feof(rp))
  { lpos=ftell(rp);
   fgets(string,165,rp);
   lnode.pos=lpos;
   fwrite(&lnode,sizeof(lnode),1,lp);
  }
  fseek(rp,0,SEEK_SET);
  fseek(lp,0,SEEK_SET);
 }
 update(w,3,40,1,163);
 if (mode==1)   ep=printfile(w,42,165,sp);
 else ep=printsearchfile(w,42,165,fp,search);
 wrefresh(w);
 wmove(w,y,x);

 while( 1 )
 {
  ch=wgetch(w);
  ////////////////////
  switch(ch)
  {
   case 27://///escape
    return 1;
   case 3: ////up
    if(y>3)
     y--;
    else
    {
     fread(&lnode,sizeof(lnode),1,lp);
     fseek(lp,-sizeof(lnode),SEEK_CUR);
     if(lnode.pos!=0)
     {
      update(w,3,40,1,163);
      fseek(lp,-sizeof(lnode),SEEK_CUR);
      fread(&lnode,sizeof(lnode),1,lp);
      fseek(lp,-sizeof(lnode),SEEK_CUR);
      fseek(sp,lnode.pos,SEEK_SET);

      if (mode==1)   ep=printfile(w,42,165,sp);
      else           ep=printsearchfile(w,42,165,fp,search);

      wrefresh(w);
     }
    }
    wmove(w,y,x);
    break;
   case 2: ///down
    if(y<40)
     y++;

    else if(y==40 && !feof(sp))
    {  update(w,3,40,1,163);
     fread(&lnode,sizeof(lnode),1,lp);
     fseek(sp,lnode.pos,SEEK_SET);

     if (mode==1)   ep=printfile(w,42,165,sp);
     else           ep=printsearchfile(w,42,165,fp,search);

     wrefresh(w);
    }
    wmove(w,y,x);
    break;
   case 4: //left
    if(x>1)
     x--;
    else if(x==1&&(y-1)>3)
    { x=163;  y--; }
    wmove(w,y,x);
    break;
   case 5: //right
    if(x<164)
     x++;
    else if(x==139&&(y-1)<40)
    { x=0; y++; }
    wmove(w,y,x);
   default:
    wrefresh(w);
    break;
  }
  //^///switch end//////////////
  wrefresh(w);
 }
 //^////while end///////////////
}
///////////////////////////////////
int menu(WINDOW *w,int y,int x)
{ char ch;
 char menu[6][8]={"  OPEN "," CLOSE ","SEARCH ","REPLACE","  EDIT ","  EXIT "};
 int opt=0;

 wattron(w,A_STANDOUT);
 mvwprintw(w,1,10*(opt)+1,"%s",menu[opt]);
 wattroff(w,A_STANDOUT);
 wrefresh(w);

 while( 1 )
 { ch=wgetch(w);
  switch(ch)
  { case 5://left//
   mvwprintw(w,1,10*(opt)+1,"%s",menu[opt]);
   opt++;
   if(opt==6)
    opt=0;
   wattron(w,A_STANDOUT);
   mvwprintw(w,1,10*(opt)+1,"%s",menu[opt]);
   wattroff(w,A_STANDOUT);
   wrefresh(w);
   break;
   case 4://right//
   mvwprintw(w,1,10*(opt)+1,"%s",menu[opt]);
   opt--;
   if(opt==-1)
    opt=5;
   wattron(w,A_STANDOUT);
   mvwprintw(w,1,10*(opt)+1,"%s",menu[opt]);
   wattroff(w,A_STANDOUT);
   wrefresh(w);
   break;
   case '\n'://enter or selected
   mvwprintw(w,1,10*(opt)+1,"%s",menu[opt]);
   return opt;
   break; 
   default:
   break;
  }
 }
}
//////////////////////////////////////////////
void main()
{
 int opt;
 char file[100],search[100],replace[100],input[100];
 WINDOW *wtext;
 FILE *fp=NULL,*dp;
 strcpy(file,"");
 initscr();
 cbreak;
 noecho();

 wtext = newwin(42,165,0,0);
 curs_set( 2 );
 keypad(wtext,TRUE);

 screen(wtext,42,165);
 while(1)
 {
  opt=menu(wtext,42,165);
  switch(opt)
  {
   case 0://openinig//
    if(fp!=NULL)
    {
     mvwprintw(wtext,1,100,"%s","                                                             ");
     mvwprintw(wtext,1,100,"please close the curent file '%s' to open another file",file);
     wrefresh(wtext);
     wgetch(wtext);
     mvwprintw(wtext,1,100,"%s","                                                             ");
    readmode(wtext,42,165,fp,file,"",0);
     wrefresh(wtext);

    }
    else
    {
     mvwprintw(wtext,1,100,"                                                                  ");
     mvwprintw(wtext,1,100,"%s","File_Name::");
     echo();
     mvwscanw(wtext,1,112,"%s",file);
     noecho();
     mvwprintw(wtext,1,100,"                                                                  ");

     fp=fopen(file,"r");
     if(fp==NULL)
     {mvwprintw(wtext,5,10,"'%s' no such file exists",file);
      wgetch(wtext);
      strcpy(file,"");
      mvwprintw(wtext,5,10,"                                                                 ");
      mvwprintw(wtext,1,100,"%s","                                                           ");
      wrefresh(wtext);
      break;
     }
     readmode(wtext,42,165,fp,file,"",0);
     wrefresh(wtext);
    }

    break;
   case 1:///closing//
    if(fp==NULL)
    {
     mvwprintw(wtext,5,10,"%s","You have not opened any file to close...");
     wrefresh(wtext);
     wgetch(wtext);
     mvwprintw(wtext,5,10,"%s","                                        ");
     wrefresh(wtext);
    }
    else
    {
     mvwprintw(wtext,1,100,"%s","                                                          ");
     mvwprintw(wtext,1,100,"closing the file '%s'...",file);
     wrefresh(wtext);
     wgetch(wtext);
     fclose(fp);
     fp=NULL;
     mvwprintw(wtext,1,100,"%s","                                                          ");
     wrefresh(wtext);
     update(wtext,3,40,1,163);
     wrefresh(wtext);
    }
    break;
   case 2://searching
    if(fp==NULL)
    {mvwprintw(wtext,5,10,"open a file to search...");
     wgetch(wtext);
     strcpy(file,"");
     mvwprintw(wtext,5,10,"                                                                ");
     //mvwprintw(wtext,1,100,"%s","                                                                         ");
     wrefresh(wtext);
     break;
    }
    strcpy(search,"");
    mvwprintw(wtext,1,100,"                                                                ");
    mvwprintw(wtext,1,100,"%s","Search::");
    echo();
    mvwscanw(wtext,1,112,"%s",search);
    noecho();
    fclose(fp);
    fp=fopen(file,"r");
    readmode(wtext,42,165,fp,file,search,2);
    break;
   case 3://///replacing
    if(fp==NULL)
    {mvwprintw(wtext,5,10,"open a file to search...");
     wgetch(wtext);
     strcpy(file,"");
     mvwprintw(wtext,5,10,"                                                                ");
     //mvwprintw(wtext,1,100,"%s","                                                                           ");
     wrefresh(wtext);
     break;
    }
    strcpy(replace,"");
    mvwprintw(wtext,1,100,"                                                                ");
    mvwprintw(wtext,1,100,"%s","Replace::");
    echo();
    mvwscanw(wtext,1,112,"%s",search);
    noecho();
    mvwprintw(wtext,1,100,"%s","                                                           ");
    mvwprintw(wtext,1,100,"%s","With::");
    wrefresh(wtext);
    echo();
    mvwscanw(wtext,1,112,"%s",replace);
    noecho();
    mvwprintw(wtext,1,100,"%s","                                                           ");
   // readmode(wtext,42,165,fp,file,search,2);
    fclose(fp);
    replacemode(file,search,replace);
    fp=fopen(file,"r");
    readmode(wtext,42,165,fp,file,replace,2);

    break;
   case 4://editting
    if(fp==NULL)
    {mvwprintw(wtext,5,10,"open a file to edit...");
     wgetch(wtext);
     strcpy(file,"");
     mvwprintw(wtext,5,10,"                                                                 ");
     //mvwprintw(wtext,1,100,"%s","                                                                        ");
     wrefresh(wtext);
     break;
    }
    strcpy(search,"");
    mvwprintw(wtext,1,100,"                                                                ");
    mvwprintw(wtext,1,100,"%s","openening file in edit mode...");
    fclose(fp);
    fp=NULL;
    update(wtext,3,40,1,163);
    editmode(wtext,42,165,file);
    update(wtext,3,40,1,163);
    //fp=fopen(file,"r");
    wrefresh(wtext);
    break;
   case 5:////exiting//
    if(fp!=NULL)
    {
     mvwprintw(wtext,1,100,"%s","                                                           "); 
     mvwprintw(wtext,1,100,"%s","please close the current file before exit...");
     wrefresh(wtext);
     break;
    }
    update(wtext,3,40,1,163);
    mvwprintw(wtext,5,10,"%s","Exiting.....");
    wrefresh(wtext);
    //wgetch(wtext);
    goto Exit;
    break;
   default:
    wrefresh(wtext);
    break;
  }
 }




Exit:
 wrefresh(wtext);
 wgetch(wtext);
 delwin(wtext);
 endwin();

}

