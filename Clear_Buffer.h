/*!
\file
*/
#ifndef __CLEAR_BUFFER_H__
#define __CLEAR_BUFFER_H__

/*!
Clear buffer and show status buffer(is there any symbols after our number/string/symbol)
\param nothing
\return Status buffer - return 1 if there is any extra symbol after our input

                        return 2 if there is only space and \n symbols after our input
*/

int ClearStatusBuffer();

#endif
