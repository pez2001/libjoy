/***************************************************************************
 *   Copyright (C) 2008 by Tim Theede                                      *
 *   pez2001@voyagerproject.de                                             *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef libjoy_H
#define libjoy_H


#include <linux/joystick.h>


//the maximum number of joysticks this api will look for
#define MAX_JOYSTICKS 16
//used by filtered get event calls to return any axis/button event
#define ANY_ID 255

//
// Opens a joystick for reading
//
// parameters: char *dev = filename of the device file representing the joystick you want to read from
//             int nonblocking = if nonzero sets reading mode to non blocking
//
// returns an int with a value != -1 if successful
// the int is an identifier to be used in later calls to the library
//
int ljoy_Open(char *dev,int nonblocking);


//
// Opens a joystick by his joyid for reading
//
// parameters: int joyid = id of joystick
//             (for example: joyid==0 will open /dev/input/js0)
//             int nonblocking = if nonzero sets reading mode to non blocking
//
// returns an int with a value != -1 if successful
// the int is an identifier to be used in later calls to the library
//
int ljoy_OpenById(int joyid,int nonblocking);


//
// Gets the Name of a joystick by his joyid
//
// parameters: int joyid = id of joystick
//             (for example: joyid==0 will get the name of /dev/input/js0)
//
// returns a string
// REMEMBER to use free on the returned char* after using 
//
char *ljoy_GetNameById(int joyid);


//
// Gets the number of joysticks connected
//
// returns an int
//
int ljoy_GetJoysticksNum();


//
// Closes a joystick 
//
// parameters: int id = identifier received from an ljoy_Open call
//
// returns an int with a value != -1 if successful
//
int ljoy_Close(int id);


//
// Gets the Name of a joystick
//
// parameters: int id = identifier received from an ljoy_Open call
//
// returns a string
// REMEMBER to use free on the returned char* after using 
//
char *ljoy_GetName(int id);


//
// Gets the Number of Axes of a joystick
//
// parameters: int id = identifier received from an ljoy_Open call
//
// returns an int
//
int ljoy_GetAxesNum(int id);


//
// Gets the Number of Buttons of a joystick
//
// parameters: int id = identifier received from an ljoy_Open call
//
// returns an int
//
int ljoy_GetButtonsNum(int id);


//
// Gets the next event in queue
//
// parameters: int id = identifier received from an ljoy_Open call
//             struct js_event *event = pointer to a js_event struct
//             which will receive the values if the function
//             returns an int > 0
//             (the struct may be modified during call)
//
// returns an int
//             -1 = no event in queue / or error if in blocking mode
//             sizeof(js_event) = the values of an event have been 
//             placed into the struct pointed to by *event
//
int ljoy_GetEvent(int id,struct js_event *event);


//
// Gets the next filtered event in queue 
// ATTENTION: all other events will be discarded
//
// parameters: int id = identifier received from an ljoy_Open call
//             struct js_event *event = pointer to a js_event struct
//             which will receive the values if the function
//             returns an int > 0
//             (the struct may be modified during call)
//             char type = types to be allowed
//             this is bit masked field which can contain
//             one or more of the following flags
//             JS_EVENT_BUTTON         0x01    /* button pressed/released */
//             JS_EVENT_AXIS           0x02    /* joystick moved */
//             JS_EVENT_INIT           0x80    /* initial state of device */
//             char number = wanted button/axis ids
//             ANY_ID can be used to get all buttons/axes
//
// returns an int
//             -1 = no event in queue / or error if in blocking mode
//             sizeof(js_event) = the values of an event have been 
//             placed into the struct pointed to by *event
//
int ljoy_GetFilteredEvent(int id,struct js_event *event,unsigned char type,unsigned char number);


