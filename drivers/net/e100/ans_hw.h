/*******************************************************************************

This software program is available to you under a choice of one of two 
licenses. You may choose to be licensed under either the GNU General Public 
License 2.0, June 1991, available at http://www.fsf.org/copyleft/gpl.html, 
or the Intel BSD + Patent License, the text of which follows:

Recipient has requested a license and Intel Corporation ("Intel") is willing
to grant a license for the software entitled Linux Base Driver for the 
Intel(R) PRO/100 Family of Adapters (e100) (the "Software") being provided 
by Intel Corporation. The following definitions apply to this license:

"Licensed Patents" means patent claims licensable by Intel Corporation which 
are necessarily infringed by the use of sale of the Software alone or when 
combined with the operating system referred to below.

"Recipient" means the party to whom Intel delivers this Software.

"Licensee" means Recipient and those third parties that receive a license to 
any operating system available under the GNU General Public License 2.0 or 
later.

Copyright (c) 1999 - 2002 Intel Corporation.
All rights reserved.

The license is provided to Recipient and Recipient's Licensees under the 
following terms.

Redistribution and use in source and binary forms of the Software, with or 
without modification, are permitted provided that the following conditions 
are met:

Redistributions of source code of the Software may retain the above 
copyright notice, this list of conditions and the following disclaimer.

Redistributions in binary form of the Software may reproduce the above 
copyright notice, this list of conditions and the following disclaimer in 
the documentation and/or materials provided with the distribution.

Neither the name of Intel Corporation nor the names of its contributors 
shall be used to endorse or promote products derived from this Software 
without specific prior written permission.

Intel hereby grants Recipient and Licensees a non-exclusive, worldwide, 
royalty-free patent license under Licensed Patents to make, use, sell, offer 
to sell, import and otherwise transfer the Software, if any, in source code 
and object code form. This license shall include changes to the Software 
that are error corrections or other minor changes to the Software that do 
not add functionality or features when the Software is incorporated in any 
version of an operating system that has been distributed under the GNU 
General Public License 2.0 or later. This patent license shall apply to the 
combination of the Software and any operating system licensed under the GNU 
General Public License 2.0 or later if, at the time Intel provides the 
Software to Recipient, such addition of the Software to the then publicly 
available versions of such operating systems available under the GNU General 
Public License 2.0 or later (whether in gold, beta or alpha form) causes 
such combination to be covered by the Licensed Patents. The patent license 
shall not apply to any other combinations which include the Software. NO 
hardware per se is licensed hereunder.

THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" 
AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE 
IMPLIED WARRANTIES OF MECHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE 
ARE DISCLAIMED. IN NO EVENT SHALL INTEL OR IT CONTRIBUTORS BE LIABLE FOR ANY 
DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES 
(INCLUDING, BUT NOT LIMITED, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; 
ANY LOSS OF USE; DATA, OR PROFITS; OR BUSINESS INTERUPTION) HOWEVER CAUSED 
AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY OR 
TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE 
OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
*******************************************************************************/

/**********************************************************************
*                                                                     *
* INTEL CORPORATION                                                   *
*                                                                     *
* This software is supplied under the terms of the license included   *
* above.  All use of this driver must be in accordance with the terms *
* of that license.                                                    *
*                                                                     *
* Module Name:  ans_hw.h                                              *
*                                                                     *
* Abstract: these are the hardware specific (not OS specific)         *
*       routines needed by the bd_ans module                          *
*                                                                     *
* Environment:                                                        *
*                                                                     *
**********************************************************************/

#ifndef _ANS_HW__H_
#define _ANS_HW__H_

#define BASE_10T        1
#define BASE_100T4      2
#define BASE_100TX      4
#define BASE_100FX      8

#define I82559_REV_ID                    8
#define I82558_REV_ID                                    4
#define EEPROM_COMPATIBILITY_FLAGS      3
#define EEPROM_COMPATIBILITY_BIT_4          0x0004
#define EEPROM_COMPATIBILITY_BIT_2          0x0002
#define IPCB_INSERT_VLAN_ENABLE         0x02

#define GAMLA_REV_ID                    12
#define D101B_REV_ID                    5

/* bit definitions of 8255x header */
typedef struct _cb_header_status_word {
    volatile unsigned status:12;
    volatile unsigned underrun:1;
    volatile unsigned ok:1;
    volatile unsigned pad:1;
    volatile unsigned c:1;
} cb_header_status_word;

/* definition of an IPCB - only the parts that I care about */
typedef struct _ipcb_bits {
    volatile unsigned ipcb_scheduling:20;
    volatile unsigned ipcb_activation:12;
    volatile UINT16 vlanid;
    volatile UINT8 ip_header_offset;
    volatile UINT8 tcp_header_offset;
} ipcb_bits;

#define HIGH_BYTE(word) ((UINT8)(word >> 8))
#define BD_ANS_HW_FLAGS(bps) \
            bd_ans_hw_flags(bps, BD_ANS_DRV_REVID(bps));
#define BD_ANS_HW_AVAILABLE_SPEEDS(bps) bd_ans_hw_AvailableSpeeds(BD_ANS_DRV_PHY_ID(bps))

/* function prototypes */
extern UINT32 bd_ans_hw_AvailableSpeeds(UINT32 phy);
extern UINT32 bd_ans_hw_flags(BOARD_PRIVATE_STRUCT *bps, UINT16 revid);
extern u16 e100_eeprom_read(struct e100_private *bdp, u16 reg);

#ifdef IANS_BASE_VLAN_TAGGING
void bd_ans_hw_ConfigEnableTagging(UINT8 *ConfigBytes, UINT16 rev_id);
#endif

#endif /* _ANS_HW__H_ */
