#
# @file Makefile
#
# @brief Makefile for Parport PWM/Encoder Driver
#
# @copy Copyright (C) 2005-2007 Luke Cole, Joshua Bobruk
#
# This program is free software; you can redistribute it and/or
# modify it under the terms of version 2 of the GNU General Public
# License as published by the Free Software Foundation
#
# This program is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU General Public License for more details.
#
# You should have received a copy of the GNU General Public License
# along with this program; if not, write to the Free Software
# Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
# 02111-1307, USA.
#
# @author  Luke Cole
#          cole@lc.homedns.org
#
# @author  Joshua Bobruk
#	   joshportlock@msn.com
# 
# @version
#   $Id: Makefile,v 1.3 2007-04-05 03:33:50 cole Exp $
#

ifneq ($(KERNELRELEASE),)
obj-m := lp_pwm.o
obj-m += lp_encoder.o

else
 KDIR:= /lib/modules/$(shell uname -r)/build
CC := gcc
CFLAGS  := -c -nostdinc \
-D__KERNEL__ -I /$(KDIR) \
 -I /usr/lib/gcc-lib/i386-linux/3.2.3/include/

PWD := $(shell pwd)

default :
	make -C $(KDIR) SUBDIRS=$(PWD) modules
	sync; sync; sync

install:
	insmod lp_pwm.ko
	insmod lp_encoder.ko

MakeDevices:
	./MakeDevices	

RemoveDevices:
	rm /dev/encoder*
	rm /dev/pwm*

clean :
	rm -f *.o *.ko *.mod.c *~ *#

endif


