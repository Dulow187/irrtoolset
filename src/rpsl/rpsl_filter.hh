//  $Id$
// Copyright (c) 2001,2002                        RIPE NCC
//
// All Rights Reserved
//
// Permission to use, copy, modify, and distribute this software and its
// documentation for any purpose and without fee is hereby granted,
// provided that the above copyright notice appear in all copies and that
// both that copyright notice and this permission notice appear in
// supporting documentation, and that the name of the author not be
// used in advertising or publicity pertaining to distribution of the
// software without specific, written prior permission.
//
// THE AUTHOR DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE, INCLUDING
// ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS; IN NO EVENT SHALL
// AUTHOR BE LIABLE FOR ANY SPECIAL, INDIRECT OR CONSEQUENTIAL DAMAGES OR ANY
// DAMAGES WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN
// AN ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
// OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
//
//
//  Copyright (c) 1994 by the University of Southern California
//  All rights reserved.
//
//    Permission is hereby granted, free of charge, to any person obtaining a copy
//    of this software and associated documentation files (the "Software"), to deal
//    in the Software without restriction, including without limitation the rights
//    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
//    copies of the Software, and to permit persons to whom the Software is
//    furnished to do so, subject to the following conditions:
//
//    The above copyright notice and this permission notice shall be included in
//    all copies or substantial portions of the Software.
//
//    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
//    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
//    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
//    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
//    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
//    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
//    THE SOFTWARE.
//
//  Questions concerning this software should be directed to 
//  irrtoolset@cs.usc.edu.
//
//  Author(s): Cengiz Alaettinoglu <cengiz@ISI.EDU>

// Comments:
// any dynamic memory allocated by lexer or parser 
// is claimed by the constructors here
// They are either used internally or deleted using delete!
// if a pointer points to something in the dictionary, that is not claimed

#ifndef FILTER_HH
#define FILTER_HH

#include "config.h"
#include <sstream> // For class ostream
#include <cassert>
extern "C" {
#ifdef HAVE_MALLOC_H
#include <malloc.h>
#endif // HAVE_MALLOC_H
}
#include "List.hh"
#include "symbols.hh"
#include "prefix.hh"
#include "dataset/prefixranges.hh"
#include "regexp.hh"
#include "rpsl_item.hh"

#ifdef ENABLE_DEBUG
#define INDENT(indent) for (int iii = 0; iii < indent; iii++) os << " "
#endif // ENABLE_DEBUG

typedef unsigned int ASt;

class AttrRPAttr;
class AttrMethod;

class Filter {
public:
   virtual ~Filter() {}
   virtual std::ostream& print(std::ostream &out) const;
   virtual Filter* dup() const = 0;
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const = 0;
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); print(os); os << std::endl;
   }
#endif // ENABLE_DEBUG
};

inline std::ostream &operator<<(std::ostream &out, const Filter &f) {
   return f.print(out);
}

class FilterOR: public Filter {
public:
   Filter *f1;
   Filter *f2;
public:
   FilterOR(Filter *_f1, Filter *_f2) : f1(_f1), f2(_f2) {
   }
   FilterOR(const FilterOR &pt) {
      f1 = (Filter *)pt.f1->dup();
      f2 = (Filter *)pt.f2->dup();
   }
   virtual ~FilterOR() {
      delete f1;
      delete f2;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Filter* dup() const {
      return new FilterOR(*this);
   } 
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "FilterOR";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "f1 (" << f1->className() << " *)" << std::endl;
      f1->printClass(os, indent + 2);
      INDENT(indent); os << "f2 (" << f2->className() << " *)" << std::endl;
      f2->printClass(os, indent + 2);
   }
#endif // ENABLE_DEBUG
};

class FilterAND: public Filter {
public:
   Filter *f1;
   Filter *f2;
public:
   FilterAND(Filter *_f1, Filter *_f2) : f1(_f1), f2(_f2) {
   }
   FilterAND(const FilterAND &pt) {
      f1 = (Filter *)pt.f1->dup();
      f2 = (Filter *)pt.f2->dup();
   }
   virtual ~FilterAND() {
      delete f1;
      delete f2;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Filter* dup() const {
      return new FilterAND(*this);
   } 
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "FilterAND";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "f1" << std::endl;
      f1->printClass(os, indent + 2);
      INDENT(indent); os << "f2" << std::endl;
      f2->printClass(os, indent + 2);
   }
#endif // ENABLE_DEBUG
};

