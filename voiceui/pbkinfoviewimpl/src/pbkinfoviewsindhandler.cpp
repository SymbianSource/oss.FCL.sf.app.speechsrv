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
* Description:  Implementation for CPbkInfoViewSindHandler
*
*/


// INCLUDE FILES
#include "pbkinfoviewsindhandler.h"
#include "pbkinfoviewreshandler.h"
#include "pbkinfoviewdefines.h"
#include <pbkinfoview.rsg>

#include <vuivoicerecogdefs.h>  // KVoiceDialContext
#include <nssvascoreconstant.h>
#include <vascvpbkhandler.h>
#include <CPbkContactEngine.h>
#include <CPbkContactItem.h>
#include <CPbkFieldInfo.h>
#include <StringLoader.h>
#include <nssvascoreconstant.h>

#include "rubydebug.h"

// ----------------------------------------------------------------------------
// RemapCommand
// Remap command to the correct order.
// @param aCommandId 
// ----------------------------------------------------------------------------
static TInt RemapCommand( TInt aCommandId )
    {
    switch (aCommandId)
        {
        case EEmailCommand:
            return EMessageCommand;
        case EVoipCommand:
            return EEmailCommand;
        case EMessageCommand:
            return EVoipCommand;
        default:
            return aCommandId;
        }
    }
// ================= MEMBER FUNCTIONS =======================

inline CPbkInfoViewSindHandler::CPbkInfoViewSindHandler()
    {
    }
    
// ----------------------------------------------------------------------------
// CPbkInfoViewSindHandler::ConstructL
// Creates a new instance of CPbkSindHandler.
// @param aContactId Contact id for the contact whose voice tags are to
//        be fetched from vas db.
// ----------------------------------------------------------------------------
inline void CPbkInfoViewSindHandler::ConstructL( TInt aContactId )
    {   
    InitializeL( aContactId );
    
    CreateVoiceTagListL( aContactId );
    
    CreatePopupArrayL();
    }
    
// ----------------------------------------------------------------------------
// CPbkInfoViewSindHandler::TestConstructL
// Creates an instance that doesn't require CCoeEnv for STIF testing purposes.
// @param aContactId Contact id for the contact whose voice tags are to
//        be fetched from vas db.
// ----------------------------------------------------------------------------
inline void CPbkInfoViewSindHandler::TestConstructL( TInt aContactId )
    {
    InitializeL( aContactId );
    
    CreateVoiceTagListL( aContactId );
    }

// ----------------------------------------------------------------------------
// CPbkInfoViewSindHandler::InitializeL
// Initialization.
// @param aContactId Contact id for the contact whose voice tags are
//        fetched from vas db.
// @return TInt KErrNone on success
// ----------------------------------------------------------------------------
void CPbkInfoViewSindHandler::InitializeL( TInt aContactId )
    {
    RUBY_DEBUG_BLOCK( "CPbkInfoViewSindHandler::InitializeL" );
    
    // create Vas db manager
    iVasDbManager = CNssVASDBMgr::NewL();

    iVasDbManager->InitializeL();
    
    // Ownership not transferred
    iContextManager = iVasDbManager->GetContextMgr();
    // Ownership not transferred
    iTagManager = iVasDbManager->GetTagMgr();
    
    iWait = new (ELeave) CActiveSchedulerWait();
    
    // Create and initialize phonebook handler that is used to get the
    // phone numbers etc used in voice tags.
    iPbkHandler = CVasVPbkHandler::NewL();
    iPbkHandler->InitializeL();

    iPbkEngine = CPbkContactEngine::NewL();
    
    if( aContactId == KVoiceTaglessContactId )
        {
        iPbkContactItem = iPbkEngine->CreateEmptyContactL();
        }
    else
        {
        iPbkHandler->FindContactL( aContactId, ETrue );
        iPbkContactItem = iPbkEngine->ReadContactL( aContactId );    
        }
    
    iTts = CTtsUtility::NewL( *this );
    
    // Put to thread local storage so that CompareL can use this pointer
    Dll::SetTls( iPbkContactItem );
    
    iPlaybackIndex = -1;
    }

