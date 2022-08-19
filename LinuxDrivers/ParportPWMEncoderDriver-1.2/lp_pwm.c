/*
 * @file lp_pwm.c
 *
 * @brief Parport PWM kernel space driver
 *
 * @copy Copyright (C) 2005-2007 Luke Cole, Joshua Bobruk
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of version 2 of the GNU General Public
 * License as published by the Free Software Foundation
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA
 * 02111-1307, USA.
 *
 * @author  Luke Cole
 *          cole@lc.homedns.org
 *
 * @author  Joshua Bobruk
 *	    joshportlock@msn.com
 *
 * @version
 *   $Id: lp_pwm.c,v 1.8 2007-04-05 03:43:25 cole Exp $
 */

//=============================================================================

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <asm/uaccess.h> // for put_user
#include <linux/interrupt.h> // for irqreturn_t
#include <asm/io.h>             
#include <linux/workqueue.h> // We scheduale tasks here

#define DEVICE_NAME "pwm" // DEV name as it appears in /proc/devices
#define LP1 0x378
#define MY_WORK_QUEUE_NAME "WQsched.c"
#define NUM_PINS 8

/* 
 * Maximum number of parallel ports to simultaneously
 * handle. NOTE:: IF THIS GETS CHANGED, DON'T FORGET TO 
 * CHANGE THE MODULE PARAMETER LINE 1-4i to 1-<n>i!! 
 */

#define NUM_PORTS 4

//=============================================================================

/** 
 * @brief Global variables are declared as static, so are global
 * within the file.
 */

static int major; // Major number assigned to our device driver
static int die = 0; // Set die to 1 for shutdown

int port_count = NUM_PORTS;

unsigned int lp_pwm_debug = 0;
unsigned int lp_pwm_int_input = 0;

static int timer_count = 0; //The number of times the timer interrupt
			    //has been called so far

static int lp_pwm_port_addresses[NUM_PORTS];
static unsigned char duty[NUM_PINS * NUM_PORTS];
static unsigned char pin_values[NUM_PORTS];
static unsigned char opened[NUM_PINS * NUM_PORTS];

/*
 * The work queue structure for this task, from workqueue.h 
 */

static struct workqueue_struct *my_workqueue;
static struct work_struct task;
static void intrpt_routine(void *irrelevant);
static DECLARE_WORK(task, intrpt_routine);

MODULE_LICENSE("GPL");

module_param(lp_pwm_debug, int, 0);
module_param(lp_pwm_int_input, int, 0);
module_param_array(lp_pwm_port_addresses, int, &port_count, 0);

MODULE_PARM_DESC(lp_pwm_debug, "debug messages, default is 0 (off)");
MODULE_PARM_DESC(lp_pwm_int_input, "Use integers for input (0-10). \
                                   default is 0, to use chars (0x00 to 0x0a)");
MODULE_PARM_DESC(lp_pwm_port_addresses, "Up to four parallel port addresses \
                                         in the from 0x378,0x123");

//=============================================================================

/** 
 * @brief This function will be called on every timer
 * interrupt. Notice the void* pointer - task functions can be used
 * for more than one purpose, each time getting a different parameter.
 */

static void intrpt_routine(void *irrelevant) {
  
  int i, j;
  
  if (timer_count == 0) {
    for (j = 0; j < NUM_PORTS; j++) {
      pin_values[j] = 0xff; 
    }
  } 


  for (j = 0; j < NUM_PORTS; j++) {
    
    if (lp_pwm_port_addresses[j] != 0) {
      
      for (i = 0; i < NUM_PINS; i++) {
	if (timer_count == duty[i + j * NUM_PINS]) {
	  pin_values[j] = pin_values[j] & ~(1 << i);
	} 
      }
    
      outb(pin_values[j], lp_pwm_port_addresses[j]);  
    }
  }

  // Increment the counter 

  timer_count++;
  if (timer_count >= 10)
    timer_count = 0;
  
  // If cleanup wants us to die

  if (die == 0)
    queue_delayed_work(my_workqueue, &task, 1);

}

//=============================================================================

/** 
 * @brief Called when a process tries to open the device file, like
 * "cat /dev/mycharfile"
 */

static int device_open(struct inode *inode, struct file *file) {
  
  unsigned int minor = iminor(inode);

  if (minor >= NUM_PINS * NUM_PORTS) 
    return -EINVAL;

  if (opened[minor])
    return -EBUSY;
  
  opened[minor]++;
  
  try_module_get(THIS_MODULE);

  file->private_data = &(duty[minor]);

  return 0;
}

