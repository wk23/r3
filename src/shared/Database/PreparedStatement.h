/*
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 */

#ifndef _PREPARED_STATEMENT_H_
#define _PREPARED_STATEMENT_H_

#include <cstdarg>

class QueryResult;

template< class D >
class PreparedStatementBase
{
    public:
        bool DirectExecute();
        bool Execute();
        QueryResult * Query();

        // used as: void DirectPExecute(...);
        template<class T> bool DirectPExecute(T arg1, ...)
            { va_list ap; va_start(ap, arg1); bool ret = (static_cast<D*>(this))->_DirectPExecute((void*)&arg1, ap); va_end(ap); return ret; }
        // used as: void PExecute(...);
        template<class T> bool PExecute(T arg1, ...)
            { va_list ap; va_start(ap, arg1); bool ret = (static_cast<D*>(this))->_PExecute((void*)&arg1, ap); va_end(ap); return ret; }
        // used as: QueryResult * PQuery(...);
        template<class T> QueryResult * PQuery(T arg1, ...)
            { va_list ap; va_start(ap, arg1); QueryResult * ret = (static_cast<D*>(this))->_PQuery((void*)&arg1, ap); va_end(ap); return ret; }

        bool Execute(char *raw_data);
        bool DirectExecute(char *raw_data);

		void Free(char *raw_data);

        // cannot access these if they are private
        bool _DirectPExecute(void *arg1, va_list ap);
        bool _PExecute(void *arg1, va_list ap);
        QueryResult * _PQuery(void *arg1, va_list ap);
};

template< class D >
class PreparedStatementBinderBase
{
    public:
        template< class T >
        D & operator << (T x)
        {
            (static_cast<D*>(this))->append(x);
            return *(static_cast<D*>(this));
        }

        void append(uint8 x);
        void append(int8 x);
        void append(uint16 x);
        void append(int16 x);
        void append(uint32 x);
        void append(int32 x);
        void append(uint64 x);
        void append(int64 x);
        void append(float x);
        void append(double x);
        void append(char x);
        void append(char *str);
        void append(uint32 len, char *buf);
};

template< class D >
class PreparedStatementAsyncBinderBase : public PreparedStatementBinderBase<D>
{
    public:
        bool Execute();
};

template< class D >
class PreparedStatementDirectBinderBase : public PreparedStatementBinderBase<D>
{
    public:
        bool DirectExecute();
};

#define PreparedStmt MySQLPreparedStatement
#define PSBinder MySQLPreparedStatementBinder
#define PSDirectBinder MySQLPreparedStatementDirectBinder
#include "Database/MySQLPreparedStatement.h"

#endif
