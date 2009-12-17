/*
* Copyright (c) 2004-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  This class handles the storage and access of user trained models
*               and utterances into a database.  It performs checksum verification
*               for the stored data.  It is also responsible for allocating memory
*               when loading the models into the recognizer.
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <e32base.h>
#include <f32file.h>
#include "siresourcehandler.h"
#include "asrplugindataloader.h"
#include "simodelbankdb.h"
#include "rubydebug.h"

// CONSTANTS
const TInt KModelBankPackageType=3;
const TInt KModelBankLanguageType=0;
const TInt KModelBankStartPos=0;

// _LIT(KModelFileName,"c:\\system\\Apps\\model_data\\qhmms_fxp"); // Commented out to remove "not referenced" warning
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CSIModelBankDB::CSIModelBankDB
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSIModelBankDB::CSIModelBankDB( RDbNamedDatabase& aDatabase, 
                                RDbs& aDbSession,
                                TInt aDrive )
                               : CSICommonDB(aDatabase, aDbSession, aDrive ),
                                 iAcousticModelPtr(0,0,0)
    {
    }

 // -----------------------------------------------------------------------------
// CSIModelBankDB::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSIModelBankDB::ConstructL()
    {
    RUBY_DEBUG_BLOCK( "CSIModelBankDB::ConstructL" );

    // Resource handler 
    CSIResourceHandler* resourceHandler = CSIResourceHandler::NewL();
    CleanupStack::PushL(resourceHandler);
    //  Data loader
    iDataLoader = CDataLoader::NewL( 
        *(resourceHandler->iDataFilenamePrefix), 
        *(resourceHandler->iDataFilenameSeperator),
        *(resourceHandler->iDataFilenamePostfix) );
    CleanupStack::PopAndDestroy(1); //resourceHandler
    }

// -----------------------------------------------------------------------------
// CSIModelBankDB::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSIModelBankDB* CSIModelBankDB::NewL( RDbNamedDatabase& aDatabase, 
                                      RDbs& aDbSession,
                                      TInt aDrive )
    {
    RUBY_DEBUG_BLOCK( "CSIModelBankDB::NewL" );
    
    CSIModelBankDB* self 
        = new( ELeave ) CSIModelBankDB( aDatabase, aDbSession, aDrive );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();	
    return self;
    }

// -----------------------------------------------------------------------------
// CSIModelBankDB::~CSIModelBankDB
// Destructor
// -----------------------------------------------------------------------------
//
CSIModelBankDB::~CSIModelBankDB()
{
	// Delete all elements of the array before deleting the array
	iModelBankArray.ResetAndDestroy();
	iModelBankArray.Close();
	
 	delete iDataLoader;
}

// -----------------------------------------------------------------------------
// CSIModelBankDB::GetAllAcousticModelsL
// 
// -----------------------------------------------------------------------------
//
const CSIModelBank* CSIModelBankDB::GetAllAcousticModelsL( TSIModelBankID aModelBankID ) 
    {
    
    // Construct the table name using the provided model bank ID
    TBuf<40> KModelBankName(KSIModelBankTable);
    KModelBankName.AppendNumUC(aModelBankID);
    
    TBuf<100> KSQLStatement;
    // Declare a literal string to hold the SQL statement
    // SELECT  KHmmColumn, KHmmChecksumColumn FROM KModelBankName
    _LIT(KSQLSelect1, "SELECT ");
    _LIT(KSQLSelect2, " FROM ");
    
    KSQLStatement = KSQLSelect1;
    KSQLStatement.Append(KHmmColumn);
    KSQLStatement.Append(KNext);
    KSQLStatement.Append(KHmmChecksumColumn);
    KSQLStatement.Append(KSQLSelect2);
    KSQLStatement.Append(KModelBankName);
    
    RDbView view;
    CleanupClosePushL(view);
    User::LeaveIfError(view.Prepare(iDb, TDbQuery(KSQLStatement, EDbCompareNormal)));
    User::LeaveIfError(view.EvaluateAll());
    
    // Get column set for column ordinals
    CDbColSet* columns = view.ColSetL();
    TDbColNo hmm_col = columns->ColNo(KHmmColumn);
    TDbColNo hmmchecksum_col = columns->ColNo(KHmmChecksumColumn);
    delete columns;
    
    // Create a model bank object
    CSIModelBank* modelBank = CSIModelBank::NewLC(aModelBankID);
    
    TSIModelID modelID=KModelID;
    TInt length;
    TInt32 checksum;
    // After evaluation, the first call to NextL() is equivalent to FirstL()
    while ( view.NextL() )
        {
        // Retrieve the current row
        view.GetL();
        length = view.ColLength(hmm_col);
        checksum = view.ColUint32(hmmchecksum_col);
        
        // Read the acoustic model into a buffer
        TUint8* hmmBuf = new (ELeave) TUint8[length];
        CleanupStack::PushL(hmmBuf);
        TPtr8 hmmPtr(hmmBuf, 0, length);
        RDbColReadStream colReadStrm;
       	colReadStrm.OpenLC(view, hmm_col);
       	colReadStrm.ReadL(hmmPtr, length);
        // Cleanup colReadStrm
        CleanupStack::PopAndDestroy();
        
        if ( VerifyChecksum( hmmPtr, checksum) )
            {
            CSIModel* model = CSIModel::NewLC(modelID);
            HBufC8* aAcousticModel=HBufC8::NewL(length); 
            *aAcousticModel=hmmPtr;
            model->SetAcousticModel(aAcousticModel);
            modelBank->AddL((CSIModel*)model);
            //!
            // Cleanup aAcousticModel and hmmBuf
            CleanupStack::Pop();
            CleanupStack::PopAndDestroy();
            
            }
        else
            {
            User::Leave( KErrCorrupt );
            }
        }
    
    // Keep the reference of the modelBank
    //User::LeaveIfError(iModelBankArray.Append(modelBank));
    // Cleanup modelBank
    CleanupStack::Pop();
    
    // Cleanup view
    CleanupStack::PopAndDestroy();
    
    return modelBank;
    }

// -----------------------------------------------------------------------------
// CSIModelBankDB::AllAcousticModelsL
// Loads all acoustic models within the specified model bank into a model bank
// object, which is loaded into recognizer during recognition session.
// -----------------------------------------------------------------------------
//
const CSIModelBank* CSIModelBankDB::AllAcousticModelsL( TSIModelBankID aModelBankID )
    {
    // Construct the table name using the provided model bank ID
    TBuf<40> KModelBankName(KSIModelBankTable);
    KModelBankName.AppendNumUC(aModelBankID);
    
    TBuf<100> KSQLStatement;
    // Declare a literal string to hold the SQL statement
    // SELECT  KHmmColumn, KHmmChecksumColumn FROM KModelBankName
    _LIT(KSQLSelect1, "SELECT * ");
    _LIT(KSQLSelect2, " FROM ");
    
    KSQLStatement = KSQLSelect1;
    //	KSQLStatement.Append(KHmmColumn);
    //	KSQLStatement.Append(KNext);
    //	KSQLStatement.Append(KHmmChecksumColumn);
    KSQLStatement.Append(KSQLSelect2);
    KSQLStatement.Append(KModelBankName);
    RDbView view;
    CleanupClosePushL(view);
    User::LeaveIfError(view.Prepare(iDb, TDbQuery(KSQLStatement, EDbCompareNormal)));
    User::LeaveIfError(view.EvaluateAll());
    
    // Get column set for column ordinals
    CDbColSet* columns = view.ColSetL();
    TDbColNo hmm_col = columns->ColNo(KHmmColumn);
    TDbColNo hmmchecksum_col = columns->ColNo(KHmmChecksumColumn);
    delete columns;
    
    // Create a model bank object
    CSIModelBank* modelBank = CSIModelBank::NewLC(aModelBankID);
    
    TSIModelID modelID=KModelID;
    TInt length;
    TInt32 checksum;
    // After evaluation, the first call to NextL() is equivalent to FirstL()
    while ( view.NextL() )
        {
        // Retrieve the current row
        view.GetL();
        length = view.ColLength(hmm_col);
        checksum = view.ColUint32(hmmchecksum_col);
        
        // Read the acoustic model into a buffer
        TUint8* hmmBuf = new (ELeave) TUint8[length];
        CleanupStack::PushL(hmmBuf);
        TPtr8 hmmPtr(hmmBuf, 0, length);
        RDbColReadStream colReadStrm;
       	colReadStrm.OpenLC(view, hmm_col);
       	colReadStrm.ReadL(hmmPtr, length);
        // Cleanup colReadStrm
        CleanupStack::PopAndDestroy();

#ifdef __SIND_HMMS_TO_FILE
        {
        TInt error( KErrNone );
        RFile sampleFile; 
        RFs fileSession; 
        User::LeaveIfError( fileSession.Connect() );

        _LIT( KCDrive,"c:\\" ); 
        _LIT( KFileName, "data\\rec\\hmm_%08d.raw" ); 
        _LIT( KStartOfYear, "20060101:000000.000000" ); 
        TTime startOfYear( KStartOfYear ); 
        TTimeIntervalSeconds secondsSince; 
        TTime nowtime; 
        nowtime.UniversalTime(); 
        nowtime.SecondsFrom( startOfYear, secondsSince );

        TFileName fileName; 
        fileName.Append( KCDrive ); 
        fileName.AppendFormat( KFileName, secondsSince.Int() ); 
        error = sampleFile.Replace( fileSession, fileName, EFileWrite );
        if ( !error ) 
            {
            TPtr8 ptr( hmmBuf, length ); 
            ptr.SetLength( length ); 
            sampleFile.Write( ptr );
            sampleFile.Close();
            }   
        fileSession.Close();
        }
#endif
        
        if ( VerifyChecksum( hmmPtr, checksum) )
            {
            CSIModel* model = CSIModel::NewLC(modelID);
            HBufC8* aAcousticModel=HBufC8::NewL(length); 
            *aAcousticModel=hmmPtr;
            SetAcousticModelL(aAcousticModel->Des());
            model->SetAcousticModel(aAcousticModel);
            modelBank->AddL((CSIModel*)model);
            //!
            // Cleanup aAcousticModel and hmmBuf
            CleanupStack::Pop();
            CleanupStack::PopAndDestroy();
            }
        else
            {
            User::Leave( KErrCorrupt );
            }
        }
    
    // Keep the reference of the modelBank
    User::LeaveIfError(iModelBankArray.Append(modelBank));
    // Cleanup modelBank
    CleanupStack::Pop();
    // Cleanup view
    CleanupStack::PopAndDestroy();
    
    return modelBank;
    }

// -----------------------------------------------------------------------------
// CSIModelBankDB::AllModelCountL
// Returns the total number of models in the system (all model banks).
// -----------------------------------------------------------------------------
//
TInt CSIModelBankDB::AllModelCountL()
    {
    TBuf<100> KSQLStatement;
    // Declare a literal string to hold the SQL statement
    // SELECT KCounterColumn FROM KModelBankIdTable WHERE KUsedColumn = KUsed
    _LIT(KSQLSelect1, "SELECT ");
    _LIT(KSQLSelect2, " FROM ");
    _LIT(KSQLSelect3, " WHERE ");
    
    KSQLStatement = KSQLSelect1;
    KSQLStatement.Append(KCounterColumn);
    KSQLStatement.Append(KSQLSelect2);
    KSQLStatement.Append(KModelBankIdTable);
    KSQLStatement.Append(KSQLSelect3);
    KSQLStatement.Append(KUsedColumn);
    KSQLStatement.Append(KEqual);
    KSQLStatement.AppendNum(KUsed);
    
    RDbView view;
    CleanupClosePushL(view);
    User::LeaveIfError(view.Prepare(iDb, TDbQuery(KSQLStatement, EDbCompareNormal)));
    User::LeaveIfError(view.EvaluateAll());
    
    // Get column set for column ordinals
    CDbColSet* columns = view.ColSetL();
    
    delete columns;
    
    TInt counter = 0;
    // After evaluation, the first call to NextL() is equivalent to FirstL()
    while ( view.NextL() )
        {
        // Retrieve the current row
        view.GetL();
        //counter += view.ColInt(counter_col);
        //!
        counter ++;//=view.ColInt(used_col); 
        }
    
    // Cleanup view
    CleanupStack::PopAndDestroy();
    
    return counter;
    }

// -----------------------------------------------------------------------------
// CSIModelBankDB::CreateIDTableL
// Creates a new model bank ID table in the database.
// -----------------------------------------------------------------------------
//
void CSIModelBankDB::CreateIDTableL()
    {
    // Invoke function in the base class CSICommonDB.
    CSICommonDB::CreateIDTableL(KModelBankIdTable, KModelBankIdColumn, KModelBankIndex);
    };

// -----------------------------------------------------------------------------
// CSIModelBankDB::CreateModelBankL
// Creates a new model bank table in the database.
// -----------------------------------------------------------------------------
//
TSIModelBankID CSIModelBankDB::CreateModelBankL( TUid aClientUid )
    {
    RUBY_DEBUG_BLOCK( "CSIModelBankDB::CreateModelBankL" );
    
    // Model bank is already exist in the 
    RArray<TSIModelBankID> aExistModelBankIDs;
    CleanupClosePushL( aExistModelBankIDs );
    GetAllClientModelBankIDsL(aClientUid,aExistModelBankIDs);
    
    
    if(aExistModelBankIDs.Count()>0) {
        TSIModelBankID aExistModelBankID=aExistModelBankIDs[0];	
        CleanupStack::PopAndDestroy( &aExistModelBankIDs );
        return aExistModelBankID;
        }
    CleanupStack::PopAndDestroy( &aExistModelBankIDs );
    
    User::LeaveIfError( iDbSession.ReserveDriveSpace( iDrive, sizeof( CSIModelBank ) ) );
    
    // Generate a new model bank ID
    TSIModelBankID modelBankID = STATIC_CAST(TSIModelBankID,CreateNewIDL(KModelBankIdTable, KModelBankIdColumn, aClientUid));
    // Construct the table name using the new model bank ID
    TBuf<40> KModelBankName(KSIModelBankTable);
    KModelBankName.AppendNumUC(modelBankID);
    // Create a table definition
    CDbColSet* columns=CDbColSet::NewLC();
    
    // add the columns  
    // 1) Binary data, 2) binary data size
    columns->AddL(TDbCol(KHmmColumn,EDbColLongBinary));
    columns->AddL(TDbCol(KHmmChecksumColumn,EDbColUint32));
    
    // Create a table
    TInt err =iDb.CreateTable(KModelBankName,*columns);
    
    if ( err != KErrNone )
        {
        // Failed to create the table.
        // Need to release the new ModelBank ID and leave.
        ReleaseIdL(KModelBankIdTable, KModelBankIdColumn, modelBankID);
        User::Leave(err);
        }
    
    // cleanup the column set
    CleanupStack::PopAndDestroy();
    
    // Put and empty Modelbank inside
        {
        //!!! Use the default model file to construct	
        //!
        HBufC8 *bf=NULL;		
        bf= iDataLoader->LoadData( KModelBankPackageType,KModelBankLanguageType,KModelBankStartPos,KMaxTUint32);
        // Modelbank file does not exist;
        if(bf==NULL)
            User::Leave(KErrNotFound);
        CleanupStack::PushL(bf);
        
        iAcousticModelPtr.Set(bf->Des());
        //.Set(_L8("empty"));
        //iAcousticModelPtr=(_L8("empty"));
        // Always 1 for each model bank
        //TSIModelID modelID=KModelID; 
        
        TInt32 hmmChecksum = CalculateChecksum(iAcousticModelPtr);
        
        // Open model bank table
        RDbTable dbTable;		// Provides access to table data as a rowset
        User::LeaveIfError(dbTable.Open(iDb, KModelBankName, RDbTable::EUpdatable));
        CleanupClosePushL(dbTable);
        
        // Get column set for column ordinals
        columns = dbTable.ColSetL();
        TDbColNo hmm_col = columns->ColNo(KHmmColumn);
        TDbColNo hmmchecksum_col = columns->ColNo(KHmmChecksumColumn);
        delete columns;
        
        // 		 No, insert new row
        dbTable.InsertL();
        dbTable.SetColL(hmm_col, iAcousticModelPtr);
        
        dbTable.SetColL(hmmchecksum_col, hmmChecksum);
        
        // Write the updated row
        TRAPD(err, dbTable.PutL());
        if( err != KErrNone )
            {
            // Error: cancel update
            dbTable.Cancel();
            dbTable.Reset();
            User::Leave(err);
            }
        
        // Cleanup bf dbTable 
        CleanupStack::PopAndDestroy(2);
        // Free up resources
        Reset();
        }
        
    iDbSession.FreeReservedSpace( iDrive );
        
    //AllAcousticModelsL(modelBankID);
    return modelBankID;
    }

// -----------------------------------------------------------------------------
// CSIModelBankDB::GetAllClientModelBankIDsL
// This function returns all model bank IDs owned by the specified client.
// -----------------------------------------------------------------------------
//
void CSIModelBankDB::GetAllClientModelBankIDsL( TUid aClientUid,
                                                RArray<TSIModelBankID>& aModelBankIDs )
    {
    // This is a hack to get the id aligned to 4-byte boundary,
    // for some reason this does not happen in winscw build if 
    // TSIModelBankID is taken from stack.
    TSIModelBankID* id = new (ELeave) TSIModelBankID;
    CleanupStack::PushL( id );
    
    RArray<TUint32> ix;
    ix.Reset();
    GetAllClientIDsL(KModelBankIdTable, KModelBankIdColumn, aClientUid, ix);
    for(TInt i=0;i<ix.Count();i++) 
        {
        *id = STATIC_CAST( TSIModelBankID, ix[i] );
        aModelBankIDs.Append( *id );
        }
    ix.Close();
    CleanupStack::PopAndDestroy( id );
    }

// -----------------------------------------------------------------------------
// CSIModelBankDB::GetAllModelBankIDsL
// This function returns all model bank IDs in the database.
// -----------------------------------------------------------------------------
//
void CSIModelBankDB::GetAllModelBankIDsL( RArray<TSIModelBankID>& aModelBankIDs )
    {
    // This is a hack to get the id aligned to 4-byte boundary,
    // for some reason this does not happen in winscw build if 
    // TSIModelBankID is taken from stack.
    TSIModelBankID* id = new ( ELeave ) TSIModelBankID;
    CleanupStack::PushL( id );
    
    RArray<TUint32> ix;
    ix.Reset();
    GetAllIDsL( KModelBankIdTable, KModelBankIdColumn, ix );
    for ( TInt i = 0; i < ix.Count(); i++ ) 
        { 
        *id = STATIC_CAST( TSIModelBankID, ix[i] );
        aModelBankIDs.Append( *id );
        }
    ix.Close();
    CleanupStack::PopAndDestroy( id );
    }

// -----------------------------------------------------------------------------
// CSIModelBankDB::GetAllModelIDsL
// This function returns all model IDs within the specified model bank. It's
// expected that the client has already done allocation on aModelIDs array object.
// -----------------------------------------------------------------------------
//
void CSIModelBankDB::GetAllModelIDsL( TSIModelBankID /*aModelBankID*/,
                                     RArray<TSIModelID>& aModelIDs )
    {
    // Only one id , and it is KModelID
    aModelIDs.Append(KModelID);	
    }
	
