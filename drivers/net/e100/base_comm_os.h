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

/*****************************************************************************
******************************************************************************
**                                                                          **
** INTEL CORPORATION                                                        **
**                                                                          **
** This software is supplied under the terms of the license included        **
** above.  All use of this software must be in accordance with the terms    **
** of that license.                                                         **
**                                                                          **
** THIS FILE IS USED WHEN CREATING ians_core.o. HENCE, IT SHOULD NOT BE     **
** MODIFIED!                                                                **
**                                                                          **
**  Module Name:    base_comm_os.h                                          **
**                                                                          **
**  Abstract:       This header file defines the os dependent communications**
**                  between the iANS module and the base drivers bound      **
**                  below it.                                               **
**                                                                          **
**  Environment:    Kernel Mode (linux 2.2.x, 2.4.x)                        **
**                                                                          **
**                  Written using Tab Size = 4, Indent Size = 4             **
**                                                                          **
**  Revision History:                                                       **
**                                                                          **
******************************************************************************
*****************************************************************************/

#ifndef _BASE_COMM_OS_H
#define _BASE_COMM_OS_H

#include <linux/sockios.h> // for SIOCDEVPRIVATE
#include "base_comm.h"

/*--------------------------------------------------------------------*
 | PRIMITIVES baring iANS communications
 | =====================================
 *--------------------------------------------------------------------*/

// The proprietary iANS IOCTL code
#define IANS_BASE_SIOC    (SIOCDEVPRIVATE+1)

// The proprietary event notifications code
#define IANS_BASE_NOTIFY  (('S' << 24)|('N' << 16)|('A' << 8)|('i')) // "iANS"

/*==========================================================================*
*                                                                           *
* Per-Message Attributes                                                    *
*                                                                           *
*==========================================================================*/

// Turn into a legal pointer
#if defined(__i386__)
 #define CelingAlignPtr(p)  (p)
#else
 #define CelingAlignPtr(p)  (p)
#endif

// The attribute header is kept at the beginning of the allocated buffer
#define iANSGetReceiveAttributeHeader(skb) \
            ((IANS_ATTR_HEADER*) CelingAlignPtr((char*)((skb)->head)))

#define iANSGetTransmitAttributeHeader(skb) \
            ((IANS_ATTR_HEADER*) CelingAlignPtr((char*)((skb)->cb)))

#endif //_BASE_COMM_OS_H
