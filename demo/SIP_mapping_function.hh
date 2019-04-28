/******************************************************************************
* Copyright (c) 2000-2019 Ericsson Telecom AB
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v2.0
* which accompanies this distribution, and is available at
* https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
*
* Contributors:
* Gabor Szalai
* Attila Balasko
* Julianna Majer
******************************************************************************/
//
//  File:               SIP_mapping_functions.hh
//  Rev:                R6A
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
