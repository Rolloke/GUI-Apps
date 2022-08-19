/*
 * @file stepper_driver.c
 *
 * @brief LPT Stepper Motor kernel space driver
 *
 * @copy Copyright Rolf Kary-Ehlers (C) 
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
 * @author  Rolf Kary-Ehlers
 *
 *
 * @version: lpt_stepper, v 1.0  $
 */

//=============================================================================

#include <linux/kernel.h>
#include <linux/module.h>
#include <linux/version.h>
#include <linux/moduleparam.h>
#include <linux/init.h>
#include <linux/fs.h>
#include <linux/slab.h> 
#include <linux/cdev.h>
#include <linux/ioport.h>
#include <asm/io.h>             
#include <linux/proc_fs.h>
#include <linux/string.h>
#include <linux/delay.h>
#include <asm/uaccess.h>        // for put_user
#include <linux/interrupt.h>    // for irqreturn_t
#include <linux/workqueue.h>    // We scheduale tasks here
#include <linux/semaphore.h>
#include <linux/mutex.h>

#include <linux/list.h>
#include <linux/irq.h>

#include <asm/signal.h>


#include "lpt_stepper.h"
#include "stepper_control.h"

MODULE_AUTHOR("Rolf Kary-Ehlers, dr_kary@t-online.de");
MODULE_LICENSE("GPL");
MODULE_DESCRIPTION("Stepper motor driver for LPT port pins");
MODULE_ALIAS("lpt_stepper");

#define DEVICE_NAME "stepper" // DEV name as it appears in /proc/devices

int no_of_stepper_devices = 4;
int major = 0;
int minor = 0;

module_param(major, int, S_IRUGO);
module_param(minor, int, S_IRUGO);

// predeclarations
static int        open_lpt_stepper(struct inode *inode, struct file *file);
static int        release_lpt_stepper(struct inode *inode, struct file *file);
static ssize_t    write_lpt_stepper(struct file *filp, const char *buff, size_t len, loff_t * off);
static ssize_t    read_lpt_stepper(struct file *filp, char *buffer, size_t length, loff_t *offset);
static void       cleanup_lpt_stepper( void );
static long       ioctl_lpt_stepper( struct file *filp, unsigned int cmd, unsigned long arg);


// structure for parameter setting through write
struct parameter_id
{
    const char * name;
    long * parameter;
    unsigned long *uparameter;
    stepper_fnct_ptr pfnct;
    unsigned int base;
};

struct parameter_id setPID(const char*name, long* parameter, unsigned long* uparameter, stepper_fnct_ptr pfnct, unsigned int base)
{
    struct parameter_id pid= {name, parameter, uparameter, pfnct, base};
    return pid;
}

lpt_stepper_dev_t *stepper_devices = 0;

// file operations structure
static struct file_operations file_ops = 
{
  .owner   = THIS_MODULE,
  .read    = read_lpt_stepper,
  .write   = write_lpt_stepper,
  .unlocked_ioctl = ioctl_lpt_stepper,
  .open    = open_lpt_stepper,
  .release = release_lpt_stepper
};

// work queue functions
void delegated_work(struct work_struct*p);

static DECLARE_WORK(delegate, delegated_work);
struct semaphore delegate_semaphore;
struct semaphore delegate_wait;
struct workqueue_struct* work_queue = 0;  // workqueue for asynchronus plotting commands
LIST_HEAD(delegate_work_list);

void initialize_workqueue(lpt_stepper_dev_t *dev, long init, long b)
{
    if (init)
    {
        if (work_queue == 0)
        {
            work_queue = create_singlethread_workqueue("stepper_work_queue");
            sema_init(&delegate_semaphore, 0);
            sema_init(&delegate_wait, 0);
            queue_work(work_queue, &delegate);
        }
        dev->use_work_queue = init;
    }    
    else
    {
        if (work_queue)
        {
            delegate_work_list.next = 0;
            up(&delegate_semaphore);
            up(&delegate_wait);
            flush_workqueue(work_queue);
            destroy_workqueue(work_queue);
            work_queue = 0;
        }
        dev->use_work_queue = 0;
    }
}

