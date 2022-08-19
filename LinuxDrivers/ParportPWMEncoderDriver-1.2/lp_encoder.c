/*
 * @file lp_encoder.c
 *
 * @brief Parport Encoder kernel space driver
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
 *   $Id: lp_encoder.c,v 1.6 2007-04-05 03:43:25 cole Exp $
 */

//=============================================================================

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/fs.h>
#include <linux/wait.h>
#include <linux/sched.h>
#include <asm/uaccess.h> // for put_user
#include <linux/interrupt.h> // For irqreturn_t
#include <asm/io.h>             
#include <linux/workqueue.h> // We scheduale tasks here

#define DEVICE_NAME "encoder" // Dev name as it appears in /proc/devices
#define MY_WORK_QUEUE_NAME "WQsched.c"
#define LP1 0x378
#define NUM_PINS 8
#define NUM_PORTS 4

#define BUFFER_LENGTH 80
#define DIRECTION_BUFFER_LENGTH 4

//=============================================================================

static int major; // Major number assigned to our device driver

static int die = 0; // Set die to 1 for shutdown

struct port_pin {
  unsigned char pin_value;
  unsigned char old_pin_value;
  unsigned char open;
  struct file* filep;
  unsigned int count;
  char speed_msg[BUFFER_LENGTH];
  int valid_message;
  wait_queue_head_t encoder_wait;
} port_pins[NUM_PINS][NUM_PORTS];

struct quadrature_pin {
  unsigned char open;
  struct file* filep;
  char direction;
  char direction_msg[DIRECTION_BUFFER_LENGTH];
  int valid_message;
  wait_queue_head_t encoder_wait;
} quadrature_pins[NUM_PINS][NUM_PORTS];

int port_count = NUM_PORTS;

unsigned int lp_encoder_debug = 0;
unsigned int lp_encoder_io_base[NUM_PORTS];

static unsigned int output_count = 0;
static unsigned int lp_encoder_output_rate = 500;

static unsigned int lp_encoder_int_output = 0;
static char *lp_encoder_linked_encoders[NUM_PORTS];
static int encoder_links[NUM_PINS][NUM_PORTS];
static unsigned int use_links = 0;

static struct workqueue_struct *my_workqueue;
static struct work_struct task;
static void intrpt_routine(void *irrelevant);
static void parse_encoder_links(void);
static DECLARE_WORK(task, intrpt_routine);

//=============================================================================

MODULE_LICENSE("GPL");

module_param(lp_encoder_debug, int, 0);
module_param_array(lp_encoder_io_base, int, &port_count, 0);
module_param(lp_encoder_output_rate, int, 0);
module_param_array(lp_encoder_linked_encoders, charp, &port_count, 0);
module_param(lp_encoder_int_output, int, 0);

MODULE_PARM_DESC(lp_encoder_debug, "debug messages, default is 0 (no)");
MODULE_PARM_DESC(lp_encoder_io_base, "Base IO address for the parallel port. "
		 "Defaults to 0x378");
MODULE_PARM_DESC(lp_encoder_output_rate, "output delay in ms to return data "
		 "from cat /dev/encoder (default 500ms)");
MODULE_PARM_DESC(lp_encoder_linked_encoders, "String containing encoder pins "
		 "to be linked in the format \"1+2;3+4\" which would link 1 "
		 "with 2 and 3 with 4.  Device nodes to access direction will "
		 "be given in syslog.");
MODULE_PARM_DESC(lp_encoder_int_output, "Output to the device nodes using "
		 "formatted integers, not raw characters\n");

//=============================================================================

static void minor_to_pin_port(unsigned int minor, unsigned int *pin,
			      unsigned int *port) {

  *pin = minor % (NUM_PINS*2);
  *port = minor / (NUM_PINS*2);

}

//=============================================================================

