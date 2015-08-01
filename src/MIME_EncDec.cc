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
//  File:               MIME_EncDec.cc
//  Rev:                R5A
//  Prodnr:             CNL 113 352
//  Reference:          RFC2045, RFC2046 
#include <string.h>
#include <strings.h>
#include "MIME_parse.h"
#include <errno.h>
#include "TTCN3.hh"
#include "memory.h"
#include "MIME_parse_.tab.h"
#include "MIME_Types.hh"
namespace MIME__Types {

CHARSTRING f__MIME__Base64__Encode(const OCTETSTRING& inent){
  char dtable[64]={
  'A','B','C','D','E','F','G','H','I','J','K','L','M','N','O','P',
  'Q','R','S','T','U','V','W','X','Y','Z','a','b','c','d','e','f',
  'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v',
  'w','x','y','z','0','1','2','3','4','5','6','7','8','9','+','/'
  };
  int num_of_triplet= (inent.lengthof()+2)/3;
  int out_length= num_of_triplet*4 + num_of_triplet/19*2 +3;
  int output_pos=0;
  char triplets[3];
  char *output;
  const unsigned char *input=(const unsigned char*)inent;
  CHARSTRING ret_val;
  output=(char *)Malloc(out_length*sizeof(char));
  for(int a=0;a<inent.lengthof()/3;a++){
    output[output_pos]= dtable[(input[3*a]&0xFF)>>2];
    output[output_pos+1]= dtable[((input[3*a]&0x3)<<4)|((input[3*a+1]&0xFF)>>4)];
    output[output_pos+2]= dtable[((input[3*a+1]&0xF)<<2)|((input[3*a+2]&0xFF)>>6)];
    output[output_pos+3]= dtable[input[3*a+2]&0x3F];
    output_pos+=4;
    if(!((output_pos+2)%78)){
      output[output_pos]='\r';
      output[output_pos+1]='\n';
      output_pos+=2;
    }
  }
  if(inent.lengthof()%3){
    int n=inent.lengthof()%3;
    triplets[0]= input[(num_of_triplet-1)*3];
    triplets[1]= n==2?input[(num_of_triplet-1)*3+1]:0;
    triplets[2]= 0;
    output[output_pos]= dtable[(triplets[0]&0xFF)>>2];
    output[output_pos+1]= dtable[((triplets[0]&0x3)<<4)|((triplets[1]&0xFF)>>4)];
    output[output_pos+2]= n==2?dtable[(triplets[1]&0xF)<<2]:'=';
    output[output_pos+3]= '=';
    output_pos+=4;
  }
  output[output_pos]='\r';
  output[output_pos+1]='\n';
  ret_val=CHARSTRING(output_pos+2,output);
  Free(output);
  return ret_val;
}

OCTETSTRING f__MIME__Base64__Decode(const CHARSTRING& inent){
  int num_4s=inent.lengthof()/4+1;
  int inpos=0;
  int outpos=0;
  int pos=0;
  int nofeq=0;
  unsigned char *output;
  const char *input=(const char*)inent;
  unsigned char table[4];
  output=(unsigned char *)Malloc((num_4s*3+1)*sizeof(char));
  table[0]=table[1]=table[2]=table[3]=0;
  OCTETSTRING ret_val;
  
  while(true){
    char ch=input[inpos];

    if(ch>='A' && ch<='Z'){
      table[pos]=ch-'A';
      pos++;
      inpos++;
    }
    else if(ch>='a' && ch<='z'){
      table[pos]=ch-'a'+26;
      pos++;
      inpos++;
    }
    else if(ch>='0' && ch<='9'){
      table[pos]=ch-'0'+52;
      pos++;
      inpos++;
    }
    else if(ch=='+'){
      table[pos]=62;
      pos++;
      inpos++;
    }
    else if(ch=='/'){
      table[pos]=63;
      pos++;
      inpos++;
    }
    else if(ch=='='){
      table[pos]=0;
      pos++;
      inpos++;
      nofeq++;
    }
    else{
      inpos++;
      continue;
    }
    
    if(pos<4 && inpos<inent.lengthof()){continue;}
    if(!(inpos<inent.lengthof())){break;}
   
    output[outpos]= (table[0]<<2)|(table[1]>>4);
    output[outpos+1]= (table[1]<<4)|(table[2]>>2);
    output[outpos+2]= (table[2]<<6)|table[3];
    
    outpos+=3;
    table[0]=table[1]=table[2]=table[3]=0;
    pos=0;
  }
  outpos-=nofeq;
  ret_val=OCTETSTRING(outpos,output);
  Free(output);
  return ret_val;
}

CHARSTRING f__MIME__build__multipart(const PDU__MIME__entity__list& entities,
                                     const CHARSTRING& delimiter){
  CHARSTRING ret_val="";
  for(int a=0;a<entities.size_of();a++){
    ret_val=ret_val+"--";
    if(((const char *)delimiter)[0]!='"') ret_val=ret_val+delimiter;
    else ret_val=ret_val+
                   CHARSTRING(delimiter.lengthof()-2,(const char *)delimiter+1);
    ret_val=ret_val+"\r\n";
    ret_val=ret_val+f__MIME__Encode(entities[a]);
  }
    ret_val=ret_val+"--";
    if(((const char *)delimiter)[0]!='"') ret_val=ret_val+delimiter;
    else ret_val=ret_val+
                   CHARSTRING(delimiter.lengthof()-2,(const char *)delimiter+1);
    ret_val=ret_val+"--";
  return ret_val;
}

PDU__MIME__entity__list f__MIME__get__multipart(const PDU__MIME__entity &inent){
  char* delimiter=NULL;
  const char *begin;
  char *mbegin, *mend, *start;
  bool more_part=true;
  PDU__MIME__entity__list ret_val(NULL_VALUE);
  int del_length=0;
  int num_ent=0;
  for(int a=0;a<inent.content__type()().parameters()().size_of();a++){
    if(!strcasecmp("boundary",
         (const char*)inent.content__type()().parameters()()[a].param__name())){
      del_length=
            inent.content__type()().parameters()()[a].param__value().lengthof();
      delimiter=(char *)Malloc((del_length+3)*sizeof(char));
      delimiter[0]=delimiter[1]='-';
      const char* atm=
          (const char*)inent.content__type()().parameters()()[a].param__value();
      if(*atm=='"') {atm++;del_length--;}
      strcpy(delimiter+2,atm);
      if(atm[del_length-1]=='"') delimiter[del_length+1]='\0';
      break;
    }
  }
  mbegin=(char*)Malloc(inent.payload()().lengthof()*sizeof(char));
  begin=(const char*)inent.payload()();
  for(int a=0;a<inent.payload()().lengthof();a++){
    mbegin[a]=begin[a]?begin[a]:'A';
  }
  start=mbegin;
  mend=strstr(mbegin,delimiter);
  if(!mend){
    TTCN_warning("The delimiter \"%s\" was not found!",delimiter);
    return ret_val;
  }
  while(more_part){
    CHARSTRING data;
    PDU__MIME__entity val;
    mbegin=strstr(mend,"\n");
    if(!mbegin){
      TTCN_warning("The CRLF was not found before the delimiter!");
      return ret_val;
    }
    mbegin++;
    mend=strstr(mbegin,delimiter);
    if(!mend){
      TTCN_warning("The delimiter \"%s\" was not found!",delimiter);
      return ret_val;
    }
    data=CHARSTRING(mend-mbegin,begin+(mbegin-start));
    val=f__MIME__Decode(data);
    ret_val[num_ent]=val;
    num_ent++;
    if(mend[del_length+2]=='-') break;
  }
  Free(delimiter);
  return ret_val;
}

CHARSTRING f__MIME__Encode(const PDU__MIME__entity& inent){
  char *buff=NULL;
  CHARSTRING chr;
  
  if(inent.content__type().ispresent()){
    buff=mputprintf(buff,"Content-Type: ");
    buff=mputprintf(buff,"%s/%s",(const char*)inent.content__type()().content__type()
                           ,(const char*)inent.content__type()().subtype());
    if(inent.content__type()().parameters().ispresent()){
      for(int a=0;a<inent.content__type()().parameters()().size_of();a++){
        buff=mputprintf(buff,";%s=%s"
        ,(const char*)inent.content__type()().parameters()()[a].param__name()
        ,(const char*)inent.content__type()().parameters()()[a].param__value());
      }
    }
    buff=mputprintf(buff,"\r\n");    
  }
  if(inent.content__encoding().ispresent()){
    buff=mputprintf(buff,"Content-transfer-encoding: ");
    buff=mputprintf(buff,"%s",(const char*)inent.content__encoding()());
    buff=mputprintf(buff,"\r\n");
  }
  if(inent.other__fields().ispresent()){
    for(int a=0;a<inent.other__fields()().size_of();a++){
      buff=mputprintf(buff,"%s: %s\r\n"
                     ,(const char*)inent.other__fields()()[a].field__name()
                     ,(const char*)inent.other__fields()()[a].field__value());
    }
  }
  buff=mputprintf(buff,"\r\n");
  chr=buff;
  if(inent.payload().ispresent()){
    chr=chr+inent.payload()()+"\r\n";
  }

  Free(buff);
  return chr;
}

PDU__MIME__entity f__MIME__Decode(const CHARSTRING& inent){
		PDU__MIME__entity ent;
    ::MIME_parse_entity_ptr = &ent;
    MIME_num_chars=0;
    ent.content__type()=OMIT_VALUE;
    ent.content__encoding()=OMIT_VALUE;
    ent.other__fields()=OMIT_VALUE;
    ent.payload()=OMIT_VALUE;
    MIME_parse_debug=0;
    MIME_parsing((const char*)inent,inent.lengthof());
    if(MIME_num_chars<inent.lengthof()){
      ent.payload()()=CHARSTRING(inent.lengthof()-MIME_num_chars-2,
                                 (const char*)inent + MIME_num_chars);
    }
    return ent;
}  
}