//============================================================================ 

/** 
 * @brief Called when a process closes the device file.
 */

static int device_release(struct inode *inode, struct file *file) {
  
  unsigned int minor = iminor(inode);
  
  opened[minor]--;
  
  // We're now ready for our next caller

  // Decrement the usage count, or else once you opened the file, you'll
  // never get get rid of the module. 
  
  module_put(THIS_MODULE);

  return 0;
}

//=============================================================================

/** 
 * @brief Called when a process, which already opened the dev file,
 * attempts to read from it.
 */

static ssize_t device_read(struct file *filp,// see include/linux/fs.h
			   char *buffer, // buffer to fill with data
			   size_t length, // length of the buffer
			   loff_t *offset) {
  
  int bytes_read = 0;
  char speed_msg[2];
  char *msg_ptr;
  int msg_len = 2;

  sprintf(speed_msg, "%c", * (unsigned char *)filp->private_data);
  
  msg_ptr = speed_msg;
  
  // raw output, so just output three chars, not formatted.
  
  while (length && msg_len) {
    
    put_user(*(msg_ptr++), buffer++);
    length--;
    msg_len--;
    bytes_read++;
  }
  
  return 0;
}

//=============================================================================

/**  
 * @brief Called when a process writes to dev file: echo "hi" >
 * /dev/hello
 */

static ssize_t
device_write(struct file *filp, const char *buff, size_t len, loff_t * off) {
  
  unsigned char speed;
  int i;
  unsigned char num;
  int input;

  if (lp_pwm_debug)
    printk("Someone attempted to write %d chars to me!\n", len);
  
  if (lp_pwm_int_input) {
    
    i = 1;
    input = 0;
    get_user(num, buff);

    while (i <= len && num >= 48 && num <= 57) {
      input = input * 10;
      input = input + (num - 48);
      if (i < len)
	get_user(num, buff + i);
      i++;
    }
    
    if (input > 10) input = 10;
    if (input < 0) input = 0;
    
    speed = input;

    *((unsigned char *) (filp->private_data)) = speed;
  
  } else {
    
    get_user(speed, buff + len - 1);
    if (speed > 10) speed = 10;
    *((unsigned char *) (filp->private_data)) = speed;
  
  }

  if (lp_pwm_debug) {
    unsigned int i;
    
    for (i = 0; i < NUM_PINS; i++)
      printk("Duty cycle %d is set to %d\n", i, duty[i]);
  }
  
  return len;
}

//=============================================================================

static struct file_operations fops = {
  
  .read = device_read,
  .write = device_write,
  .open = device_open,
  .release = device_release

};

//=============================================================================

int init_module() {
  
  int i, j;

  for (i = 0; i < NUM_PINS * NUM_PORTS; i++) {
    opened[i] = 0;
    duty[i] = 0;
  }

  for (j = 0; j < NUM_PORTS; j++) {
    pin_values[j] = 0;
  }

  if (lp_pwm_port_addresses[0] == 0) {
    lp_pwm_port_addresses[0] = LP1;
  }

  printk("Currently Using Ports:\n");
  for (j = 0; j < NUM_PORTS; j++) {
    printk("Port %d = %d\n", j, lp_pwm_port_addresses[j]);
  }

  major = register_chrdev(0, DEVICE_NAME, &fops);

  if (major < 0) {
    printk("Registering the character device failed with %d\n", major);    
    return major;
  }
  
  printk("I was assigned major number %d.  Create a dev files with\n"
	 "'mknod /dev/pwm<n> c %d <n>'.\n", major, major);

  if (lp_pwm_int_input) {
    printk("Using integer input, 0 to 10\n");
  }

  my_workqueue = create_workqueue(MY_WORK_QUEUE_NAME);
  queue_delayed_work(my_workqueue, &task, 1);
  
  return 0;
}

//=============================================================================

void cleanup_module(void) {

  //if (
  unregister_chrdev(major, DEVICE_NAME);
  // < 0)
  //  printk("Error in unregister_chrdev\n");

  die = 1; // keep intrp_routine from queueing itself
  cancel_delayed_work(&task); // no "new ones"
  flush_workqueue(my_workqueue); // wait till all "old ones" finished
  destroy_workqueue(my_workqueue);
}

//=============================================================================
