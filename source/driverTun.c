/*
 * Copyright (c) 2015-2015, ARM Limited, All Rights Reserved
 */

#include <pthread.h>
#include <stdio.h>
#include <fcntl.h>
//#include <sys/ioctl.h>
#include <unistd.h>
#include <errno.h>
//#include <socket.h>
//#include <net_if.h>
//#include <linux/if_tun.h>
//#include <netinet/in.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
//#include <poll.h>
//#include <arpa/inet.h>
#include <signal.h>
#include "platform/arm_hal_phy.h"
//#include "Platforms/linux/whiteboard.h"
//#include "Platforms/linux/linux_netlink.h"
//#include "global_config.h"
#include "ns_trace.h"
#include "cmsis_os.h"
#include "mbed_toolchain.h"

#define TRACE_GROUP "drTUN"

#define PROTOCOL_ID_IPV6 0x86dd
#define TUN_MTU 1500
#define BUFFERSIZE (TUN_MTU)

#define IFF_TUN 0x0001

static int virtual_device_descriptor = -1;
static char dump_array[BUFFERSIZE];
#define THREAD_STACKSIZE 2048

/* TUN callbacks */
static int8_t tun_state_control(phy_interface_state_e, uint8_t);
static int8_t tun_tx(uint8_t *, uint16_t, uint8_t, data_protocol_e);
static int8_t tun_address_write(phy_address_type_e , uint8_t *);
static int8_t tun_extension(phy_extension_type_e, uint8_t *);
//static int linux_interface_up(char *devname);
static int8_t register_tun_phy(void);
static int8_t br_tasklet_id = -1;

static int8_t device_id = -1;
static uint8_t fake_mac[6] = {0, 0, 0, 0, 0, 1};

static phy_device_driver_s tun_phy_driver = {
    .link_type = PHY_LINK_TUN,
    .data_request_layer = IPV6_DATAGRAMS_DATA_FLOW,
    .PHY_MAC = fake_mac,
    .driver_description = "VPN TUN interface",
    .phy_MTU = 1500,
    .phy_tail_length = 0,
    .phy_header_length = 0,
    .state_control = tun_state_control,
    .tx = tun_tx,
    .address_write = tun_address_write,
    .extension = tun_extension,
    .phy_rx_cb = NULL,
    .phy_tx_done_cb = NULL,
    .tunnel_type = IFF_TUN
};

/* Threads control blocks */
osRtxThread_t tun_thread_cb = {0};

/* Threads stacks */
MBED_ALIGN(8) uint8_t tun_thread_stack[THREAD_STACKSIZE] = {0};

osThreadAttr_t tunnel_thread_attr = {
    .name = "tunnel_reader_thread",
    .attr_bits = 0,
    .cb_mem = &tun_thread_cb,
    .cb_size = sizeof(tun_thread_cb),
    .stack_mem = tun_thread_stack,
    .stack_size = THREAD_STACKSIZE,
    .priority = osPriorityNormal,
    .tz_module = 0,
    .reserved = 0
};

int8_t init_tunnel(void)
{
	return register_tun_phy();
}
/*
 * A function to create TUN interface.
 */
//static pthread_t vtun_thread;
void tun_reader_thread(void *ptr)
{
	printf("\ntun_reader_thread started\n\n");
	uint16_t len = 10;
    for (;;) {
		//tun_receiving(dump_array[0], len);
	}
}

static int open_tun(const char *name)
{/*
    struct ifreq ifr;
    char devname[100];

    strcpy(devname, name);

    tr_info("Creating tun interface: %s", devname);
    int rf_fd = -1;
    //open clone dev
    //tuntap = fopen("/dev/net/tun", O_RDWR);

    //create empty header for tun and eth
    memset(dump_array, 0x00, sizeof(dump_array));

    //set next protocol to be 802.15.4 in wireshark
    dump_array[16] = 0x80;
    dump_array[17] = 0x9a;

    if ((rf_fd = open("/dev/net/tun", O_RDWR)) < 0) {
        //tr_debug("%s: Cannot open /dev/net/tun: %s. Do modprobe tun; lsmod", _progname, strerror(errno));
        return -1;
    }
    if (rf_fd < 0) {
        return -1;
    }
    memset(&ifr, 0, sizeof(ifr));
    //set if type
    ifr.ifr_flags = IFF_TUN; // IFF_TUN   - TUN device (no Ethernet headers)

    //set if name
    strncpy(ifr.ifr_name, devname, IFNAMSIZ);

    //create interface info struct
    if (ioctl(rf_fd, TUNSETIFF, (void *) &ifr) == -1) {
        int err = errno;
        tr_error("ioctl() failed to create TUN interface:\n%s", strerror(err));
        close(rf_fd);
        return -1;
    }
    //copy name if changed by kernel
    strcpy(devname, ifr.ifr_name);

    //call interface up
    if (linux_interface_up(devname) < 0) {
        return -1;
    }

    // Generate dummy address for TUN interface based on the prefix + :1 at the end.
    const char *prefix = cfg_string(global_config, "PREFIX", "fd00:1::");
    uint8_t addr[16];
    inet_pton(AF_INET6, prefix, addr);
    addr[15] = 1;
    lnl_iface_add_address(addr, 64, devname);

    // Generate dummy link-local address for TUN interface
    prefix = cfg_string(global_config, "TUN_LL_ADDR", "fe80::1");
    inet_pton(AF_INET6, prefix, addr);
    lnl_iface_add_address(addr, 64, devname);

    //save global descriptor
    virtual_device_descriptor = rf_fd; */
    return 0;
}

