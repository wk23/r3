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

#ifndef __SQLOPERATIONS_H
#define __SQLOPERATIONS_H

#include "Common.h"

#include <queue>
#include "Utilities/Callback.h"
#include "ace/Thread_Mutex.h"
#include "ace/Method_Request.h"
#include "LockedQueue.h"
#include "Database/PreparedStatement.h"

/// ---- BASE ---

class Database;
class SqlDelayThread;

class SqlOperation
{
    public:
        virtual void OnRemove() { delete this; }
        virtual void Execute(Database *db) = 0;
        virtual ~SqlOperation() {}
};

/// ---- ASYNC STATEMENTS / TRANSACTIONS ----

class SqlStatement : public SqlOperation
{
    private:
        const char *m_sql;
    public:
        SqlStatement(const char *sql) : m_sql(strdup(sql)){}
        ~SqlStatement() { void* tofree = const_cast<char*>(m_sql); free(tofree); }
        void Execute(Database *db);
};

class SqlPreparedStatement : public SqlOperation
{
    private:
        PreparedStmt *m_stmt;
        char *m_data;
    public:
        SqlPreparedStatement(PreparedStmt *stmt, char *data) : m_stmt(stmt), m_data(data) {}
        ~SqlPreparedStatement() { m_stmt->Free(m_data); }
        void Execute(Database *db);
};

class SqlTransaction : public SqlOperation
{
    private:
        // prepared statement + data or NULL + sql statement
        typedef std::pair<PreparedStmt *, const char *> StmtPair;
        std::queue<StmtPair> m_queue;
    public:
        SqlTransaction() {}
        void DelayExecute(const char *sql) { m_queue.push(StmtPair(NULL, strdup(sql))); }
        void DelayExecute(PreparedStmt *stmt, const char *data) { m_queue.push(StmtPair(stmt, data)); }
        void Execute(Database *db);
};

/// ---- ASYNC QUERIES ----

class SqlQuery;                                             /// contains a single async query
class QueryResult;                                          /// the result of one
class SqlResultQueue;                                       /// queue for thread sync
class SqlQueryHolder;                                       /// groups several async quries
class SqlQueryHolderEx;                                     /// points to a holder, added to the delay thread

class SqlResultQueue : public ACE_Based::LockedQueue<MaNGOS::IQueryCallback* , ACE_Thread_Mutex>
{
    public:
        SqlResultQueue() {}
        void Update();
};

class SqlQuery : public SqlOperation
{
    private:
        const char *m_sql;
        MaNGOS::IQueryCallback * m_callback;
        SqlResultQueue * m_queue;
    public:
        SqlQuery(const char *sql, MaNGOS::IQueryCallback * callback, SqlResultQueue * queue)
            : m_sql(strdup(sql)), m_callback(callback), m_queue(queue) {}
        ~SqlQuery() { void* tofree = const_cast<char*>(m_sql); free(tofree); }
        void Execute(Database *db);
};

class SqlQueryHolder
{
    friend class SqlQueryHolderEx;
    private:
        typedef std::pair<const char*, QueryResult*> SqlResultPair;
        std::vector<SqlResultPair> m_queries;
    public:
        SqlQueryHolder() {}
        ~SqlQueryHolder();
        bool SetQuery(size_t index, const char *sql);
        bool SetPQuery(size_t index, const char *format, ...) ATTR_PRINTF(3,4);
        void SetSize(size_t size);
        QueryResult* GetResult(size_t index);
        void SetResult(size_t index, QueryResult *result);
        bool Execute(MaNGOS::IQueryCallback * callback, SqlDelayThread *thread, SqlResultQueue *queue);
};

class SqlQueryHolderEx : public SqlOperation
{
    private:
        SqlQueryHolder * m_holder;
        MaNGOS::IQueryCallback * m_callback;
        SqlResultQueue * m_queue;
    public:
        SqlQueryHolderEx(SqlQueryHolder *holder, MaNGOS::IQueryCallback * callback, SqlResultQueue * queue)
            : m_holder(holder), m_callback(callback), m_queue(queue) {}
        void Execute(Database *db);
};

class SqlAsyncTask : public ACE_Method_Request
{
public:
    SqlAsyncTask(Database * db, SqlOperation * op) : m_db(db), m_op(op) {}
    ~SqlAsyncTask() { if(!m_op) return; delete m_op; }

    int call()
    {
        if(!m_db || !m_op)
            return -1;

        m_op->Execute(m_db);
        return 0;
    }

private:
    Database * m_db;
    SqlOperation * m_op;
};
#endif                                                      //__SQLOPERATIONS_H
