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

#include <sys/time.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>

#include "libjoy.h"

//
//internal vars
//
int ordered_mode = 0;

//
//internal function to get the real id from an ordered id 
//(returns -1 if no device was found)
//
int ljoy_GetRealId(int id)
{
//if stat returns false try next higher till joy found | till MAX_JOYSTICKS
	int i;
	for(i=id;i<MAX_JOYSTICKS;i++)
	{
		char filename[256];
		struct stat filestat;
		sprintf((char*)&filename,"/dev/input/js%d",i);
		if(stat((char*)&filename,&filestat) != -1)
			return(i);
	}
	return(-1);
}


//
//public functions
//

int ljoy_GetOrderedMode()
{
	return(ordered_mode);
}

void ljoy_SetOrderedMode(int order_ids)
{
	ordered_mode = order_ids;
}

int ljoy_Open(char *dev,int nonblocking)
{
	if(nonblocking)
		return(open(dev,O_RDONLY | O_NONBLOCK));
	else
		return(open(dev,O_RDONLY));
}

int ljoy_OpenById(int joyid,int nonblocking)
{
	char filename[256];
	int id = joyid;
	if(ordered_mode)
		id = ljoy_GetRealId(joyid);
	if(id<0)
		return(-1);
	sprintf((char*)&filename,"/dev/input/js%d",id);
	if(nonblocking)
		return(open((char*)&filename,O_RDONLY | O_NONBLOCK));
	else
		return(open((char*)&filename,O_RDONLY));
}

char *ljoy_GetNameById(int joyid)
{
	int id = ljoy_OpenById(joyid,0);
	char *ret = ljoy_GetName(id);
	ljoy_Close(id);
	return(ret);
}

int ljoy_GetJoysticksNum()
{
	int num = 0;
	int i = 0;
	for(i=0;i<MAX_JOYSTICKS;i++)
	{
		char filename[256];
		struct stat filestat;
		sprintf((char*)&filename,"/dev/input/js%d",i);
		if(stat((char*)&filename,&filestat) != -1)
			num++;
	}
	return(num);
}

int ljoy_Close(int id)
{
	return(close(id));
}

char *ljoy_GetName(int id)
{
	char *name = malloc(128);
	if(ioctl(id,JSIOCGNAME(128),name) < 0)
		strcpy(name,"Unknown");
	return(name);
}

int ljoy_GetAxesNum(int id)
{
	char num = 0;
	ioctl(id,JSIOCGAXES,&num);
	return((int)num);
}

int ljoy_GetButtonsNum(int id)
{
	char num = 0;
	ioctl(id,JSIOCGBUTTONS,&num);
	return((int)num);
}

int ljoy_GetEvent(int id,struct js_event *event)
{
	return(read(id,event,sizeof(struct js_event)));
}

int ljoy_GetFilteredEvent(int id,struct js_event *event,unsigned char type,unsigned char number)
{
	int r = -1;
	int err=0;
	r=read(id,event,sizeof(struct js_event));
	err = errno;
	if (r== -1 && err != EAGAIN)
	{
		fprintf(stderr,"error reading from joystick device: (%d)%s\n",err,strerror(err));
		return(-1);
	}

	if(!(fcntl(id,F_GETFL) & O_NONBLOCK))
	{
		while((event->type & ~type) || (event->number != number && number != ANY_ID))
		{
			r=read(id,event,sizeof(struct js_event));
			err = errno;
			if (r== -1 && err != EAGAIN)
			{
				fprintf(stderr,"error reading from joystick device: (%d)%s\n",err,strerror(err));
				return(-1);
			}
		}
	}
	if((event->type & ~type) || (event->number != number && number != ANY_ID))
		r=-1;
	return(r);
}

int ljoy_GetFilteredTypeEvent(int id,struct js_event *event,unsigned char type)
{
	int r = -1;
	int err=0;
	r=read(id,event,sizeof(struct js_event));
	err = errno;
	if (r== -1 && err != EAGAIN)
	{
		fprintf(stderr,"error reading from joystick device: (%d)%s\n",err,strerror(err));
		return(-1);
	}

	if(!(fcntl(id,F_GETFL) & O_NONBLOCK))
	{
		while((event->type & ~type))
		{
			r=read(id,event,sizeof(struct js_event));
			err = errno;
			if (r== -1 && err != EAGAIN)
			{
				fprintf(stderr,"error reading from joystick device: (%d)%s\n",err,strerror(err));
				return(-1);
			}
		}
	}
	if((event->type & ~type))
		r=-1;
	return(r);
}


//
//internal vars for the state manager
//

int sm_refcount = 0;
struct ljoy_sm_state **sm_states;
int *sm_state_id_table;
int sm_states_num;
fd_set sm_fds;
int sm_topfd = -1;