// -----------------------------------------------------------------------------
// CSIModelBankDB::IsModelBankValidL
// Checks if the specified model bank table exists in the database and also verify
// ownership.
// -----------------------------------------------------------------------------
//
TBool CSIModelBankDB::IsModelBankValidL( TUid aClientUid,
                                         TSIModelBankID aModelBankID )
    {
    VerifyOwnershipL(aClientUid, KModelBankIdTable, KModelBankIndex, aModelBankID);
    return ETrue;
    }

// -----------------------------------------------------------------------------
// CSIModelBankDB::IsModelValidL
// Checks if the specified model exists in the specified model bank table.
// -----------------------------------------------------------------------------
//
TBool CSIModelBankDB::IsModelValidL( TSIModelBankID /*aModelBankID*/,
                                     TSIModelID aModelID )
    {
    // Assume that ModelBankd ID is valid
    if (aModelID==KModelID)
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }
    
// -----------------------------------------------------------------------------
// CSIModelBankDB::UpdateModelBankIfInvalidL
// Validate the adapted model bank against ROM version and update if not valid.
// -----------------------------------------------------------------------------
//
void CSIModelBankDB::UpdateModelBankIfInvalidL( TSIModelBankID aModelBankID )
    {    
    RUBY_DEBUG_BLOCK( "CSIModelBankDB::UpdateModelBankIfInvalidL" );
    
    const TUint KModelBankHeaderSize(20);
    
    HBufC8 *bf = iDataLoader->LoadData( KModelBankPackageType,
                                        KModelBankLanguageType,
                                        KModelBankStartPos,
                                        KMaxTUint32 );
    
    if( bf == NULL )
        {
        // Modelbank file does not exist;
        User::Leave( KErrNotFound );
        }
        
    CleanupStack::PushL( bf );
    
    TPtr8 origModelPtr( bf->Des() );

    // Compare phoneme definitions
    TBool areEqual( EFalse );
    
    TRAPD( error, 
        const CSIModelBank* adaptedModelBank = GetAllAcousticModelsL( aModelBankID );
        CleanupStack::PushL( (CSIModelBank*)adaptedModelBank );

        TPtrC8 adaptedModelPtr = adaptedModelBank->AtL(0).AcousticModel();
        
        // 1. Compare size of phoneme definition
        TInt adaptedSize = adaptedModelPtr[18];
        adaptedSize |= adaptedModelPtr[19] << 8;
            
        TInt origSize = origModelPtr[18];
        origSize |= origModelPtr[19] << 8;
        
        if ( adaptedSize == origSize )
            {
            // 2. compare content of phoneme definitions
            TInt size = origSize + KModelBankHeaderSize;
            if ( Mem::Compare( origModelPtr.Ptr(), size, adaptedModelPtr.Ptr(), size ) == 0 )
                {
                areEqual = ETrue;
                }
            }

        CleanupStack::PopAndDestroy( (CSIModelBank*)adaptedModelBank );
    );
    
    // GetAllAcousticModelsL will leave with KErrCorrupt if checksum doesn't match
    // This case should be handled by updating model bank
    if ( error != KErrNone && error != KErrCorrupt )
        {
        User::Leave( error );
        }
    
    if ( !areEqual )
        {
        // replace the adapted model bank with the original model bank
        RUBY_DEBUG0("phoneme definitions changed or model bank corrupted, update models" );
        
        SetAcousticModelL( origModelPtr );
        SaveModelL( aModelBankID );
        }

    CleanupStack::PopAndDestroy( bf );
    }
    
