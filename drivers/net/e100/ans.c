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
* Module Name:  ans.c                                                 *
*                                                                     *
* Abstract:                                                           *
*                                                                     *
* Environment:  This file is intended to be shared among Linux and    *
*               Unixware operating systems.                           *
*                                                                     *
**********************************************************************/

//#define _IANS_MAIN_MODULE_C_

#include "ans_driver.h"

/* I meant to make these inline, but don't have time to figure out
** compiler problems right now...
*/
BD_ANS_BOOLEAN BD_ANS_BCMP(UCHAR *s1, UCHAR *s2, UINT32 length)
{
    while (length) {
        if (*s1 != *s2)
            return BD_ANS_FALSE;
        length--; s1++; s2++;
    }
    return BD_ANS_TRUE;
}
 
VOID BD_ANS_BCOPY(UCHAR *destination, UCHAR *source, UINT32 length) 
{
    while (length--) {
        *destination++ = *source++;
    }
}
 
VOID BD_ANS_BZERO(UCHAR *s, UINT32 length)
{
    while (length--)
        *s++ = 0;
}                  

/* bd_ans_ProcessRequest()
**
**  This routine is called if iANS has issued a command to the driver through
**  the driver's private ioctl routine.  It will parse the header for the 
**  opcode of the command to execute, and call the appropriate functions.
**
**  Arguments:  BOARD_PRIVATE_STRUCT *bps - a pointer to the adapters hw 
**                                          specific data structure.
**              iANSsupport_t *iANSdata   - pointer to the iANS required
**                                          support structure
**              IANS_BD_PARAM_HEADER *header - a pointer to the start of the
**                                             ans command.
**              
**
**  Returns:    BD_ANS_STATUS -  SUCCESS if command is recognized and was 
**                            processed successfully, FAILURE otherwise.
*/ 
BD_ANS_STATUS bd_ans_ProcessRequest(BOARD_PRIVATE_STRUCT *bps, 
                                    iANSsupport_t *iANSdata,
                                    IANS_BD_PARAM_HEADER *header)
{
    
    DEBUGLOG("bd_ans_ProcessRequest: enter\n");

    /* Only allow IANS_OP_BD_IDENTIFY if iANS comm is down */
    if((iANSdata->iANS_status == IANS_COMMUNICATION_DOWN) && 
       (header->Opcode!=IANS_OP_BD_IDENTIFY)) {
        DEBUGLOG("bd_ans_ProcessRequest: ANS communication not up\n");
        return BD_ANS_FAILURE;
    }
    
    switch (header->Opcode) {
    case IANS_OP_BD_IDENTIFY:
        DEBUGLOG("bd_ans_ProcessRequest: Identify request\n");
        return (bd_ans_Identify(bps, iANSdata, header));
      
    case IANS_OP_BD_DISCONNECT:
        DEBUGLOG("bd_ans_ProcessRequest: Disconnect request\n");
        return (bd_ans_Disconnect(bps, iANSdata, header));
      
    case IANS_OP_EXT_GET_CAPABILITY:
        DEBUGLOG("bd_ans_ProcessRequeest: Ext Get Capabilities request\n");
        return (bd_ans_ExtendedGetCapability(bps, iANSdata, header));
      
    case IANS_OP_EXT_SET_MODE:
        DEBUGLOG("bd_ans_ProcessRequest: Ext Set mode request\n");
        return (bd_ans_ExtendedSetMode(bps, iANSdata, header));
      
    case IANS_OP_EXT_GET_STATUS:
        DEBUGLOG("bd_ans_ProcessRequest: Ext Get Status request\n");
        return (bd_ans_ExtendedGetStatus(bps, iANSdata, header));
#ifdef IANS_BASE_VLAN_TAGGING
    case IANS_OP_ITAG_GET_CAPABILITY:
        DEBUGLOG("bd_ans_ProcessRequest: get itag capability request\n");
        return (bd_ans_TagGetCapability(bps, iANSdata, header));
      
    case IANS_OP_ITAG_SET_MODE:
        DEBUGLOG("bd_ans_ProcessRequest: itag set mode request\n");
        return (bd_ans_TagSetMode(bps, iANSdata, header));
#endif
#ifdef IANS_BASE_VLAN_ID        
    case IANS_OP_IVLAN_ID_GET_CAPABILITY:
        DEBUGLOG("bd_ans_ProcessRequest: get vlan capability request\n");
        return (bd_ans_VlanGetCapability(bps, iANSdata, header));
      
    case IANS_OP_IVLAN_ID_SET_MODE:
        DEBUGLOG("bd_ans_ProcessRequest: vlan set mode request\n");
        return (bd_ans_VlanSetMode(bps, iANSdata, header));
      
    case IANS_OP_IVLAN_ID_SET_TABLE:
        DEBUGLOG("bd_ans_ProcessRequest: vlan set table request\n");
        return (bd_ans_VlanSetTable(bps, iANSdata, header));
#endif       
    default:
        return (bd_ans_os_ProcessRequest(bps, iANSdata, header));
    } 
}
                                    
/* bd_ans_Identify()
**
**  This routine will identify the base driver to the ANS module by filling out
**  the required structure.
**
**  Arguments:  BOARD_PRIVATE_STRUCT *bps - a pointer to the adapters hw 
**                                          specific data structure.
**              iANSsupport_t *iANSdata   - pointer to the iANS required
**                                          support structure
**              IANS_BD_PARAM_HEADER *header - a pointer to the start of the
**                                             ans command.
**
**  Returns:    BD_ANS_STATUS -  SUCCESS if command was processed 
**                               successfully, FAILURE otherwise.
*/
BD_ANS_STATUS bd_ans_Identify(BOARD_PRIVATE_STRUCT *bps,
                              iANSsupport_t *iANSdata,
                              IANS_BD_PARAM_HEADER *header)

{
    IANS_BD_PARAM_IDENTIFY *iANSidentify;
    UINT32 BDCommVersion;               /* base driver communication version */

    /* Get our comm version from the #defines */
    BDCommVersion = (IANS_BD_COMM_VERSION_MAJOR << 16) +  IANS_BD_COMM_VERSION_MINOR;
    iANSidentify = (IANS_BD_PARAM_IDENTIFY *)header;

    /* if copyright string doesnt match or iANS comm version is older return error */
    if ((iANSidentify->iANSCommVersion < BDCommVersion)||
        (BD_ANS_BCMP(iANSidentify->iANSSignature,
                     (UCHAR *)IntelCopyrightString,
                     IANS_SIGNATURE_LENGTH)) != BD_ANS_TRUE) {
        return BD_ANS_FAILURE;
    }
   
    BD_ANS_DRV_LOCK;
        
    /* else set communication to iANS as up */
    iANSdata->iANS_status = IANS_COMMUNICATION_UP;
    iANSidentify->BDCommVersion = BDCommVersion;
    BD_ANS_BCOPY(iANSidentify->BDSignature,
                 (UCHAR *)IntelCopyrightString,
                 IANS_SIGNATURE_LENGTH);

    /* initialize the iANSsupport_t strucutre */
    /* at this point, this is the only place where we initialize the
     * support flags for the driver.  In may be that we should do this 
     * someplace else as well - for example, if we ever support hot-add
     * then the capabilities may change dynamically, in which case
     * we will need to call GetAllCapabilities again
     */
    return (bd_ans_GetAllCapabilities(bps, iANSdata));    
}                               

