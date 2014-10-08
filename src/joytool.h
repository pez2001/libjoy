/***************************************************************************
 *   Copyright (C) 2008 by Tim Theede   *
 *   pez2001@voyagerproject.de   *
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


#include "libjoy.h"

//max axis supported
#define MAX_AXIS 16
//max buttons supported
#define MAX_BUTTONS 32
//default is ordered id mode
#define DEFAULT_ORDERED_MODE 1

const char *version_string = "joytool - Version: 0.1\n\
 written by Tim 'pez2001' Theede (c) 2008\n";

const char *helpmsg = "Usage: joytool [OPTION]... [DEV]\n\
Read values from the joystick device which DEV represents.\n\
\n\
  -i <joyid>            set joystick id (0 - (number of joysticks-1))\n\
  -a <axis id>          set axis id\n\
  -b <button id>        set button id\n\
  -w                    waits for the next axis/button event\n\
  -c                    waits for the next complete axis/button event\n\
  -t <timeout>          set timeout (ms)\n\
  -s                    print only the values\n\
  -k                    keep joytool open\n\
  -l                    list joysticks found\n\
  -A                    print number of axes\n\
  -B                    print number of buttons\n\
  -N                    print name of joystick\n\
  -n                    print number of joysticks found\n\
  -h                    display this help and exit\n\
  -v                    output version information and exit\n\
\n\
If joyid was set [DEV] can be omitted.\n\
Joystick ids start from 0 and reach to number of joysticks-1.\n\
You can use the 'Any' id to read all buttons/axis.\n\
\n\
The -s (short option) prints an easily parseable output.\n\
 In short mode values will be printed in the following fashion:\n\
  type(b|a):axis/button id:value.\n\
\n\
Timeout counts from the last joystick axis/button change\n\
 (blame it on the implementation of select at least for now)\n\
 20 ms seems to be a reasonable minimum,else you may loose\n\
 consistency of joystick values returned\n\
\n\
The -c (complete option) will wait for an action to be completed,\n\
 this means that you have to pull the joystick into the middle\n\
 position back or release the pressed button before it will exit.\n\
\n\
Report bugs to <pez2001@voyagerproject.de>.\n";
