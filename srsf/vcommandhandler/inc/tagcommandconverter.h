/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Converts VCommands into tags and vise veraa
*
*/


 
#ifndef TAGCOMMANDCONVERTER_H
#define TAGCOMMANDCONVERTER_H

#include <nssvasmtag.h>
#include <nssvasmtagmgr.h>
#include "vcommandinternalapi.h"

class CVCommand;

/**
* A VCommand can be translated in no more, than KMaxTagsPerCommand tags
*/
const TInt KMaxTagsPerCommand = 2;

/** Index of the voice command id in the voice tag's rrd int array */
const TInt KCommandIdRrdIntIndex = 0;



/**
* Converts VCommands into tags and vise veraa
*/
class CTagCommandConverter : public CBase
	{
	public:
		/**
		* Converts given tags to the VCommand. These tags must have been generated
		* from a single VCommand (via CommandToTagsLC2 ) 
		* @leave KErrOverflow if given array has more, than two elements
		* @leave KErrUnderflow if given array is empty
		* @leave KErrArgument if given tags correspond to the different VCommands
		*/
		static CStoredVCommand* TagsToCommandLC( const CArrayPtr<MNssTag>& aTags );
		
		/**
		* Constructs new VAS tags from a command and a context. 
		* Does not save to VAS, just creates. At the end of this function two
		* items are pushed to the cleanup stack. The first PopAndDestroy will
		* ResetAndDestroy the generated tags, the second one will delete the
		* CArrayPtr itself
		*
		* @return array of one or two VAS tags. Two tags are created if a given 
		*         command has a UserText set. The created tags bear the identical
		*         data
		*/
		static CArrayPtr<MNssTag>* CommandToTagsLC2( const CVCommand& aCommand, 
						const MNssContext& aContext, MNssTagMgr& aTagMgr );
		
		/**
		* Converts given tag to the CStoredVCommand
		* @leave KErrTotalLossOfPrecision if the aTag's intArray is empty
		*/
		static CStoredVCommand* TagToCommandLC( const MNssTag& aTag );
		
		/**
		* Constructs a new VAS tag from a command and a context. 
		* Does not save to VAS, just creates. Created tag stores the streamed
		* VCommand in its RRDText array. The tag's rrd int array of the created 
		* tag is empty
		*/
		static MNssTag* CommandToTagLC( const CVCommand& aCommand, 
						const MNssContext& aContext, MNssTagMgr& aTagMgr );
	};

#endif // TAGCOMMANDCONVERTER_H