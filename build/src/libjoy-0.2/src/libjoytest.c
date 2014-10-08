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
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
//#include <errno.h>
#include <string.h>
#include "libjoy.h"

int main(int argc, char** argv)
{
	int id=-1;
	//int id2 = -1;
	//int axis=0;
	//int button=0;
	char *name=NULL;
	int num = 0;
	int i;
	int r = -1 ;

	struct ljoy_sm_state *state;
	//get infos
	num = ljoy_GetJoysticksNum();
	printf("Number of Joysticks found: %d\n",num);
	for(i=0;i<num;i++)
	{
		id = ljoy_OpenById(i,0);
		name=ljoy_GetName(id);
		printf("Name of Joystick %d: [%s]\n",i+1,name);
		free(name);
		printf("Number of Axis of Joystick %d: %d\n",i+1,ljoy_GetAxesNum(id));
		printf("Number of Buttons of Joystick %d: %d\n",i+1,ljoy_GetButtonsNum(id));
		ljoy_Close(id);
	}
	
	//event test
	printf("reopening joystick0 for event test.\n");
	id = ljoy_Open("/dev/input/js0",0);
	if(id==-1)
		printf("opening failed.\n");
	struct js_event e;
	r = ljoy_GetEvent(id,&e);
	if(r != sizeof(struct js_event))
		printf("error reading event from joystick(r=%d and it should be %d)\n",r,sizeof(struct js_event));
	printf("event type: %d\n",e.type);
	printf("event number: %d\n",e.number);
	printf("event value: %d\n",e.value);
	printf("event time: %d\n",e.time);
	ljoy_Close(id);
	
	//state manager test
	printf("initiating state manager.\n");
	ljoy_sm_Init();	
	printf("reopening joystick0 for state manager test.\n");
	id = ljoy_Open("/dev/input/js0",1);
	if(id==-1)
		printf("opening failed.\n");
	printf("adding joystick0 to state manager(device needs to be in non blocking mode).\n");
	if(ljoy_sm_AddDevice(id))
		printf("error adding device to state manager.\n");
	if(ljoy_sm_RemoveDevice(id))
		printf("error removing joystick0 from state manager.\n");
	if(!ljoy_sm_RemoveDevice(id))
		printf("error removing joystick0 from state manager(second remove).\n");
	if(ljoy_sm_AddDevice(id))
		printf("error readding device to state manager.\n");
	printf("polling state manager.\n");
	if(ljoy_sm_Wait(30))
		printf("error polling state manager.\n");
	sleep(1);
	if(ljoy_sm_Wait(30))
		printf("error polling state manager.\n");
	sleep(1);
	if(ljoy_sm_Wait(30))
		printf("error polling state manager.\n");

	if(!ljoy_sm_GetState(id,&state))
	{
		printf("joystick state:\n");
		for(i=0;i<state->num_buttons;i++)
			printf("button(%d):%d\n",i,state->buttons[i]);
		for(i=0;i<state->num_axes;i++)
			printf("axis(%d):%d\n",i,state->axes[i]);
	}
	else
		printf("error getting state.\n");
		
	
	ljoy_Close(id);
	printf("shutting down state manager.\n");
	ljoy_sm_Shutdown();
	
	//non blocking test
	printf("reopening joystick0 for non blocking event test.\n");
	id = ljoy_Open("/dev/input/js0",1);
	if(id==-1)
		printf("opening failed.\n");
	while(1)
	{
		//r = ljoy_GetEvent(id,&e);
		//r = ljoy_GetFilteredTypeEvent(id,&e,JS_EVENT_BUTTON|JS_EVENT_INIT);
		r = ljoy_GetFilteredEvent(id,&e,JS_EVENT_BUTTON|JS_EVENT_INIT,0);
		if(r > 0)
		{
			printf("event type: %d\n",e.type);
			printf("event number: %d\n",e.number);
			printf("event value: %d\n",e.value);
			printf("event time: %d\n",e.time);
		}
		else
		{
			printf("...");
			printf("\b\b\b");
		}
	}
	ljoy_Close(id);
	
	
	
	return 0;
}