// ----------------------------------------------------------------------------
// CPbkInfoViewSindHandler::NewL
//
// ----------------------------------------------------------------------------
CPbkInfoViewSindHandler* CPbkInfoViewSindHandler::NewL( TInt aContactId )
    {
    RUBY_DEBUG_BLOCK( "CPbkInfoViewSindHandler::NewL" );
    
    CPbkInfoViewSindHandler* self = new (ELeave) CPbkInfoViewSindHandler;
    CleanupStack::PushL( self );
    self->ConstructL( aContactId );
    CleanupStack::Pop(self);
    return self;
    }
    
// ----------------------------------------------------------------------------
// CPbkInfoViewSindHandler::TestNewL
//
// ----------------------------------------------------------------------------
CPbkInfoViewSindHandler* CPbkInfoViewSindHandler::TestNewL( TInt aContactId )
    {
    CPbkInfoViewSindHandler* self = new (ELeave) CPbkInfoViewSindHandler;
    CleanupStack::PushL( self );
    self->TestConstructL( aContactId );
    CleanupStack::Pop(self);
    return self;
    }

// ----------------------------------------------------------------------------
// CPbkInfoViewSindHandler::~CPbkInfoViewSindHandler
//
// ----------------------------------------------------------------------------
CPbkInfoViewSindHandler::~CPbkInfoViewSindHandler()
    {
    delete iVasDbManager;
    iVasDbManager = NULL;
    
    delete iContext;
    iContext = NULL;
    
    delete iWait;
    iWait = NULL;
    
    iTagArray.ResetAndDestroy();
    
    // Free iPbkContactItem from thread local storage
    Dll::FreeTls();
    
    delete iPbkContactItem;
    iPbkContactItem = NULL;
    
    delete iPbkEngine;
    iPbkEngine = NULL;
    
    iPopupArray.ResetAndDestroy();
    
    delete iPbkHandler;
    
    delete iTts;
    }
    
// ----------------------------------------------------------------------------
// CPbkInfoViewSindHandler::VoiceTagCount
// Returns the number of voice tags for the contact.
// @return TInt voice tag count.
// ----------------------------------------------------------------------------
TInt CPbkInfoViewSindHandler::VoiceTagCount()
    {
    RUBY_DEBUG_BLOCK( "CPbkInfoViewSindHandler::VoiceTagCount" );
    
    return iTagArray.Count();
    }
    
// ----------------------------------------------------------------------------
// CPbkInfoViewSindHandler::VoiceTagL
// Returns a voice tag.
// @param aIndex index for the voice tag. 0 <= aIndex <= VoiceTagCount() - 1
// @return TDesC& the voice tag
// ----------------------------------------------------------------------------
const TDesC& CPbkInfoViewSindHandler::VoiceTagL( TInt aIndex )
    {
    RUBY_DEBUG_BLOCK( "CPbkInfoViewSindHandler::VoiceTagL" );
    
    __ASSERT_ALWAYS( aIndex >= 0 && aIndex < VoiceTagCount(),
                     User::Leave( KErrArgument ) );
    
    return iTagArray[aIndex]->SpeechItem()->Text();
    }
    