void call_stepper_function(stepper_fnct_ptr pfnc, lpt_stepper_dev_t* dev, long p1, long p2)
{
    if (dev->use_work_queue)
    {
        struct delegated_data *data = kmalloc(sizeof(struct delegated_data), GFP_KERNEL);
        if (data)
        {
            data->pfnct = pfnc;
            data->pdev = dev;
            data->p1 = p1;
            data->p2 = p2;
            list_add_tail(&data->list, &delegate_work_list);
            up(&delegate_semaphore);
            if (dev->use_work_queue > 1)
            {
                down_interruptible(&delegate_wait);
            }
        }     
    }
    else
    {
        pfnc(dev, p1, p2);
    }                        
}

void delegated_work(struct work_struct*p)
{
    struct list_head   *ptr;
    struct delegated_data *pdata;
    while (stepper_devices != 0)
    {
        down_interruptible(&delegate_semaphore);
        ptr = delegate_work_list.next;
        if (ptr)
        {
            pdata = list_entry(ptr, struct delegated_data, list);
            if (pdata)
            {
                pdata->pfnct(pdata->pdev, pdata->p1, pdata->p2);
                //printk(KERN_ERR "delegated_work: %ld, %ld\n", pdata->p1, pdata->p2);
                if (pdata->pdev->use_work_queue > 1)
                {
                    up(&delegate_wait);
                }
                list_del(ptr);
                kfree(pdata);
            }
        }
        else
        {
            break;
        }
    }
}

void check_device_address(struct lpt_stepper_dev *ls_dev)
{
    struct resource*iop_res=0;
    iop_res = request_region(ls_dev->base_address, 3, "parport0");
    if (iop_res == 0)
    {
        iop_res = request_region(ls_dev->base_address, 3, "parport1");
    }
    if (iop_res == 0)
    {
        iop_res = request_region(ls_dev->base_address, 3, "serial");
    }
    if (iop_res)
    {
        printk(KERN_ERR "stepper device %s, %x - %x, flags: %x, (%p, %p, %p)\n", 
        iop_res->name, (unsigned  int)iop_res->start,
        (unsigned  int)iop_res->end, (unsigned  int)iop_res->flags, 
        iop_res->parent, iop_res->sibling, iop_res->child);
    }
    else
    {
        printk(KERN_ERR "Error stepper device (%x) not availeable\n",
         (unsigned  int)ls_dev->base_address);
    }
}

const char* getDirectionName(enum eDirection dir)
{
    static const char* directionName[] = { "x", "y", "z", "a", "b", "c" };
    if (dir >= xdir && dir < directions)
    {
        return directionName[dir];
    }
    return "";
}

enum eDirection getDirection(const char* name)
{
    int dir;
    for (dir=0; dir<directions; ++dir)
    {
        if (strstr(name, getDirectionName(dir)) != 0)
        {
            return (enum eDirection)dir;
        }
    }
    return invalid_dir;
}

