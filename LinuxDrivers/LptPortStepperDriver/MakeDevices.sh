#!/bin/sh

# MakeDevices - Create device nodes /dev/stepper?
#
# Copyright (C) 2005-2007 Luke Cole, Joshua Bobruk
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
# @version
#   $Id: MakeDevices,v 1.0 2007-04-05 03:33:49 cole Exp $
#

module="lpt_stepper"
device="stepper"
mode="664"

# install module with insmod
/sbin/insmod ./$module.ko $* || exit 1

# remove stale nodes
rm -f /dev/${device}[0-3]

major=$(awk "\$2==\"$device\" {print \$1}" /proc/devices)

echo "major Nr is: $major"

mknod "/dev/${device}0" c "$major" 0
mknod "/dev/${device}1" c "$major" 1
mknod "/dev/${device}2" c "$major" 2
mknod "/dev/${device}3" c "$major" 3

group="staff"
grep -q '^/staff:' /etc/group || group="rolfke"

chgrp $group /dev/${device}[0-3]
chmod $mode /dev/${device}[0-3]