/* bd_ans_Init()
**
**  This function initializes the communication flags.  It should be called
**  at init time by the driver to initialize this part of the iANSsupport_t 
**  structure.
**
**  Arguments:  iANSsupport_t *iANSdata - the ans related data
**
**  Returns:    void
*/
VOID
bd_ans_Init(iANSsupport_t *iANSdata)
{
    /* set all the communication flags to initial values */
    iANSdata->iANS_status = IANS_COMMUNICATION_DOWN;
#ifdef IANS_BASE_VLAN_TAGGING
    iANSdata->vlan_mode = IANS_VLAN_MODE_OFF;
    iANSdata->vlan_filtering_mode = IANS_VLAN_FILTERING_OFF;
    iANSdata->num_vlan_filter = 0;
    iANSdata->tag_mode = IANS_BD_TAGGING_NONE;
#endif
    iANSdata->reporting_mode = IANS_STATUS_REPORTING_OFF;
    iANSdata->timer_id = 0;
    iANSdata->attributed_mode = BD_ANS_FALSE;
    iANSdata->routing_mode = IANS_ROUTING_OFF;
}

/* bd_ans_Disconnect()
**
**  This request is sent by ANS when the ANS module is unloading or will
**  no longer be bound to this particular board.  
**
**  Arguments:  BOARD_PRIVATE_STRUCT *bps - a pointer to the adapters hw 
**                                          specific data structure.
**              iANSsupport_t *iANSdata   - pointer to the iANS required
**                                          support structure
**              IANS_BD_PARAM_HEADER *header - a pointer to the start of the
**                                             ans command.
**
**  Returns:    BD_ANS_STATUS -  SUCCESS if command was processed 
**                               successfully, FAILURE otherwise.
*/
BD_ANS_STATUS bd_ans_Disconnect(BOARD_PRIVATE_STRUCT *bps,
                                iANSsupport_t *iANSdata,
                                IANS_BD_PARAM_HEADER *header)
{
    if (iANSdata->reporting_mode == IANS_STATUS_REPORTING_ON)
        bd_ans_DeActivateFastPolling(bps, iANSdata);

    BD_ANS_DRV_UNLOCK;

    return (bd_ans_ResetAllModes(bps, iANSdata));
}
                                 
/* bd_ans_ExtendedGetCapability()
**
**  This function will fill out the structure required for the extended
**  capabilities query.  
**
**  Arguments:  BOARD_PRIVATE_STRUCT *bps - a pointer to the adapters hw 
**                                          specific data structure.
**              iANSsupport_t *iANSdata   - pointer to the iANS required
**                                          support structure
**              IANS_BD_PARAM_HEADER *header - a pointer to the start of the
**                                             ans command.
**
**  Returns:    BD_ANS_STATUS -  SUCCESS if command was processed 
**                               successfully, FAILURE otherwise.
*/
BD_ANS_STATUS bd_ans_ExtendedGetCapability(BOARD_PRIVATE_STRUCT *bps,
                                           iANSsupport_t *iANSdata,
                                           IANS_BD_PARAM_HEADER *header)
{   
    bd_ans_os_ExtendedGetCapability(bps, iANSdata, header);

    /* Report that base driver supports setting of MAC address */
    ((IANS_BD_PARAM_EXT_CAP *)header)->BDCanSetMacAddress = 
        ANS_BD_SUPPORTS(iANSdata->can_set_mac_addr);
        
    /* Report supported version of the status reporting structure */
    ((IANS_BD_PARAM_EXT_CAP *)header)->BDIansStatusVersion = 
                IANS_STATUS_VERSION;
    ((IANS_BD_PARAM_EXT_CAP *)header)->BDAllAvailableSpeeds = 
        iANSdata->available_speeds;
    
    return BD_ANS_SUCCESS;        
}

/* bd_ans_ExtendedSetMode()
**
**  This request is sent by ANS to enable either tx/rx of tlv's with
**  packet data, or to enable routing of all rx packets to ANS.  
**
**  Arguments:  BOARD_PRIVATE_STRUCT *bps - a pointer to the adapters hw 
**                                          specific data structure.
**              iANSsupport_t *iANSdata   - pointer to the iANS required
**                                          support structure
**              IANS_BD_PARAM_HEADER *header - a pointer to the start of the
**                                             ans command.
**
**  Returns:    BD_ANS_STATUS -  SUCCESS if command was processed 
**                               successfully, FAILURE otherwise.
*/
BD_ANS_STATUS bd_ans_ExtendedSetMode(BOARD_PRIVATE_STRUCT *bps,
                                     iANSsupport_t *iANSdata,
                                     IANS_BD_PARAM_HEADER *header)
{
    BD_ANS_STATUS status;
    IANS_BD_PARAM_EXT_SET_MODE *request = (IANS_BD_PARAM_EXT_SET_MODE *)header;
    
    /* this function call will enable/disable fast polling mode
     * if it was requested here 
     */
    status = bd_ans_SetReportingMode(bps, iANSdata, header);

    /* see if we are being configured to tx/rx tlv */
    if (request->BDIansAttributedMode == IANS_REQUEST_SUPPORT)
        iANSdata->attributed_mode = (UINT32) BD_ANS_TRUE;
    else
        iANSdata->attributed_mode = (UINT32) BD_ANS_FALSE;

    /* see if we are being requested to send packets to the
     * ANS protocol
     */  
    bd_ans_os_ExtendedSetMode(bps, iANSdata, header);
    return (status);
}
                                      
/* bd_ans_ExtendedStopPromiscuousMode()
**
**  This function will make the driver stop sending in promiscuous mode
**  if it is requested by ANS.  It is only required for OSs which don't
**  provide a native mechanism to do this (UnixWare).  
**
**  Arguments:  BOARD_PRIVATE_STRUCT *bps - a pointer to the adapters hw 
**                                          specific data structure.
**              iANSsupport_t *iANSdata   - pointer to the iANS required
**                                          support structure
**
**  Returns:    BD_ANS_STATUS -  SUCCESS if command was processed 
**                               successfully, FAILURE otherwise.
*/
BD_ANS_STATUS bd_ans_ExtendedStopPromiscuousMode(BOARD_PRIVATE_STRUCT *bps,
                                                 iANSsupport_t *iANSdata)
{
    /* if we don't support this, then just leave now. */
    if (iANSdata->supports_stop_promiscuous == BD_ANS_FALSE)
        return BD_ANS_FAILURE;
        
    if (bd_ans_drv_StopPromiscuousMode(bps))
        return (BD_ANS_FAILURE);
    return (BD_ANS_SUCCESS);
}                                                  