class FilterNOT: public Filter {
public:
   Filter *f1;
public:
   FilterNOT(Filter *_f1) : f1(_f1) {
   }
   FilterNOT(const FilterNOT &pt) {
      f1 = (Filter *)pt.f1->dup();
   }
   virtual ~FilterNOT() {
      delete f1;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Filter* dup() const {
      return new FilterNOT(*this);
   } 
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "FilterNOT";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "f1 (NOT)" << std::endl;
      f1->printClass(os, indent + 2);
   }
#endif // ENABLE_DEBUG
};

class FilterEXCEPT: public Filter {
public:
   Filter *f1;
   Filter *f2;
public:
   FilterEXCEPT(Filter *_f1, Filter *_f2) : f1(_f1), f2(_f2) {
   }
   FilterEXCEPT(const FilterEXCEPT &pt) {
      f1 = (Filter *)pt.f1->dup();
      f2 = (Filter *)pt.f2->dup();
   }
   virtual ~FilterEXCEPT() {
      delete f1;
      delete f2;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Filter* dup() const {
      return new FilterEXCEPT(*this);
   } 
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "FilterEXCEPT";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "f1" << std::endl;
      f1->printClass(os, indent + 2);
      INDENT(indent); os << "f2" << std::endl;
      f2->printClass(os, indent + 2);
   }
#endif // ENABLE_DEBUG
};

class FilterMS: public Filter {
public:
   Filter *f1;
   int code;			// 0 for ^-, 1 for ^+, 2 for ^n and ^n-m
   int n;
   int m;
public:
   FilterMS(int _code) : f1(NULL), code(_code), n(0), m(0) {
   }
   FilterMS(int _code, int _n, int _m) : 
      f1(NULL), code(_code), n(_n), m(_m) {
   }
   FilterMS(const FilterMS &pt) : code(pt.code), n(pt.n), m(pt.m) {
      if (pt.f1)
	 f1 = (Filter *)pt.f1->dup();
      else
	 f1 = NULL;
   }
   virtual ~FilterMS() {
      if (f1)
	 delete f1;
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Filter* dup() const {
      return new FilterMS(*this);
   } 

#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "FilterMS";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "f1 (" << f1->className() << " *)" << std::endl;
      f1->printClass(os, indent + 2);
   }
#endif // ENABLE_DEBUG
};

class FilterASNO : public Filter {
public:
   ASt asno;
public:
   FilterASNO(ASt asn) {
      asno = asn;
   }
   virtual ~FilterASNO() {}
   virtual std::ostream& print(std::ostream &out) const;
   virtual Filter* dup() const {
      return new FilterASNO(*this);
   } 
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "FilterASNO";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "asno = " << asno << std::endl;
   }
#endif // ENABLE_DEBUG
};

class FilterASNAME : public Filter {
public:
   SymID asname;
public:
   FilterASNAME(SymID sid) {
      asname = sid;
   }
   virtual ~FilterASNAME() {}
   virtual std::ostream& print(std::ostream &out) const;
   virtual Filter* dup() const {
      return new FilterASNAME(*this);
   } 
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "FilterASNAME";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "asname = \"" << asname << "\"" << std::endl;
   }
#endif // ENABLE_DEBUG
};

class FilterRSNAME : public Filter {
public:
   SymID rsname;
public:
   FilterRSNAME(SymID sid) {
      rsname = sid;
   }
   virtual ~FilterRSNAME() {}
   virtual std::ostream& print(std::ostream &out) const;
   virtual Filter* dup() const {
      return new FilterRSNAME(*this);
   } 
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "FilterRSNAME";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "rsname = \"" << rsname << "\"" << std::endl;
   }
#endif // ENABLE_DEBUG
};

class FilterRTRSNAME : public Filter {
public:
   SymID rtrsname;
public:
   FilterRTRSNAME(SymID sid) {
      rtrsname = sid;
   }
   virtual ~FilterRTRSNAME() {}
   virtual std::ostream& print(std::ostream &out) const;
   virtual Filter* dup() const {
      return new FilterRTRSNAME(*this);
   } 
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "FilterRTRSNAME";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "rtrsname = \"" << rtrsname << "\"" << std::endl;
   }
#endif // ENABLE_DEBUG
};

class FilterFLTRNAME : public Filter {
public:
   SymID fltrname;
public:
   FilterFLTRNAME(SymID sid) {
      fltrname = sid;
   }
   virtual ~FilterFLTRNAME() {}
   virtual std::ostream& print(std::ostream &out) const;
   virtual Filter* dup() const {
      return new FilterFLTRNAME(*this);
   } 
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "FilterFLTRNAME";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "fltrname = \"" << fltrname << "\"" << std::endl;
   }
#endif // ENABLE_DEBUG
};