static void setup_stepper_cdev(struct lpt_stepper_dev *ls_dev, int index)
{
    int i, err = 0;
    int devno = MKDEV(major, index);
    
    printk(KERN_ERR "setup_stepper_cdev(%x, %d), %d, %x\n", (unsigned int)ls_dev, index, major, (unsigned int)devno);
    
    cdev_init(&ls_dev->cdev, &file_ops);
    ls_dev->cdev.owner = THIS_MODULE;
    ls_dev->cdev.ops = &file_ops;
    
    err = cdev_add(&ls_dev->cdev, devno, 1);
    
    ls_dev->device_number = index;

    for (i=0; i<directions; ++i)
    {
        ls_dev->delay[i]       = 2000;
        ls_dev->start_delay[i] = 3000;
        ls_dev->ramp_steps[i]  = 50;
        ls_dev->motor[i]       = Invalid;
    }

    switch(index)
    {
        case 0:
            ls_dev->base_address = 0x278;
            ls_dev->irq_nr = 2;
            break;
        case 1:
            ls_dev->base_address = 0x378;
            ls_dev->irq_nr = 7;
            break;
        case 2:
            ls_dev->base_address = 0x3bc;
            ls_dev->irq_nr = 5;
            break;
        case 3:
            ls_dev->base_address = 0x2bc;
            ls_dev->irq_nr = 0;
            break;
    }
    check_device_address(ls_dev);
    ls_dev->data_control[0] = inb(ls_dev->base_address+data_port);
    ls_dev->data_control[1] = inb(ls_dev->base_address+control_port);
    
    if (err)
    {
        printk(KERN_ERR "Error %d adding stepper%d\n", err, index);
    }
}
/* struct inode *inode, struct file *file */
static int __init init_lpt_stepper( void )
{
    int result, i;
    dev_t dev = 0;
    // Get a range of minor numbers to work with, asking for a dynamic
    //major unless directed otherwise at load time.
    if (major)
    {
        dev = MKDEV(major, minor);
        result = register_chrdev_region(dev, no_of_stepper_devices, DEVICE_NAME);
        printk(KERN_WARNING "%s: register chrdev %d\n", DEVICE_NAME, result);
    }
    else
    {
        result = alloc_chrdev_region(&dev, minor, no_of_stepper_devices, DEVICE_NAME);
                 major = MAJOR(dev);
        printk(KERN_WARNING "%s: major is %d\n", DEVICE_NAME, major);
    }
    if (result < 0)
    {
        printk(KERN_WARNING "%s: can't get major %d\n", DEVICE_NAME, major);
        return result;
    }

    // allocate the devices -- we can't have them static, as the number
    // can be specified at load time
    stepper_devices = kmalloc(no_of_stepper_devices * sizeof(lpt_stepper_dev_t), GFP_KERNEL);
    if (!stepper_devices) 
    {
        result = -ENOMEM;
        goto fail;
    }
    memset(stepper_devices, 0, no_of_stepper_devices * sizeof(lpt_stepper_dev_t));
    
    // Initialize each device.
    for (i=0; i<no_of_stepper_devices; ++i)
    {
        setup_stepper_cdev(&stepper_devices[i], minor+i);
    }
    #ifdef __KERNEL__
    printk(KERN_ALERT "initialized kernel lpt stepper driver :%d\n", major);
    #else
    printk(KERN_ALERT "initialized user lpt stepper driver :%d\n", major);
    #endif
    return 0;

fail:
    printk(KERN_ALERT "failed to initialize lpt stepper driver :%d\n", major);
    cleanup_lpt_stepper();
    return result;
    
}

module_init(init_lpt_stepper);

static void cleanup_lpt_stepper( void )
{
    int i;
    dev_t devno = MKDEV(major, minor);
    
    printk(KERN_ALERT "exited lpt stepper driver: %d\n", major);
    initialize_workqueue(stepper_devices, 0, 0);
    /* Get rid of our char dev entries */
    if (stepper_devices) 
    {
        for (i = 0; i < no_of_stepper_devices; ++i) 
        {
            release_region(stepper_devices[i].base_address, 3);
            cdev_del(&stepper_devices[i].cdev);
        }
        kfree(stepper_devices);
        stepper_devices = 0;
    }
    unregister_chrdev_region(devno, no_of_stepper_devices);
}

module_exit(cleanup_lpt_stepper);


static int open_lpt_stepper(struct inode *inode, struct file *file) 
{
    int res = 0;
    unsigned int minor = iminor(inode);
    lpt_stepper_dev_t *dev = container_of(inode->i_cdev, lpt_stepper_dev_t, cdev);
    file->private_data = dev; // for other methods 
    if (file->f_flags & O_NONBLOCK)
    {
        initialize_workqueue(dev, 1, 0);
    }          
#ifdef __KERNEL__
  printk(KERN_ALERT "opened kernel lpt stepper driver %d:%d\n", major, minor);
#else
  printk(KERN_ALERT "opened lpt stepper driver %d:%d\n", major, minor);
#endif
  try_module_get(THIS_MODULE);

  return res;
}