/* bd_ans_ExtendedGetStatus()
**
**  This function is called as part of an IOCTL request by ANS to get the 
**  current status of the driver.  
**
**  Arguments:  BOARD_PRIVATE_STRUCT *bps - a pointer to the adapters hw 
**                                          specific data structure.
**              iANSsupport_t *iANSdata   - pointer to the iANS required
**                                          support structure
**              IANS_BD_PARAM_HEADER *header - a pointer to the start of the
**                                             ans command.
**
**  Returns:    BD_ANS_STATUS -  SUCCESS if command was processed 
**                               successfully, FAILURE otherwise.
*/
BD_ANS_STATUS bd_ans_ExtendedGetStatus(BOARD_PRIVATE_STRUCT *bps,
                                       iANSsupport_t *iANSdata,
                                       IANS_BD_PARAM_HEADER *header)
{
        DEBUGLOG("bd_ans_ExtendedGetStatus: enter\n");

    /* make sure the driver's status fields are current */
    bd_ans_drv_UpdateStatus(bps);    
    
    /* fill out the required status structure with the updated status */
    return bd_ans_FillStatus(bps,
            iANSdata,
            (void *)&(((IANS_BD_IOC_PARAM_STATUS *)header)->Status));
}
                                       
#ifdef IANS_BASE_VLAN_TAGGING                                       
/* bd_ans_TagGetCapability()
**
**  This function is called as part of an IOCTL request by ANS to get the
**  tagging capabilities of the driver.  
**
**  Arguments:  BOARD_PRIVATE_STRUCT *bps - a pointer to the adapters hw 
**                                          specific data structure.
**              iANSsupport_t *iANSdata   - pointer to the iANS required
**                                          support structure
**              IANS_BD_PARAM_HEADER *header - a pointer to the start of the
**                                             ans command.
**
**  Returns:    BD_ANS_STATUS -  SUCCESS if command was processed 
**                               successfully, FAILURE otherwise.
*/
BD_ANS_STATUS bd_ans_TagGetCapability(BOARD_PRIVATE_STRUCT *bps,
                                      iANSsupport_t *iANSdata,
                                      IANS_BD_PARAM_HEADER *header)
{
    /* these fields in the iANSdata structure have all been
     * initialized by GetAllCapabilities during the Identify 
     * request.
     */
    ((IANS_BD_PARAM_ITAG_CAP *)header)->ISLTagMode = 
        ANS_BD_SUPPORTS(iANSdata->ISL_tag_support);
    ((IANS_BD_PARAM_ITAG_CAP *)header)->IEEE802_3acTagMode = 
        ANS_BD_SUPPORTS(iANSdata->IEEE_tag_support);
    
    return (BD_ANS_SUCCESS);
}

/* bd_ans_TagSetMode()
**
**  This function is called as part of an IOCTL request by ANS to 
**  enable/disable tagging on the adapter.
**
**  Arguments:  BOARD_PRIVATE_STRUCT *bps - a pointer to the adapters hw
**                                          specific data structure.
**              iANSsupport_t *iANSdata   - pointer to the iANS required
**                                          support structure
**              IANS_BD_PARAM_HEADER *header - a pointer to the start of the
**                                             ans command.
**
**  Returns:    BD_ANS_STATUS -  SUCCESS if command was processed
**                               successfully, FAILURE otherwise.
*/
BD_ANS_STATUS bd_ans_TagSetMode(BOARD_PRIVATE_STRUCT *bps,
                                iANSsupport_t *iANSdata,
                                IANS_BD_PARAM_HEADER *header)
{
    DEBUGLOG("bd_ans_TagSetMode: enter\n");

    /* filter off requests for unsupported modes */ 
    switch(((IANS_BD_PARAM_ITAG_SET_MODE *)header)->SetTagMode){
    case IANS_BD_TAGGING_ISL:
        DEBUGLOG("bd_ans_TagSetMode: ISL support requested\n");
        if (iANSdata->ISL_tag_support == BD_ANS_FALSE)
            return BD_ANS_FAILURE;
        break;
        
    case IANS_BD_TAGGING_802_3AC:
        DEBUGLOG("bd_ans_TagSetMode: IEEE support requested\n");
        if (iANSdata->IEEE_tag_support == BD_ANS_FALSE)
            return BD_ANS_FAILURE;
        break;
        
    case IANS_BD_TAGGING_NONE:
        DEBUGLOG("bd_ans_TagSetMode: UNTAGGED mode requested\n");
        /* it's ok to break here and not just return, because
         * if we were previously in tagging mode, this is
         * essentially telling the driver that we no longer
         * want to be in tagging mode.  In this case, we do need
         * to call the ConfigureTagging function to make sure
         * that the driver disables tagging on the adapter.
         */
        break;
        
    default:
        DEBUGLOG("bd_ans_TagSetMode: Failed\n");
        return BD_ANS_FAILURE;
        break;
    }
    /* configure driver/hw to run in supported mode */
    iANSdata->tag_mode = ((IANS_BD_PARAM_ITAG_SET_MODE *)header)->SetTagMode; 
    return (bd_ans_drv_ConfigureTagging(bps));
}
#endif                                      
                                        
#ifdef IANS_BASE_VLAN_ID
/* bd_ans_VlanGetCapability()
**
**  This function gets the VLAN capabilities of the driver and is called
**  as part of an IOCTL query by ANS.  
**
**  Arguments:  BOARD_PRIVATE_STRUCT *bps - a pointer to the adapters hw 
**                                          specific data structure.
**              iANSsupport_t *iANSdata   - pointer to the iANS required
**                                          support structure
**              IANS_BD_PARAM_HEADER *header - a pointer to the start of the
**                                             ans command.
**
**  Returns:    BD_ANS_STATUS -  SUCCESS if command was processed 
**                               successfully, FAILURE otherwise.
*/
BD_ANS_STATUS bd_ans_VlanGetCapability(BOARD_PRIVATE_STRUCT *bps,
                                       iANSsupport_t *iANSdata,
                                       IANS_BD_PARAM_HEADER *header)
{
    /* these support fields in the iANSdata were all initialized
     * in GetAllCapabilities during the Identify query
     */
    ((IANS_BD_PARAM_IVLAN_CAP *)header)->VlanIDCapable = 
        ANS_BD_SUPPORTS(iANSdata->vlan_support);
    ((IANS_BD_PARAM_IVLAN_CAP *)header)->VlanIDFilteringAble = 
        ANS_BD_SUPPORTS(iANSdata->vlan_filtering_support);
    ((IANS_BD_PARAM_IVLAN_CAP *)header)->MaxVlanIDSupported =
        iANSdata->max_vlan_ID_supported;
    ((IANS_BD_PARAM_IVLAN_CAP *)header)->MaxVlanTableSize =
        iANSdata->vlan_table_size;
    
    return (BD_ANS_SUCCESS);
}