//
// Gets the next filtered event in queue 
// ATTENTION: all other events will be discarded
//
// parameters: int id = identifier received from an ljoy_Open call
//             struct js_event *event = pointer to a js_event struct
//             which will receive the values if the function
//             returns an int > 0
//             (the struct may be modified during call)
//             char type = types to be allowed
//             this is bit masked field which can contain
//             one or more of the following flags
//             JS_EVENT_BUTTON         0x01    /* button pressed/released */
//             JS_EVENT_AXIS           0x02    /* joystick moved */
//             JS_EVENT_INIT           0x80    /* initial state of device */
//
// returns an int
//             -1 = no event in queue / or error if in blocking mode
//             sizeof(js_event) = the values of an event have been 
//             placed into the struct pointed to by *event
//
int ljoy_GetFilteredTypeEvent(int id,struct js_event *event,unsigned char type);


//
// Gets the Ordered Mode State
// 
// returns an int
// 
// 1 = Ordered Mode is activated
// (ljoy_OpenById() will skip unused joystick devices
// 0 = Ordered Mode is deactivated 
// (ljoy_OpenById() will not skip unused joystick devices
//
int ljoy_GetOrderedMode();


//
// Sets the Ordered Mode State
//
// parameters: int order_ids = New State of Ordered Mode
//             1 = Ordered Mode is activated
//             (ljoy_OpenById() will skip unused joystick devices
//             0 = Ordered Mode is deactivated 
//             (ljoy_OpenById() will not skip unused joystick devices
//
void ljoy_SetOrderedMode(int order_ids);






//=======================
//state manager subsystem
//=======================






//
// struct that will contain joystick values after a ljoy_sm_Poll() call
// these structs can be retrieved via the ljoy_sm_GetState() call
//
struct ljoy_sm_state
{
	unsigned int num_axes;//number of axes  in this sm_state struct
	unsigned int num_buttons;//number of buttons in this sm_state struct
	int *axes;//array of axis values
	int *buttons;//array of button values
	long timestamp;//last update time
};

//
// Initiates the state manager
//
// returns an int
// 1 = error (init was called again)
// 0 = init was successful
//
int ljoy_sm_Init();


//
// Shuts down the state manager
//
// returns an int
// 1 = error (state manager was not initiated before)
// 0 = shut down was successful
//
int ljoy_sm_Shutdown();


//
// Adds a joystick to the state manager 
//
// parameters: int id = identifier received from an ljoy_Open call
//
// returns an int
// 1 = error adding device (device not opened / device not in nonblocking mode)
// 0 = adding was successful
//
int ljoy_sm_AddDevice(int id);


//
// Removes a joystick from the state manager
//
// parameters: int id = identifier received from an ljoy_Open call
//
// returns an int
// 1 = error removing device (device was not added to the state manager before)
// 0 = removing was successful
//
int ljoy_sm_RemoveDevice(int id);


//
// Gets the actual state of a joystick
//
// parameters: int id = identifier received from an ljoy_Open call
//             struct ljoy_sm_state **state = pointer to an pointer to a struct ljoy_sm_state
//             which will be changed to point to the state of the joystick
//             (this pointer can be cached if no joysticks get removed/added afterwards)
//
// returns an int
// 1 = error state was not retrieved 
// 0 = getting state was successful
//
int ljoy_sm_GetState(int id,struct ljoy_sm_state **state);


//
// Polls all managed joysticks
// needs to be called often, (game loop)
// or you may loose events 
// and the state will become inconsistent
//
// returns an int
// 1 = timed out (no state was updated)
// 0 = some state was updated
//
int ljoy_sm_Poll();


//
// Polls all managed joysticks and waits
// till some event has been caught
//
// parameters: long timeout_s = poll waits for timeout_s s
//
// returns an int
// 1 = timed out (no state was updated)
// 0 = some state was updated
//
int ljoy_sm_Wait(long timeout_s);


//
// Polls all managed joysticks and waits
// till some event has been caught
//
// parameters: long timeout_s = poll waits for timeout_s s
//             long timeout_ms = poll waits for timeout_ms ms
//
// returns an int
// 1 = timed out (no state was updated)
// 0 = some state was updated
//
int ljoy_sm_WaitExtraPrecision(long timeout_s,long timeout_ms);

#endif