class FilterANY: public Filter {
public:
   FilterANY() {
   }
   virtual ~FilterANY() {}
   virtual std::ostream& print(std::ostream &out) const;
   virtual Filter* dup() const {
      return new FilterANY(*this);
   } 
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "FilterANY";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "ANY" << std::endl;
   }
#endif // ENABLE_DEBUG
};

class FilterPeerAS: public Filter {
public:
   FilterPeerAS() {
   }
   virtual ~FilterPeerAS() {}
   virtual std::ostream& print(std::ostream &out) const;
   virtual Filter* dup() const {
      return new FilterPeerAS(*this);
   } 
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "FilterPeerAS";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "peerAS" << std::endl;
   }
#endif // ENABLE_DEBUG
};

class FilterASPath: public Filter {
public:
   regexp *re;
public:
   FilterASPath(regexp *_re) : re(_re) {
   }
   virtual ~FilterASPath() {
      delete re;
   }
   FilterASPath(const FilterASPath &b) {
      re = b.re->dup();
   }

   virtual std::ostream& print(std::ostream &out) const;
   virtual Filter* dup() const {
      return new FilterASPath(*this);
   } 
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "FilterASPath";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "re = ..." << std::endl;
   }
#endif // ENABLE_DEBUG
};

class FilterPRFXList: public Filter, public PrefixRanges {
public:
   FilterPRFXList() {
   }
   virtual ~FilterPRFXList() {}

   virtual std::ostream& print(std::ostream &out) const;
   virtual Filter* dup() const {
      return new FilterPRFXList(*this);
   } 
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "FilterPRFXList";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << *this << std::endl;
   }
#endif // ENABLE_DEBUG
};

class FilterMPPRFXList: public Filter, public MPPrefixRanges {
public:
   FilterMPPRFXList() {
   }
   FilterMPPRFXList(const MPPrefixRanges &list) {
     (MPPrefixRanges &) *this = list;
   }
   virtual ~FilterMPPRFXList() {}

   virtual std::ostream& print(std::ostream &out) const;
   virtual Filter* dup() const {
      return new FilterMPPRFXList(*this);
   } 
   virtual FilterPRFXList* get_v4();
   virtual FilterMPPRFXList* get_v6();
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "FilterMPPRFXList";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); print(os); os << std::endl;
   }
#endif // ENABLE_DEBUG
};

class FilterAFI: public Filter {

public:
   Filter *f;
   ItemList *afi_list;

public:
   FilterAFI() {
   }
   FilterAFI(ItemList *_afi_list, Filter *_f) :
     afi_list(_afi_list),
     f(_f)
   {
   }
   FilterAFI(ItemAFI *_afi, Filter *_f) :
     f(_f) {
     afi_list = new ItemList;
     afi_list->append(_afi);
   }
   FilterAFI(const FilterAFI &pt) {
      f = (Filter *)pt.f->dup();
      afi_list = (ItemList *)pt.afi_list->dup();
   }
   virtual ~FilterAFI() {}

   virtual std::ostream& print(std::ostream &out) const;
   virtual Filter* dup() const {
      return new FilterAFI(*this);
   }
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "FilterAFI";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << *this << std::endl;
   }
#endif // ENABLE_DEBUG



}; 

class FilterRPAttribute: public Filter {
public: 
   const AttrRPAttr *rp_attr;   // into rp-attribute in the dictionary
   const AttrMethod *rp_method; // into the method
   ItemList         *args;      // arguments to the method
public:
   FilterRPAttribute(const AttrRPAttr *_rp_attr, 
		     const AttrMethod *_rp_mtd, 
		     ItemList *_args): 
      rp_attr(_rp_attr), rp_method(_rp_mtd), args(_args) {}

   FilterRPAttribute(const FilterRPAttribute &b) :
      rp_attr(b.rp_attr), rp_method(b.rp_method) {
      args = (ItemList *) b.args->dup();
   }

   virtual ~FilterRPAttribute() {
      if (args) 
	 delete args;
   }

   virtual std::ostream& print(std::ostream &out) const;
   virtual Filter* dup() const {
      return new FilterRPAttribute(*this);
   } 
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "FilterRPAttribute";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      // For rp_attr
      INDENT(indent); os << "rp_attr" << std::endl;
      INDENT(indent); os << "  _name = \"" << "\"" << std::endl;
      INDENT(indent); os << "  methods (RPMethod *)" << std::endl;
      INDENT(indent); os << "    _name = \"" << "\"" << std::endl;
      // For rp_method
      INDENT(indent); os << "rp_method" << std::endl;
      INDENT(indent); os << "  _name = \"" << "\"" << std::endl;
      // For rp_args
      INDENT(indent); os << "args (ItemList *)" << std::endl;
      args->printClass(os, indent + 2);
   }
