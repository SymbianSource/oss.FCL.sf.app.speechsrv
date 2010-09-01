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



#include <e32def.h>
#include <s32mem.h>  // memory streams
#include <vcommandapi.h>
#include "rubydebug.h"
#include "desarrayconverter.h"
#include "tagcommandconverter.h"
#include "vcommandinternalapi.h"

// For CleanupResetAndDestroy
#include <mmfcontrollerpluginresolver.h>  

/** The index of the first element of the tag RRD text array, that is used to 
 * store the streamed VCommand
 */
const TInt KStreamStartIndex = 1;

/** 
 * Streamimg the VCommand might take arbitrary amount of space.
 * Every time the streaming buffer needs to be increased it grows up
 * this amount of bytes
 */
const TInt KCommandBufferSizeExpansion = 500;

/** 
* Granularity for the returned tag array
*/ 
const TInt KDefaultTagsPerCommand = 2;

/**
* Converts given tag to the VCommand
* @leave KErrTotalLossOfPrecision if the aTag's intArray is empty
*/
CStoredVCommand* CTagCommandConverter::TagToCommandLC( const MNssTag& aTag )
	{
    MNssTag& varTag ( const_cast<MNssTag&>(aTag) );
    CArrayFixFlat<TInt>& intArray = *( varTag.RRD()->IntArray() );
    __ASSERT_ALWAYS( intArray.Count() > 0, User::Leave( KErrTotalLossOfPrecision  ) );
    
    CArrayFixFlat<NssRRDText>& tagArray = *(varTag.RRD()->TextArray());
    TDesC8* storage = CDesArrayConverter::ArrayToDesC8LC( tagArray, KStreamStartIndex, 
                            tagArray.Count() - KStreamStartIndex );
    TInt commandId = intArray[KCommandIdRrdIntIndex];
        
    RDesReadStream readStream( *storage );
    CleanupClosePushL( readStream );
    CStoredVCommand* command = CStoredVCommand::NewL( readStream, commandId );
    
    CleanupStack::PopAndDestroy( &readStream );    
    CleanupStack::PopAndDestroy( storage );
    CleanupStack::PushL( command );
    return command;
	}
    
/**
* Constructs a new VAS tag from a command and a context. 
* Does not save to VAS, just creates. Created tag stores the streamed
* VCommand in its RRDText array. The tag's rrd int array is empty
*/
MNssTag* CTagCommandConverter::CommandToTagLC( const CVCommand& aCommand, 
						const MNssContext& aContext, MNssTagMgr& aTagMgr )
    {
    MNssTag* tag = aTagMgr.CreateTagL( const_cast<MNssContext*>( &aContext ) );
    CleanupDeletePushL( tag );

    // the first (and only) element of the rrdint array is the application uid
    // the first element of the rrdtext array is the command line arguments 
    // converted to 16 bit representation (on 16-bit systems)
    /** @todo consider removing this "simple" shortcut of saving the arguments 
     *        into the first RRD text element. This information is 
     *        anyway streamed into the other rrdtext slots
     */
    CArrayFixFlat<TInt> *theId = new (ELeave) CArrayFixFlat<TInt>( 1 );
    CleanupStack::PushL( theId );
    
    
    CBufFlat* storage = CBufFlat::NewL( KCommandBufferSizeExpansion );
    CleanupStack::PushL( storage );
    
    RBufWriteStream writeStream( *storage);
    CleanupClosePushL( writeStream );
    writeStream << aCommand;    
    CleanupStack::PopAndDestroy( &writeStream );
   
    CArrayFixFlat<NssRRDText>* streamChunks = 
                        CDesArrayConverter::DesC8ToArrayLC( storage->Ptr( 0 ) );
    // 1 for the command line args. They are saved separately
    CArrayFixFlat<NssRRDText>* theText = 
    	new (ELeave) CArrayFixFlat<NssRRDText>( streamChunks->Count() + 1 );
    CleanupStack::PushL(theText);
    
    NssRRDText bufArguments;
    // converts to 16-bits in unicode builds
    bufArguments.Copy( aCommand.Runnable().Arguments() );
    theText->AppendL( bufArguments );
        
    for( TInt i(0); i < streamChunks->Count(); i++ )
        {
        // the last piece
        theText->AppendL( streamChunks->At( i ) );
        }
    RUBY_DEBUG0( "CTagCommandConverter::CommandToTagLC Saved command to the rrd text array" );
    tag->RRD()->SetIntArrayL( theId );
    tag->RRD()->SetTextArrayL( theText );
    
    CleanupStack::PopAndDestroy( theText );
    CleanupStack::PopAndDestroy( streamChunks );
    CleanupStack::PopAndDestroy( storage );
    CleanupStack::PopAndDestroy( theId );
    
    return tag;
    }    

/**
* Converts given tags to the VCommand. This tags must have been generated
* from a single VCommand (via CommandToTagsLC2 ) 
* @leave KErrOverflow if given array has more, than two elements
* @leave KErrUnderflow if given array is empty
* @leave KErrArgument if given tags correspond to the different VCommands
*/
CStoredVCommand* CTagCommandConverter::TagsToCommandLC( const CArrayPtr<MNssTag>& aTags )
    {
    __ASSERT_ALWAYS( aTags.Count() <= KMaxTagsPerCommand, User::Leave( KErrOverflow ) );
    __ASSERT_ALWAYS( aTags.Count() > 0, User::Leave( KErrUnderflow ) );
    CStoredVCommand* firstCommand = TagToCommandLC( *aTags[0] );
    // Comparing tags is complex. Comparing VCommands is simple
    if( aTags.Count() == KMaxTagsPerCommand )
        {
        CStoredVCommand* secondCommand = TagToCommandLC( *aTags[1] );
        if( !( *firstCommand == *secondCommand ) )
            {
            RUBY_ERROR0( "CTagCommandConverter::TagsToCommandLC firstCommand is NOT equal to second command. Leaving with KErrArgument");
            User::Leave( KErrArgument );
            }
        CleanupStack::PopAndDestroy( secondCommand );
        }
    return firstCommand;    
    }

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
CArrayPtr<MNssTag>* CTagCommandConverter::CommandToTagsLC2( const CVCommand& aCommand, 
				const MNssContext& aContext, MNssTagMgr& aTagMgr )
    {
    CArrayPtr<MNssTag>* array = new (ELeave) CArrayPtrFlat<MNssTag>( KDefaultTagsPerCommand );
    CleanupStack::PushL( array );
    CleanupResetAndDestroyPushL( *array );
    
    MNssTag* firstTag = CommandToTagLC( aCommand, aContext, aTagMgr );
    array->AppendL( firstTag );
    CleanupStack::Pop( firstTag );
    if( aCommand.AlternativeSpokenText() != KNullDesC )
        {
        // Second tag is needed
        MNssTag* secondTag = CommandToTagLC( aCommand, aContext, aTagMgr );
        array->AppendL( secondTag );
        CleanupStack::Pop( secondTag );
        }
    return array;    
    }

//End of file