/* bd_ans_VlanSetMode()
**
**  This function is called as part of an ANS ioctl to request that 
**  the driver configure itself to run in vlan mode.  
**
**  Arguments:  BOARD_PRIVATE_STRUCT *bps - a pointer to the adapters hw 
**                                          specific data structure.
**              iANSsupport_t *iANSdata   - pointer to the iANS required
**                                          support structure
**              IANS_BD_PARAM_HEADER *header - a pointer to the start of the
**                                             ans command.
**
**  Returns:    BD_ANS_STATUS -  SUCCESS if command was processed 
**                               successfully, FAILURE otherwise.
*/
BD_ANS_STATUS bd_ans_VlanSetMode(BOARD_PRIVATE_STRUCT *bps,
                                 iANSsupport_t *iANSdata,
                                 IANS_BD_PARAM_HEADER *header)
{
    int i;
    IANS_BD_PARAM_IVLAN_SET_MODE *request = 
        (IANS_BD_PARAM_IVLAN_SET_MODE *)header;

    DEBUGLOG("bd_ans_VlanSetMode: enter\n");

    /* all the support flags were initialized in GetAllCapabilities 
     * as part of the Identify call
     */
    
    /* check to see if we are requested to enable/disable vlan mode */
    if (iANSdata->vlan_support == BD_ANS_FALSE) {
        DEBUGLOG("bd_ans_VlanSetMode: driver does NOT support vlan\n");
        return BD_ANS_FAILURE;
    }
    if (request->VlanIDRequest == IANS_REQUEST_SUPPORT)
        iANSdata->vlan_mode = IANS_VLAN_MODE_ON;
    else if (request->VlanIDRequest == IANS_DONT_SUPPORT)
        iANSdata->vlan_mode = IANS_VLAN_MODE_OFF;
    
    /* check to see if we are being requested to do some hw filtering
     * of vlan ids.
     */
    if(request->VlanIDFilteringRequest == IANS_REQUEST_SUPPORT) {
        if (iANSdata->vlan_filtering_support == BD_ANS_FALSE) {
            DEBUGLOG("bd_ans_VlanSetMode: driver does NOT support vlan filter\n");
            return BD_ANS_FAILURE;
        } else {
            iANSdata->vlan_filtering_mode = IANS_VLAN_FILTERING_ON;         
            /* initialize the vlan table structures */
            iANSdata->num_vlan_filter = 0;
            for (i = 0; i < MAX_NUM_VLAN; i++) 
                iANSdata->VlanID[i] = 0;
        }
    }    
    else 
        iANSdata->vlan_filtering_mode = IANS_VLAN_FILTERING_OFF;
        
    /* don't assume that we don't need to reconfigure the adapter here.
     * we also don't want to assume that the driver configures itself
     * for vlan mode the same way it configures itself for tagging mode.
     * (although chances are it does).
     */
    DEBUGLOG1("bd_ans_VlanSetMode:: vlan mode = %d\n", iANSdata->vlan_mode);
    return (bd_ans_drv_ConfigureVlan(bps));
}                                 

/* bd_ans_VlanSetTable()
**
**  This function is called as part of an ANS ioctl request to add some
**  vlan id's to the hardware vlan filter table.  
**
**  Arguments:  BOARD_PRIVATE_STRUCT *bps - a pointer to the adapters hw 
**                                          specific data structure.
**              iANSsupport_t *iANSdata   - pointer to the iANS required
**                                          support structure
**              IANS_BD_PARAM_HEADER *header - a pointer to the start of the
**                                             ans command.
**
**  Returns:    BD_ANS_STATUS -  SUCCESS if command was processed 
**                               successfully, FAILURE otherwise.
*/
BD_ANS_STATUS bd_ans_VlanSetTable(BOARD_PRIVATE_STRUCT *bps,
                                  iANSsupport_t *iANSdata,
                                  IANS_BD_PARAM_HEADER *header)
{
    IANS_BD_PARAM_IVLAN_TABLE *request = (IANS_BD_PARAM_IVLAN_TABLE *)header;
    int i;

    /* filtering mode cannot be ON unless the adapter supports 
     * vlan filtering.  This was set in VlanSetMode.
     */
    if (iANSdata->vlan_filtering_mode == IANS_VLAN_FILTERING_ON) {
        /* this function assumes that ANS sends us a complete 
         * table - so we blow away the old one
         */
        iANSdata->num_vlan_filter = request->VLanIDNum;
        for (i = 0; i < iANSdata->num_vlan_filter; i++) {
            iANSdata->VlanID[i] = request->VLanIDTable[i];
        }
        /* let the driver call the hardware routine to configure
         * the vlan table.
         */
        bd_ans_drv_ConfigureVlanTable(bps); 
        return BD_ANS_SUCCESS;
    }
    return BD_ANS_FAILURE;
}                              
                                 
#endif

/* bd_ans_ActivateFastPolling()
**
**  This function is called as part of an IOCTL sent by ANS to tell the 
**  driver to periodically check it's status and send status indications
**  if the status has changed.  
**
**  Arguments:  BOARD_PRIVATE_STRUCT *bps - a pointer to the adapters hw 
**                                          specific data structure.
**              iANSsupport_t *iANSdata   - pointer to the iANS required
**                                          support structure
**
**  Returns:    BD_ANS_STATUS -  SUCCESS if command was processed 
**                               successfully, FAILURE otherwise.
*/
BD_ANS_STATUS bd_ans_ActivateFastPolling(BOARD_PRIVATE_STRUCT *bps,
                                         iANSsupport_t *iANSdata)
{
    /* see if timer already active */
    if (iANSdata->timer_id != 0)
        return BD_ANS_SUCCESS;
    return (bd_ans_os_ActivateFastPolling(bps, iANSdata));
}