static void parse_encoder_links() {
  
  int from;
  int to;
  int input;
  int port;
  char *link_str;

  for (port = 0; port < NUM_PORTS; port++) {
    
    link_str = lp_encoder_linked_encoders[port];
    
    if (link_str != NULL) {
      
      while ((*link_str != 0) && (use_links == 1)) {
	
	if ((*link_str != 43) && (*link_str != 59) &&
	    ((*link_str < 48) || (*link_str > 57))) {
	  printk("Malformed encoder link string!!! Ignoring encoder links!\n");
	  use_links = 0;
	}
	
	input = 0;
	
	while (*link_str >= 48 && *link_str <= 57 && *link_str != 0) {
	  input = input * 10;
	  input = input + (*link_str - 48);
	  link_str++;
	}
	
	from = input;
	
	if (*link_str != 43) {
	  
	  printk("Malformed encoder link string!!! Ignoring encoder links!\n");
	  use_links = 0;
	
	} else {
	  
	  link_str++;
	  
	  input = 0;
	  
	  while (*link_str >= 48 && 
		 *link_str <= 57 &&
		 link_str != 0) {
	    input = input * 10;
	    input = input + (*link_str - 48);
	    link_str++;
	  }
	  
	  to = input;
	  
	  if (from >= NUM_PINS || to >= NUM_PINS) {
	    printk("Please only use pins 0 to %d! Ignoring encoder links!\n", 
		   NUM_PINS-1);
	    use_links = 0;
	  }
	  
	  if (from == to) {
	    printk("Linked pins can't be equal! Ignoring encoder links!\n");
	    use_links = 0;
	  }
	  
	  if (use_links) {
	    if (encoder_links[from][port] == -1 &&
		encoder_links[to][port] == -1) {
	      encoder_links[from][port] = to;
	      encoder_links[to][port] = -2;
	    } else {
	      printk("Either pin %d or %d has a duplicate linking. Ignoring "
		     "encoder links!\n", from, to);
	      use_links = 0;
	    }
	  }
	  
	  if (*link_str != 0)
	    link_str++;

	}    

      }

    }

  }

}

//=============================================================================

/**
 * @brief Examines the state of the pins and gives count/direction information
 */

static void intrpt_routine(void *irrelevant) {
  
  int pin, port;
  int p1, p2;
  int to;
  int current_pins[NUM_PORTS];

  for (port = 0; port < NUM_PORTS; port++) {

    if (lp_encoder_io_base[port] != 0) {
      
      if (output_count == 0) {
	
	for (pin = 0; pin < NUM_PINS; pin++) {
	  
	  if (lp_encoder_int_output) {
	    sprintf(port_pins[pin][port].speed_msg, "%d %d\n", pin, 
		    port_pins[pin][port].count);
	    sprintf(quadrature_pins[pin][port].direction_msg,
		    "%d\n", quadrature_pins[pin][port].direction);
	  } else {
	    sprintf(port_pins[pin][port].speed_msg, "%c%c", 
		    (port_pins[pin][port].count & 0xFF00) >> 8,
		    port_pins[pin][port].count & 0x00FF);
	    sprintf(quadrature_pins[pin][port].direction_msg, 
		    "%c%c", 
		    (quadrature_pins[pin][port].direction & 0xFF00) >> 8,
		    quadrature_pins[pin][port].direction & 0x00FF);
	  }
	  
	  port_pins[pin][port].valid_message = 1;
	  quadrature_pins[pin][port].valid_message = 1;
	  
	  wake_up_interruptible(&(port_pins[pin][port].encoder_wait));
	  wake_up_interruptible(&(quadrature_pins[pin][port].encoder_wait));
	  
	}

      } 

    }

  }
    
  for (port = 0; port < NUM_PORTS; port++) {
    if (lp_encoder_io_base[port] != 0) {
      current_pins[port] = inb(lp_encoder_io_base[0]+1);    
    }
  }
    
  for (port = 0; port < NUM_PORTS; port++) {
    
    if (lp_encoder_io_base[port] != 0) {
      
      for (pin = 0; pin < NUM_PINS; pin++) {
	
	p1 = current_pins[port] & (1 << pin);
	
	port_pins[pin][port].old_pin_value = port_pins[pin][port].pin_value;
	port_pins[pin][port].pin_value = p1;
	
	p2 = port_pins[pin][port].old_pin_value;
	
	if ((p1 != p2) && (p1 == 0)) {
	  
	  // we see a falling edge
	  port_pins[pin][port].count++;
	  
	  if (port_pins[pin][port].count > 0xFFFF) 
	    port_pins[pin][port].count = 0;
	  
	  // because we just saw a falling edge, check to see if linked
	  // encoder is currently in high, or low position, and give
	  // direction based on that.
	  
	  if (use_links) {
	    to = encoder_links[pin][port];
	    if (to >= 0) {
	      if (current_pins[port] & (1 << to)) {
		quadrature_pins[pin][port].direction = 1;
	      } else {
		quadrature_pins[pin][port].direction = -1;
	      }
	    }
	  }

	}
 
      }  
    
      output_count++;
      if (output_count >= lp_encoder_output_rate)
	output_count = 0;

    }

  }
  
  // If cleanup wants us to die

  if (die == 0)
    queue_delayed_work(my_workqueue, &task, 1);

}