//
//internal function to update a fdset used during polling
//
void ljoy_sm_UpdateFds()
{
	int i;
	FD_ZERO(&sm_fds);
	//printf("watching %d devices.\n",sm_states_num);
	for(i=0;i<sm_states_num;i++)
	{
		//printf("adding id to fdset:%d\n",sm_state_id_table[i]);
		FD_SET( sm_state_id_table[i], &sm_fds );
		if(sm_state_id_table[i]>sm_topfd)
			sm_topfd = sm_state_id_table[i];
	}
}

//
//public state manager functions
//

int ljoy_sm_Init()
{
	if(!sm_refcount)
	{
		sm_refcount++;
		sm_states = (struct ljoy_sm_state**)malloc(0);
		sm_states_num = 0;
		sm_state_id_table = (int*) malloc(0);
		return(0);
	}
	else 
		return(1);
}

int ljoy_sm_Shutdown()
{
	int i;
	if(sm_refcount)
	{
		for(i=0;i<sm_states_num;i++)
		{
			free(sm_states[i]->axes);
			free(sm_states[i]->buttons);
			free(sm_states[i]);
		}
		free(sm_states);
		sm_refcount--;
		return(0);
	}
	else
		return(1);
}

int ljoy_sm_AddDevice(int id)
{
	struct ljoy_sm_state *state;
	if(!(fcntl(id,F_GETFL) & O_NONBLOCK))
	{
		return(1);
	}
	sm_states = (struct ljoy_sm_state**)calloc(sm_states_num+1,sizeof(struct ljoy_sm_state*));
	state = (struct ljoy_sm_state*)malloc(sizeof(struct ljoy_sm_state));
	sm_states[sm_states_num]=state;
	state->timestamp = 0;
	state->num_axes = ljoy_GetAxesNum(id);
	state->num_buttons = ljoy_GetButtonsNum(id);
	state->axes = (int*)malloc(ljoy_GetAxesNum(id)*sizeof(int));
	state->buttons = (int*)malloc(ljoy_GetButtonsNum(id)*sizeof(int));
	sm_state_id_table = (int*)calloc(sm_states_num+1,sizeof(int));
	sm_state_id_table[sm_states_num]=id;
	//prepare fdset
	sm_states_num++;
	ljoy_sm_UpdateFds();
	return(0);
}
int ljoy_sm_RemoveDevice(int id)
{
	int i;
	int c = -1;
	for(i=0;i<sm_states_num;i++)
	{
		if(sm_state_id_table[i]==id)
			c = i;
			
	}
	if(c==-1)
		return(1);
	sm_state_id_table[c]=sm_state_id_table[sm_states_num-1];
	free(sm_states[c]->axes);
	free(sm_states[c]->buttons);
	free(sm_states[c]);
	sm_states[c] = sm_states[sm_states_num-1];
	sm_states = (struct ljoy_sm_state**)calloc(sm_states_num-1,sizeof(struct ljoy_sm_state*));
	sm_state_id_table = (int*)calloc(sm_states_num-1,sizeof(int));
	sm_states_num--;
	//update fdset
	ljoy_sm_UpdateFds();
	return(0);
}
int ljoy_sm_GetState(int id,struct ljoy_sm_state **state)
{
	int i;
	int c = -1;
	for(i=0;i<sm_states_num;i++)
	{
		if(sm_state_id_table[i]==id)
			c = i;
			
	}
	if(c==-1)
		return(1);
	if(sm_states[c]->timestamp!=0)
		*state = sm_states[c];
	else
		return(1);
	
	return(0);
}
int ljoy_sm_WaitExtraPrecision(long timeout_s,long timeout_ms)
{
	struct js_event e;
	struct timeval tv;
	int r;
	int r2;
	int i;
	tv.tv_sec = timeout_s;
	tv.tv_usec = timeout_ms;
	
	r = select( sm_topfd+1, &sm_fds,0,0,&tv);
	if( r == 0 )
	{//timeout
		ljoy_sm_UpdateFds();
		return(1);
	}
	else if( r > 0 )
	{
		for(i=0;i<sm_states_num;i++)
		{
			if(FD_ISSET(sm_state_id_table[i],&sm_fds))
			{
				r2=0;
				while(r2!=-1)
				{
					r2 = ljoy_GetEvent(sm_state_id_table[i],&e);
					if(r2 != -1)
					{
						//update state
						if((e.type & ~JS_EVENT_INIT) == JS_EVENT_AXIS && e.number <sm_states[i]->num_axes)
						{
							sm_states[i]->axes[e.number]=e.value;
						}
						if((e.type & ~JS_EVENT_INIT) == JS_EVENT_BUTTON && e.number<sm_states[i]->num_buttons)
						{
							sm_states[i]->buttons[e.number]=e.value;
						}
						sm_states[i]->timestamp = e.time;
					}
				}
			}
		}
	}
	ljoy_sm_UpdateFds();
	return(0);
}

int ljoy_sm_Wait(long timeout_s)
{
	return(ljoy_sm_WaitExtraPrecision(timeout_s,0));
}
int ljoy_sm_Poll()
{
	return(ljoy_sm_Wait(0));
}