// ----------------------------------------------------------------------------
// CPbkInfoViewSindHandler::VoiceTagLabelLC
// Returns the label associated with a voice tag (mobile, home, etc).
// @param aIndex index for the voice tag label.
//        0 <= aIndex <= VoiceTagCount() - 1
// @return TPtrC the value
// ----------------------------------------------------------------------------
HBufC* CPbkInfoViewSindHandler::VoiceTagLabelLC( TInt aIndex )
    {
    RUBY_DEBUG0( "CPbkInfoViewSindHandler::VoiceTagLabelLC START" );
    
    __ASSERT_ALWAYS( aIndex >= 0 && aIndex < VoiceTagCount(),
                     User::Leave( KErrArgument ) );
        
    TInt contactId = iTagArray[aIndex]->RRD()
                         ->IntArray()->At( KVasContactIdRrdLocation );
    TInt fieldId = iTagArray[aIndex]->RRD()
                       ->IntArray()->At( KVasFieldIdRrdLocation );
    
    // RawText is in the format of _1firstName_1lastName_2extension
    // or _2extension _1firstName_1lastName
    HBufC* tagLabel = iTagArray[aIndex]->SpeechItem()->RawText().AllocLC();
    tagLabel->Des().TrimAll();
    
    TInt separatorIndex = tagLabel->Des().Find( KTagExtensionSeparator );
    // Checks whether this a voice tag for nick name (with or without
    // extensions) or for first-last name combination without extension.
    // In this case show the whole tag, just remove separators.
    if( iTagArray[aIndex]->RRD()->IntArray()->At( KVasTagTypeRrdLocation )
        == ETagTypeNickName || separatorIndex == KErrNotFound ) 
        {
        // Remove name separators
        while( (separatorIndex = tagLabel->Des().Find( KTagNameSeparator ) )
               != KErrNotFound )
            {
            tagLabel->Des().Replace( separatorIndex, KTagSeparatorLength,
                                     KSpace );
            }
        // Remove extension separator
        separatorIndex = tagLabel->Des().Find( KTagExtensionSeparator );
        if( separatorIndex != KErrNotFound )
            {
            tagLabel->Des().Replace( separatorIndex, KTagSeparatorLength,
                                     KSpace );
            }
            
        RUBY_DEBUG0( "CPbkInfoViewSindHandler::VoiceTagLabelLC EXIT" );
        return tagLabel;
        }
    
    TInt nameSeparatorIndex = tagLabel->Des().Find( KTagNameSeparator );
    CleanupStack::PopAndDestroy( tagLabel );
    
    // Show the extension only
    
    // RawText is in format _2extension _1firstName_1lastName
    if( nameSeparatorIndex > separatorIndex )
        {
        TInt length( nameSeparatorIndex - separatorIndex - KTagSeparatorLength );
        tagLabel = iTagArray[aIndex]->SpeechItem()->RawText()
                   .Mid( separatorIndex + KTagSeparatorLength, length )
                   .AllocLC();
        }
    // _1firstName_1lastName_2extension
    else
        {
        tagLabel = iTagArray[aIndex]->SpeechItem()->RawText()
                   .Mid( separatorIndex + KTagSeparatorLength ).AllocLC();
        }
    
    RUBY_DEBUG0( "CPbkInfoViewSindHandler::VoiceTagLabelLC EXIT" );
    
    return tagLabel;
    }
    
// ----------------------------------------------------------------------------
// CPbkInfoViewSindHandler::VoiceTagValueL
// Returns the phone number, e-mail address, etc. associated with a voice tag.
// @param aIndex index for the voice tag. 0 <= aIndex <= VoiceTagCount() - 1
// @return TPtrC the value
// ----------------------------------------------------------------------------
TPtrC CPbkInfoViewSindHandler::VoiceTagValueL( TInt aIndex )
    {
    RUBY_DEBUG_BLOCK( "CPbkInfoViewSindHandler::VoiceTagValueL" );
    
    __ASSERT_ALWAYS( aIndex >= 0 && aIndex < VoiceTagCount(),
                     User::Leave( KErrArgument ) );
       
    // By using CVasVPbkHandler, info view will show consistent information with
    // Recognition UI.                    
    iPbkHandler->FindContactFieldL( iTagArray[aIndex] );
    return iPbkHandler->TextL();
    }
    
// ----------------------------------------------------------------------------
// CPbkInfoViewSindHandler::PopupTextL
// Returns the popup text associated with a voice tag.
// @param aIndex index for the voice tag. 0 <= aIndex <= VoiceTagCount() - 1
// @return TDesC& popup text
// ----------------------------------------------------------------------------
TDesC& CPbkInfoViewSindHandler::PopupTextL( TInt aIndex )
    {
    __ASSERT_ALWAYS( aIndex >= 0 && aIndex < VoiceTagCount(),
                     User::Leave( KErrArgument ) );
    __ASSERT_ALWAYS( aIndex >= 0 && aIndex < iPopupArray.Count(),
                     User::Leave( KErrArgument ) );
    
    return *iPopupArray[aIndex];
    }
    
