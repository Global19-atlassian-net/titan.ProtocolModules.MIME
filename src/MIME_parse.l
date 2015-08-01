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
//  File:               MIME_parse.l
//  Rev:                R5A
//  Prodnr:             CNL 113 352
//  Reference:          RFC2045, RFC2046

#include <string.h>
#include <strings.h>
#include <ctype.h>
#include "memory.h"
#include "MIME_parse.h"                              //needed for re-definition of YY_INPUT !!
#include "MIME_parse_.tab.h"
#include "MIME_Types.hh"
int MIME_num_chars;
%}
%option noyywrap
%option nounput
%option never-interactive
%x front
%x tail
%x other

OCTET   [\000-\377]
CHAR    [\000-\177]
SP      [ ]
HT      [\t]
CR      [\r]
LF      [\n]
A       [Aa]
B       [Bb]
C       [Cc]
D       [Dd]
E       [Ee]
F       [Ff]
G       [Gg]
H       [Hh]
I       [Ii]
J       [Jj]
K       [Kk]
L       [Ll]
M       [Mm]
N       [Nn]
O       [Oo]
P       [Pp]
Q       [Qq]
R       [Rr]
S       [Ss]
T       [Tt]
U       [Uu]
V       [Vv]
W       [Ww]
X       [Xx]
Y       [Yy]
Z       [Zz]
DIGIT   [0-9]
COLON   [:]

UTF8CON [\200-\277]
UTF8CO2 {UTF8CON}{UTF8CON}
UTF8CO3 {UTF8CO2}{UTF8CON}
UTF8CO4 {UTF8CO2}{UTF8CO2}
UTF8CO5 {UTF8CO3}{UTF8CO2}

XC0_DF  [\300-\337]
XE0_EF  [\340-\357]
XF0_F7  [\360-\367]
XF8_FB  [\370-\373]
XFC_FD  [\374\375]
U1      {XC0_DF}{UTF8CON}
U2      {XE0_EF}{UTF8CO2}
U3      {XF0_F7}{UTF8CO3}
U4      {XF8_FB}{UTF8CO4}
U5      {XFC_FD}{UTF8CO5}
UTF8NNA {U1}|{U2}|{U3}|{U4}|{U5}

UTF8WOE [\041-\176]|{UTF8NNA}|{SP}|{HT}
CRLF    {CR}{LF}
OLDCRLF {CR}|{LF}|({CR}{LF})
ANCRLF  {ANLWS}*{OLDCRLF}

QDTEXT  {ANLWS}|[\041]|[\043-\133]|[\135-\176]|{UTF8NNA}
QDPAIR  [\134][\000-\011\013-\014\016-\177]
QDSTR   [\042](({QDTEXT}|{QDPAIR})*)[\042]

