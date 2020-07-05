/* -------------------------------------------------------------------------- */
/* string_util.c                                                              */
/* -------------------------------------------------------------------------- */
#include <string.h>
#include <uuid/uuid.h>
#include "common.h"

/* -------------------------------------------------------------------------- */
/* Macro                                                                      */
/* -------------------------------------------------------------------------- */

/* -------------------------------------------------------------------------- */
/* Functions                                                                  */
/* -------------------------------------------------------------------------- */
/*
 * DESCRIPTION
 * This function builds the message with UUID follow by the content of the text
 * parameter
 *
 * PARAMETERS:
 * - msg = Message
 * - text = Text
 *
 * RETURN VALUES:
 * Upon successful completion, buildUuidMsg returns a pointer to the
 * string that contains the message.
 */
char* buildUuidMsg(char *msg, char *text)
{
  uuid_t binuuid;
  char uuid[UUID_LEN];
  uuid_generate_random(binuuid);
  uuid_unparse_upper(binuuid, uuid);
  strcpy(msg, uuid);
  strcat(msg, "::");
  strcat(msg, text);
  return msg;
}

/*
 * DESCRIPTION
 * This function gets a substring from a suorce string.
 *
 * PARAMETERS:
 * - source =  Source string which will be read from the user and provided to
 *             the function as an argument;
 * - target = Target string, substring will be assigned in this variable;
 * - from = Start index from where we will extract the substring (it should be
 *          greater than or equal to 0 and less than string length);
 * - to = End index, the last character of the substring (it should be less than
 *        string length)
 *
 * RETURN VALUES:
 * - 0 = everything's ok
 * - 1 = Invalid 'from' index, this condition happens when from is minor
 *       than 0 or it's greater than the length of the source string;
 * - 2 = Invalid 'to' index, this condition happens when to id greater
 *       than the length of the source string;
 * - 3 = Invalid 'from' and 'to' index, this condition happens when the
 *       from index greater than to index.
 */
int getSubStr(char *source, char *target,int from, int to)
{
  int length=0;
  int i=0,j=0;

  /* get length of source string with out '\0' */
  while(source[i++]!='\0')
    length++;

  if(from<0 || from>length) {
    return 1; /* Invalid 'from' index" */
  }

  if(to>length) {
    return 2; /* Invalid 'to' index */
  }

  if (from>to) {
    return 3; /* Invalid 'from' and 'to' index */
  }

  for(i=from, j=0; i<=to; i++, j++) {
    target[j]=source[i];
  }

  //assign NULL at the end of string
  target[j]='\0';

  return 0;
}

/*
 * DESCRIPTION
 * This function reads UUID string from a string if the UUID is in the first 35
 * chars.
 *
 * PARAMETERS:
 * - uuid = UUID
 * - string = String
 *
 * RETURN VALUES:
 * Upon successful completion, getUUID returns a pointer to the string that
 * contains the UUID. Otherwise, return NULL.
 */
char* getUUID(char *uuid, char *string)
{
  if (getSubStr(string, uuid, 0, 35) == 0) {
    return uuid;
  } else {
    return NULL;
  }
}
