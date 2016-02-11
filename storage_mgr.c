#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "dberror.h"
#include "storage_mgr.h"
#include <sys/stat.h>
/******************************************************************************************************************/
/********************************************manipulating page files***********************************************/
/******************************************************************************************************************/

 void initStorageManager (void){
}


 RC createPageFile (char *fileName){

  FILE *fp;  
  char *page = malloc(2*PAGE_SIZE); 
  memset(page, 0, 2*PAGE_SIZE);
  fp = fopen(fileName, "wb");	                             /*create a new pagefile as fileName for reading and writing*/		 
  fwrite(page, 2*PAGE_SIZE, 1, fp);                          /*initialize file size is PAGE_SIZE(one page)  and reserve extra space (i.e. onepage)
																in the beginning of file to store information about the total no.of pages*/
															 /*initialize page with zero bytes in the memset*/

  fclose(fp);                                                /*close file*/ 
  return RC_OK ;											 /*Returns true if the file is closed*/
  }


 RC openPageFile (char *fileName, SM_FileHandle *fHandle){

  FILE *fp;  
  fp = fopen(fileName, "rb+");                                /*open filename to read to perform read and write*/

															  
  if(fp == NULL){											  /*Check whether the file exists or not */

     return  RC_FILE_NOT_FOUND;                               /*if it doesnt returns a exception as file not found*/
  }

  else                                                    
                                                              /*initialize fHandle*/
  
  {

    fseek(fp, 0, SEEK_END);                                   /*set pointer to end of file*/
    long totalPages=ftell(fp)/PAGE_SIZE;		     		  /*get the current size of the file*/
    fHandle->fileName=fileName;                               /*fHandle contains the filename*/           
    fHandle->totalNumPages = totalPages-1;                    /*fHandle has total number of actual data pages present in the file*/

                                                        
    fwrite(fHandle, sizeof(struct SM_FileHandle), 1, fp);     /*store metadata info in beginning of file*/
    

    fHandle->curPagePos =0;                                   /*when the file is opened ,set curPagePos should be 0*/

    fseek(fp, 0, SEEK_SET);                                   /*reset pointer to beginning of file*/
    fHandle->mgmtInfo=fp;                                     /*fHandle has the pointer to beginning of file*/
    return RC_OK ;                                            /*return 0*/
  }
  
 
  }


 RC closePageFile (SM_FileHandle *fHandle){
															   /*close file*/

   if(fHandle == NULL)                                        /*check whether the fhandle is initialized*/
     return RC_FILE_HANDLE_NOT_INIT;                          /*return an error as file handle not initialized*/                  

   if(fclose(fHandle->mgmtInfo) == 0)                         /*close file stream and check error*/
    {
     fHandle->mgmtInfo = NULL;                                /*set fHandle file pointer to null */
     return RC_OK;                                            /*return 0*/
    }
   else
     return RC_FILE_NOT_FOUND;                                /*return error when fclose function fails */
   
   }


 RC destroyPageFile (char *fileName){						 /*destroy the file*/
															  
 
  if(unlink(fileName)==0)                                     /*delete file and check error*/
     return RC_OK;                                            /*File Deleted Successfully*/
  else
     return RC_FILE_NOT_FOUND;                                /*returns error when delete fail*/

  }