// -----------------------------------------------------------------------------
// CSIModelBankDB::GetUtteranceDurationL
//
// -----------------------------------------------------------------------------
//
void CSIModelBankDB::GetUtteranceDurationL( TSIModelBankID /*aModelBankID*/, 
                                            TSIModelID /*aModelID*/,
                                            TTimeIntervalMicroSeconds32& /*aDuration*/ )
    {
    User::Leave(KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// CSIModelBankDB::ModelCountL
// Returns the number of models in the specified model bank.
// -----------------------------------------------------------------------------
//
TInt CSIModelBankDB::ModelCountL( TSIModelBankID /*aModelBankID*/ )
    {
    
    //User::Leave(KErrNotSupported);
    return 1;
    // one model per modelbank by defination
    }

// -----------------------------------------------------------------------------
// CSIModelBankDB::RemoveModelBankL
// Deletes a model bank table from the database.
// -----------------------------------------------------------------------------
//
void CSIModelBankDB::RemoveModelBankL( TUid aClientUid,
                                       TSIModelBankID aModelBankID )
    {
    VerifyOwnershipL(aClientUid, KModelBankIdTable, KModelBankIndex, aModelBankID);
    
    TInt diskSpace = ( ModelCountL(aModelBankID) * sizeof( CSIModel ) )
        + sizeof( CSIModelBank );
    User::LeaveIfError( iDbSession.ReserveDriveSpace( iDrive, diskSpace ) );
    
    // Construct the table name using the provided model bank ID
    TBuf<40> KModelBankName(KSIModelBankTable);
    KModelBankName.AppendNumUC(aModelBankID);
    
    TBuf<50> KSQLStatement;
    // Declare a literal string to hold the SQL statement
    // DROP TABLE KModelBankName
    _LIT(KSQLDelete1, "DROP TABLE ");
    
    KSQLStatement = KSQLDelete1;
    KSQLStatement.Append(KModelBankName);
    
    User::LeaveIfError(iDb.Execute(KSQLStatement));
    
    // Release the ModelBank ID
    ReleaseIdL(KModelBankIdTable, KModelBankIdColumn, aModelBankID);
    
    // Cancel free disk space request
    iDbSession.FreeReservedSpace( iDrive );
    }

// -----------------------------------------------------------------------------
// CSIModelBankDB::RemoveModelL
// Removes a model from the model bank table by marking it as "not used".
// -----------------------------------------------------------------------------
//
void CSIModelBankDB::RemoveModelL( TUid /*aClientUid*/,
                                   TSIModelBankID /*aModelBankID*/,
                                   TSIModelID /*aModelID*/ )
    {
    User::Leave(KErrNotSupported);
    }

// -----------------------------------------------------------------------------
// CSIModelBankDB::Reset
// Deallocates the temporary memory allocated during training.
// -----------------------------------------------------------------------------
//
void CSIModelBankDB::Reset()
    {
    iAcousticModelPtr.Set(0,0,0);
    }

// -----------------------------------------------------------------------------
// CSIModelBankDB::Reset
// Deallocates the temporary memory allocated during recognition.
// -----------------------------------------------------------------------------
//
void CSIModelBankDB::ResetAndDestroy()
    {
    iModelBankArray.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CSIModelBankDB::SaveModelL
// Saves the trained model in the temporary memory into the database.
// -----------------------------------------------------------------------------
//
TSIModelID CSIModelBankDB::SaveModelL( TSIModelBankID aModelBankID )
    {
    if ( iAcousticModelPtr.Length() <= 0 )
        {
        User::Leave(KErrNotReady);
        }

    User::LeaveIfError( iDbSession.ReserveDriveSpace( iDrive, sizeof( CSIModel ) ) );
    
    // Always 1 for each model bank
    TSIModelID modelID=KModelID; 
    // Construct the table name using the provided model bank ID
    TBuf<40> KModelBankName(KSIModelBankTable);
    KModelBankName.AppendNumUC(aModelBankID);
    
    TInt32 hmmChecksum = CalculateChecksum(iAcousticModelPtr);
    
    // Open model bank table
    RDbTable dbTable;		// Provides access to table data as a rowset
    User::LeaveIfError(dbTable.Open(iDb, KModelBankName, RDbTable::EUpdatable));
    CleanupClosePushL(dbTable);
    
    // Get column set for column ordinals
    CDbColSet* columns = dbTable.ColSetL();
    TDbColNo hmm_col = columns->ColNo(KHmmColumn);
    TDbColNo hmmchecksum_col = columns->ColNo(KHmmChecksumColumn);
    delete columns;
    
    // 		Update it
    dbTable.FirstL(); 	
    dbTable.UpdateL();
    
    dbTable.SetColL(hmm_col, iAcousticModelPtr);
    dbTable.SetColL(hmmchecksum_col, hmmChecksum);
    
    // Write the updated row
    TRAPD(err, dbTable.PutL());
    if( err != KErrNone )
        {
        // Error: cancel update
        dbTable.Cancel();
        dbTable.Reset();
        User::Leave(err);
        }
    iDbSession.FreeReservedSpace( iDrive );
    // Cleanup dbTable
    CleanupStack::PopAndDestroy();
    // Free up resources
    Reset();
    
    return modelID;
    }

// -----------------------------------------------------------------------------
// CSIModelBankDB::SetAcousticModelL
// Saves the acoustic model into a temporary memory during training.
// -----------------------------------------------------------------------------
//
void CSIModelBankDB::SetAcousticModelL( const TPtr8& aAcousticModelBuf )
    {
    if ( aAcousticModelBuf.Length() <= 0 )
        {
        User::Leave(KErrNotReady);
        }
    
    iAcousticModelPtr.Set(aAcousticModelBuf);
    }

// -----------------------------------------------------------------------------
// CSIModelBankDB::SetUtteranceDurationL
// Saves the utterance duration of a trained model in temporary memory during
// training.
// -----------------------------------------------------------------------------
//
void CSIModelBankDB::SetUtteranceDurationL( TInt /*aUtteranceDuration*/ )
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CSIModelBankDB::CalculateChecksum
// Calculates a checksum value for the given buffer.
// -----------------------------------------------------------------------------
//
TInt32 CSIModelBankDB::CalculateChecksum( const TPtr8& aBuf )
    {
    TInt32 checksum = 0;
    TInt length = aBuf.Length();
    
    for( TInt i = 0; i < length; i++ )
        {
        checksum += aBuf[i];
        }
    return checksum;
    }

// -----------------------------------------------------------------------------
// CSIModelBankDB::VerifyChecksum
// Verifies the checksum value for the given buffer by comparing it with the
// provided aChecksum value.
// -----------------------------------------------------------------------------
//
TBool CSIModelBankDB::VerifyChecksum( const TPtr8& aBuf,
                                      TInt32 aChecksum )
    {
    TInt32 checksum = CalculateChecksum(aBuf);
    return (checksum == aChecksum);
    }


// -----------------------------------------------------------------------------
// Unit Test only
// -----------------------------------------------------------------------------
//
#ifdef __CONSOLETEST__
// Returns array of loaded model banks
RPointerArray<CSIModelBank>& CSIModelBankDB::ModelBankArray()
    {
    return iModelBankArray;
    }

#endif

//  End of File