// ----------------------------------------------------------------------------
// CPbkInfoViewSindHandler::PlayVoiceCommandL
// Plays a voice tag.
// @param aIndex index for the voice tag. 0 <= aIndex <= VoiceTagCount() - 1
// ----------------------------------------------------------------------------
void CPbkInfoViewSindHandler::PlayVoiceCommandL( TInt aIndex )
    {
    __ASSERT_ALWAYS( aIndex >= 0 && aIndex < VoiceTagCount(),
                     User::Leave( KErrArgument ) );
    
    if( IsPlaying() )
        {
        CancelPlaybackL();
        }
    iTts->Stop();
    
    iPlaybackIndex = aIndex;
    
    // fetch contact name and contact type (mobile, e-mail etc.)
    // ignore KErrNotFound leave
    HBufC* contact = NULL;
    HBufC* type = NULL;
    TRAPD( err, contact = iTagArray[aIndex]->SpeechItem()->PartialTextL( KNameTrainingIndex ) );
    if ( err != KErrNone && err != KErrNotFound) 
        {
        User::Leave( err );
        }
    CleanupStack::PushL( contact );
    TRAPD( err2, type = iTagArray[aIndex]->SpeechItem()->PartialTextL( KExtensionTrainingIndex ) );
    if ( err2 != KErrNone && err2 != KErrNotFound) 
        {
        User::Leave( err2 );
        }
    CleanupStack::PushL( type );

    // fetch whole tts string 
    const TDesC& text = iTagArray[aIndex]->SpeechItem()->Text();
    TInt contactIndex;
    TInt typeIndex;
    if ( contact && type )
        {
        // get contact and type indexes
        // assume contactIndex is before typeIndex
        contactIndex = text.Find( *contact );
        typeIndex = text.Right( text.Length()-contactIndex-contact->Length() ).Find( *type );
        if ( typeIndex==KErrNotFound )
            {
            // if not, change the order
            typeIndex = text.Find( *type );
            contactIndex = text.Right( text.Length()-typeIndex-type->Length() ).Find( *contact );
            contactIndex = type->Length();
            }
        else
            {
            typeIndex = contact->Length();
            }
        // create segments
        CTtsParsedText* parsedText = CTtsParsedText::NewL();
        CleanupStack::PushL( parsedText );
        
        TTtsStyle* contactStyle = new (ELeave) TTtsStyle();
        CleanupStack::PushL( contactStyle );
        
        TTtsStyleID* contactStyleId = new (ELeave) TTtsStyleID( 0 );
        CleanupStack::PushL( contactStyleId );
        
        TTtsSegment* contactSegment = new (ELeave) TTtsSegment( 0 );
        CleanupStack::PushL( contactSegment );
        
        contactStyle->iLanguage = User::Language();
        *contactStyleId = iTts->AddStyleL( *contactStyle );
        contactSegment->SetStyleID( *contactStyleId );
        
        TTtsStyle* typeStyle = new (ELeave) TTtsStyle();
        CleanupStack::PushL( typeStyle );
        
        TTtsStyleID* typeStyleId = new (ELeave) TTtsStyleID( 0 );
        CleanupStack::PushL( typeStyleId );
        
        TTtsSegment* typeSegment = new (ELeave) TTtsSegment( 0 );
        CleanupStack::PushL( typeSegment );
        
        typeStyle->iLanguage = User::Language();
        *typeStyleId = iTts->AddStyleL( *typeStyle );
        typeSegment->SetStyleID( *typeStyleId );

        if (contactIndex < typeIndex)
            {
            contactSegment->SetTextPtr( text.Mid( contactIndex, contact->Length() ) );
            typeSegment->SetTextPtr( text.Right( text.Length()-typeIndex ) );
            parsedText->AddSegmentL( *contactSegment );
            parsedText->AddSegmentL( *typeSegment );
            }
        else
            {
            typeSegment->SetTextPtr( text.Mid( typeIndex, type->Length() ) );
            contactSegment->SetTextPtr( text.Right( text.Length()-contactIndex ) );
            parsedText->AddSegmentL( *typeSegment );
            parsedText->AddSegmentL( *contactSegment );
            }
        parsedText->SetTextL( text );
        iTts->OpenAndPlayParsedTextL( *parsedText );

        CleanupStack::PopAndDestroy( typeSegment );
        CleanupStack::PopAndDestroy( typeStyleId );
        CleanupStack::PopAndDestroy( typeStyle );
        CleanupStack::PopAndDestroy( contactSegment );
        CleanupStack::PopAndDestroy( contactStyleId );
        CleanupStack::PopAndDestroy( contactStyle );
        CleanupStack::PopAndDestroy( parsedText );
        }
    else
        {
        // if there is only one part (contact), play normally
        iTagArray[aIndex]->SpeechItem()->PlayL( this );
        }
    CleanupStack::PopAndDestroy( 2,contact );
    }
    