//=============================================================================

static int device_open(struct inode *inode, struct file *file) {
  
  unsigned int minor = iminor(inode);
  unsigned int pin, port;

  minor_to_pin_port(minor, &pin, &port);

  if (lp_encoder_io_base[port] == 0)
    return -EINVAL;

  if (pin < NUM_PINS) {
    if (port_pins[pin][port].open)
      return -EBUSY;

    port_pins[pin][port].open++;    

    port_pins[pin][port].filep = file;

  } else if (pin < NUM_PINS*2) {
    if (quadrature_pins[pin-NUM_PINS][port].open)
      return -EBUSY;

    if (encoder_links[pin - NUM_PINS][port] < 0)
      return -EINVAL;

    quadrature_pins[pin - NUM_PINS][port].open++;    

    quadrature_pins[pin - NUM_PINS][port].filep = file;

  } else {
    return -EINVAL;
  }

  try_module_get(THIS_MODULE);

  return 0;
}

//=============================================================================
 
static int device_release(struct inode *inode, struct file *file) {
  unsigned int minor = iminor(inode);
  unsigned int pin, port;

  minor_to_pin_port(minor, &pin, &port);

  if (pin < NUM_PINS) {

    port_pins[pin][port].open--;
    port_pins[pin][port].filep = 0;

  } else if (pin < NUM_PINS*2) {

    quadrature_pins[pin - NUM_PINS][port].open--;
    quadrature_pins[pin - NUM_PINS][port].filep = 0;
  }

  module_put(THIS_MODULE);

  return 0;
}

//=============================================================================

static ssize_t pin_read(struct file *filp, char *buffer, size_t length,
			loff_t * offset, unsigned int pin, unsigned int port) {
  
  int bytes_read = 0;
  int retval;
  char *msg_ptr;
  int msg_len = 3;

  retval = wait_event_interruptible(port_pins[pin][port].encoder_wait, 
				    port_pins[pin][port].valid_message != 0);

  if (retval) {
    return retval;
  }
  
  msg_ptr = port_pins[pin][port].speed_msg;

  if (*(msg_ptr) == 0 && lp_encoder_int_output) {
    printk("Message not filled\n");
    return 0;
  }

  if (lp_encoder_int_output) {
    
    while (length && *(msg_ptr)) {
      
      put_user(*(msg_ptr++), buffer++);
      length--;
      bytes_read++;
    }

    if (!(*(msg_ptr))) {
      port_pins[pin][port].valid_message = 0;
    }

  } else {
    
    // raw output, so just output three chars, not formatted.
    
    while (length && msg_len) {
      put_user(*(msg_ptr++), buffer++);
      length--;
      msg_len--;
      bytes_read++;
    }

    if (msg_len == 0) {
      port_pins[pin][port].valid_message = 0;
    }

  }
  
  return bytes_read;
}

//=============================================================================

static ssize_t quadrature_read(struct file *filp, char *buffer, size_t length,
			       loff_t *offset, unsigned int pin, 
			       unsigned int port) {
  
  int bytes_read = 0;
  int retval;
  char *msg_ptr;
  int msg_len = 3;

  retval = 
    wait_event_interruptible(quadrature_pins[pin][port].encoder_wait, 
			     quadrature_pins[pin][port].valid_message != 0);

  if (retval) {
    return retval;
  }
    
  msg_ptr = quadrature_pins[pin][port].direction_msg;

  if (*(msg_ptr) == 0 && lp_encoder_int_output) {
    printk("Message not filled\n");
    return 0;
  }

  if (lp_encoder_int_output) {
    
    while (length && *(msg_ptr)) {
      
      put_user(*(msg_ptr++), buffer++);
      length--;
      bytes_read++;
    }

    if (!(*(msg_ptr))) {
      quadrature_pins[pin][port].valid_message = 0;
    }

  } else {
    
    // raw output, so just output three chars, not formatted.
    
    while (length && msg_len) {
      put_user(*(msg_ptr++), buffer++);
      length--;
      msg_len--;
      bytes_read++;
    }

    if (msg_len == 0) {
      quadrature_pins[pin][port].valid_message = 0;
    }

  }
  
  return bytes_read;
}

