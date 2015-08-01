%{
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
//  File:               MIME_parse.y
//  Rev:                R5A
//  Prodnr:             CNL 113 352
//  Reference:          RFC2045, RFC2046
#include <stdlib.h>
#include <ctype.h>
#include <stdio.h>
#include <string.h>
#include <strings.h>
#include "memory.h"

#include "MIME_Types.hh"

#define YYDEBUG 1

extern void yyerror(const char *);
extern int yylex();
MIME__Types::PDU__MIME__entity *MIME_parse_entity_ptr;

%}

%union {
  char *sv;
  char cv;
}

%token <sv> _TOKEN _STOKEN
%token _CRLF SLASH EQSIGN LONE_CRLF LWS SEMICOLON
%token CONTENTTYPE
%token CONETNTCODEING
%token <sv> EXT_HEADER

%%

MIME_entity:
    mime_headers
    LONE_CRLF {YYACCEPT;};
    
mime_headers:
    /*empty*/
    | headers_1toN;

headers_1toN:
    mime_header
    | headers_1toN mime_header;

mime_header:
    content_type_header
    | content_encoding_header
    | other_header;

content_type_header:
    CONTENTTYPE LWS_0toN _TOKEN SLASH _TOKEN _CRLF{
          MIME_parse_entity_ptr->content__type()().content__type()=$3;
          MIME_parse_entity_ptr->content__type()().subtype()=$5;
          MIME_parse_entity_ptr->content__type()().parameters()=OMIT_VALUE;
          Free($3);
          Free($5);
      }
    | CONTENTTYPE LWS_0toN _TOKEN SLASH _TOKEN ct_parameter_1toN _CRLF{
          MIME_parse_entity_ptr->content__type()().content__type()=$3;
          MIME_parse_entity_ptr->content__type()().subtype()=$5;
          Free($3);
          Free($5);
      };

ct_parameter_1toN:
    ct_parameter
    | ct_parameter_1toN ct_parameter;

ct_parameter:
    SEMICOLON LWS_0toN _TOKEN EQSIGN _TOKEN{
      int a;
      if(!MIME_parse_entity_ptr->content__type()().parameters().is_bound())
        {a=0;}
      else
        {a=MIME_parse_entity_ptr->content__type()().parameters()().size_of();}
    MIME_parse_entity_ptr->content__type()().parameters()()[a].param__name()=$3;
    MIME_parse_entity_ptr->content__type()().parameters()()[a].param__value()=$5;
    Free($3);
    Free($5);
    };

LWS_0toN:
    /*empty*/ {}
    | LWS     {};

content_encoding_header:
    CONETNTCODEING LWS_0toN _TOKEN _CRLF{
      MIME_parse_entity_ptr->content__encoding()()=$3;
      Free($3);
    };

other_header:
    EXT_HEADER LWS_0toN _STOKEN _CRLF{
      int a;
      if(!MIME_parse_entity_ptr->other__fields()().is_bound())
        {a=0;}
      else
        {a=MIME_parse_entity_ptr->other__fields()().size_of();}
      $1[strlen($1)-1]='\0';
      MIME_parse_entity_ptr->other__fields()()[a].field__name()=$1;
      MIME_parse_entity_ptr->other__fields()()[a].field__value()=$3;
      Free($3);
      Free($1);
    };


%%
