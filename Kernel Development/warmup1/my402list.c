/******************************************************************************/
/* Important CSCI 402 usage information:                                      */
/*                                                                            */
/* This fils is part of CSCI 402 warmup programming assignments at USC.       */
/* Please understand that you are NOT permitted to distribute or publically   */
/*         display a copy of this file (or ANY PART of it) for any reason.    */
/* If anyone (including your prospective employer) asks you to post the code, */
/*         you must inform them that you do NOT have permissions to do so.    */
/* You are also NOT permitted to remove or alter this comment block.          */
/* If this comment block is removed or altered in a submitted file, 20 points */
/*         will be deducted.                                                  */
/******************************************************************************/

/*
 * Author:      William Chia-Wei Cheng (bill.cheng@acm.org)
 *
 * @(#)$Id: listtest.c,v 1.1 2015/08/21 03:52:20 william Exp $
 */

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include "cs402.h"
#include "my402list.h"

  int  My402ListLength(My402List* list){

return list->num_members;
}
  int  My402ListEmpty(My402List* list) {
if(list->num_members==0)
return TRUE;
else
return FALSE;
}

  int  My402ListAppend(My402List* list, void* obj) {
My402ListElem *newelement;
newelement=(My402ListElem *)malloc(sizeof(My402ListElem));
if(newelement)
{
if(list->num_members==0)//if the list is empty
{
list->anchor.next=newelement;
newelement->prev=&list->anchor;
list->anchor.prev=newelement;
newelement->next=&list->anchor;

}
else
{
My402ListElem *last;
last=list->anchor.prev;
last->next=newelement;
newelement->prev=last;
newelement->next=&list->anchor;
list->anchor.prev=newelement;
}
newelement->obj=obj;
list->num_members++;
return TRUE;
}
else
return FALSE;
}
  int  My402ListPrepend(My402List* list, void* obj) {
My402ListElem *newelement;
newelement=(My402ListElem *)malloc(sizeof(My402ListElem));
if(newelement)
{
if(list->num_members==0)//if the list is empty
{
list->anchor.next=newelement;
newelement->prev=&list->anchor;
list->anchor.prev=newelement;
newelement->next=&list->anchor;

}
else
{
My402ListElem *first;
first=list->anchor.next;
first->prev=newelement;
newelement->next=first;
newelement->prev=&list->anchor;
list->anchor.next=newelement;
}
newelement->obj=obj;
list->num_members++;
return TRUE;
}
else
return FALSE;
}
  void My402ListUnlink(My402List* list, My402ListElem* element) {
element->obj=NULL;
if(list->anchor.next==element)//check if element to be deleted is first 1
{
list->anchor.next=element->next;
element->next->prev=&list->anchor;

}
else if(list->anchor.prev==element)// check if element to be deleted is last 1
{
list->anchor.prev=element->prev;
element->prev->next=&list->anchor;
}
else
{
element->prev->next=element->next;
element->next->prev=element->prev;
}
free(element);
list->num_members--;

}
  void My402ListUnlinkAll(My402List* list) {//deletes elements from a list and makes list empty
if(list->num_members!=0){
My402ListElem *item;
item=list->anchor.next;//item is set to first
while(item->next!=&list->anchor)
{
list->anchor.next=item->next;
item->next->prev=&list->anchor;
free(item);
list->num_members--;
item=item->next;
}
list->anchor.next=&list->anchor;
list->anchor.prev=&list->anchor;
free(item);
list->num_members--;
}


}

int  My402ListInsertBefore(My402List* list, void* obj, My402ListElem* element) {
//if element is NULL then it is same as prepend
short int result=0;
if(element==NULL){
result=My402ListPrepend(list,obj);
return result;
}
else {
My402ListElem *newelement;
newelement=(My402ListElem *)malloc(sizeof(My402ListElem));
if(newelement){
element->prev->next=newelement;
newelement->prev=element->prev;
element->prev=newelement;
newelement->next=element;
newelement->obj=obj;
list->num_members++;
return TRUE;
}
else
return FALSE;
}

}
  int  My402ListInsertAfter(My402List* list, void* obj, My402ListElem* element) {
//if element is NULL then it is same as append
short int result=0;
if(element==NULL){
result=My402ListAppend(list,obj);
return result;
}
else {
My402ListElem *newelement;
newelement=(My402ListElem *)malloc(sizeof(My402ListElem));
if(newelement){
element->next->prev=newelement;
newelement->next=element->next;
element->next=newelement;
newelement->prev=element;
newelement->obj=obj;
list->num_members++;
return TRUE;
}
else
return FALSE;
}

}

 
My402ListElem *My402ListFirst(My402List* list) {
if(list->num_members==0)
return NULL;
else
return (list->anchor.next);
}

My402ListElem *My402ListLast(My402List* list) {
if(list->num_members==0)
return NULL;
else
return (list->anchor.prev);
}

My402ListElem *My402ListNext(My402List* list, My402ListElem* element) {
if(list->anchor.prev==element)
return NULL;
else
return element->next;
}

My402ListElem *My402ListPrev(My402List* list, My402ListElem* element) {
if(list->anchor.next==element)
return NULL;
else
return element->prev;
}

  My402ListElem *My402ListFind(My402List* list, void* obj) {

if(list->num_members==0)
return NULL;

My402ListElem * item;
item=list->anchor.next;//item is set to first element in a list
while(item->next!=&list->anchor)
{
if(item->obj==obj)
return item;
else
item=item->next;
}
if(item->obj==obj)// item will be the last
return item;
else
return NULL;
}

int My402ListInit(My402List* list) {

if(list==NULL)
return FALSE;
list->num_members=0;
list->anchor.next=&list->anchor;
list->anchor.prev=&list->anchor;
list->anchor.obj=NULL;
return TRUE;
//failure condition
}