/* bd_ans_DeActivateFastPolling()
**
**  This function is called as part of an ANS IOCTL request to
**  disable status reporting or as part of a disconnect IOCTL request.
**  It will tell the driver that it no longer needs to keep updating
**  its status in the Watchdog routine.   
**
**  Arguments:  BOARD_PRIVATE_STRUCT *bps - a pointer to the adapters hw 
**                                          specific data structure.
**              iANSsupport_t *iANSdata   - pointer to the iANS required
**                                          support structure
**
**  Returns:    BD_ANS_STATUS -  SUCCESS if command was processed 
**                               successfully, FAILURE otherwise.
*/
BD_ANS_STATUS bd_ans_DeActivateFastPolling(BOARD_PRIVATE_STRUCT *bps,
                                           iANSsupport_t *iANSdata)
{
    /* if we have a non-zero timer_id, it means that we have
     * a watchdog routine going.
     */
    if (iANSdata->timer_id) {
        bd_ans_drv_StopWatchdog(bps);
        iANSdata->timer_id = 0;
    }    
    return (BD_ANS_SUCCESS);
}

/* bd_ans_SetReportingMode()
**
**  This function is called as part of an ANS ioctl request to start/stop
**  reporting status changes from the driver.  
**
**  Arguments:  BOARD_PRIVATE_STRUCT *bps - a pointer to the adapters hw 
**                                          specific data structure.
**              iANSsupport_t *iANSdata   - pointer to the iANS required
**                                          support structure
**              IANS_BD_PARAM_HEADER *header - a pointer to the start of the
**                                             ans command.
**
**  Returns:    BD_ANS_STATUS -  SUCCESS if command was processed 
**                               successfully, FAILURE otherwise.
*/
BD_ANS_STATUS bd_ans_SetReportingMode(BOARD_PRIVATE_STRUCT *bps, 
                                      iANSsupport_t *iANSdata,
                                      VOID *ans_buffer)
{
    BD_ANS_STATUS status;
    IANS_BD_PARAM_EXT_SET_MODE *iANSreport;
    iANSreport = (IANS_BD_PARAM_EXT_SET_MODE *)ans_buffer;

    DEBUGLOG1("bd_ans_SetReportingMode: %d\n",
        (iANSreport->BDIansStatusReport == IANS_REQUEST_SUPPORT));

    if (iANSreport->BDIansStatusReport == IANS_REQUEST_SUPPORT){
        status = bd_ans_ActivateFastPolling(bps, iANSdata);
        iANSdata->reporting_mode = IANS_STATUS_REPORTING_ON;
    } else {
        status = bd_ans_DeActivateFastPolling(bps, iANSdata);
        iANSdata->reporting_mode = IANS_STATUS_REPORTING_OFF;
    }

    return status;
}                                      

/* bd_ans_FillStatus()
**
**  This function is called both as part of an IOCTL request to get the
**  driver's current status, and as part of a Watchdog routine to compare
**  the current status to the previous status.  
**
**  Arguments:  BOARD_PRIVATE_STRUCT *bps - a pointer to the adapters hw 
**                                          specific data structure.
**              iANSsupport_t *iANSdata   - pointer to the iANS required
**                                          support structure
**              IANS_BD_PARAM_HEADER *header - a pointer to the start of the
**                                             ans command.
**
**  Returns:    BD_ANS_STATUS -  SUCCESS if command was processed 
**                               successfully, FAILURE otherwise.
*/                                      
BD_ANS_STATUS bd_ans_FillStatus(BOARD_PRIVATE_STRUCT *bps,
                                iANSsupport_t *iANSdata,
                                VOID *ans_buffer)
{
    IANS_BD_PARAM_STATUS *iANSstatus = (IANS_BD_PARAM_STATUS *)ans_buffer;

    DEBUGLOG("bd_ans_FillStatus: enter\n");

    /* tell iANS the supported version of this structure */
    iANSstatus->StatusVersion = IANS_STATUS_VERSION;

    /* all these support fields are initialized during the Identify request */
    /* check for link status */ 
    if (iANSdata->status_support_flags & BD_ANS_LINK_STATUS_SUPPORTED) {
        iANSstatus->LinkStatus  = *(iANSdata->link_status);

        if (iANSstatus->LinkStatus == IANS_STATUS_LINK_FAIL) {
            iANSstatus->LinkSpeed = IANS_STATUS_LINK_SPEED_NOT_SUPPORTED;
            iANSstatus->Duplex = IANS_STATUS_DUPLEX_NOT_SUPPORTED;
        } else {
            if (iANSdata->status_support_flags & BD_ANS_SPEED_STATUS_SUPPORTED) {
                switch(*(iANSdata->line_speed)) {
                case BD_ANS_10_MBPS:
                    iANSstatus->LinkSpeed = IANS_STATUS_LINK_SPEED_10MBPS;
                    break;
                case BD_ANS_100_MBPS:
                    iANSstatus->LinkSpeed = IANS_STATUS_LINK_SPEED_100MBPS;
                    break;
                case BD_ANS_1000_MBPS:
                    iANSstatus->LinkSpeed = IANS_STATUS_LINK_SPEED_1000MBPS;
                    break;
                default:
                    iANSstatus->LinkSpeed = IANS_STATUS_LINK_SPEED_NOT_SUPPORTED;
                    break;
                }
            } else {
                iANSstatus->LinkSpeed = IANS_STATUS_LINK_SPEED_NOT_SUPPORTED;
            }
            
            /* check for duplex status */   
            if (iANSdata->status_support_flags & BD_ANS_DUPLEX_STATUS_SUPPORTED) {          
                switch (*(iANSdata->duplex)) {
                case BD_ANS_DUPLEX_FULL:
                    iANSstatus->Duplex = IANS_STATUS_DUPLEX_FULL;
                    break;
                case BD_ANS_DUPLEX_HALF:
                    iANSstatus->Duplex = IANS_STATUS_DUPLEX_HALF;
                    break;
                default:
                    iANSstatus->Duplex = IANS_STATUS_DUPLEX_NOT_SUPPORTED;
                    break;
                }
            } else {
                iANSstatus->Duplex = IANS_STATUS_DUPLEX_NOT_SUPPORTED;
            }
        }
    
    } else {
        iANSstatus->LinkStatus = IANS_STATUS_LINK_NOT_SUPPORTED;
        iANSstatus->Duplex = IANS_STATUS_LINK_NOT_SUPPORTED;
        iANSstatus->LinkSpeed = IANS_STATUS_LINK_NOT_SUPPORTED;
    }

    /* check for hardware failure */
    if (iANSdata->status_support_flags & BD_ANS_HW_FAIL_STATUS_SUPPORTED) {
        iANSstatus->HardwareFailure =
            (*(iANSdata->hw_fail))?IANS_STATUS_HARDWARE_FAILURE:IANS_STATUS_HARDWARE_OK;
    } else {
        iANSstatus->HardwareFailure = IANS_STATUS_HARDWARE_NOT_SUPPORTED;
    }
    
    if (iANSdata->status_support_flags & BD_ANS_RESET_STATUS_SUPPORTED) {
        iANSstatus->DuringResetProcess = 
            (*(iANSdata->in_reset))?IANS_STATUS_DURING_RESET:IANS_STATUS_NOT_DURING_RESET;
    } else {
        iANSstatus->DuringResetProcess = IANS_STATUS_RESET_NOT_SUPPORTED;
    }
    
    /* check for suspended state */ 
    if (iANSdata->status_support_flags & BD_ANS_SUSPEND_STATUS_SUPPORTED) {
        iANSstatus->Suspended = 
            (*(iANSdata->suspended))?IANS_STATUS_SUSPENDED:IANS_STATUS_NOT_SUSPENDED; 
    } else {
        iANSstatus->Suspended = IANS_STATUS_SUSPENDED_NOT_SUPPORTED;
    }
    return (BD_ANS_SUCCESS);
}                                                                      

