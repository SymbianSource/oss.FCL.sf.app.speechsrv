/*
* Copyright (c) 2004-2005 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Icon definitions
*
*/


#ifndef VUIVOICEICONDEFS_H
#define VUIVOICEICONDEFS_H

#include <avkon.mbg>
#include <AknsConstants.h>
#include <voiceui.mbg>

const TInt KSindNumberOfIcons = 11;

// Icon resource indices for different icons
_LIT( KIconPhone,        "0" );
//_LIT( KIconHome,         "1" );
//_LIT( KIconWork,         "2" );
_LIT( KIconMobile,       "3" );
_LIT( KIconFax,          "4" );
_LIT( KIconEmail,        "5" );
_LIT( KIconBlank,        "6" );
_LIT( KIconProfile,      "7" );
_LIT( KIconApplication,  "8" );
_LIT( KIconVideo,        "9" );
_LIT( KIconVoip,        "10" );

enum TIconId
    {
    EIconPhone,
    EIconHome,
    EIconWork,
    EIconMobile,
    EIconFax,
    EIconEmail,
    EIconBlank,
    EIconProfile,
    EIconApplication,
    EIconVideo,
    EIconVoip
    };

static const TInt KBitmapId[] = 
    {
    EMbmAvkonQgn_prop_nrtyp_phone,
    EMbmAvkonQgn_prop_nrtyp_home,
    EMbmAvkonQgn_prop_nrtyp_work,
    EMbmAvkonQgn_prop_nrtyp_mobile,
    EMbmAvkonQgn_prop_nrtyp_fax,
    EMbmAvkonQgn_prop_nrtyp_email,
    EMbmAvkonQgn_stat_bt_blank,
    EMbmVoiceuiQgn_prop_profiles_small,
    EMbmVoiceuiQgn_prop_fmgr_file_apps,
    EMbmAvkonQgn_prop_nrtyp_video,
    EMbmAvkonQgn_prop_nrtyp_voip
    };
    
static const TInt KMaskId[] = 
    {
    EMbmAvkonQgn_prop_nrtyp_phone_mask,
    EMbmAvkonQgn_prop_nrtyp_home_mask,
    EMbmAvkonQgn_prop_nrtyp_work_mask,
    EMbmAvkonQgn_prop_nrtyp_mobile_mask,
    EMbmAvkonQgn_prop_nrtyp_fax_mask,
    EMbmAvkonQgn_prop_nrtyp_email_mask,
    EMbmAvkonQgn_stat_bt_uni_blank,
    EMbmVoiceuiQgn_prop_profiles_small_mask,
    EMbmVoiceuiQgn_prop_fmgr_file_apps_mask,
    EMbmAvkonQgn_prop_nrtyp_video_mask,
    EMbmAvkonQgn_prop_nrtyp_voip_mask
    };
       
static const TAknsItemID* const KIconId[] = 
    {
    &KAknsIIDQgnPropNrtypPhone,
    &KAknsIIDQgnPropNrtypHome,
    &KAknsIIDQgnPropNrtypWork,
    &KAknsIIDQgnPropNrtypMobile,
    &KAknsIIDQgnPropNrtypFax,
    &KAknsIIDQgnPropNrtypEmail,
    &KAknsIIDQgnStatBtBlank,
    &KAknsIIDQgnPropProfilesSmall,
    &KAknsIIDQgnPropFmgrFileApps,
    &KAknsIIDQgnPropNrtypVideo,
    &KAknsIIDQgnPropNrtypVoip
    };

#endif	// VUIVOICEICONDEFS_H

// End of File

