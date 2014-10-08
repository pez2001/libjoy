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

//
// a small tool to read joystick values
// (accompanies libjoy on its way to glory) 
//

#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/time.h>
#include <sys/types.h>
#include "joytool.h"

int main(int argc, char** argv)
{
	int short_print = 0;
	int joyid = -1;//Any == 32768
	int axis = -1;
	int button = -1;
	int wait = 0;
	int wait_complete = 0;
	int print_axes= 0;
	int print_buttons = 0;
	int print_list = 0;
	int print_name = 0;
	int print_num_joysticks = 0;
	int keep_open = 0;
	char *filename = NULL;
	long timeout = 100*1000;//in nano secs ->  100000

	int c = 0;
	int r = -1 ;
	int r2;
	int i = 0;

	int id = 0;
	struct js_event e;
	char event_type = 0;

	fd_set fds;
	int topfd = -1;
	struct timeval tv;
	int quit=0;
	int axis_value[MAX_AXIS];
	int button_value[MAX_BUTTONS];


	while ((c = getopt (argc, argv, "i:a:b:t:ABNklwcnhsv")) != -1)
		switch (c)
	{
		case 'i'://joyid
			if(strcasecmp(optarg,"ANY")==0)
			{
				fprintf (stderr,"At the moment only reading from one joystick at a time is allowed,sorry.\n");
				return(1);
		//joyid=ANY_ID;
			}
			else
				joyid=atoi(optarg);
			break;
		case 'a'://axis
			if(strcasecmp(optarg,"ANY")==0)
				axis=ANY_ID;
			else
				axis=atoi(optarg);
			break;
		case 'b'://button
			if(strcasecmp(optarg,"ANY")==0)
				button=ANY_ID;
			else
				button=atoi(optarg);
			break;
		case 't'://timeout
			timeout=atol(optarg);
			timeout*=1000;
			break;
		case 'w'://wait for next event (skips init events)
			wait = 1;
			break;
		case 'k'://keep joytoool open
			keep_open = 1;
			break;
		case 'c'://wait for next COMPLETE button/axis event
			wait_complete = 1;
			break;
		case 'n'://print number of joysticks found
			print_num_joysticks = 1;
			break;
		case 'A'://print number of axes
			print_axes = 1;
			break;
		case 'B'://print number of buttons
			print_buttons = 1;
			break;
		case 'N'://print name of joystick
			print_name = 1;
			break;
		case 'l'://list joysticks found
			print_list = 1;
			break;
		case 's'://print just the value
			short_print = 1;
			break;
		case 'h':
			printf(helpmsg);
			return(0);
		case 'v':
			printf(version_string);
			return(0);
		case '?':
			if (optopt == 'c')
				fprintf (stderr, "Option -%c requires an argument.\n", optopt);
			else if (isprint (optopt))
				fprintf (stderr, "Unknown option `-%c'.\n", optopt);
			else
				fprintf (stderr,"Unknown option character `\\x%x'.\n",optopt);
			return(1);
		default:
			abort ();
	}
	//check parameters for validity
	if(wait_complete && wait)
	{
		fprintf(stderr,"You can only use one of the wait options\n");
		return(1);
	}
	filename = argv[optind];

	if(print_list)
	{
	//print a list of connected joysticks
		int joy_num = ljoy_GetJoysticksNum();
		if(!short_print)
			printf("List of Joysticks (%d)\n",joy_num);
		for(i=0;i<joy_num;i++)
		{
			char *name = ljoy_GetNameById(i);
			if(short_print)
				printf("%d:%s\n",i,name);
			else
				printf("Joystick(%d): %s\n",i,name);
			free(name);//don't forget to free up any name received from libjoy
		}
		return(0);
	}

	if(print_num_joysticks)
	{
		if(short_print)
			printf("%d",ljoy_GetJoysticksNum());
		else
			printf("Number of Joysticks found: %d\n",ljoy_GetJoysticksNum());
		return(0);
	}
	if(joyid != -1)
		id=ljoy_OpenById(joyid,1);// !(wait || wait_complete)
	else if(filename!=NULL)
		id=ljoy_Open(filename,1);// !(wait || wait_complete)
	else
	{
		fprintf(stderr,"no device/joyid given.\n");
		return(1);
	}
	if(id==-1)

	{
		fprintf(stderr,"error opening joystick.\n");
		return(1);
	}
	if(!short_print)
		printf("opened joystick device.\n");
	if(print_axes)
	{
		if(!short_print)
			printf("Number of Axes: %d\n",ljoy_GetAxesNum(id));
		else
			printf("%d\n",ljoy_GetAxesNum(id));
	}
	if(print_buttons)
	{
		if(!short_print)
			printf("Number of Buttons: %d\n",ljoy_GetButtonsNum(id));
		else
			printf("%d\n",ljoy_GetButtonsNum(id));
	}
	if(print_name)
	{
		char *name = ljoy_GetName(id);
		if(short_print)
			printf("%d:%s\n",i,name);
		else
			printf("Joystick(%d): %s\n",i,name);
		free(name);//don't forget to free up any name received from libjoy
	}

	if(axis != -1)
		event_type = JS_EVENT_AXIS;
	if(button != -1)
		event_type |= JS_EVENT_BUTTON;
	if(!(wait_complete || wait))
		event_type |= JS_EVENT_INIT;

	r = 0;
	FD_ZERO(&fds);
	FD_SET( id, &fds );
	topfd = id;

	for(i=0;i<MAX_AXIS;i++)
		axis_value[i] = -1;
	for(i=0;i<MAX_BUTTONS;i++)
		button_value[i] = -1;

	while(!quit)
	{
		tv.tv_sec = 0;
		tv.tv_usec = timeout;//1000000;
		r2 = select( topfd+1, &fds,0,0,&tv);
		if( r2 == 0 )
		{//timeout
			if(!keep_open && !wait && !wait_complete)
			{
				quit=1;
				break;
			}
		}
		else if( r2 > 0 )
		{
			if(FD_ISSET(id,&fds))
			{
				r=0;
				while(r!=-1)
				{
					r = ljoy_GetFilteredTypeEvent(id,&e,event_type);
					if(r != -1)
					{
						if((e.number == axis || axis == ANY_ID) && e.type & JS_EVENT_AXIS)
						{
							if(e.number < MAX_AXIS)
							{
								if(wait_complete)
								{
									if(axis_value[e.number]!=-1 && e.value == 0)
									{
										quit = 1;
										break;
									}
								}
								axis_value[e.number] = e.value;
							}
							if(keep_open || wait)
							{
								if(short_print)
								{
									printf("a:%d:%d\n",e.number,e.value);
								}
								else
								{
									printf("axis (%d) :%d\n",e.number,e.value);
								}
								if(wait)
									quit = 1;
							}
						}
						if((e.number==button||button==ANY_ID) && e.type & JS_EVENT_BUTTON)
						{
							if(e.number < MAX_BUTTONS)
							{
								if(wait_complete)
								{
									if(button_value[e.number]!=-1 && e.value == 0)
									{
										quit = 1;
										break;
									}
								}
								button_value[e.number] = e.value;
							}
							if(keep_open || wait)
							{
								if(short_print)
								{
									printf("b:%d:%d\n",e.number,e.value);
								}
								else
								{
									printf("button (%d) :%d\n",e.number,e.value);
								}
								if(wait)
									quit = 1;
							}
						}

					}
				}
			}
			fflush(stdout);
		}
		else 
		{
			fprintf(stderr,"error on select.\n");
			return(1);
		}
		FD_ZERO(&fds);
		FD_SET( id, &fds );
		topfd = id;
	}

	if(!wait)
	{

		for(i=0;i<MAX_BUTTONS;i++)
		{
			if(short_print)
			{
				if(button_value[i] != -1)
					printf("b:%d:%d\n",i,button_value[i]);
			}
			else
			{
				if(button_value[i] != -1)
					printf("button (%d) :%d\n",i,button_value[i]);
			}
		}
		for(i=0;i<MAX_AXIS;i++)
		{
			if(short_print)
			{
				if(axis_value[i] != -1)
					printf("a:%d:%d\n",i,axis_value[i]);
			}
			else
			{
				if(axis_value[i] != -1)
					printf("axis (%d) :%d\n",i,axis_value[i]);
			}
		}

	}

	if(ljoy_Close(id))
	{
		fprintf(stderr,"error closing joystick device.\n");
		return(1);
	}
	if(!short_print)
		printf("closed joystick device.\n");
	return(0);
}