//=============================================================================

/**
 * @param filp see include/linux/fs.h
 */

static ssize_t device_read(struct file *filp, char *buffer, size_t length,
			   loff_t * offset) {
  
  unsigned int pin = NUM_PINS * 2;
  unsigned int port = NUM_PORTS;
  int i, j;

  for (j = 0; j < NUM_PORTS; j++) {
    for (i = 0; i < NUM_PINS; i++) {
      
      if (port_pins[i][j].filep == filp) {
	pin = i;
	port = j;
	return pin_read(filp, buffer, length, offset, pin, port);
      }

      if (quadrature_pins[i][j].filep == filp) {
	pin = i;
	port = j;
	return quadrature_read(filp, buffer, length, offset, pin, port);
      }

    }
  }

  printk("Pin not found!\n");

  return 0;
}

//=============================================================================

static ssize_t
device_write(struct file *filp, const char *buff, size_t len, loff_t * off) {
  
  printk("Sorry, this operation isn't supported.\n");

  return -EINVAL;
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

  for (j = 0; j < NUM_PORTS; j++) {
    for (i = 0; i < NUM_PINS; i++) {
      port_pins[i][j].pin_value = 0;
      port_pins[i][j].old_pin_value = 0;
      port_pins[i][j].open = 0;
      port_pins[i][j].filep = NULL;
      port_pins[i][j].count = 0;
      port_pins[i][j].valid_message = 0;
      quadrature_pins[i][j].open = 0;
      quadrature_pins[i][j].filep = NULL;
      quadrature_pins[i][j].direction = 0;
      quadrature_pins[i][j].valid_message = 0;

      init_waitqueue_head(&(port_pins[i][j].encoder_wait));
      init_waitqueue_head(&(quadrature_pins[i][j].encoder_wait));

      encoder_links[i][j] = -1;
    }
  }

  major = register_chrdev(0, DEVICE_NAME, &fops);

  if (major < 0) {
    printk("Registering the character device failed with %d\n", major);    
    return major;
  }

  if (lp_encoder_io_base[0] == 0) {
    lp_encoder_io_base[0] = LP1;
  }

  printk("Currently Using Ports:\n");
  for (j = 0; j < NUM_PORTS; j++) {
    printk("Port %d = %d\n", j, lp_encoder_io_base[j]);
  }

  if (lp_encoder_int_output) {
    printk("Using Integer Output\n");
  }
  
  printk("I was assigned major number %d.  Create a dev files with\n"
	 "'mknod /dev/encoder<n> c %d <n>'.\n", major, major);

  printk("   Where n = 1..%d for port 0, %d..%d for port 2 etc\n", 
	 NUM_PINS, NUM_PINS*2, NUM_PINS*3);

  if (lp_encoder_linked_encoders[0] == NULL) {
    printk("No linked encoders present\n");
    use_links = 0;
  } else {
    use_links = 1;
    parse_encoder_links();
  }

  if (use_links) {
    
    int link_count;
    
    printk("Encoder links were found as follows:\n");
    
    for (j = 0; j < NUM_PORTS; j++) {
      
      if (lp_encoder_io_base[j] != 0) {
	
	printk("For port %d : \n", j);
	link_count = 0;
	
	for (i = 0; i < NUM_PINS; i++) {
	  if (encoder_links[i][j] >= 0) {
	    printk("  %d -> %d (use `mknod /dev/encoder%d c %d %d')\n", 
		   i, encoder_links[i][j], 
		   NUM_PINS + i + j * NUM_PINS*2, 
		   major, NUM_PINS + i + j * NUM_PINS*2);
	    link_count++;
	  }
	}

	if (link_count == 0) {
	  printk("  None\n");
	}

      }

    }

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

  printk("Unloading lp_encoder module : Done\n");
}

//=============================================================================
