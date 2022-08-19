
ccflags-$(CONFIG_DEBUG_GPIO)	+= -DDEBUG



#
# @file Makefile
#
# @brief Makefile for Zynq GPIO Driver
#
# @copy Copyright (C) 
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
# @author  Rolf Kary-Ehlers
#          rolf-kary-ehlers@t-online.de
#
# 
# @version
#   $Id: Makefile, v 1.0 2014-07-03 $
#

module=gpiolib

ifneq ($(KERNELRELEASE),)

$(module)-objs := devres.o gpiolib.o gpiolib-of.o gpiolib-acpi.o gpio-zynq.o

obj-m := $(module).o 

else
 KDIR:= /lib/modules/$(shell uname -r)/build


CC := gcc
CFLAGS  := -c -nostdinc \
 -D__KERNEL__ -I /$(KDIR) \
 -Wall \
 -I /usr/lib/gcc-lib/i386-linux/3.2.3/include/ 

PWD := $(shell pwd)

default :
	echo "kernel: $(KDIR)"
	make -C $(KDIR) SUBDIRS=$(PWD) modules
	sync; sync; sync

install:
	./MakeDevices.sh

uninstall:
	rm /dev/stepper*
	/sbin/rmmod lpt_stepper
	
clean :
	rm -f *.o *.ko *.mod.c *~ *#

endif


