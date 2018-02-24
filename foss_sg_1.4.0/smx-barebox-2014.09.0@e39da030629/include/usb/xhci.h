/*
 * xHCI host controller driver
 *
 * Sebastian Hesselbarth <sebastian.hesselbarth@gmail.com>
 *
 * Some code borrowed from the Linux xHCI driver
 *   Author: Sarah Sharp
 *   Copyright (C) 2008 Intel Corp.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of MERCHANTABILITY
 * or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License
 * for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software Foundation,
 * Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 */

#ifndef __XHCI_HCD_H
#define __XHCI_HCD_H

struct xhci_data {
	void __iomem *regs;
};

int xhci_register(struct device_d *dev, struct xhci_data *data);

#endif