static int release_lpt_stepper(struct inode *inode, struct file *file)
{
  unsigned int minor = iminor(inode);
  
  printk(KERN_ALERT "released lpt stepper driver %d:%d\n", major, minor);
  
  module_put(THIS_MODULE);

  return 0;
}

static ssize_t write_lpt_stepper(struct file *filp, const char *buff, size_t length, loff_t * offset)
{
    ssize_t ret = 0;
    lpt_stepper_dev_t *dev = filp->private_data;
    int i=0, motordir = xdir;
    
    if (buff)
    {
        ((char*)buff)[length] = 0;
    }
    if (dev)
    {
        unsigned long value = 0;
        char* token = (char*)buff;
        char* nexttoken = 0;
        struct parameter_id ids[] = {
        { "set address:", 0, &value, 0, 16 },                                // 0
        { "set data:"   , 0, &value, 0, 16},                                 // 1
        { "set control:", 0, &value, 0, 16},                                 // 2
        { "set status:"  , 0, &value, 0, 16},                                // 3
        { "set motor:", 0, 0, 0, 0 },                                        // 4
        { "set motor bits:", 0, 0, 0, 0 },                                   // 5
        { "set position:", &dev->position[xdir], 0, 0, 10},                  // 6
        { "set delay:", &dev->delay[xdir], 0, 0, 10},                        // 7
        { "set start delay:", &dev->start_delay[xdir], 0, 0, 10},            // 8
        { "set ramp steps:", &dev->ramp_steps[xdir], 0, 0, 10},              // 9
        { "set enable motors bit:", &dev->enable_motors, 0, 0, 10},          // 10
        { "set pen position up:", &dev->pen_position[Pen_up], 0, 0, 10},     // 11
        { "set pen position down:", &dev->pen_position[Pen_down], 0, 0, 10}, // 12
        { "set pen position drill:", &dev->pen_position[Pen_drill], 0, 0, 10},//13
        { "set left end bit:", &dev->status_bits[left_end], 0, 0, 10},       // 14
        { "set right end bit:", &dev->status_bits[right_end], 0, 0, 10},     // 15
        { "set top end bit:", &dev->status_bits[top_end], 0, 0, 10},         // 16
        { "set bottom end bit:", &dev->status_bits[bottom_end], 0, 0, 10},   // 17
        { "set pen down bit:", &dev->status_bits[pen_down], 0, 0, 10},       // 18
        { "set stop all bit:", &dev->status_bits[stop_all], 0, 0, 10},       // 19
        { "move to:"       , 0, 0, stepper_move_to, 10},                     // 20
        { "line to:"       , 0, 0, stepper_line_to, 10},                     // 21
        { "move relative:" , 0, 0, stepper_move_relative, 10},               // 22
        { "line relative:" , 0, 0, stepper_line_relative, 10},               // 23
        { "move pen:"      , 0, 0, movePen, 10},                             // 24
        { "enable motors:" , 0, 0, enableMotors, 10},                        // 25
        { "set output bit:", 0, 0, setOutputBit, 10},                        // 26
        { "move motor:"    , 0, 0, moveMotor, 10},                           // 27
        { "set async:"     , 0, 0, initialize_workqueue, 10},                // 28
        { 0, 0, 0, 0, 0}
        };
        do
        {
            nexttoken = strstr(token, "\n");
            if (nexttoken)
            {
                *nexttoken = 0;
                nexttoken++;
            }
         
            for (i=0; ids[i].name != 0; ++i)
            {
                char*ptr = strstr(token, ids[i].name);
                if (ptr)
                {
                    size_t pos = strlen(ids[i].name);
                    ptr = skip_spaces(&ptr[pos]);
                    
                    motordir = getDirection(ptr);
                    if (motordir != invalid_dir)
                    {
                        ptr++;
                        ptr = skip_spaces(ptr);
                    }
                    else if (ids[i].pfnct == 0)
                    {
                        motordir = 0;
                    }
                    
                    if       (ids[i].uparameter)
                    {
                        ret = kstrtoul(ptr, ids[i].base, &ids[i].uparameter[motordir]);
                    }
                    else  if (ids[i].parameter)
                    {
                        ret = kstrtol(ptr, ids[i].base, &ids[i].parameter[motordir]);
                    }
                    else if (ids[i].pfnct)
                    {
                        long x, y;
                        ret = sscanf(ptr, "%ld %ld", &x, &y);
                        if (motordir == invalid_dir)
                        {
                            call_stepper_function(ids[i].pfnct, dev, x, y);
                        }
                        else
                        {
                            call_stepper_function(ids[i].pfnct, dev, motordir, x);
                        }

                    }
                    switch (i)
                    {
                        case 0:
                        release_region(dev->base_address, 3);
                        dev->base_address = value;
                        check_device_address(dev);
                        break;
                        case 1:
                            outb(value, dev->base_address);
                            dev->data_control[0] = value;
                        break;
                        case 2: 
                            outb(value, dev->base_address+control_port); 
                            dev->data_control[1] = value;
                        break;
                        case 3: 
                            outb(value, dev->base_address+status_port); 
                        break;
                        case 4: 
                        {
                            int j;
                            for (j=0; j<motors; ++j)
                            {
                                if (strstr(ptr, getNameOfMotor(j)))
                                {
                                    dev->motor[motordir] = j;
                                }
                            }
                        }break;
                        case 5:
                        {
                            long b1=0, b2=0, b3=0, b4=0;
                            ret = sscanf (ptr, "%ld %ld %ld %ld", &b1, &b2, &b3, &b4);
                            dev->motor_bits[motordir][bit0] = b1;
                            dev->motor_bits[motordir][bit1] = b2;
                            dev->motor_bits[motordir][bit2] = b3;
                            dev->motor_bits[motordir][bit3] = b4;
                        }
                        break;
                        case 6: case 7: case 8: case 9:
                        {
                            
                        }
                        default: break;
                    }
                }
            }
            token = nexttoken;
        }  while (token != 0);
    }
    else
    {
        printk(KERN_ALERT "no private Data\n");
    }

    if(ret < 0)
    {
        printk(KERN_ALERT "ERROR %d in write_lpt_stepper %s\n", ret, buff);
    }
    
    ret = length;
    *offset += length;
    return ret;
}

