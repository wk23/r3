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

#if !defined(QUERYRESULT_H)
#define QUERYRESULT_H

#include "Utilities/UnorderedMap.h"
#include "Platform/CompilerDefs.h"

#if PLATFORM == PLATFORM_WINDOWS
#   define FD_SETSIZE 1024
#   include <winsock2.h>
#   include <mysql/mysql.h>
#else
#   include <mysql.h>
#endif

class MANGOS_DLL_SPEC QueryResult
{
    public:
        QueryResult(MYSQL_RES *result, uint64 rowCount, uint32 fieldCount);

        ~QueryResult();

        bool NextRow();

        typedef UNORDERED_MAP<uint32, std::string> FieldNames;

        uint32 GetField_idx(const std::string &name) const
        {
            for(FieldNames::const_iterator iter = GetFieldNames().begin(); iter != GetFieldNames().end(); ++iter)
            {
                if(iter->second == name)
                    return iter->first;
            }
            printf("Unknown field in request: %s\n", name.c_str());
            exit(1);
            return uint32(-1);
        }

        Field *Fetch() const { return mCurrentRow; }

        const Field & operator [] (int index) const { return mCurrentRow[index]; }

        const Field & operator [] (const std::string &name) const
        {
            return mCurrentRow[GetField_idx(name)];
        }

        uint32 GetFieldCount() const { return mFieldCount; }
        uint64 GetRowCount() const { return mRowCount; }
        FieldNames const& GetFieldNames() const {return mFieldNames; }

    private:
        enum Field::DataTypes ConvertNativeType(enum_field_types mysqlType) const;
        void EndQuery();

        Field *mCurrentRow;
        uint32 mFieldCount;
        uint64 mRowCount;
        FieldNames mFieldNames;
        MYSQL_RES *mResult;
};
#endif