/* bd_ans_ResetAllModes()
**
**  This function is called as part of an ANS IOCTL request to disconnect  
**
**  Arguments:  BOARD_PRIVATE_STRUCT *bps - a pointer to the adapters hw 
**                                          specific data structure.
**              iANSsupport_t *iANSdata   - pointer to the iANS required
**                                          support structure
**
**  Returns:    BD_ANS_STATUS -  SUCCESS if command was processed 
**                               successfully, FAILURE otherwise.
*/                                                                     
BD_ANS_STATUS bd_ans_ResetAllModes(BOARD_PRIVATE_STRUCT *bps,
                                   iANSsupport_t *iANSdata)
{
    int i;

    DEBUGLOG("bd_ans_ResetAllModes: enter\n");
        
    /* in most cases I don't check the return values here because there isn't
     * much I can do about it if it fails 
     */
    (void)bd_ans_DeActivateFastPolling(bps, iANSdata);
    BD_ANS_BZERO((UCHAR *)&iANSdata->prev_status,sizeof(IANS_BD_PARAM_STATUS));
    
    /* set link to iANS as down  */
    iANSdata->iANS_status          = IANS_COMMUNICATION_DOWN;
    iANSdata->attributed_mode      = (UINT32) BD_ANS_FALSE;
    iANSdata->routing_mode         = IANS_ROUTING_OFF;
    
#ifdef IANS_BASE_VLAN_TAGGING
    /* need to reconfigure the adapter to disable tag mode */
    iANSdata->tag_mode             = IANS_BD_TAGGING_NONE;
    (void) bd_ans_drv_ConfigureTagging(bps);
    
    /* need to reconfigure the adapter to disable vlan mode */    
    iANSdata->vlan_mode = IANS_VLAN_MODE_OFF;
    (void) bd_ans_drv_ConfigureVlan(bps);
    
    /* need to reset the vlan filter table and configure the
     * adapter to disable vlan filtering 
     */
    iANSdata->vlan_filtering_mode = IANS_VLAN_FILTERING_OFF;
    iANSdata->num_vlan_filter = 0;
    for (i = 0; i < MAX_NUM_VLAN; i++) {
        iANSdata->VlanID[i] = 0;
    }
    (void) bd_ans_drv_ConfigureVlanTable(bps);
#endif

    return (BD_ANS_SUCCESS);
}

/* bd_ans_GetAllCapabilities()
**
**  This function is called as part of an ANS IOCTL request to get
**  open communication with the base driver (Identify).  It will
**  initialize all the support flags of the support structure.  
**
**  Arguments:  BOARD_PRIVATE_STRUCT *bps - a pointer to the adapters hw 
**                                          specific data structure.
**              iANSsupport_t *iANSdata   - pointer to the iANS required
**                                          support structure
**
**  Returns:    BD_ANS_STATUS -  SUCCESS if command was processed 
**                               successfully, FAILURE otherwise.
*/                                                                     

BD_ANS_STATUS bd_ans_GetAllCapabilities(BOARD_PRIVATE_STRUCT *bps,
                                        iANSsupport_t *iANSdata)
{
    /* get the OS specific capabilities */
    iANSdata->can_set_mac_addr = BD_ANS_OS_MAC_ADDR_SUPPORT;
    iANSdata->supports_stop_promiscuous = BD_ANS_OS_STOP_PROM_SUPPORT;
    
    /* get the Driver specific capabilities */
    iANSdata->status_support_flags = BD_ANS_DRV_STATUS_SUPPORT_FLAGS;
    iANSdata->max_vlan_ID_supported = BD_ANS_DRV_MAX_VLAN_ID(bps);
    iANSdata->vlan_table_size = BD_ANS_DRV_MAX_VLAN_TABLE_SIZE(bps);
    iANSdata->ISL_tag_support = BD_ANS_DRV_ISL_TAG_SUPPORT(bps);
    iANSdata->IEEE_tag_support = BD_ANS_DRV_IEEE_TAG_SUPPORT(bps);
    iANSdata->vlan_support =   BD_ANS_DRV_VLAN_SUPPORT(bps);
    iANSdata->vlan_filtering_support = BD_ANS_DRV_VLAN_FILTER_SUPPORT(bps);
    iANSdata->vlan_offload_support = BD_ANS_DRV_VLAN_OFFLOAD_SUPPORT(bps);
    
    /* get the hardware specific capabilities */
    iANSdata->available_speeds  = BD_ANS_HW_AVAILABLE_SPEEDS(bps);
    return (bd_ans_os_GetAllCapabilities(bps, iANSdata));
}                                        