static ssize_t read_lpt_stepper(struct file *filp, char *buffer, size_t length, loff_t *offset)
{
    static int print_eof = 0;
    int dir;
    ssize_t ret = 0, result_length=0;
    lpt_stepper_dev_t *dev = filp->private_data; 
    if (dev)
    {
        if (print_eof)
        {
            length = 0;
            print_eof = 0;
        }
        else
        {
            result_length = snprintf (buffer, length, "LPT Stepper port Parameter:\n"
                " device number %d \n"
                " base address : %x\n"
                " Current values\n"
                "   data       : %x\n"
                "   control    : %x\n"
                "   status     : %x\n"
                "  data_control %x\n"
                "   pen positions up: %ld, down: %ld, drill: %ld\n"
                " enable motors bit: %ld\n"
                " Status and according bits: (3 - 7)\n"
                "   left end bit  : %ld -> %d\n"
                "   right end bit : %ld -> %d\n"
                "   top end bit   : %ld -> %d\n"
                "   bottom end bit: %ld -> %d\n"
                "   pen down bit  : %ld -> %d\n"
                "   stop all bit  : %ld -> %d\n"
                "%s\n"
                "Motor types (%s, %s, %s, %s)\n"
                "   output bits (1 - 12)\n"
                "Direction dependent parameter:\n"
                , dev->device_number
                , (int)dev->base_address
                , (unsigned int) inb(dev->base_address)
                , (unsigned int) inb(dev->base_address+control_port)
                , (unsigned int) inb(dev->base_address+status_port)
                , *((unsigned short*)&dev->data_control[0])
                , dev->pen_position[Pen_up]
                , dev->pen_position[Pen_down]
                , dev->pen_position[Pen_drill]
                , dev->enable_motors
                , dev->status_bits[left_end]
                , dev->status_bits[left_end] != 0 ? testBit(dev->status_bits[left_end], getStatus(dev)) : -1 
                , dev->status_bits[right_end]
                , dev->status_bits[right_end] != 0 ? testBit(dev->status_bits[right_end], getStatus(dev)) : -1 
                , dev->status_bits[top_end]
                , dev->status_bits[top_end] != 0 ? testBit(dev->status_bits[top_end], getStatus(dev)) : -1 
                , dev->status_bits[bottom_end]
                , dev->status_bits[bottom_end] != 0 ? testBit(dev->status_bits[bottom_end], getStatus(dev)) : -1 
                , dev->status_bits[pen_down]
                , dev->status_bits[pen_down] != 0 ? testBit(dev->status_bits[pen_down], getStatus(dev)) : -1 
                , dev->status_bits[stop_all]
                , dev->status_bits[stop_all] != 0 ? testBit(dev->status_bits[stop_all], getStatus(dev)) : -1 
                , getHelpText()
                , getNameOfMotor(BipolarFull), getNameOfMotor(BipolarHalf)
                , getNameOfMotor(UnipolarFull), getNameOfMotor(UnipolarHalf)
                );
                for (dir=xdir; dir<directions; ++dir) 
                {
                    if (dev->motor[dir] != Invalid)
                    {
                        result_length += snprintf (&buffer[result_length], length-result_length, 
                        "\nDirection %s has position: %ld\n"
                        " - motor: %s\n"
                        " - motor bits : %ld, %ld, %ld, %ld\n"
                        " - delays: from %ld us to %ld us in %ld steps\n"
                        , getDirectionName(dir)
                        , dev->position[dir]
                        , getNameOfMotor(dev->motor[dir])
                        , dev->motor_bits[dir][bit0], dev->motor_bits[dir][bit1]
                        , dev->motor_bits[dir][bit2], dev->motor_bits[dir][bit3]
                        , dev->start_delay[dir], dev->delay[dir], dev->ramp_steps[dir]
                        );
                    }
                    
                }
                        
            print_eof = 1;
        }
    }
    else
    {
        printk(KERN_ALERT "no private Data\n");
    }
    

    ret = result_length;
    *offset += result_length;
    return ret;
}