static int8_t register_tun_phy(void)
{
	/*    int rc;

    if (cfg_int(global_config, "START_TUN", 1)) {
        rc = open_tun(name);
        if (rc) {
            return rc;
        }
    }
*/
    /* Register device to nanostack */

	device_id = arm_net_phy_register(&tun_phy_driver);
    if (device_id < 0) {
        tr_error("Registering TUN device driver failed");
    }

    osThreadNew(tun_reader_thread, NULL, &tunnel_thread_attr);

/*
    if (cfg_int(global_config, "START_TUN", 1)) {
        pthread_create(&vtun_thread, 0, tun_reader_thread, 0);
        whiteboard_os_enable();
    } else {
        tr_error("No tun!");
    }
*/
    return device_id;
}

/*
void *tun_reader_thread(void *param)
{
    struct pollfd fds[1];
    int length;
    unsigned char buffer[BUFFERSIZE];
    (void)param;

    fds[0].fd = virtual_device_descriptor;
    fds[0].events = POLLIN;

    // Block SIGTERM for this thread.
    // It is handled elsewhere
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGTERM);
    if (pthread_sigmask(SIG_BLOCK,&set, NULL))
        perror("pthread_sigmask()"); // Carry on, not so fatal

    tr_info("%s : Entered. tun_fd = %d", __FUNCTION__, fds[0].fd);

    for (;;) {
        if ((poll(fds, 1, -1)) == -1) {
            tr_error("poll");
            return NULL;
        }

        if ((length = read(fds[0].fd , buffer, BUFFERSIZE)) < 0) {
            tr_error("read");
            close(fds[0].fd);
            return NULL;
        } else if (length > 4) {
            if (tun_phy_driver.phy_rx_cb) {
                tun_phy_driver.phy_rx_cb(buffer, length, 0x80, 0, device_id);
            }else{
                tr_error("phy_rx_cb missing, %s, %d", trace_array(buffer, length), device_id);
            }
        } else {
            tr_error("length");
        }
    }
    return 0;
}*/

static int8_t tun_tx(uint8_t *ptr, uint16_t len, uint8_t handle, data_protocol_e proto)
{
    (void)handle;
    (void)proto;
    /*
    if (virtual_device_descriptor < 0) {
        return -1;
    }*/

    ptr[0] = (IFF_TUN >> 8); // Flags
    ptr[1] = (IFF_TUN & 0xFF); // Flags
    //TODO: check protocol, currently just assume IPv6
/*
    write(virtual_device_descriptor, ptr, len);
    return 0;*/ // TX done

    uint8_t result = tun_sending(ptr, len);

}

static int8_t tun_state_control(phy_interface_state_e state, uint8_t arg1)
{
    (void)arg1;
//    tr_info("tun_state_control(): ");
    switch (state) {
        case PHY_INTERFACE_UP:
            tr_info("up");
            break;
        case PHY_INTERFACE_DOWN:
            tr_info("down");
            break;
        case PHY_INTERFACE_RESET:
            tr_info("reset");
            break;
        default:
            tr_info("..other");
    }
    return 0;
}

static int8_t tun_address_write(phy_address_type_e arg1, uint8_t *arg2)
{
    (void)arg1;
    (void)arg2;
    return 0;
}

static int8_t tun_extension(phy_extension_type_e arg1, uint8_t *arg2)
{
    (void)arg1;
    (void)arg2;
    return 0;
}
/*
static int linux_interface_up(char *devname)
{
    int sockfd;
    struct ifreq ifr;
    int err;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    if (sockfd < 0) {
        return -1;
    }

    memset(&ifr, 0, sizeof ifr);

    strncpy(ifr.ifr_name, devname, IFNAMSIZ);

    // Read current flags
    if ((err = ioctl(sockfd, SIOCGIFFLAGS, &ifr)) < 0) {
        tr_error("ioctl() SIOCGIFFLAGS:");
        tr_error("failed to read device flags. ioctl returned %d", err);
        goto FAILED;
    }

    // Set interface up
    ifr.ifr_flags |= IFF_UP;
    if ((err = ioctl(sockfd, SIOCSIFFLAGS, &ifr)) < 0) {
        tr_error("ioctl() SIOCSIFFLAGS:");
        tr_error("failed to bring device up. ioctl returned %d", err);
        goto FAILED;
    }

    close(sockfd);
    return 0;
FAILED:
    close(sockfd);
    return -1;
}*/

void unregister_tun_phy(void)
{ /*
    if (device_id  == -1) {
        return;
    }
    pthread_cancel(vtun_thread);
    pthread_join(vtun_thread, NULL);
    close(virtual_device_descriptor);*/
	arm_net_phy_unregister(device_id);
}
