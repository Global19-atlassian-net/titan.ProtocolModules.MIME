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
//  File:               SIP_mapping_functions.cc
//  Rev:                R5A
//  Prodnr:             CNL 113 352
//  Reference:          RFC2045, RFC2046 

#include "SIP_SDPandMIME_Mapping.hh"
#include "memory.h"
#include <strings.h>
#include <string.h>
#include "SIP_mapping_function.hh"
#include <sys/times.h>
#include <limits.h>

namespace SIP__SDPandMIME__Mapping {

CHARSTRING f__SIP__generate_content_type(const MIME__Types::MIME__FIELD__Content__type &type){
    char *buff=NULL;
    CHARSTRING ret_val;
    buff=mputprintf(buff,"%s/%s",(const char*)type.content__type()
                           ,(const char*)type.subtype());
    if(type.parameters().ispresent()){
      for(int a=0;a<type.parameters()().size_of();a++){
        buff=mputprintf(buff,";%s=%s"
        ,(const char*)type.parameters()()[a].param__name()
        ,(const char*)type.parameters()()[a].param__value());
      }
    }
    ret_val=buff;
    Free(buff);
    return ret_val;
}

CHARSTRING f__SIP__add__message__body(SIPmsg__Types::MessageHeader &headers,
                             const ASP__SIP__body__content &contents){
  ASP__SIP__body__content__advanced val;
  if(contents.SDP__message().ispresent()){
    val.SDP__message()().SDP__message()=contents.SDP__message()();
    val.SDP__message()().MIME__header()=OMIT_VALUE;
  } else val.SDP__message()=OMIT_VALUE;
  if(contents.ISUP__message().ispresent()){
    val.ISUP__message()().ISUP__message()=contents.ISUP__message()();
    val.ISUP__message()().MIME__header()=OMIT_VALUE;
  } else val.ISUP__message()=OMIT_VALUE;
  if(contents.AOC__message().ispresent()){
    val.AOC__message()().AOC__message()=contents.AOC__message()();
    val.AOC__message()().MIME__header()=OMIT_VALUE;
  } else val.AOC__message()=OMIT_VALUE;
  if(contents.other__messages().ispresent()) 
   val.other__messages()()=contents.other__messages()();
  else val.other__messages()=OMIT_VALUE;
  
  return f__SIP__add__message__body__adv(headers,val);
}

CHARSTRING f__SIP__add__message__body__adv(SIPmsg__Types::MessageHeader &headers,
                             const ASP__SIP__body__content__advanced &contents){
  int a=0;
  CHARSTRING ret_val="";
  
  if(contents.SDP__message().ispresent()) a++;
  if(contents.ISUP__message().ispresent()) a++;
  if(contents.AOC__message().ispresent()) a++;
  if(contents.other__messages().ispresent()){
    a+=contents.other__messages()().size_of();
  }
  if(a==0) return ret_val;
  else if(a==1){
    if(contents.SDP__message().ispresent()){
      if(contents.SDP__message()().MIME__header().ispresent()){
        if(contents.SDP__message()().MIME__header()().content__type().ispresent()){
          headers.contentType()().fieldName()=SIPmsg__Types::FieldName::CONTENT__TYPE__E;
          headers.contentType()().mediaType()=f__SIP__generate_content_type(
                   contents.SDP__message()().MIME__header()().content__type()());
        } else {
          headers.contentType()().fieldName()=SIPmsg__Types::FieldName::CONTENT__TYPE__E;
          headers.contentType()().mediaType()="application/sdp";
        }
        if(contents.SDP__message()().MIME__header()().content__encoding().ispresent()){
          headers.contentEncoding()().fieldName()=SIPmsg__Types::FieldName::CONTENT__TYPE__E;
          headers.contentEncoding()().contentCoding()[0]=
                contents.SDP__message()().MIME__header()().content__encoding()();
        }
      } else {
        headers.contentType()().fieldName()=SIPmsg__Types::FieldName::CONTENT__TYPE__E;
        headers.contentType()().mediaType()="application/sdp";
      }
      ret_val=f__SDP__enc__Message(contents.SDP__message()().SDP__message());
      headers.contentLength()().fieldName()=SIPmsg__Types::FieldName::CONTENT__LENGTH__E;
      headers.contentLength()().len()=ret_val.lengthof();
    }
    else if(contents.ISUP__message().ispresent()) {
      CHARSTRING *ret_val2;
      if(contents.ISUP__message()().MIME__header().ispresent()){
        if(contents.ISUP__message()().MIME__header()().content__type().ispresent()){
          headers.contentType()().fieldName()=SIPmsg__Types::FieldName::CONTENT__TYPE__E;
          headers.contentType()().mediaType()=f__SIP__generate_content_type(
                   contents.ISUP__message()().MIME__header()().content__type()());
        }
        if(contents.ISUP__message()().MIME__header()().content__encoding().ispresent()){
          headers.contentEncoding()().fieldName()=SIPmsg__Types::FieldName::CONTENT__TYPE__E;
          headers.contentEncoding()().contentCoding()[0]=
                contents.ISUP__message()().MIME__header()().content__encoding()();
        }
      } else {
        headers.contentType()().fieldName()=SIPmsg__Types::FieldName::CONTENT__TYPE__E;
        headers.contentType()().mediaType()="application/isup";
      }
      ret_val2 = new CHARSTRING(contents.ISUP__message()().ISUP__message().lengthof(),
                 (const char*)(const unsigned char*)contents.ISUP__message()().ISUP__message());
      ret_val=*ret_val2;
      headers.contentLength()().fieldName()=SIPmsg__Types::FieldName::CONTENT__LENGTH__E;
      headers.contentLength()().len()=ret_val.lengthof();
      delete ret_val2;
    }
    else if(contents.AOC__message().ispresent()) {
      if(contents.AOC__message()().MIME__header().ispresent()){
        if(contents.AOC__message()().MIME__header()().content__type().ispresent()){
          headers.contentType()().fieldName()=SIPmsg__Types::FieldName::CONTENT__TYPE__E;
          headers.contentType()().mediaType()=f__SIP__generate_content_type(
                   contents.AOC__message()().MIME__header()().content__type()());
        }
        if(contents.AOC__message()().MIME__header()().content__encoding().ispresent()){
          headers.contentEncoding()().fieldName()=SIPmsg__Types::FieldName::CONTENT__TYPE__E;
          headers.contentEncoding()().contentCoding()[0]=
                contents.AOC__message()().MIME__header()().content__encoding()();
        }
      } else {
        headers.contentType()().fieldName()=SIPmsg__Types::FieldName::CONTENT__TYPE__E;
        headers.contentType()().mediaType()="application/vnd.etsi.sci+xml";
      }
      ret_val=(const char*)(const unsigned char*)enc__MessageType(contents.AOC__message()().AOC__message());
      headers.contentLength()().fieldName()=SIPmsg__Types::FieldName::CONTENT__LENGTH__E;
      headers.contentLength()().len()=ret_val.lengthof();
    }
    else {
      headers.contentType()().fieldName()=SIPmsg__Types::FieldName::CONTENT__TYPE__E;
      headers.contentType()().mediaType()=f__SIP__generate_content_type(
                             contents.other__messages()()[0].content__type()());
      if(contents.other__messages()()[0].content__encoding().ispresent()){
        headers.contentEncoding()().fieldName()=SIPmsg__Types::FieldName::CONTENT__TYPE__E;
        headers.contentEncoding()().contentCoding()[0]=
                          contents.other__messages()()[0].content__encoding()();
        if(!strcasecmp((const char*)headers.contentEncoding()().contentCoding()[0],
                      "base64")){
          ret_val=MIME__Types::f__MIME__Base64__Encode(
                                   char2oct(contents.other__messages()()[0].payload()()));
        }
        else ret_val=contents.other__messages()()[0].payload()();
      }
      else ret_val=contents.other__messages()()[0].payload()();
      headers.contentLength()().fieldName()=SIPmsg__Types::FieldName::CONTENT__LENGTH__E;
      headers.contentLength()().len()=ret_val.lengthof();
    }
  }
  else{
    MIME__Types::PDU__MIME__entity__list list;
    MIME__Types::MIME__FIELD__Content__type type_val;
    int num_part=0;
    type_val.content__type()="multipart";
    type_val.subtype()="mixed";
    type_val.parameters()()[0].param__name()="boundary";
    type_val.parameters()()[0].param__value()=f__MIME__generate__delimiter();
    headers.contentType()().fieldName()=SIPmsg__Types::FieldName::CONTENT__TYPE__E;
    headers.contentType()().mediaType()=f__SIP__generate_content_type(type_val);

    if(contents.SDP__message().ispresent()){
      if(contents.SDP__message()().MIME__header().ispresent()){
        if(contents.SDP__message()().MIME__header()().content__type().ispresent()){
           list[num_part].content__type()()=
              contents.SDP__message()().MIME__header()().content__type()();
        } else {
          list[num_part].content__type()().content__type()="application";
          list[num_part].content__type()().subtype()="sdp";
          list[num_part].content__type()().parameters()=OMIT_VALUE;
        }
        if(contents.SDP__message()().MIME__header()().content__encoding().ispresent()){
          list[num_part].content__encoding()=
             contents.SDP__message()().MIME__header()().content__encoding()();
        } else list[num_part].content__encoding()=OMIT_VALUE;
        if(contents.SDP__message()().MIME__header()().other__fields().ispresent()){
          list[num_part].other__fields()=
             contents.SDP__message()().MIME__header()().other__fields()();
        } else list[num_part].other__fields()=OMIT_VALUE;
      } else {
        list[num_part].content__type()().content__type()="application";
        list[num_part].content__type()().subtype()="sdp";
        list[num_part].content__type()().parameters()=OMIT_VALUE;
        list[num_part].content__encoding()=OMIT_VALUE;
        list[num_part].other__fields()=OMIT_VALUE;
      }
      list[num_part].payload()=f__SDP__enc__Message(contents.SDP__message()().SDP__message());
      num_part++;
    }
    if(contents.ISUP__message().ispresent()) {
      CHARSTRING *ret_val2;
      if(contents.ISUP__message()().MIME__header().ispresent()){
        if(contents.ISUP__message()().MIME__header()().content__type().ispresent()){
           list[num_part].content__type()()=
              contents.ISUP__message()().MIME__header()().content__type()();
        } else {
          list[num_part].content__type()().content__type()="application";
          list[num_part].content__type()().subtype()="isup";
          list[num_part].content__type()().parameters()=OMIT_VALUE;
        }
        if(contents.ISUP__message()().MIME__header()().content__encoding().ispresent()){
          list[num_part].content__encoding()=
             contents.ISUP__message()().MIME__header()().content__encoding()();
        } else list[num_part].content__encoding()=OMIT_VALUE;
        if(contents.ISUP__message()().MIME__header()().other__fields().ispresent()){
          list[num_part].other__fields()=
             contents.ISUP__message()().MIME__header()().other__fields()();
        } else list[num_part].other__fields()=OMIT_VALUE;
      } else {
        list[num_part].content__type()().content__type()="application";
        list[num_part].content__type()().subtype()="isup";
        list[num_part].content__type()().parameters()=OMIT_VALUE;
        list[num_part].content__encoding()=OMIT_VALUE;
        list[num_part].other__fields()=OMIT_VALUE;
      }
      ret_val2 = new CHARSTRING(contents.ISUP__message()().ISUP__message().lengthof(),
                 (const char*)(const unsigned char*)contents.ISUP__message()().ISUP__message());
      list[num_part].payload()=*ret_val2;
      num_part++;
      delete ret_val2;
    }
    if(contents.AOC__message().ispresent()){
      if(contents.AOC__message()().MIME__header().ispresent()){
        if(contents.AOC__message()().MIME__header()().content__type().ispresent()){
           list[num_part].content__type()()=
              contents.AOC__message()().MIME__header()().content__type()();
        } else {
          list[num_part].content__type()().content__type()="application";
          list[num_part].content__type()().subtype()="vnd.etsi.sci+xml";
          list[num_part].content__type()().parameters()=OMIT_VALUE;
        }
        if(contents.AOC__message()().MIME__header()().content__encoding().ispresent()){
          list[num_part].content__encoding()=
             contents.AOC__message()().MIME__header()().content__encoding()();
        } else list[num_part].content__encoding()=OMIT_VALUE;
        if(contents.AOC__message()().MIME__header()().other__fields().ispresent()){
          list[num_part].other__fields()=
             contents.AOC__message()().MIME__header()().other__fields()();
        } else list[num_part].other__fields()=OMIT_VALUE;
      } else {
        list[num_part].content__type()().content__type()="application";
        list[num_part].content__type()().subtype()="vnd.etsi.sci+xml";
        list[num_part].content__type()().parameters()=OMIT_VALUE;
        list[num_part].content__encoding()=OMIT_VALUE;
        list[num_part].other__fields()=OMIT_VALUE;
      }
      list[num_part].payload()=(const char*)(const unsigned char*)enc__MessageType(contents.AOC__message()().AOC__message());
      num_part++;
    }
    if(contents.other__messages().ispresent()){
      for(int i=0;i<contents.other__messages()().size_of();i++){
        list[num_part].content__type()=
                          contents.other__messages()()[i].content__type();
        list[num_part].content__encoding()=
                          contents.other__messages()()[i].content__encoding();
        list[num_part].other__fields()=
                          contents.other__messages()()[i].other__fields();
        if(contents.other__messages()()[0].content__encoding().ispresent() &&
           !strcasecmp((const char*)contents.other__messages()()[0].
                      content__encoding()(),"base64")){
          list[num_part].payload()=MIME__Types::f__MIME__Base64__Encode(
                          char2oct(contents.other__messages()()[i].payload()()));
        }
        else list[num_part].payload()=
                          contents.other__messages()()[i].payload()();
        num_part++;
      }
    }
    ret_val=f__MIME__build__multipart(list,
                          type_val.parameters()()[0].param__value());
  }
  return ret_val;
}

ASP__SIP__body__content__advanced f__SIP__get__message__body__adv(const SIPmsg__Types::MessageHeader &headers,
                       const CHARSTRING &body){
  ASP__SIP__body__content__advanced ret_val;
  MIME__Types::PDU__MIME__entity header;
  CHARSTRING ch="Content-Type: ";
  ch=ch+headers.contentType()().mediaType();
  ch=ch+" \r\n\r\nf \r\n";
  header=MIME__Types::f__MIME__Decode(ch);
  ret_val.SDP__message()=OMIT_VALUE;
  ret_val.ISUP__message()=OMIT_VALUE;
  ret_val.other__messages()=OMIT_VALUE;
  header.payload()=body;
  if(headers.contentEncoding().ispresent()){
    header.content__encoding()=headers.contentEncoding()().contentCoding()[0];
  }

  if(!strcasecmp(header.content__type()().content__type(),"multipart")){
    MIME__Types::PDU__MIME__entity__list list;
    list=MIME__Types::f__MIME__get__multipart(header);
    for(int i=0;i<list.size_of();i++){
      f_SIP_add_one_entity(ret_val,list[i]);
    }
  }
  else {
    f_SIP_add_one_entity(ret_val,header);
  }
  return ret_val;
}

ASP__SIP__body__content f__SIP__get__message__body(const SIPmsg__Types::MessageHeader &headers,
                       const CHARSTRING &body){
  ASP__SIP__body__content ret_val;
  ASP__SIP__body__content__advanced get_val=
      f__SIP__get__message__body__adv(headers,body);
  
  if(get_val.SDP__message().ispresent())
    ret_val.SDP__message()= get_val.SDP__message()().SDP__message();
  else ret_val.SDP__message()=OMIT_VALUE;
  if(get_val.ISUP__message().ispresent())
    ret_val.ISUP__message()= get_val.ISUP__message()().ISUP__message();
  else ret_val.ISUP__message()=OMIT_VALUE;
  if(get_val.other__messages().ispresent())
    ret_val.other__messages()= get_val.other__messages()();
  else ret_val.other__messages()=OMIT_VALUE;
  return ret_val;
}


void f_SIP_add_one_entity(ASP__SIP__body__content__advanced &ret_val,
                                               const MIME__Types::PDU__MIME__entity &header){
 if(!strcasecmp(header.content__type()().subtype(),"sdp")){
    ret_val.SDP__message()().MIME__header()().content__type()()=header.content__type()();
    ret_val.SDP__message()().MIME__header()().content__encoding()=header.content__encoding();
    ret_val.SDP__message()().MIME__header()().other__fields()=header.other__fields();
    ret_val.SDP__message()().SDP__message()=SDP__Types::f__SDP__dec__Message(header.payload()());
  }
  else if(!strcasecmp(header.content__type()().subtype(),"isup")){
    ret_val.ISUP__message()().MIME__header()().content__type()()=header.content__type()();
    ret_val.ISUP__message()().MIME__header()().content__encoding()=header.content__encoding();
    ret_val.ISUP__message()().MIME__header()().other__fields()=header.other__fields();
    ret_val.ISUP__message()().ISUP__message()=OCTETSTRING(header.payload()().lengthof(),
             (const unsigned char*)(const char*)header.payload()());
  }
  else if(!strcasecmp(header.content__type()().subtype(),"vnd.etsi.sci+xml")){
    ret_val.AOC__message()().MIME__header()().content__type()()=header.content__type()();
    ret_val.AOC__message()().MIME__header()().content__encoding()=header.content__encoding();
    ret_val.AOC__message()().MIME__header()().other__fields()=header.other__fields();
    ret_val.AOC__message()().AOC__message()=
        uri__etsi__org__ngn__params__xml__simservs__sci::dec__MessageType(
            OCTETSTRING(header.payload()().lengthof(),
            (const unsigned char*)(const char*)header.payload()()));
  }
  else {
    int a;
    if(ret_val.other__messages().ispresent()){
      a=ret_val.other__messages()().size_of();
    }
    else a=0;
    ret_val.other__messages()()[a].content__type()=header.content__type();
    if(header.content__encoding().ispresent()){
      ret_val.other__messages()()[a].content__encoding()=
                        header.content__encoding();
      if(!strcasecmp((const char*)header.content__encoding()(),"base64")){
        ret_val.other__messages()()[a].payload()=
                      MIME__Types::f__MIME__Base64__Encode(char2oct(header.payload()()));
      } else ret_val.other__messages()()[a].payload()=header.payload()(); 
    }
    else{
      ret_val.other__messages()()[a].content__encoding()=OMIT_VALUE;
      ret_val.other__messages()()[a].payload()=header.payload();
    }
    ret_val.other__messages()()[a].other__fields()=OMIT_VALUE;
  }
  
}

CHARSTRING f__MIME__generate__delimiter(){
  char tomb[64]={
  'q','w','e','r','t','y','u','i','o','p','1','2','3','4','5','6',
  '7','8','9','0','A','S','D','F','G','H','J','K','L','Z','X','C',
  'v','b','n','m','Q','W','E','R','T','Y','U','I','O','P','M','N',
  'B','V','C','a','s','d','f','g','h','j','k','l','Q','W','R','T'};
  struct tms t;
  clock_t ti=times(&t);
  int c1=ti%64;
  int c2=(ti/64)%4*2+1;
  char res[26];
  res[25]='\0';
  for(int a=0;a<25;a++) {res[a]=tomb[c1]; c1=(c1+c2)%64;}
  CHARSTRING ret_val=res;
  return ret_val;
}

}
