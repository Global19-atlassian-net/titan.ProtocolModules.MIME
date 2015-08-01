/******************************************************************************
* Copyright (c) 2005, 2015  Ericsson AB
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v1.0
* which accompanies this distribution, and is available at
* http://www.eclipse.org/legal/epl-v10.html
*
* Contributors:
* Gabor Szalai
* Attila Balasko
* Julianna Majer
******************************************************************************/
//
//  File:               SIP_mapping_functions.hh
//  Rev:                R5A
//  Prodnr:             CNL 113 352
//  Reference:          RFC2045, RFC2046 

#ifndef Sip_mapping_function_HH
#define Sip_mapping_function_HH

#include "SIP_SDPandMIME_Mapping.hh"

namespace SIP__SDPandMIME__Mapping {

CHARSTRING f__SIP__generate_content_type(const MIME__Types::MIME__FIELD__Content__type &type);
void f_SIP_add_one_entity(ASP__SIP__body__content__advanced &ret_val,
    const MIME__Types::PDU__MIME__entity &header);

}

#endif
