#include<ncurses.h>
#include<string.h>
#include<stdlib.h>

////////////////////////////////////////////
void screen(WINDOW *w,int y,int x)
{ int dx,opt;
 char menu[6][8]={"  OPEN "," CLOSE ","SEARCH ","REPLACE"," FORMAT","  EXIT "};
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
typedef struct line
{
 char *str;
 int strleng;
 struct line *prev,*next;
}line;

line *createlinenode()
{
 line *temp;
 temp=(line*)calloc(sizeof(line),1);
 temp->str=NULL;
 temp->prev = temp->next=NULL;
 return temp;
}

///////////////////////////////////////////////////////////////////
/*
void editmode(WINDOW *w,int y,int x,char file[100])
{
FILE *fp=fopen(file,"r");
line *head=NULL,*trav=NULL,*temp=NULL;
char ch,*str1,*str2,*string=(char*)calloc(sizeof(char),1);

trav=head=createlinenode();
trav->str=string;
trav->strleng=0;

}

*/
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
 char menu[6][8]={"  OPEN "," CLOSE ","SEARCH ","REPLACE"," FORMAT","  EXIT "};
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
     mvwprintw(wtext,1,100,"%s","                                                                         ");
     mvwprintw(wtext,1,100,"please close the curent file '%s' to open another file",file);
     wrefresh(wtext);
     wgetch(wtext);
     mvwprintw(wtext,1,100,"%s","                                                                         ");
     readmode(wtext,42,165,fp,file,"",0);
     wrefresh(wtext);

    }
    else
    {
     mvwprintw(wtext,1,100,"                                                                               ");
     mvwprintw(wtext,1,100,"%s","File_Name::");
     echo();
     mvwscanw(wtext,1,112,"%s",file);
     noecho();
     mvwprintw(wtext,5,10,"                                                                               ");

     fp=fopen(file,"r");
     if(fp==NULL)
     {mvwprintw(wtext,5,10,"'%s' no such file exists",file);
      wgetch(wtext);
      strcpy(file,"");
      mvwprintw(wtext,5,10,"                                                                               ");
      mvwprintw(wtext,1,100,"%s","                                                                         ");
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
     mvwprintw(wtext,1,100,"%s","                                                                         ");
     mvwprintw(wtext,1,100,"closing the file '%s'...",file);
     wrefresh(wtext);
     wgetch(wtext);
     fclose(fp);
     fp=NULL;
     mvwprintw(wtext,1,100,"%s","                                                                         ");
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
     mvwprintw(wtext,5,10,"                                                                               ");
     //mvwprintw(wtext,1,100,"%s","                                                                         ");
     wrefresh(wtext);
     break;
    }
    strcpy(search,"");
    mvwprintw(wtext,1,100,"                                                                               ");
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
     mvwprintw(wtext,5,10,"                                                                               ");
     //mvwprintw(wtext,1,100,"%s","                                                                         ");
     wrefresh(wtext);
     break;
    }
    strcpy(replace,"");
    mvwprintw(wtext,1,100,"                                                                               ");
    mvwprintw(wtext,1,100,"%s","Replace::");
    echo();
    mvwscanw(wtext,1,112,"%s",search);
    noecho();
    mvwprintw(wtext,1,100,"%s","                                                                         ");
    mvwprintw(wtext,1,100,"%s","With::");
    wrefresh(wtext);
    echo();
    mvwscanw(wtext,1,112,"%s",replace);
    noecho();
    mvwprintw(wtext,1,100,"%s","                                                                         ");
   // readmode(wtext,42,165,fp,file,search,2);
    fclose(fp);
    replacemode(file,search,replace);
    fp=fopen(file,"r");
    readmode(wtext,42,165,fp,file,replace,2);

    break;
   case 5:////exiting//
    if(fp!=NULL)
    {
     mvwprintw(wtext,1,100,"%s","                                                                         "); 
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

