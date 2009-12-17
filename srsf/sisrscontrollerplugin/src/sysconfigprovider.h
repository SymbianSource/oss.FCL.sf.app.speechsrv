/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Interface to provide system configuration information.
*
*/


#ifndef SYSCONFIGPROVIDER_H
#define SYSCONFIGPROVIDER_H

// FORWARD DECLARATIONS

class CArrayFixFlat<TInt>;
class RFs;


class MSysConfigProvider {
public:
	virtual TInt GetInstalledLanguages( CArrayFixFlat<TInt>*& aLanguages, RFs* aFileServerSession = NULL ) = 0;
};

#endif

// end of file
