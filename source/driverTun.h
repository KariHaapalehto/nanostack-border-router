/*
 * Copyright (c) 2015-2015, ARM Limited, All Rights Reserved
 */

#ifndef DRIVERTUN_H
#define DRIVERTUN_H

/**
 * Register TUN PHY interface to Nanostack.
 * This function creates Linux TUN interface and sets it up and then register it with Nanostack as PHY interface.
 * \param name Interface name that is used with kernel device.
 * \return device_id, or less than zero, if failure occurs.
 */
int8_t register_tun_phy(void);
void unregister_tun_phy(void);

#endif /* DRIVERTUN_H */