/* bd_ans_Receive()
**
**  This function is called when the driver has been configured to
**  run in attributed mode (meaning we are attaching TLVs to each
**  packet).  It will perform the neccessary operations to create
**  the needed TLVs and attach them to the frame.  
**
**  Arguments:  BOARD_PRIVATE_STRUCT *bps - a pointer to the adapters hw 
**                                          specific data structure.
**              iANSSupport_t *iANSdata - pointer to ANS data structure.
**              HW_RX_DESCRIPTOR *rxd - pointer to hw dependent rx struct
**              FRAME_DATA *frame - pointer to an ethernet frame
**              OS_DATA *os_data - pointer to OS dependent frame data
**              OS_DATA **os_tlv - pointer to a pointer to the os structure
**                                 containing the tlv data.  This pointer is
**                                 modified by this routine.
**              UINT32 *tlv_list_length - pointer to the length of the
**                                      new tlv list.  This is modified
**                                      by this routine.  It is provided in 
**                                      case some OS needs to adjust the
**                                      length value in it's OS_DATA structure.
**
**  Returns:    BD_ANS_STATUS -  SUCCESS if command was processed 
**                               successfully, FAILURE otherwise.
*/                                                                     
BD_ANS_STATUS bd_ans_Receive(BOARD_PRIVATE_STRUCT *bps,
                             iANSsupport_t *iANSdata,
                             HW_RX_DESCRIPTOR *rxd,
                             FRAME_DATA *frame,
                             OS_DATA *os_frame_data,
                             OS_DATA **os_tlv,
                             UINT32 *tlv_list_length)
{
    BD_ANS_BOOLEAN Frame_is_tagged;
    peth_vlan_header_t peth_vlan_header =  (peth_vlan_header_t )frame;
    UINT16 tag;
    UINT32 tlvlist_length = 0; /* Total length of all TLVs */

    DEBUGLOG("bd_ans_Receive: enter\n");

#ifdef IANS_BASE_VLAN_TAGGING
    /* check to see if this is a qtag packet */
    
    Frame_is_tagged = bd_ans_IsQtagPacket(bps, iANSdata, rxd, peth_vlan_header);
    DEBUGLOG1("bd_ans_Receive: fram_is_tagged=%d\n", Frame_is_tagged);
    /* get the vlan id */
    tag = bd_ans_GetVlanId(bps, iANSdata, rxd, peth_vlan_header);
    
    switch (iANSdata->tag_mode)
    {
        case IANS_BD_TAGGING_NONE:
            /* the rules are as follows for this situation:
             * vlan tagged - drop
             * priority tagged - strip, and send w/o OOB
             * untagged - receive and send w/o OOB
             *
             * As we are required to send attributed packets,
             * we will fill out just the last attribute and send it
             * if we accept the packet. 
             */
            DEBUGLOG("bd_ans_Receive: Tagging mode NONE\n");
            if (Frame_is_tagged) {
                if (tag) {
                    DEBUGLOG("bd_ans_Receive: Invalid VLAN packet\n");
                    return BD_ANS_FAILURE;
                }
                /* priority tagged */
                if (iANSdata->vlan_offload_support == BD_ANS_FALSE)
                    bd_ans_os_StripQtagSW(os_frame_data);
            }
            break;
            
        case IANS_BD_TAGGING_802_3AC:
            /* here we have the following rules:
             * VLAN-tagged: strip, report vlan id in tlv
             * Priority-tagged: drop
             * Untagged: receive with untagged tlv.
             *
             *  The Untagged rule is there for 802.3ad requirements.
             *  The 802.3ad spec allows for sending protocol packets
             *  to and from the switch that ANS must be able to
             *  receive.
             */
            DEBUGLOG("bd_ans_Receive: Tagging mode 802.3ac\n");
            if (Frame_is_tagged && !tag) {
                DEBUGLOG("bd_ans_Receive: Invalid VLAN packet\n");
                return BD_ANS_FAILURE;
            }
            if (tlvlist_length == 0) {
                if (bd_ans_os_AllocateTLV(os_frame_data, os_tlv) == BD_ANS_FAILURE) {
                    DEBUGLOG("bd_ans_Receive: Failed to allocated TLV\n");
                    return BD_ANS_FAILURE;
                }
            }
            if (Frame_is_tagged) {
                /* VLAN_ID TLV */
                tlvlist_length += bd_ans_os_AttributeFill(IANS_ATTR_VLAN_ID, 
                                                          *os_tlv, 
                                                          tlvlist_length,
                                                          (void *)&tag);
                if (iANSdata->vlan_offload_support == BD_ANS_FALSE)
                    bd_ans_os_StripQtagSW(os_frame_data);
            } else {
                /* Send along, but untagged */
                /* Untagged TLV */
                tlvlist_length += 
                    bd_ans_os_AttributeFill(IANS_ATTR_TAGGING_UNTAGGED, 
                                            *os_tlv, 
                                            tlvlist_length,
                                            NULL);
            }
            break;
        default:
            DEBUGLOG("bd_ans_Receive: Invalid tagging mode\n");
            return BD_ANS_FAILURE;        
    }
#endif
    /* allocate space for the last attribute TLV */
    if (tlvlist_length == 0) {
        if (bd_ans_os_AllocateTLV(os_frame_data, os_tlv) ==
            BD_ANS_FAILURE) {
            DEBUGLOG("bd_ans_Receive: failed to allocated TLV\n");
            return BD_ANS_FAILURE;                 
        }
    }       
    
    /* Last Attribute TLV */
    tlvlist_length += bd_ans_os_AttributeFill(IANS_ATTR_LAST_ATTR, 
                                              *os_tlv, 
                                              tlvlist_length,
                                              NULL);
    *tlv_list_length = tlvlist_length;
    return BD_ANS_SUCCESS;
}
                             
/* bd_ans_Transmit()
**
**  This function is called when the driver has been configured to be
**  run in attributed mode (meaning we are receiving TLVs along with
**  the frame from ANS).  It will perform the necessary operations 
**  according to what TLVs it finds.  Note that it does NOT remove the
**  TLV list from the frame, it is up to the OS routines to do that 
**  if it is needed.  
**
**  Arguments:  BOARD_PRIVATE_STRUCT *bps - a pointer to the adapters hw 
**                                          specific data structure.
**              HW_RX_DESCRIPTOR *rxd - pointer to hw dependent rx struct
**              FRAME_DATA *frame - pointer to an ethernet frame
**              OS_DATA *os_data - pointer to OS dependent frame data
**
**  Returns:    BD_ANS_STATUS -  SUCCESS if command was processed 
**                               successfully, FAILURE otherwise.
*/                                                                     
BD_ANS_STATUS bd_ans_Transmit(BOARD_PRIVATE_STRUCT *bps,
                              iANSsupport_t *iANSdata,
                              pPer_Frame_Attribute_Header pTLV,
                              HW_TX_DESCRIPTOR *txd,
                              OS_DATA **frame_ptr,
                              UINT16 *vlanid)
{
    DEBUGLOG("bd_ans_Transmit: enter\n");
 
#ifdef IANS_BASE_VLAN_TAGGING
    *vlanid = INVALID_VLAN_ID;
   
    /* if we are not in tagging mode, we have nothing to do */
    if ( iANSdata->tag_mode == IANS_BD_TAGGING_NONE ) {
        return BD_ANS_SUCCESS;
    }
    /* traverse the list of TLVs until we get to the Last TLV. */
    while (pTLV->AttributeID != IANS_ATTR_LAST_ATTR) { 
        switch( pTLV->AttributeID ) {
        case IANS_ATTR_VLAN_ID:
            *vlanid = (UINT16) bd_ans_ExtractValue(pTLV);
            break;
        default:
            break;
        } /* switch AttributeID */
        
        pTLV = GET_NEXT_TLV(pTLV);
    }

    DEBUGLOG1("bd_ans_Transmit: vlanid=%d\n", *vlanid);
    if (*vlanid != INVALID_VLAN_ID) {
        /* we can insert the qtag here.  Doing this here instead of
         * within the while loop insures that the TLV list is no longer
         * needed, and we can recycle that extra memory (if needed)
         */
        if (iANSdata->vlan_offload_support == BD_ANS_FALSE) {
            if (bd_ans_os_InsertQtagSW(bps, frame_ptr, vlanid) == BD_ANS_FAILURE) { 
                DEBUGLOG("bd_ans_Transmit: Failed to insert qtag sw\n");
                return BD_ANS_FAILURE; 
            }              
        } else {
            if (bd_ans_hw_InsertQtagHW(bps, txd, vlanid) == BD_ANS_FAILURE) { 
                DEBUGLOG("bd_ans_Transmit: Failed to insert qtag hw\n");
                return BD_ANS_FAILURE;
            }
        }
    }
#endif
    return (BD_ANS_SUCCESS);
}