/*******************************************************************************************************************/
/********************************************reading blocks from disc***********************************************/
/*******************************************************************************************************************/


 RC readBlock (int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
 {

	/*check whether the file is open and validate the pages*/

  if(fHandle->mgmtInfo == NULL || pageNum > fHandle->totalNumPages - 1|| pageNum < 0)

       return RC_READ_NON_EXISTING_PAGE;                        
 /*return existing page not founf in the fhandle*/
    else

      {

       fseek(fHandle->mgmtInfo, PAGE_SIZE*(pageNum+1), SEEK_SET); /*set pointer to beginning of pageNum page*/

       fread(memPage, PAGE_SIZE, 1, fHandle->mgmtInfo);          /*read page by assigning fhandle as mgmtInfo */

       fHandle->curPagePos = pageNum;                            /*reassign fhandler to curPagePos with the currPage position*/



       }
    return RC_OK;												 /*Returns the response */
   
      
   }

            
 int getBlockPos (SM_FileHandle *fHandle){
             
   if(fHandle->mgmtInfo== NULL)                                  /*file handle not initialized*/

      return RC_FILE_HANDLE_NOT_INIT;                            /*return file handle not initialized*/

   else

       return fHandle->curPagePos;                               /*return current page position in file by assignning 
																   the fhandle to the curPagePos*/            

 }


 RC readFirstBlock(SM_FileHandle *fHandle, SM_PageHandle memPage)
  {
  
   if(fHandle->mgmtInfo == NULL)                                 /*if file handle not initialized*/

    return RC_READ_NON_EXISTING_PAGE;                            /*return read operation not able to perform on the first block*/
  
   else

   {
     fseek(fHandle->mgmtInfo,PAGE_SIZE, SEEK_SET);               /*set pointer to beginning of file data page*/

     fread(memPage,PAGE_SIZE,1,fHandle->mgmtInfo );              /*read first data page*/

     fHandle->curPagePos = 0;                                    /*after read point to the next page*/

   }
  return RC_OK;
   

  }

 RC readPreviousBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
 {
                  

  if(fHandle->mgmtInfo== NULL || fHandle->curPagePos == 0)      /*if fhandle is set to null or only onr page exist*/

    return RC_READ_NON_EXISTING_PAGE;                           /*return error as existing page doesnt have previous block */
   
  else
  {
                                                                /*set the pointer to the beginning of previous block*/
  fseek(fHandle->mgmtInfo, (fHandle->curPagePos)*PAGE_SIZE, SEEK_SET);

  fread(memPage, PAGE_SIZE, 1, fHandle->mgmtInfo );             /*read blothe blocks*/

  fHandle->curPagePos= fHandle->curPagePos-1;                   /*set file handle to the current page position*/

  }
   return RC_OK;                                                 /*return as true*/

  
        
 }


RC readCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)       
         
{																/*check if the current block position is valid*/
                                                                
  if(fHandle->mgmtInfo== NULL || fHandle->curPagePos < 0 || fHandle->curPagePos > fHandle->totalNumPages)

    return RC_READ_NON_EXISTING_PAGE;                           /*return as not able to read the existing page*/

  else
  {
																/*set the pointer to the beginning of current block*/

  fseek(fHandle->mgmtInfo, (fHandle->curPagePos+1) * PAGE_SIZE, SEEK_SET);

  fread(memPage, PAGE_SIZE, 1, fHandle->mgmtInfo );              /*read the current block*/

  }
  return RC_OK;                                                  
        
  
 }

 

 RC readNextBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
 {
                  
																 /*check if the current block position is valid*/

  if(fHandle->mgmtInfo == NULL || fHandle->curPagePos < 0 ||fHandle->curPagePos + 1 > fHandle->totalNumPages)
																

    return RC_READ_NON_EXISTING_PAGE;                            /*return error as existing page doesnt have next block */
   

else
 {


  fseek(fHandle->mgmtInfo, (fHandle->curPagePos+2)*PAGE_SIZE, SEEK_SET); /*set pointer to beginning of next block*/

  fread(memPage, PAGE_SIZE, 1, fHandle->mgmtInfo );              /*read the next block*/

  fHandle->curPagePos = fHandle->curPagePos + 1;                 /*set file handle to the current page position*/

}
  return RC_OK;													 /*return as 0*/
         
 
 }


 RC readLastBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
  {

  if(fHandle->mgmtInfo == NULL || fHandle->curPagePos < 0)        /*check if the current block position is valid*/

     return RC_READ_NON_EXISTING_PAGE;                            /*return error*/

 else
 {

  fseek(fHandle->mgmtInfo, (fHandle->totalNumPages)*PAGE_SIZE, SEEK_SET); /*set pointer to the beginning of last block*/

  fread(memPage, PAGE_SIZE, 1, fHandle->mgmtInfo );               /*read block*/

  fHandle->curPagePos = fHandle->totalNumPages-1;                 /*set fhandle to the current page position as we initialize
																	the pages as 2 on start*/
                                                 
        
  }

  return RC_OK;   					          					 /*return 0*/
}