#endif // ENABLE_DEBUG
};

////// The following is needed by route class ///////////////////////////

class FilterHAVE_COMPONENTS: public Filter {
public:
   FilterPRFXList *prfxs;
public:
   FilterHAVE_COMPONENTS(FilterPRFXList *p) : prfxs(p) {
   }
   FilterHAVE_COMPONENTS(const FilterHAVE_COMPONENTS &b) {
      prfxs = new FilterPRFXList(*b.prfxs);
   }
   virtual ~FilterHAVE_COMPONENTS() {
      delete prfxs;
   }

   virtual std::ostream& print(std::ostream &out) const;
   virtual Filter* dup() const {
      return new FilterHAVE_COMPONENTS(*this);
   } 
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "FilterHAVE_COMPONENTS";
   }
#endif // ENABLE_DEBUG
};

class FilterEXCLUDE: public Filter {
public:
   FilterPRFXList *prfxs;
public:
   FilterEXCLUDE(FilterPRFXList *p) : prfxs(p) {
   }
   FilterEXCLUDE(const FilterEXCLUDE &b) {
      prfxs = new FilterPRFXList(*b.prfxs);
   }
   virtual ~FilterEXCLUDE() {
      delete prfxs;
   }

   virtual std::ostream& print(std::ostream &out) const;
   virtual Filter* dup() const {
      return new FilterEXCLUDE(*this);
   } 
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "FilterEXCLUDE";
   }
#endif // ENABLE_DEBUG
};

//// the following is needed by route6 class ///////////////////////////

class FilterV6HAVE_COMPONENTS: public Filter {
public:
   FilterMPPRFXList *prfxs;
public:
   FilterV6HAVE_COMPONENTS(FilterMPPRFXList *p) : prfxs(p) {
   }
   FilterV6HAVE_COMPONENTS(const FilterV6HAVE_COMPONENTS &b) {
      prfxs = new FilterMPPRFXList(*b.prfxs);
   }
   virtual ~FilterV6HAVE_COMPONENTS() {
      delete prfxs;
   }

   virtual std::ostream& print(std::ostream &out) const;
   virtual Filter* dup() const {
      return new FilterV6HAVE_COMPONENTS(*this);
   }
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "FilterV6HAVE_COMPONENTS";
   }
#endif // ENABLE_DEBUG
};

class FilterV6EXCLUDE: public Filter {
public:
   FilterMPPRFXList *prfxs;
public:
   FilterV6EXCLUDE(FilterMPPRFXList *p) : prfxs(p) {
   }
   FilterV6EXCLUDE(const FilterV6EXCLUDE &b) {
      prfxs = new FilterMPPRFXList(*b.prfxs);
   }
   virtual ~FilterV6EXCLUDE() {
      delete prfxs;
   }

   virtual std::ostream& print(std::ostream &out) const;
   virtual Filter* dup() const {
      return new FilterV6EXCLUDE(*this);
   }
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "FilterV6EXCLUDE";
   }
#endif // ENABLE_DEBUG
};



////// The following is needed by <peering/mp-peering> ///////////////////////////

class FilterRouter : public Filter {
public:
   MPPrefix *ip;
public:
   FilterRouter(IPAddr *_ip) {
     ip = new MPPrefix((PrefixRange *) _ip);
   }
   FilterRouter(IPv6Addr *_ip) {
     ip = new MPPrefix((IPv6PrefixRange *) _ip);
   }
   FilterRouter(const FilterRouter &b) {
      ip = new MPPrefix(*b.ip);
   }

   virtual ~FilterRouter() {
      delete ip;
   }

   virtual std::ostream& print(std::ostream &out) const;
   virtual Filter* dup() const {
      return new FilterRouter(*this);
   } 
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "FilterRouter";
   }
#endif // ENABLE_DEBUG
};

class FilterRouterName : public Filter {
public:
   SymID name;
public:
   FilterRouterName(SymID dns) : name(dns) {}
   FilterRouterName(const FilterRouterName &b) {
      name = b.name;
   }
   virtual ~FilterRouterName() {
   }
   virtual std::ostream& print(std::ostream &out) const;
   virtual Filter* dup() const {
      return new FilterRouterName(*this);
   } 
#ifdef ENABLE_DEBUG
   virtual const char *className(void) const {
      return "FilterRouterName";
   }
   virtual void printClass(std::ostream &os, int indent) const {
      INDENT(indent); os << "name = \"" << name << "\"" << std::endl;
   }
#endif // ENABLE_DEBUG
};




#endif   // FILTER_HH
