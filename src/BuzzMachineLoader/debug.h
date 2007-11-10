/* $Id: debug.h,v 1.2 2007-11-10 19:06:37 ensonic Exp $
 *
 * Buzz Machine Loader
 * Copyright (C) 2006 Buzztard team <buzztard-devel@lists.sf.net>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public
 * License along with this library; if not, write to the
 * Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

typedef void(*DebugLoggerFunc)(char *str);

extern DebugLoggerFunc debug_log_func;

#ifdef HAVE_CONFIG_H
#include "config.h"
#ifdef LOG
#define _DEBUG
#endif
#endif

#ifdef _DEBUG
extern void DebugLogger(const char *file, unsigned int line, const char *func, const char *obj, const char *fmt,...);

#define DBG(str) DebugLogger(__FILE__,__LINE__,__FUNCTION__,NULL,str)
#define DBG1(str,p1) DebugLogger(__FILE__,__LINE__,__FUNCTION__,NULL,str,p1)
#define DBG2(str,p1,p2) DebugLogger(__FILE__,__LINE__,__FUNCTION__,NULL,str,p1,p2)
#define DBG3(str,p1,p2,p3) DebugLogger(__FILE__,__LINE__,__FUNCTION__,NULL,str,p1,p2,p3)
#define DBG4(str,p1,p2,p3,p4) DebugLogger(__FILE__,__LINE__,__FUNCTION__,NULL,str,p1,p2,p3,p4)
#define DBG5(str,p1,p2,p3,p4,p5) DebugLogger(__FILE__,__LINE__,__FUNCTION__,NULL,str,p1,p2,p3,p4,p5)
#define DBG6(str,p1,p2,p3,p4,p5,p6) DebugLogger(__FILE__,__LINE__,__FUNCTION__,NULL,str,p1,p2,p3,p4,p5,p6)
#define FIXME DebugLogger(__FILE__,__LINE__,__FUNCTION__,NULL,"!!! FIXME !!!\n")
#define DBGO(obj,str) DebugLogger(__FILE__,__LINE__,__FUNCTION__,obj,str)
#define DBGO1(obj,str,p1) DebugLogger(__FILE__,__LINE__,__FUNCTION__,obj,str,p1)
#define DBGO2(obj,str,p1,p2) DebugLogger(__FILE__,__LINE__,__FUNCTION__,obj,str,p1,p2)
#define DBGO3(obj,str,p1,p2,p3) DebugLogger(__FILE__,__LINE__,__FUNCTION__,obj,str,p1,p2,p3)
#define DBGO4(obj,str,p1,p2,p3,p4) DebugLogger(__FILE__,__LINE__,__FUNCTION__,obj,str,p1,p2,p3,p4)
#define DBGO5(obj,str,p1,p2,p3,p4,p5) DebugLogger(__FILE__,__LINE__,__FUNCTION__,obj,str,p1,p2,p3,p4,p5)
#define DBGO6(obj,str,p1,p2,p3,p4,p5,p6) DebugLogger(__FILE__,__LINE__,__FUNCTION__,obj,str,p1,p2,p3,p4,p5,p6)
#else
#define DBG(str)
#define DBG1(str,p1)
#define DBG2(str,p1,p2)
#define DBG3(str,p1,p2,p3)
#define DBG4(str,p1,p2,p3,p4)
#define DBG5(str,p1,p2,p3,p4,p5)
#define DBG6(str,p1,p2,p3,p4,p5,p6)
#define FIXME
#define DBGO(obj,str)
#define DBGO1(obj,str,p1)
#define DBGO2(obj,str,p1,p2)
#define DBGO3(obj,str,p1,p2,p3)
#define DBGO4(obj,str,p1,p2,p3,p4)
#define DBGO5(obj,str,p1,p2,p3,p4,p5)
#define DBGO6(obj,str,p1,p2,p3,p4,p5,p6)
#endif
