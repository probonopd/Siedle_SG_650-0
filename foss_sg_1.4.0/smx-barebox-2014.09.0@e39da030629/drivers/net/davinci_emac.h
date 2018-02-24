/*
 * Copyright (C) 2007 Sergey Kubushyn <ksi@koi8.net>
 *
 * Based on:
 *
 * ----------------------------------------------------------------------------
 *
 * dm644x_emac.h
 *
 * TI DaVinci (DM644X) EMAC peripheral driver header for DV-EVM
 *
 * Copyright (C) 2005 Texas Instruments.
 *
 * ----------------------------------------------------------------------------
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 *  You should have received a copy of the GNU General Public License
 *  along with this program; if not, write to the Free Software
 *  Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 * ----------------------------------------------------------------------------
 */

#ifndef _DAVINCI_EMAC_H_
#define _DAVINCI_EMAC_H_

/* PHY mask - set only those phy number bits where phy is/can be connected */
#define EMAC_MDIO_PHY_NUM           1
#define EMAC_MDIO_PHY_MASK          (1 << EMAC_MDIO_PHY_NUM)

/* Ethernet Min/Max packet size */
#define EMAC_MIN_ETHERNET_PKT_SIZE	60
#define EMAC_MAX_ETHERNET_PKT_SIZE	1518
#define EMAC_PKT_ALIGN			18	/* 1518 + 18 = 1536 (packet aligned on 32 byte boundry) */

/* Number of RX packet buffers
 * NOTE: Only 1 buffer supported as of now
 */
#define EMAC_MAX_RX_BUFFERS		10

/***********************************************
 ******** Internally used macros ***************
 ***********************************************/

#define EMAC_CH_TX			1
#define EMAC_CH_RX			0

/* Each descriptor occupies 4 words, lets start RX desc's at 0 and
 * reserve space for 64 descriptors max
 */
#define EMAC_RX_DESC_BASE		0x0
#define EMAC_TX_DESC_BASE		0x1000

/* EMAC Teardown value */
#define EMAC_TEARDOWN_VALUE		0xfffffffc

/* MII Status Register */
#define MII_STATUS_REG			1

/* Number of statistics registers */
#define EMAC_NUM_STATS			36

/* EMAC Descriptor Offsets */
#define EMAC_DESC_NEXT			0x0	/* Pointer to next descriptor in chain */
#define EMAC_DESC_BUFFER		0x4	/* Pointer to data buffer */
#define EMAC_DESC_BUFF_OFF_LEN		0x8	/* Buffer Offset(MSW) and Length(LSW) */
#define EMAC_DESC_PKT_FLAG_LEN		0xc	/* Packet Flags(MSW) and Length(LSW) */
#define EMAC_DESC_SIZE			0x10

/* CPPI bit positions */
#define EMAC_CPPI_SOP_BIT		(0x80000000)
#define EMAC_CPPI_EOP_BIT		(0x40000000)
#define EMAC_CPPI_OWNERSHIP_BIT		(0x20000000)
#define EMAC_CPPI_EOQ_BIT		(0x10000000)
#define EMAC_CPPI_TEARDOWN_COMPLETE_BIT	(0x08000000)
#define EMAC_CPPI_PASS_CRC_BIT		(0x04000000)

#define EMAC_CPPI_RX_ERROR_FRAME	(0x03fc0000)

#define EMAC_MACCONTROL_MIIEN_ENABLE		(0x20)
#define EMAC_MACCONTROL_FULLDUPLEX_ENABLE	(0x1)
#define EMAC_MACCONTROL_GIGABIT_ENABLE		(1 << 7)
#define EMAC_MACCONTROL_GIGFORCE		(1 << 17)
#define EMAC_MACCONTROL_RMIISPEED_100		(1 << 15)

#define EMAC_MAC_ADDR_MATCH		(1 << 19)
#define EMAC_MAC_ADDR_IS_VALID		(1 << 20)

#define EMAC_RXMBPENABLE_RXCAFEN_ENABLE	(0x200000)
#define EMAC_RXMBPENABLE_RXBROADEN	(0x2000)

#define MDIO_CONTROL_IDLE		(0x80000000)
#define MDIO_CONTROL_ENABLE		(0x40000000)
#define MDIO_CONTROL_FAULT_ENABLE	(0x40000)
#define MDIO_CONTROL_FAULT		(0x80000)
#define MDIO_USERACCESS0_GO		(0x80000000)
#define MDIO_USERACCESS0_WRITE_READ	(0x0)
#define MDIO_USERACCESS0_WRITE_WRITE	(0x40000000)
#define MDIO_USERACCESS0_ACK		(0x20000000)