static long ioctl_lpt_stepper(struct file *filp, unsigned int cmd, unsigned long arg)
{
    int retval = -ENOMEM;
    lpt_stepper_dev_t *dev = filp->private_data; 
    if (dev)
    {
        retval = 0;
        //printk(KERN_ALERT "ioctl_lpt_stepper %d, %x \n", cmd, (unsigned int)arg);
        switch (cmd)
        {
            case LPT_STEPPER_IOCT_BASE_ADDRESS:
            dev->base_address = arg;
            retval = 0;
            break;
            
            case LPT_STEPPER_IOCT_VALUE:
            {
                char value = (arg&0x000000ff);
                short type = ((arg>>16)&0xffff);
                if (type < ports)
                {
                    if (type == data_port)    dev->data_control[0] = value;
                    if (type == control_port) dev->data_control[1] = value;
                    outb(value, dev->base_address+type);
                    retval = 0;
                }
                else
                {
                    retval = -EINVAL;
                }
            } break;
            
            case LPT_STEPPER_IOCQ_VALUE:
            {
                if (arg < ports)
                {
                    retval = inb(dev->base_address+arg);
                }
                else
                {
                    retval = -EINVAL;
                }
            } break;
            case LPT_STEPPER_IOCQ_STATUS_BIT:
            {
                if (arg < states && dev->status_bits[arg] != 0)
                {
                    retval = testBit(dev->status_bits[arg], getStatus(dev));
                }
                else
                {
                    retval = -EINVAL;
                }
            } break;
            case LPT_STEPPER_IOCQ_BASE_ADDRESS:
                retval = dev->base_address;
            break;
            case LPT_STEPPER_IOCS_MOVE_TO:
            {
                long pos[2];
                retval = raw_copy_from_user(pos, (void __user *)arg, sizeof(pos));
                call_stepper_function(stepper_move_to, dev, pos[0], pos[1]);
                retval = 0;
            } break;
            case LPT_STEPPER_IOCS_LINE_TO:
            {
                long pos[2];
                retval = raw_copy_from_user(pos, (void __user *)arg, sizeof(pos));
                call_stepper_function(stepper_line_to, dev, pos[0], pos[1]);
                retval = 0;
            } break;
            case LPT_STEPPER_IOCS_MOVE_RELATIVE:
            {
                long pos[2];
                retval = raw_copy_from_user(pos, (void __user *)arg, sizeof(pos));
                call_stepper_function(stepper_move_relative, dev, pos[0], pos[1]);
                retval = 0;
            } break;
            case LPT_STEPPER_IOCS_LINE_RELATIVE:
            {
                long pos[2];
                retval = raw_copy_from_user(pos, (void __user *)arg, sizeof(pos));
                call_stepper_function(stepper_line_relative, dev, pos[0], pos[1]);
                retval = 0;
            } break;
            case LPT_STEPPER_IOCS_LINE_TO_3D:
            {
                long pos[3];
                retval = raw_copy_from_user(pos, (void __user *)arg, sizeof(pos));
                stepper_line_to_3d(dev, pos[0], pos[1], pos[2]);
                retval = 0;
            } break;
            case LPT_STEPPER_IOCS_PEN_POS:
            {
                long pen_pos[2];
                retval = raw_copy_from_user(pen_pos, (void __user *)arg, sizeof(pen_pos));
                if (pen_pos[0] >= 0 && pen_pos[0] < Pen_positions)
                {
                    dev->pen_position[pen_pos[0]] = pen_pos[1];
                    retval = 0;
                }
                else
                {
                    retval = -EINVAL;
                }
            } break;
            case LPT_STEPPER_IOCT_MOVE_TO_PEN_POS:
                call_stepper_function(movePen, dev, arg, 0);
                retval = 0;
                break;
            case LPT_STEPPER_IOCT_ENABLE_MOTORS: 
                enableMotors(dev, arg, 0);
                retval = 0;
                break;
            case LPT_STEPPER_IOCT_ENABLE_MOTORS_BIT:
                dev->enable_motors = arg;
                retval = 0;
                break;
            case LPT_STEPPER_IOCS_MOTORS:
            {
                unsigned long dir_motor[2];
                retval = raw_copy_from_user(dir_motor, (void __user *)arg, sizeof(dir_motor));
                if (dir_motor[0] < directions)
                {
                    dev->motor[dir_motor[0]] = (enum eMotors)dir_motor[1];
                    retval = 0;
                }
                else
                {
                    retval = -EINVAL;
                }
            } break;
            case LPT_STEPPER_IOCS_MOTOR_BITS:
            {
                long dir;
                retval = get_user(dir, (long __user *)arg);
                if (dir >= 0 && dir < directions)
                {
                    retval = raw_copy_from_user(dev->motor_bits[dir], &((long *)arg)[1], sizeof(dev->motor_bits[dir]));
                }
                else
                {
                    retval = -EINVAL;
                }
            } break;
            case LPT_STEPPER_IOCS_STATUS_BIT:
            {
                unsigned long status_bit[2];
                retval = raw_copy_from_user(status_bit, (void __user *)arg, sizeof(status_bit));
                if (status_bit[0] < states)
                {
                    dev->status_bits[status_bit[0]] = status_bit[1];
                    retval = 0;
                }
                else
                {
                    retval = -EINVAL;
                }
            } break;
            case LPT_STEPPER_IOCS_POSITION:
            {
                long dir_pos[2];
                retval = raw_copy_from_user(dir_pos, (void __user *)arg, sizeof(dir_pos));
                if (dir_pos[0] >= 0 && dir_pos[0] < directions)
                {
                    dev->position[dir_pos[0]] = dir_pos[1];
                    retval = 0;
                }
                else
                {
                    retval = -EINVAL;
                }
            } break;
            case LPT_STEPPER_IOCQ_POSITION:
            if (arg < directions)
            {
                retval = dev->position[arg];
            }
            else
            {
                retval = -EINVAL;
            } break;
            case LPT_STEPPER_IOCS_DELAY:
            {
                long dir_delay[2];
                retval = raw_copy_from_user(dir_delay, (void __user *)arg, sizeof(dir_delay));
                if (dir_delay[0] >= 0 && dir_delay[0] < directions)
                {
                    dev->delay[dir_delay[0]] = dir_delay[1];
                    retval = 0;
                }
                else
                {
                    retval = -EINVAL;
                }
            } break;
            case LPT_STEPPER_IOCS_START_DELAY:
            {
                long dir_delay[2];
                retval = raw_copy_from_user(dir_delay, (void __user *)arg, sizeof(dir_delay));
                if (dir_delay[0] >= 0 && dir_delay[0] < directions)
                {
                    dev->start_delay[dir_delay[0]] = dir_delay[1];
                    retval = 0;
                }
                else
                {
                    retval = -EINVAL;
                }
            } break;
            case LPT_STEPPER_IOCS_RAMP_STEPS:
            {
                long dir_steps[2];
                retval = raw_copy_from_user(dir_steps, (void __user *)arg, sizeof(dir_steps));
                if (dir_steps[0] >= 0 && dir_steps[0] < directions)
                {
                    dev->ramp_steps[dir_steps[0]] = dir_steps[1];
                }
                else
                {
                    retval = -EINVAL;
                }
            } break;
            case LPT_STEPPER_IOCT_REGISTER_INTERRUPT_TO_STATE:
            {
                if (arg == no_state)
                {
                    free_irq(dev->irq_nr, dev);
                    retval = 0;
                }
                else if (arg < states)
                {
                    char dev_name[16] = "stepper";
                    int len = strlen(dev_name);
                    dev_name[len++] = '0' + dev->device_number;
                    dev_name[len++] = 0;
//                    retval = request_irq(dev->irq_nr, handleLptInterrupt, IRQF_DISABLED, dev_name, dev);
                    setOutputBit(dev, EnableInterrupt, 1);
                    dev->status_bits[arg] = 10;
                }
            } break;
            case LPT_STEPPER_IOCS_MOVE_TO_POSITION:
            {
                long dir_pos[2];
                retval = raw_copy_from_user(dir_pos, (void __user *)arg, sizeof(dir_pos));
                if (dir_pos[0] >= 0 && dir_pos[0] < directions)
                {
                    call_stepper_function(moveMotor, dev, dir_pos[0], dir_pos[1]);
                }
                else
                {
                    retval = -EINVAL;
                }
            } break;
            case LPT_STEPPER_IOCS_LPT_CONTROL_BIT:
            {
                long bit_value[2];
                retval = raw_copy_from_user(bit_value, (void __user *)arg, sizeof(bit_value));
                if (bit_value[0] >= DataStrobe && bit_value[0] <= EnableInterrupt)
                {
                    setOutputBit(dev, bit_value[0], bit_value[1]);
                }
                else
                {
                    retval = -EINVAL;
                }
            } break;
            case LPT_STEPPER_IOCQ_LPT_STATUS_BIT:
            {
                if (arg >= Error && arg <= Busy)
                {
                    retval = testBit(arg, getStatus(dev));
                }
                else
                {
                    retval = -EINVAL;
                }
            } break;
            case LPT_STEPPER_IOCT_MICRO_DELAY:
            {
                printk(KERN_ALERT "delay: %lu, us\n", arg);
                if (arg > 0)
                {
                    udelay(arg);
                }
                else
                {
                    retval = -EINVAL;
                }
            } break;
            default: retval = -ENOTTY; break;
        }
    }
    return retval;
}