// ----------------------------------------------------------------------------
// CPbkInfoViewSindHandler::CancelPlaybackL
// Cancels voice tag playback.
// ----------------------------------------------------------------------------
void CPbkInfoViewSindHandler::CancelPlaybackL()
    {
    if( !( iPlaybackIndex > -1 &&
        iPlaybackIndex < iTagArray.Count() ) )
        {
        return;
        }
        
    iTagArray[iPlaybackIndex]->SpeechItem()->CancelL();
    
    iPlaybackIndex = -1;
    }
    
// ----------------------------------------------------------------------------
// CPbkInfoViewSindHandler::IsPlaying
// Is voice tag playing ongoing.
// @return TBool boolean value
// ----------------------------------------------------------------------------
TBool CPbkInfoViewSindHandler::IsPlaying()
    {
    return iPlaybackIndex > -1;
    }

// ----------------------------------------------------------------------------
// CPbkInfoViewSindHandler::IconIdL
// Returns an icon id for the contact field the voice tag is associated with.
// @param aIndex index for the voice tag. 0 <= aIndex <= VoiceTagCount() - 1
// @return TInt the icon id
// ----------------------------------------------------------------------------
TInt CPbkInfoViewSindHandler::IconIdL( TInt aIndex )
    {
    RUBY_DEBUG_BLOCK( "CPbkInfoViewSindHandler::IconIdL" );
    
    __ASSERT_ALWAYS( aIndex >= 0 && aIndex < VoiceTagCount(),
                     User::Leave( KErrArgument ) );
        
    TInt contactId = iTagArray[aIndex]->RRD()
                         ->IntArray()->At( KVasContactIdRrdLocation );
    TInt fieldId = iTagArray[aIndex]->RRD()
                       ->IntArray()->At( KVasFieldIdRrdLocation );
    
    iPbkContactItemField = iPbkContactItem->FindField( fieldId );
    if ( !iPbkContactItemField )
        {
        iPbkContactItemField = FindDefaultField( iPbkContactItem, fieldId );
        }
    
    if ( !iPbkContactItemField )
       {
       User::Leave( KErrNotFound );
       }
       
    return iPbkContactItemField->IconId();
    }

// ----------------------------------------------------------------------------
// CPbkInfoViewSindHandler::FieldIdL
// Returns an field id for the contact field.
// @param aIndex index for the voice tag. 0 <= aIndex <= VoiceTagCount() - 1
// @return TInt the field id
// ----------------------------------------------------------------------------
TInt CPbkInfoViewSindHandler::FieldIdL( TInt aIndex )
	{    
    RUBY_DEBUG_BLOCK( "CPbkInfoViewSindHandler::FieldIdL" );
    
    __ASSERT_ALWAYS( aIndex >= 0 && aIndex < VoiceTagCount(),
                     User::Leave( KErrArgument ) );
        
    iPbkHandler->FindContactFieldL( iTagArray[aIndex] );
    
	return iPbkHandler->FieldIdL();
	}

// ----------------------------------------------------------------------------
// CPbkInfoViewSindHandler::LabelL
// Returns label for the contact field.
// @param aIndex index for the voice tag. 0 <= aIndex <= VoiceTagCount() - 1
// @return TPtrC the tag label
// ----------------------------------------------------------------------------
TPtrC CPbkInfoViewSindHandler::LabelL( TInt aIndex )
    {
    RUBY_DEBUG_BLOCK( "CPbkInfoViewSindHandler::LabelL" );
    
    __ASSERT_ALWAYS( aIndex >= 0 && aIndex < VoiceTagCount(),
                     User::Leave( KErrArgument ) );
        
    iPbkHandler->FindContactFieldL( iTagArray[aIndex] );
    
    return iPbkHandler->LabelL();
    }