/* Ethernet MAC Registers */
#define EMAC_TXIDVER			0x000
#define EMAC_TXCONTROL			0x004
#define EMAC_TXTEARDOWN			0x008
#define EMAC_RXIDVER			0x010
#define EMAC_RXCONTROL			0x014
#define EMAC_RXTEARDOWN			0x018
#define EMAC_TXINTSTATRAW		0x080
#define EMAC_TXINTSTATMASKED		0x084
#define EMAC_TXINTMASKSET		0x088
#define EMAC_TXINTMASKCLEAR		0x08c
#define EMAC_MACINVECTOR		0x090
#define EMAC_MACEOIVECTOR		0x094
#define EMAC_RXINTSTATRAW		0x0a0
#define EMAC_RXINTSTATMASKED		0x0a4
#define EMAC_RXINTMASKSET		0x0a8
#define EMAC_RXINTMASKCLEAR		0x0ac
#define EMAC_MACINTSTATRAW		0x0b0
#define EMAC_MACINTSTATMASKED		0x0b4
#define EMAC_MACINTMASKSET		0x0b8
#define EMAC_MACINTMASKCLEAR		0x0bc
#define EMAC_RXMBPENABLE		0x100
#define EMAC_RXUNICASTSET		0x104
#define EMAC_RXUNICASTCLEAR		0x108
#define EMAC_RXMAXLEN			0x10c
#define EMAC_RXBUFFEROFFSET		0x110
#define EMAC_RXFILTERLOWTHRESH		0x114
#define EMAC_RX0FLOWTHRESH		0x120
#define EMAC_RX1FLOWTHRESH		0x124
#define EMAC_RX2FLOWTHRESH		0x128
#define EMAC_RX3FLOWTHRESH		0x12c
#define EMAC_RX4FLOWTHRESH		0x130
#define EMAC_RX5FLOWTHRESH		0x134
#define EMAC_RX6FLOWTHRESH		0x138
#define EMAC_RX7FLOWTHRESH		0x13c
#define EMAC_RX0FREEBUFFER		0x140
#define EMAC_RX1FREEBUFFER		0x144
#define EMAC_RX2FREEBUFFER		0x148
#define EMAC_RX3FREEBUFFER		0x14c
#define EMAC_RX4FREEBUFFER		0x150
#define EMAC_RX5FREEBUFFER		0x154
#define EMAC_RX6FREEBUFFER		0x158
#define EMAC_RX7FREEBUFFER		0x15c
#define EMAC_MACCONTROL			0x160
#define EMAC_MACSTATUS			0x164
#define EMAC_EMCONTROL			0x168
#define EMAC_FIFOCONTROL		0x16c
#define EMAC_MACCONFIG			0x170
#define EMAC_SOFTRESET			0x174
#define EMAC_MACSRCADDRLO		0x1d0
#define EMAC_MACSRCADDRHI		0x1d4
#define EMAC_MACHASH1			0x1d8
#define EMAC_MACHASH2			0x1dc
#define EMAC_BOFFTEST			0x1e0
#define EMAC_TPACETEST			0x1e4
#define EMAC_RXPAUSE			0x1e8
#define EMAC_TXPAUSE			0x1ec
#define EMAC_RXGOODFRAMES		0x200
#define EMAC_RXBCASTFRAMES		0x204
#define EMAC_RXMCASTFRAMES		0x208
#define EMAC_RXPAUSEFRAMES		0x20c
#define EMAC_RXCRCERRORS		0x210
#define EMAC_RXALIGNCODEERRORS		0x214
#define EMAC_RXOVERSIZED		0x218
#define EMAC_RXJABBER			0x21c
#define EMAC_RXUNDERSIZED		0x220
#define EMAC_RXFRAGMENTS		0x224
#define EMAC_RXFILTERED			0x228
#define EMAC_RXQOSFILTERED		0x22c
#define EMAC_RXOCTETS			0x230
#define EMAC_TXGOODFRAMES		0x234
#define EMAC_TXBCASTFRAMES		0x238
#define EMAC_TXMCASTFRAMES		0x23c
#define EMAC_TXPAUSEFRAMES		0x240
#define EMAC_TXDEFERRED			0x244
#define EMAC_TXCOLLISION		0x248
#define EMAC_TXSINGLECOLL		0x24c
#define EMAC_TXMULTICOLL		0x250
#define EMAC_TXEXCESSIVECOLL		0x254
#define EMAC_TXLATECOLL			0x258
#define EMAC_TXUNDERRUN			0x25c
#define EMAC_TXCARRIERSENSE		0x260
#define EMAC_TXOCTETS			0x264
#define EMAC_FRAME64			0x268
#define EMAC_FRAME65T127		0x26c
#define EMAC_FRAME128T255		0x270
#define EMAC_FRAME256T511		0x274
#define EMAC_FRAME512T1023		0x278
#define EMAC_FRAME1024TUP		0x27c
#define EMAC_NETOCTETS			0x280
#define EMAC_RXSOFOVERRUNS		0x284
#define EMAC_RXMOFOVERRUNS		0x288
#define EMAC_RXDMAOVERRUNS		0x28c
#define EMAC_MACADDRLO			0x500
#define EMAC_MACADDRHI			0x504
#define EMAC_MACINDEX			0x508
#define EMAC_TX0HDP			0x600
#define EMAC_TX1HDP			0x604
#define EMAC_TX2HDP			0x608
#define EMAC_TX3HDP			0x60c
#define EMAC_TX4HDP			0x610
#define EMAC_TX5HDP			0x614
#define EMAC_TX6HDP			0x618
#define EMAC_TX7HDP			0x61c
#define EMAC_RX0HDP			0x620
#define EMAC_RX1HDP			0x624
#define EMAC_RX2HDP			0x628
#define EMAC_RX3HDP			0x62c
#define EMAC_RX4HDP			0x630
#define EMAC_RX5HDP			0x634
#define EMAC_RX6HDP			0x638
#define EMAC_RX7HDP			0x63c
#define EMAC_TX0CP			0x640
#define EMAC_TX1CP			0x644
#define EMAC_TX2CP			0x648
#define EMAC_TX3CP			0x64c
#define EMAC_TX4CP			0x650
#define EMAC_TX5CP			0x654
#define EMAC_TX6CP			0x658
#define EMAC_TX7CP			0x65c
#define EMAC_RX0CP			0x660
#define EMAC_RX1CP			0x664
#define EMAC_RX2CP			0x668
#define EMAC_RX3CP			0x66c
#define EMAC_RX4CP			0x670
#define EMAC_RX5CP			0x674
#define EMAC_RX6CP			0x678
#define EMAC_RX7CP			0x67c

