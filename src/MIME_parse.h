/******************************************************************************
* Copyright (c) 2000-2018 Ericsson Telecom AB
* All rights reserved. This program and the accompanying materials
* are made available under the terms of the Eclipse Public License v2.0
* which accompanies this distribution, and is available at
* https://www.eclipse.org/org/documents/epl-2.0/EPL-2.0.html
*
* Contributors:
* Gabor Szalai
* Attila Balasko
* Julianna Majer
* Eduard Czimbalmos
******************************************************************************/
//
//  File:               MIME_parse.h
//  Rev:                R6A
//  Prodnr:             CNL 113 352
//  Updated:            2005-06-24
//  Contact:            http://ttcn.ericsson.se
//  Reference:          RFC2045, RFC2046

#ifndef PARSE_HH
#define PARSE_HH

#include "MIME_Types.hh"
extern int MIME_parse_parse();
extern int MIME_num_chars;
extern MIME__Types::MIME__entity__header *MIME_parse_entity_header_ptr;


extern int MIME_parse_lex(void);
extern int MIME_parse_debug;



extern int MIME_num_chars;
void MIME_parsing(const char* s, int len);

#endif