// ----------------------------------------------------------------------------
// CPbkInfoViewSindHandler::CreateVoiceTagListL
// Fetches voice tag list from vas db.
// @param aContactId Contact id for the contact whose voice tags are
//        fetched from vas db.
// ----------------------------------------------------------------------------
void CPbkInfoViewSindHandler::CreateVoiceTagListL( TInt aContactId )
    {
    RUBY_DEBUG_BLOCK( "CPbkInfoViewSindHandler::CreateVoiceTagListL" );
    
    // Get context
    TInt err = iContextManager->GetContext( this, KVoiceDialContext );
    if ( err == KErrNone )
        {            
        if ( iWait && !iWait->IsStarted() )
            {
            // Let's synchronize GetContext-method.
            iWait->Start();
            if ( !iContext )
                {
                // the context getting has failed.
                // cannot continue
                User::Leave( KErrGeneral );
                }
                
            }
        else
            {
            User::Leave( KErrGeneral );
            }
        }
    else
        {
        User::Leave( KErrGeneral );
        }

    iErr = iTagManager->GetTagList( this, iContext, 
                                    aContactId, KVasContactIdRrdLocation );

    if( !iErr )
        {
        // Let's synchronize the GetTagList call
        if ( iWait && !iWait->IsStarted() )
            {
            iWait->Start();
            RUBY_DEBUG1( "CPbkInfoViewSindHandler::CreateVoiceTagListL - iErr=[%d]", iErr );
            }
        else
            {
            User::Leave( KErrGeneral );
            }        
        }
    }
    
// ----------------------------------------------------------------------------
// CPbkInfoViewSindHandler::CreatePopupArrayL
// Creates an array for popups in info view list.
// @param aContactId Contact id for the contact whose voice tags are
//        fetched from vas db.
// @return TInt KErrNone on success
// ----------------------------------------------------------------------------
void CPbkInfoViewSindHandler::CreatePopupArrayL()
    {
    RUBY_DEBUG_BLOCK( "CPbkInfoViewSindHandler::CreatePopupArrayL" );
    
    // Create popup text for each voice tag
    for( TInt i( 0 ); i < iTagArray.Count(); i++ )
        {
        HBufC* voiceTag( iTagArray[i]->SpeechItem()->RawText().AllocLC() );
        
        HBufC* resourceText;
        TInt index = voiceTag->Des().Find( KTagExtensionSeparator );
        // Checks whether this a voice tag without an extension
        if( index == KErrNotFound )
            {
            resourceText = StringLoader::LoadLC( R_INFOVIEW_POPUP_CONTACT );
            }
        else
            {
            resourceText = StringLoader::LoadLC( R_INFOVIEW_POPUP_GENERAL );
            }
            
        // Remove name separators
        while( (index = voiceTag->Des().Find( KTagNameSeparator ) )
               != KErrNotFound )
            {
            voiceTag->Des().Replace( index, KTagSeparatorLength,
                                     KSpace );
            }
        // Remove extension separator
        index = voiceTag->Des().Find( KTagExtensionSeparator );
        if( index != KErrNotFound )
            {
            voiceTag->Des().Replace( index, KTagSeparatorLength,
                                     KSpace );
            }
        voiceTag->Des().TrimAll();
        
        TName popupText;
        // Insert voiceTag to resourceText to create popupText
        StringLoader::Format( popupText, *resourceText, -1, *voiceTag );
        
        iPopupArray.AppendL( popupText.AllocL() );
        
        //CleanupStack::Pop( popupText );
        CleanupStack::PopAndDestroy( resourceText );
        CleanupStack::PopAndDestroy( voiceTag );
        }
    }

// ----------------------------------------------------------------------------
// CPbkInfoViewSindHandler::GetContextCompleted
// 
// ----------------------------------------------------------------------------
void CPbkInfoViewSindHandler::GetContextCompleted( MNssContext* aContext,
                                                   TInt aErrorCode )
    {
    if( aErrorCode == KErrNone )
        {
        // ownership is transferred
        iContext = aContext;

        // if the iWait has been started, the completion of GetContext method
        // has been synchronized in VoiceTagField - method. Otherwise
        // the completion has happened in background.
        if ( iWait && iWait->IsStarted() )
            {
            iWait->AsyncStop();
            }        
        }
    else
        {
        iErr = aErrorCode;
        // if the get context has been synchronized:
        if ( iWait && iWait->IsStarted() )
            {
            iWait->AsyncStop();
            }        
        }
    }