/* EMAC Wrapper Registers */
#define EMAC_EWRAP_IDVER		0x00
#define EMAC_EWRAP_SOFTRESET		0x04
#define EMAC_EWRAP_INTCTRL		0x0c
#define EMAC_EWRAP_C0RXTHRESHEN		0x10
#define EMAC_EWRAP_C0RXEN		0x14
#define EMAC_EWRAP_C0TXEN		0x18
#define EMAC_EWRAP_C0MISCEN		0x1c
#define EMAC_EWRAP_C1RXTHRESHEN		0x20
#define EMAC_EWRAP_C1RXEN		0x24
#define EMAC_EWRAP_C1TXEN		0x28
#define EMAC_EWRAP_C1MISCEN		0x2c
#define EMAC_EWRAP_C2RXTHRESHEN		0x30
#define EMAC_EWRAP_C2RXEN		0x34
#define EMAC_EWRAP_C2TXEN		0x38
#define EMAC_EWRAP_C2MISCEN		0x3c
#define EMAC_EWRAP_C0RXTHRESHSTAT	0x40
#define EMAC_EWRAP_C0RXSTAT		0x44
#define EMAC_EWRAP_C0TXSTAT		0x48
#define EMAC_EWRAP_C0MISCSTAT		0x4c
#define EMAC_EWRAP_C1RXTHRESHSTAT	0x50
#define EMAC_EWRAP_C1RXSTAT		0x54
#define EMAC_EWRAP_C1TXSTAT		0x58
#define EMAC_EWRAP_C1MISCSTAT		0x5c
#define EMAC_EWRAP_C2RXTHRESHSTAT	0x60
#define EMAC_EWRAP_C2RXSTAT		0x64
#define EMAC_EWRAP_C2TXSTAT		0x68
#define EMAC_EWRAP_C2MISCSTAT		0x6c
#define EMAC_EWRAP_C0RXIMAX		0x70
#define EMAC_EWRAP_C0TXIMAX		0x74
#define EMAC_EWRAP_C1RXIMAX		0x78
#define EMAC_EWRAP_C1TXIMAX		0x7c
#define EMAC_EWRAP_C2RXIMAX		0x80
#define EMAC_EWRAP_C2TXIMAX		0x84

/* EMAC MDIO Registers */
#define EMAC_MDIO_VERSION		0x00
#define EMAC_MDIO_CONTROL		0x04
#define EMAC_MDIO_ALIVE			0x08
#define EMAC_MDIO_LINK			0x0c
#define EMAC_MDIO_LINKINTRAW		0x10
#define EMAC_MDIO_LINKINTMASKED		0x14
#define EMAC_MDIO_USERINTRAW		0x20
#define EMAC_MDIO_USERINTMASKED		0x24
#define EMAC_MDIO_USERINTMASKSET	0x28
#define EMAC_MDIO_USERINTMASKCLEAR	0x2c
#define EMAC_MDIO_USERACCESS0		0x80
#define EMAC_MDIO_USERPHYSEL0		0x84
#define EMAC_MDIO_USERACCESS1		0x88
#define EMAC_MDIO_USERPHYSEL1		0x8c

#endif  /* _DAVINCI_EMAC_H_ */