NONLWS  [\041-\377]
ANLWS   (({SP}|{HT})*{CRLF}?({SP}|{HT})+)
TOKEN   ([[:alnum:]]|[-]|[.]|[!]|[%]|[\*]|[_]|[\+]|[`]|[']|[~])+
STOKEN  ({NONLWS})+
%%
<INITIAL>{CRLF}                                                                 MIME_num_chars+=MIME_parse_leng; BEGIN(INITIAL); return LONE_CRLF;
<INITIAL>({C}{O}{N}{T}{E}{N}{T}[-]{T}{Y}{P}{E}){ANLWS}*{COLON}                  MIME_num_chars+=MIME_parse_leng; BEGIN(tail); return CONTENTTYPE;
<INITIAL>({C}{O}{N}{T}{E}{N}{T}[-]{T}{R}{A}{N}{S}{F}{E}{R}[-]{E}{N}{C}{O}{D}{I}{N}{G}){ANLWS}*{COLON}           MIME_num_chars+=MIME_parse_leng; BEGIN(tail); return CONETNTCODEING;
<INITIAL>({TOKEN}){ANLWS}*{COLON}                                               MIME_num_chars+=MIME_parse_leng; BEGIN(other); MIME_parse_lval.sv=(char *)Malloc(MIME_parse_leng+1); strcpy(MIME_parse_lval.sv,MIME_parse_text); return EXT_HEADER;
<front>{CRLF}                                                                   MIME_num_chars+=MIME_parse_leng; BEGIN(INITIAL); return LONE_CRLF;
<front>({C}{O}{N}{T}{E}{N}{T}[-]{T}{Y}{P}{E}){ANLWS}*{COLON}                    MIME_num_chars+=MIME_parse_leng; BEGIN(tail); return CONTENTTYPE;
<front>({C}{O}{N}{T}{E}{N}{T}[-]{T}{R}{A}{N}{S}{F}{E}{R}[-]{E}{N}{C}{O}{D}{I}{N}{G}){ANLWS}*{COLON}           MIME_num_chars+=MIME_parse_leng; BEGIN(tail); return CONETNTCODEING;
<front>({TOKEN}){ANLWS}*{COLON}                                                 MIME_num_chars+=MIME_parse_leng; BEGIN(other); MIME_parse_lval.sv=(char *)Malloc(MIME_parse_leng+1); strcpy(MIME_parse_lval.sv,MIME_parse_text); return EXT_HEADER;
<tail,other>{ANCRLF}                                                            MIME_num_chars+=MIME_parse_leng; BEGIN(front); return _CRLF;
<tail>{QDSTR}                                                                   MIME_num_chars+=MIME_parse_leng; MIME_parse_lval.sv=(char *)Malloc(MIME_parse_leng+1); strcpy(MIME_parse_lval.sv,MIME_parse_text+1); MIME_parse_lval.sv[MIME_parse_leng-2]='\0'; return _TOKEN;
<tail>{TOKEN}                                                                   MIME_num_chars+=MIME_parse_leng; MIME_parse_lval.sv=(char *)Malloc(MIME_parse_leng+1); strcpy(MIME_parse_lval.sv,MIME_parse_text); return _TOKEN;
<tail>{ANLWS}*[;]                                                               MIME_num_chars+=MIME_parse_leng; return SEMICOLON;
<tail>{ANLWS}*[=]({ANLWS}*)                                                     MIME_num_chars+=MIME_parse_leng; return EQSIGN;
<tail>{ANLWS}*[/]({ANLWS}*)                                                     MIME_num_chars+=MIME_parse_leng; return SLASH;
<other>{STOKEN}(({ANLWS}*{STOKEN})*)                                            MIME_num_chars+=MIME_parse_leng; MIME_parse_lval.sv=(char *)Malloc(MIME_parse_leng+1); strcpy(MIME_parse_lval.sv,MIME_parse_text); return _STOKEN;
<other,tail>{ANLWS}+                                                            MIME_num_chars+=MIME_parse_leng;  return LWS;
<*>.                                                                            MIME_num_chars+=MIME_parse_leng; MIME_parse_lval.cv=MIME_parse_text[0];  return MIME_parse_text[0];

<*><<EOF>>                                                                      BEGIN(INITIAL); yyterminate();
%%
#include "MIME_parse.h"

int MIME_parse_error(const char *s)
{
        if (MIME_parse_text[0]=='\n')
            TTCN_error("MIME Test Port:  %s in \"\\n\" -> unexpected character.",s);
        else if (MIME_parse_text[0]=='\r')
                TTCN_error("MIME Test Port:  %s in \"\\r\" -> unexpected character.",s);
        else
                TTCN_error("MIME Test Port:  %s in \"%s\" -> unexpected character.",s,MIME_parse_text);
        return 0;
}

void MIME_parsing(const char* buff, int len){
  yy_buffer_state *flex_buffer = MIME_parse__scan_bytes (buff, len);
  if (flex_buffer == NULL) {
    TTCN_error("Flex buffer creation failed.");
  }
  BEGIN(INITIAL);
  MIME_parse_debug=0;
  MIME_parse_parse();  // also sets appropriate fields of msg through pointers..
  MIME_parse__delete_buffer(flex_buffer);
}