#ifdef IANS_BASE_VLAN_TAGGING

/* bd_ans_IsQtagPacket()
**
**  This function will determine whether or not a given packet has a
**  Qtag in it.
**
**  Arguments:  BOARD_PRIVATE_STRUCT *bps - pointer to the driver's private
**                                          data structure
**              iANSsupport_t *iANSdata   - pointer to the required ANS
**                                          support structure.
**              HW_RX_DESCRIPTOR *rxd     - pointer to hw specific rx frame
**                                          descriptor
**              eth_vlan_header_t *header - pointer to the head of the actual
**                                          frame data.
**
**  Returns:    BD_ANS_BOOLEAN      BD_ANS_TRUE if it is a qtag packet
**                                  BD_ANS_FALSE if it is not.
*/
BD_ANS_BOOLEAN
bd_ans_IsQtagPacket(BOARD_PRIVATE_STRUCT *bps,
                    iANSsupport_t *iANSdata, 
                    HW_RX_DESCRIPTOR *rxd,
                    peth_vlan_header_t header)
{
    /* if vlan_offload_support flag is set, then the driver has already 
     * stripped the packet and will have a proprietary means of indicating 
     * that it found a qtag packet.  If so, call the hw module routine to 
     * check it. 
     */
    DEBUGLOG("bd_ans_IsQtagPacket: enter\n");

    if (iANSdata->vlan_offload_support) {
        return (bd_ans_hw_IsQtagPacket(bps, rxd));
    }
    /* print the packet for debugging */
    DEBUGLOG1("bd_ans_IsQtagPacket: type is 0x%x\n",
              ntohs(header->Qtag.EtherType));
    return (ntohs(header->Qtag.EtherType) == QTAG_TYPE);
}                                                 

/* bd_ans_GetVlanId()
**
**  This function will call the hw proprietary function to get the
**  vlan id if the driver supports vlan offloading, otherwise, it
**  will get the IEEE vlan id from the packet.
**
**  TBD - what about ISL...
**
**  Arguments:  BOARD_PRIVATE_STRUCT *bps - pointer to the driver's private
**                                          data structure.
**              iANSsupport_t *iANSdata   - pointer to the ans required
**                                          support structure.
**              HW_RX_DESCRIPTOR *rxd     - pointer to the hw specific
**                                          rx frame descriptor
**              eth_vlan_header_t *header - pointer to the actual frame data
**
**  Returns:    UINT16      - the IEEE vlan id.
*/
UINT16
bd_ans_GetVlanId(BOARD_PRIVATE_STRUCT *bps,
                 iANSsupport_t *iANSdata, 
                 HW_RX_DESCRIPTOR *rxd,
                 peth_vlan_header_t header)    
{
    if (iANSdata->vlan_offload_support) {
        return (bd_ans_hw_GetVlanId(bps, rxd));
    }
    return (ntohs(header->Qtag.VLAN_ID) & VLAN_ID_MASK);
}                     
#endif

/* bd_ans_AttributeFill()
**
**  This routine will fill out a TLV based on a given attribute ID
**
**  Arguments:   iANS_Attribute_ID attr_id - id which identifies which 
**                                           TLV this is.
**               void *pTLV                - pointer to where the TLV should
**                                           be copied.
**               void *data                - optional data to be added to the
**                                           TLV (the V part)
**
** Returns:      UINT32                    - the length of the new TLV
*/
UINT32
bd_ans_AttributeFill(iANS_Attribute_ID attr_id, 
                     void *pTLV, 
                     void *data)
{
#ifdef IANS_BASE_VLAN_TAGGING
    VLAN_ID_Per_Frame_Info *vlan_pfi;
    Untagged_Attribute *untagged;
#endif
    Last_Attribute *last;    
    int tlv_length = 0;
    
    DEBUGLOG("bd_ans_AttributeFill: enter\n");
    
    switch(attr_id) 
    {
#ifdef IANS_BASE_VLAN_TAGGING
    case IANS_ATTR_VLAN_ID:
        DEBUGLOG("bd_ans_AttributeFill: filling vlan id attr\n");
        tlv_length = sizeof(VLAN_ID_Per_Frame_Info);
        vlan_pfi = (VLAN_ID_Per_Frame_Info *)pTLV;
        vlan_pfi->AttrHeader.AttributeID = IANS_ATTR_VLAN_ID;
        vlan_pfi->AttrHeader.AttributeLength = 
            tlv_length - sizeof(Per_Frame_Attribute_Header);
        vlan_pfi->VLanID = *((UINT16 *)data);
        break;
      
    case IANS_ATTR_TAGGING_UNTAGGED: 
        DEBUGLOG("bd_ans_AttributeFill: filling untagged attr\n");
        tlv_length = sizeof(Untagged_Attribute);
        untagged = (Untagged_Attribute *)pTLV;
        untagged->AttrHeader.AttributeID = IANS_ATTR_TAGGING_UNTAGGED;
        untagged->AttrHeader.AttributeLength = 
            tlv_length - sizeof(Per_Frame_Attribute_Header);
        break;
#endif            
    case IANS_ATTR_LAST_ATTR:
        DEBUGLOG("bd_ans_AttributeFill: filling last attr\n");
        tlv_length = sizeof(Last_Attribute);
        last = (Last_Attribute *)pTLV;
        last->LastHeader.AttributeID = IANS_ATTR_LAST_ATTR; 
        last->LastHeader.AttributeLength = 0;   
        break;

    default:
        break;
    }
    return tlv_length;
}    

/* bd_ans_ExtractValue()
**
**  This function will extract the value from a TLV
**
**  Arguments:  Per_Frame_Attribute_Header *pTLV - pointer to the TLV
**
**  Returns:    UINT32 - dword value from the TLV.  Smaller values 
**                       should be cast correctly by the caller.
*/
UINT32
bd_ans_ExtractValue(Per_Frame_Attribute_Header *pTLV)
{
    UINT32 ret_val;
    UCHAR *p;
    p = (UCHAR *) &(pTLV->AttributeLength);
    p += sizeof(pTLV->AttributeLength);
    ret_val = *((UINT32 *)p);
    return (ret_val); 
}                     