/************************************************************************************************************/
/****************************************** writing blocks to a page file************************************/
/************************************************************************************************************/

 RC writeBlock(int pageNum, SM_FileHandle *fHandle, SM_PageHandle memPage)
 {
							          /*check whether the file is open and validate the page numbers*/

   if(fHandle->mgmtInfo == NULL || pageNum > fHandle->totalNumPages - 1|| pageNum < 0)
   
        return RC_FILE_HANDLE_NOT_INIT;                           /*return error*/

   else

 {

    fseek(fHandle->mgmtInfo, PAGE_SIZE *( pageNum+1) , SEEK_SET); /*set pointer to the beginning of pageNum*/

    fwrite(memPage, PAGE_SIZE, 1, fHandle->mgmtInfo);             /*write block by setting fhandle to the mgmtInfo*/

    fHandle->curPagePos = pageNum;                                /*reassign fhandler with the current Page position as curPagePos*/
    
    
 }   return RC_OK;
 
 }


RC writeCurrentBlock (SM_FileHandle *fHandle, SM_PageHandle memPage)
{

 

    if(fHandle->mgmtInfo == NULL)                                /*check if file open and has valid pointer to the current block*/
  
        return RC_FILE_NOT_FOUND;                                 /*return error as File Not Found*/

    else

   {
    
    
    fseek(fHandle->mgmtInfo, (fHandle->curPagePos+1)*PAGE_SIZE , SEEK_SET);  /*set pointer to beginning of current block*/

    fwrite(memPage, PAGE_SIZE, 1, fHandle->mgmtInfo);             /*write block*/
    
    
   } 

   return RC_OK;                                                  /*return 0*/
   

   }



 RC  appendEmptyBlock (SM_FileHandle *fHandle)
   {
     char *page;

     if(fHandle->mgmtInfo== NULL)                                 /*check whether the fhandle is initialized*/

         return RC_FILE_NOT_FOUND;                                /*return error as file not found*/

     else
     {

     fseek(fHandle->mgmtInfo,0 ,SEEK_END);                        /*set the pointer to end of file 'SEEK_END'*/

     page = malloc(PAGE_SIZE);                                    /*allocate memory PAGE_SIZE*/    
      
     memset(page, 0, PAGE_SIZE);                                  
  	 
     fwrite(page, PAGE_SIZE, 1, fHandle->mgmtInfo);               /*append page of PAGE_SIZE to file and set fhandle to mgmtInfo*/                

     fseek(fHandle->mgmtInfo,0 ,SEEK_SET);                        /*set pointer to beginning of file*/

     fHandle->totalNumPages++;                                    /*increase total number of pages of a limit of 1*/

      }

    return RC_OK;                                                  /*return 0*/
      

    }


RC ensureCapacity (int numberOfPages, SM_FileHandle *fHandle)

{
  int numNewPages; 	                                          	/*to ensure capacity add the new number of pages*/ 

     
  if (fHandle->mgmtInfo == NULL)                                /*check the fhandle is initialized*/
 
    return RC_FILE_NOT_FOUND;	                 		  		/*return error*/

  else
  {

     if (numberOfPages > fHandle->totalNumPages){                 /*if number of pages required is more than total pages*/
      
       numNewPages = numberOfPages - fHandle->totalNumPages;      /*newpages will be added to the fhandle*/

       while(numNewPages > 0)                                     /*Ierate using while loop until we get the empty block*/
        {
          appendEmptyBlock(fHandle);                              /*perform appendEmptyBlock operations*/

          numNewPages=numNewPages - 1 ;
        }
      }

  }
  return RC_OK;													/*return as 0*/
    
  
 }
/*********************************************End of Program***************************************************************/


