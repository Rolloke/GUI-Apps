#!/bin/sh
logger "delayed mount of drives"

DRIVENAME=INTENSO
DRIVENAME_FOR_ALL=${DRIVENAME}

MEDIA_USER=/media/pi

#mount for all users
#mkdir ${MEDIA_USER}/${DRIVENAME_FOR_ALL}
#mount -t vfat -ouser,umask=0000 /dev/sda1 ${MEDIA_USER}/${DRIVENAME_FOR_ALL}

# mount for git
GIT_USER=pi
mount --bind ${MEDIA_USER}/${DRIVENAME_FOR_ALL}/GitRepository/ /home/${GIT_USER}/git.repository
mount --bind /home/${GIT_USER}/git.repository/ /home/${GIT_USER}/projects/.git/

#mount for NAS storage
NAS_USER=pi
mount --bind ${MEDIA_USER}/${DRIVENAME_FOR_ALL}/NasDrive/ /home/${NAS_USER}/NAS.drive
