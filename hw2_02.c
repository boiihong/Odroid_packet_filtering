#include <linux/sys.h>
#include <linux/types.h>
#include <linux/module.h>
#include <linux/moduleparam.h>
#include <linux/kernel.h>
#include <linux/unistd.h>
#include <linux/timer.h>
#include <linux/init.h>
#include <linux/skbuff.h>
#include <linux/netdevice.h>
#include <linux/inet.h>
#include <linux/inetdevice.h>
#include <linux/rtnetlink.h>
#include <linux/wait.h>
#include <linux/proc_fs.h>
#include <linux/kthread.h>
#include <linux/completion.h>
#include <linux/time.h>
#include <linux/rbtree.h>
#include <linux/ip.h>
#include <asm/uaccess.h>
#include <linux/ktime.h>
#include <linux/if_ether.h>
#define hwp_num 100
static struct proc_dir_entry * hw2_proc = NULL;
struct iphdr *hw2_iphdr = NULL;	// ip header is declared in here but
//struct machdr *hw2_machdr = NULL; mac header is inline function so i declare at the inside of function
struct hw2_packet_store{
	unsigned int saddr; //sender ip address
	unsigned  long arriv_time;        //when is come?
	char *dev;                   //sender device name
	char *state;                 //packet is consumed or passed
};	//make struct packet_store 
struct hw2_packet_store hw2_packet[hwp_num];
int hw2_head = 0;
// later ....

static int packetfilter_device_notifier( struct notifier_block *unused, unsigned long event, void *ptr);
static int __init packetfilter_init(void);
static int __exit packetfilter_exit(void);
int hw2_desktop_on = 0;	//this is for on and off by proc_write function
int hw2_desktop_filter = 0;	//this is for knowing packet filter.
int hw2_otherdev_on = 0; 
int hw2_otherdev_filter = 0;
 



static struct notifier_block device_nb = {
	.notifier_call = packetfilter_device_notifier,
};

static int my_write(struct file *file, const char __user *buffer,unsigned long count, void *data)
{
	int len;
	int i;
	char hw2_buff[20];
	char *hw2_desktop_str = "hw2_desktop";   //these strings are for comparing with comming data.
	char *hw2_otherdev_str = "hw2_otherdev";	
	char *hw2_read = "hw2_read";
	len = count;
	if(copy_from_user(hw2_buff,buffer,len))
	return -EFAULT;
	
	hw2_buff[len] = 0;
	printk(KERN_INFO "what is come? : %s \n",hw2_buff);	//check what string is come (what button is pressed
	
	
	if(strncmp(hw2_buff,hw2_desktop_str,11) == 0){      // if string is "hw2_desktop"
	if(hw2_desktop_on == 0)                             // if filter is state of off
	{
	printk(KERN_INFO "desktop filter is on \n");        //then turn on the filter
	hw2_desktop_on = 1;
	}
	else 
	{hw2_desktop_on = 0;                                //if filter is alreadt state of on , then turn off the filter 
	printk(KERN_INFO "desktop filter is off \n");       
	}
	}
	
	if(strncmp(hw2_buff,hw2_otherdev_str,12) == 0){
	if(hw2_otherdev_on ==0)
	{
	printk(KERN_INFO "otherdev filter is on \n");
	hw2_otherdev_on = 1;
	}
	else 
	{hw2_otherdev_on = 0;
	printk(KERN_INFO "otherdev filter is off \n");
	}
	}
	
	if(strncmp(hw2_buff,hw2_read,8) == 0){   // if store button is clicked then this function work.
	
	for(i=0;i<hwp_num;i++)
	{
	printk(KERN_INFO  "state : %s dev : %s time : %lld ip_sdr :%u.%u.%u.%u \n",hw2_packet[i].state,hw2_packet[i].dev,hw2_packet[i].arriv_time ,(hw2_packet[i].saddr & 0xFF000000) >>24,(hw2_packet[i].saddr & 0x00FF0000) >>16,(hw2_packet[i].saddr & 0x0000FF00) >>8 ,(hw2_packet[i].saddr & 0x000000FF)); 
	}
	}								
		
	return len;
}
static int my_read(char *page, char **start , off_t off, int count, int *eof, void *data)  // not implemented
{
	int len = 0;
	start = &page;
	*eof = 1;
	return len;
}