// ----------------------------------------------------------------------------
// CPbkInfoViewSindHandler::GetContextListCompleted
// Callback, getting of contexts has completed successfully.
// ----------------------------------------------------------------------------
void CPbkInfoViewSindHandler::GetContextListCompleted(
    MNssContextListArray* /*aContextList*/, TInt /*aErrorCode*/ )
    {
    // never comes here, defined as pure virtual in MNssGetContextClient.
    }

// ----------------------------------------------------------------------------
// CPbkInfoViewSindHandler::GetTagListCompleted
// Callback, getting of tags has completed successfully.
// ----------------------------------------------------------------------------
void CPbkInfoViewSindHandler::GetTagListCompleted( MNssTagListArray* aTagList,
                                                   TInt aErrorCode )
    {
    RUBY_DEBUG0( "CPbkInfoViewSindHandler::GetTagListCompleted" );
    
    if( aErrorCode == KErrNone )
        {
        // Fill the tag array
        for( TInt i( 0 ); i < aTagList->Count(); i++ )
            {
            MNssTag* tag = aTagList->At( i );
    
            TInt fieldId = tag->RRD()->IntArray()->At( KVasFieldIdRrdLocation );
            
            // Get pointer to CPbkContactItem-object from thread local storage
            CPbkContactItem* pbkContactItem =
                static_cast<CPbkContactItem*>( Dll::Tls() );
    
            TPbkContactItemField* field = pbkContactItem->FindField( fieldId );
            if ( !field )
                {
                field = FindDefaultField( pbkContactItem, fieldId );
                }
            
            // Add only tags that have field in phonebook
            if ( field )
                {
                iTagArray.Append( tag );
                }
            else
                {
                // If some tag is missing then we don't want to show anything
                iTagArray.ResetAndDestroy();
                break;
                }
            }
            
        aTagList->Reset();
        delete aTagList;
            
        iTagArray.Sort( TLinearOrder<MNssTag>( CPbkInfoViewSindHandler::Compare ) );
        
        // Continue with CreateVoiceTagListL-method.
        if ( iWait && iWait->IsStarted() )
            {
            iWait->AsyncStop();
            }        
        }
    
    else // aErrorCode != KErrNone
        {
        iErr = aErrorCode;
        if ( iWait && iWait->IsStarted() )
            {
            iWait->AsyncStop();
            }        
        }
    }
    
// -----------------------------------------------------------------------------
// CPbkInfoViewSindHandler::HandlePlayComplete
// Callback, playback has been completed.
// -----------------------------------------------------------------------------
//         
void CPbkInfoViewSindHandler::HandlePlayComplete( TNssPlayResult /*aResult*/ )
    {
    iPlaybackIndex = -1;        
    }

    
// -----------------------------------------------------------------------------
// Calls CompareL and catches possible leaves
// -----------------------------------------------------------------------------
//     
TInt CPbkInfoViewSindHandler::Compare( const MNssTag& aTag1, const MNssTag& aTag2 )
    {
    TInt result = 0;
    
    TRAP_IGNORE( result = CompareL( aTag1, aTag2 ) );
        
    return result;
    }
    
