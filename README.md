# simpleTextEditor  
#By   : M.Bala Sai Kiran 
#Reg  : 173222
#class: B.Sc(hon) Mathematics

  ************  A novice implementation of a simple text editor in C *************

  Description   :
    Current project is to meet the basic needs of any text editor i.e. to open a file, close a file, search a word, 
    replace a word, edit a file , exit from editor.User can open any file he wants, if the file does not exist than 
    reminds the user that the file entered is not present in the current directory. If the file exist the file is 
    opened in readmode(default), so that the user can traverse through the file. If the user chooses 'search' in the menu ,
    then the user have to enter a word to search in the current file and also can scroll up and down to see where and all the
    searched word is present. If user chooses 'replace' the the user have to    enter the to be replaced and the new word 
    to be placed instead of it. In the edit mode the user can enter the text anywhere in the file he wants in the end user
    get    to choose whether to save the edited file by renaming the file name or in a new file. 
    
    Note:  i. Editor only open the files that are in the current directory. 
          ii. Press Return to choose the feature in the menu.
	 iii. Press Escape to go to the menu or to come out of the choosen mode.
  Files required :
    Header files - <ncurses.h> , <string.h> , <stdlib.h> .
    line.txt     - to keep track of the position of the newlines occured in the current file opened to read(read mode). 
                   So that it will be easy to scroll the window to view the remaining content of the file.
    Note: <ncurses.h>, header file is used only to give an interface to the user.
          If the move(y,x) function is available we need not use this header file.  
        
  Limitations    :
    - In the edit mode one can't scroll down to view the remaining content of the file.
    - One can't open an empty file to write in edit mode.
    - User can't delete a character once entered.(major drawback) 
 




FUNCTIONS
 
1. void screen(WINDOW *w,int y,int x)

     - Function to print the user screen with only MENU and border for the screen.

2. linenode *createlinenode()  
     
     - structure for linenode is
          typedef struct linenode
          {
            char *str;
            int strleng,size;
            struct linenode *prev,*next;
          }linenode;

     - It is to create an node dynamically and return the address of the node. 

3. void update(WINDOW *w,int sy,int ey,int sx,int ex);
    
     - update(..) clears the text screen(i.e. where the text is visible to user) and make it available for 
       writting a new file onto the screen.

4. void  printeditmode(WINDOW *w,int y,int x,linenode *head) 
    
     - This particular function deals with the actions when the file is already opened to edit.
     
     - Actions: arrows(up,down,left,right), entering a character ,entering a newline ,exit from edit mode.  
     
     - It reads the whole file in the form of text and create a double linked list so that it will be easy to
       traverse through the file and enter the text wherever user enters. 

     - In the end it asks the user whether to save the edited file or not.
     
5. FILE *printfile(WINDOW *w,int y,int x,FILE *fp)

     - It prints the given file( by taking file pointer ) onto the text screen how much ever the screen can accomidate. 

6. FILE * printsearchfile(WINDOW *w,int y,int x,FILE *fp,char search[100])

     - It prints the given file( by taking file pointer ) onto the text screen how much ever the screen can accomidate. 
     - It highlights the word that is searched in the give file.
         
7. int  readmode(WINDOW *w,int wy,int wx,FILE *fp,char file[100],char search[100],int mode)
    
     - It reads the given file by keeping track of the new lines in the give file. 
     - If parameter search is empty it doesn't search any word in the file , else it searches the given word in the file.

     - Actions: arrows(up,down,left,right), exit from read mode.  
8. int menu(WINDOW *w,int y,int x)
  
     - This function take cares of the menu for the user . Whether the user wants to open a file, close a file, 
       search a word, replace a word,edit a file ,exit from editor. 