rx_handler_result_t check_packet(struct sk_buff **skb){
	
	struct ethhdr *hw2_machdr;// inline function is just translated to 
	unsigned int ip_sdr;	  // the code so i have to declare machdr here.
	unsigned char* hw2_smac;  // to store sender mac address.
	long long hw2_time;       // for time
	hw2_iphdr = ip_hdr(*skb); // get ip header 
	hw2_machdr = eth_hdr(*skb);  //get mac header  
	ip_sdr = ntohl(hw2_iphdr->saddr);            //store ip 
	hw2_smac = hw2_machdr->h_source;             //store mac ,,, here we get pointer of mac address unsigned char h_source[6]
    hw2_time = jiffies_to_msecs(jiffies)         //use jiffies to get packet time.
	
	
	printk(KERN_INFO "time : %ld ip_sdr :%u.%u.%u.%u 	mac_sdr =%x:%x:%x:%x:%x:%x \n",hw2_time,(ip_sdr & 0xFF000000) >>24,(ip_sdr & 0x00FF0000) >>16,(ip_sdr & 0x0000FF00) >>8 ,(ip_sdr & 0x000000FF),hw2_smac[0],hw2_smac[1],hw2_smac[2],hw2_smac[3],hw2_smac[4],hw2_smac[5]); 
	
	if((hw2_smac[0] == 0x20) && (hw2_smac[1] == 0x16) && (hw2_smac[2]==0xd8) && (hw2_smac[3] == 0x87) && (hw2_smac[4] == 0x9d) && (hw2_smac[5] == 0x89))
	{               //check mac address is from desktop. 20:16:d8:87:9d:89 is my desktop network card mac address
	hw2_packet[hw2_head].saddr = ip_sdr;   // these line is for store data to the array
	hw2_packet[hw2_head].arriv_time = hw2_time;
	if(hw2_desktop_filter ==1) hw2_packet[hw2_head].state = "consumed\0";
	else hw2_packet[hw2_head].state = "pass\0";
	hw2_packet[hw2_head++].dev = "desktop\0";
	if(hw2_head ==hwp_num) hw2_head =0;
	
	hw2_desktop_filter = 1;}        //packet parameter to 1
	else 
	hw2_desktop_filter = 0;
	
	// here is for other dev mac address comming...
	if((hw2_smac[0] == 0xbc) && (hw2_smac[1] == 0x72) && (hw2_smac[2] == 0xb1) && (hw2_smac[3] == 0xd0) && (hw2_smac[4] == 0x02) && (hw2_smac[5] == 0x71))
	{               //check mac address is from otherdev(phone).  bc:72:b1:d0:02:71 is my phone network card mac address
	hw2_packet[hw2_head].saddr = ip_sdr;// these line is for store data to the array
	hw2_packet[hw2_head].arriv_time = hw2_time;
	if(hw2_otherdev_filter ==1) hw2_packet[hw2_head].state = "consumed\0";
	else hw2_packet[hw2_head].state = "pass\0";
	hw2_packet[hw2_head++].dev = "otherdev\0";
	if(hw2_head ==hwp_num) hw2_head =0;
	
	hw2_otherdev_filter = 1;}        //packet parameter to 1
	else
	hw2_otherdev_filter = 0;
	

	if((hw2_desktop_filter == 1) && (hw2_desktop_on ==1))// if filter on
	{						     // and target shotted 
	printk(KERN_INFO "\n\n\n desktop packet is consumed !!! \n\n\n");
	return RX_HANDLER_CONSUMED;      //consume if mac is desktop and filtering is on
	}
	else if((hw2_otherdev_filter ==1) && (hw2_otherdev_on ==1))
	{
	printk(KERN_INFO "\n\n\n otherdev packet is consumed !!! \n\n\n");
	return RX_HANDLER_CONSUMED;       //consume if mac is otherdev and filtering is on
	}
	else
	return RX_HANDLER_PASS;    // pass.
}

static int packetfilter_register_device (struct net_device *netdev){

	if(!strcmp(netdev->name,"wlan0")){
		printk(KERN_INFO "register!! device name: %s \n",netdev->name);
	netdev_rx_handler_register(netdev,(rx_handler_func_t *) check_packet , NULL);
}
	return 0;
}

static int packetfilter_unregister_device(struct net_device *netdev){
	if(!strcmp(netdev->name,"wlan0")){           
	printk(KERN_INFO "unregister device : %s \n",netdev->name);
	netdev_rx_handler_unregister(netdev);

}
	return 0;
}
static int packetfilter_device_notifier(struct notifier_block *unused, unsigned long event, void *ptr)
{
	struct net_device *netdev = ptr;
	printk(KERN_INFO " packetfilter_device_notifier call!! event : %ld \n",event);
	switch(event){
	
		case NETDEV_UP:
			break;
		case NETDEV_REGISTER:
			packetfilter_register_device(netdev);
			break;
		case NETDEV_DOWN:
			break;
		case NETDEV_REBOOT:
			break;
		case NETDEV_UNREGISTER:
			packetfilter_unregister_device(netdev);
			break;
		default :
			break;
		}
		
		return NOTIFY_DONE;
}

static int __init packetfilter_init(void)
{
	int ret;
	ret = register_netdevice_notifier(&device_nb);
	printk(KERN_INFO "pkfilter module init!! \n"); // make proc file system.
	hw2_proc = create_proc_entry("hw2_proc",0777,NULL);    
	hw2_proc->read_proc = my_read;
	hw2_proc->write_proc = my_write;

	return ret;
}
static int  __exit packetfilter_exit(void)
{
	int ret;
	ret = unregister_netdevice_notifier(&device_nb);
	
	printk(KERN_INFO "pkfilter module exit!! \n");
	return ret;
}

module_init(packetfilter_init);
module_exit(packetfilter_exit);

MODULE_LICENSE("GPL");