// -----------------------------------------------------------------------------
// Compares two voice tags and decides which one should be displayed first in
// info view list. Used in the sorting of voice tags.
// @param aTag1 A voice tag
// @param aTag2 A voice tag
// @return TInt -1 if aTag1 should be displayed first, 1 if aTag2 should be 
//         displayed first, 0 otherwise.
// -----------------------------------------------------------------------------
//     
TInt CPbkInfoViewSindHandler::CompareL( const MNssTag& aTag1, const MNssTag& aTag2 )
    {
    RUBY_DEBUG_BLOCK( "CPbkInfoViewSindHandler::GetTagListCompleted" );
    
    MNssTag& tag1 = const_cast<MNssTag&>( aTag1 );
    MNssTag& tag2 = const_cast<MNssTag&>( aTag2 );
    
    TInt tagType1 = tag1.RRD()->IntArray()->At( KVasTagTypeRrdLocation );
    TInt tagType2 = tag2.RRD()->IntArray()->At( KVasTagTypeRrdLocation );
    
    // Check if one of the tags is for nickname
    if( ( tagType1 == ETagTypeName || tagType1 == ETagTypeCompanyName ) &&
        tagType2 == ETagTypeNickName )
        {
        return -1;
        }
    if( ( tagType2 == ETagTypeName || tagType2 == ETagTypeCompanyName ) &&
        tagType1 == ETagTypeNickName )
        {
        return 1;
        }
        
    TInt fieldId1 = tag1.RRD()->IntArray()->At( KVasFieldIdRrdLocation );
    TInt fieldId2 = tag2.RRD()->IntArray()->At( KVasFieldIdRrdLocation );
    
    // Get pointer to CPbkContactItem-object from thread local storage
    CPbkContactItem* pbkContactItem =
        static_cast<CPbkContactItem*>( Dll::Tls() );
    
    TPbkContactItemField* field1 = pbkContactItem->FindField( fieldId1 );
    if ( !field1 )
        {
        field1 = FindDefaultField( pbkContactItem, fieldId1 );
        }
    TPbkContactItemField* field2 = pbkContactItem->FindField( fieldId2 );
    if ( !field2 )
        {
        field2 = FindDefaultField( pbkContactItem, fieldId2 );
        }
        
    if ( !field1 || !field2 )
        {
        User::Leave( KErrNotFound );
        }
    
    TBool noExtension1 = EFalse;
    TBool noExtension2 = EFalse;
    
    HBufC* label = tag1.SpeechItem()->RawText().AllocLC();
    label->Des().TrimAll();
    
    TInt index = label->Des().Find( KTagExtensionSeparator );
    // Checks whether this a voice tag without extension
    if( index == KErrNotFound )
        {
        noExtension1 = ETrue;
        }
        
    CleanupStack::PopAndDestroy( label );
    
    label = tag2.SpeechItem()->RawText().AllocLC();
    label->Des().TrimAll();
    
    index = label->Des().Find( KTagExtensionSeparator );
    // Checks whether this a voice tag without extension
    if( index == KErrNotFound )
        {
        noExtension2 = ETrue;
        }
        
    CleanupStack::PopAndDestroy( label );
            
    if( noExtension1 && !noExtension2 )
        {
        return -1;
        }
    else if ( !noExtension1 && noExtension2 )    
        {
        return 1;
        }
            
//    // compare the two neighbors
//    TInt ret = field1->Compare( *field2 );
//    
//    // Sorting for normal call, sms, and multimedia msg fields
//    if ( ret == 0)
//        {
//        TInt actionId1 = tag1.RRD()->IntArray()->At( KVasExtensionRrdLocation );
//        TInt actionId2 = tag2.RRD()->IntArray()->At( KVasExtensionRrdLocation );
//        
//        if ( actionId1 != actionId2 )
//            {
//            if ( actionId1 < actionId2 )
//                {
//                return -1;
//                }
//            return 1;
//            }
//        }
//        
//    return ret;
    TInt cmdId1 = tag1.RRD()->IntArray()->At( KVasExtensionCommandRrdLocation );
    TInt cmdId2 = tag2.RRD()->IntArray()->At( KVasExtensionCommandRrdLocation );
    return ( RemapCommand( cmdId1 ) - RemapCommand( cmdId2 ));    
    }

// -----------------------------------------------------------------------------
// Tries to find a default contact item field
// @param aContactItem A contact item
// @param aFieldId A field to search
// @return TPbkContactItemField* Default field if found, otherwise NULL
// -----------------------------------------------------------------------------
//      
TPbkContactItemField* CPbkInfoViewSindHandler::FindDefaultField
        ( CPbkContactItem* aContactItem, TInt aFieldId )
    {
    TPbkContactItemField* field = NULL;
    
    if ( aFieldId == EPbkFieldIdPhoneNumberVideo )
        {
        field = aContactItem->DefaultVideoNumberField();
        }
    else if ( aFieldId == EPbkFieldIdEmailAddress )
        {
        field = aContactItem->DefaultEmailField();
        }
    else if ( aFieldId == EPbkFieldIdVOIP )
        {
        field = aContactItem->DefaultVoipField();
        }

    return field;                    
    }

// End of file
