/* This file is part of the Navitab project. See the README and LICENSE for details. */

#include "backingstore.h"
#include "navitab/platform.h"
#include "navitab/window.h"
#include <fmt/core.h>
#include <sqlite3.h>
#include <cassert>

namespace navitab {

// TODO - A 2nd instance of a Navitab executable will NOT coexist well
// since the BackingStore database is not multi-process capable. So we
// will need to consider (eg) an OS named semaphore to identify if this
// instance is the first or a subsequent process. Only the first process
// should get to use the normal BackingStore database. The rest will need
// to create their own database and delete it on shutdown.

static int callback(void *p, int n, char **data, char **names);

BackingStore::BackingStore(std::shared_ptr<PathServices> ps)
:   LOG(std::make_unique<logging::Logger>("store")),
    dbHandle(nullptr)
{
    sqlite3_initialize();
    std::filesystem::path db = ps->DataFilesPath();
    db /= "navitab.sqlite";
    int flags = SQLITE_OPEN_READWRITE | SQLITE_OPEN_CREATE;
    int r = sqlite3_open_v2(db.string().c_str(), &dbHandle, flags, 0);

    // If the database was only just created, then this command will fail.
    char *errmsg = nullptr;
    r = sqlite3_exec(dbHandle, "SELECT COUNT(*) FROM pixmap;",  callback, this, &errmsg);
    if (r || errmsg) {
        sqlite3_free(errmsg);
        errmsg = nullptr;
        CreateTables();
    }
}

BackingStore::~BackingStore()
{
    if (dbHandle) {
        int r = sqlite3_close(dbHandle);
        dbHandle = nullptr;
        sqlite3_shutdown();
    }
}

std::shared_ptr<ImageBuffer> BackingStore::GetPixmap(const std::string &name)
{
    std::shared_ptr<ImageBuffer> pixmap = nullptr;
    sqlite3_stmt* stmtRetrieve = nullptr;
    sqlite3_prepare_v2(dbHandle, "SELECT height, width, pixels FROM pixmap WHERE name = ?", -1, &stmtRetrieve, nullptr);
    sqlite3_bind_text(stmtRetrieve, 1, name.c_str(), (int)name.size(), SQLITE_STATIC);
    if (sqlite3_step(stmtRetrieve) == SQLITE_ROW)
    {
        auto height = sqlite3_column_int(stmtRetrieve, 0);
        auto width = sqlite3_column_int(stmtRetrieve, 1);
        pixmap = std::make_shared<ImageBuffer>(width, height);
        auto bsize = sqlite3_column_bytes(stmtRetrieve, 2);
        assert(bsize <= (height * width * sizeof(uint32_t)));
        auto bptr = sqlite3_column_blob(stmtRetrieve, 2);
        memcpy(pixmap->GetBufferPtr(), bptr, bsize);
    }
    sqlite3_finalize(stmtRetrieve);
    return pixmap;
}

void BackingStore::StorePixmap(const std::string &name, std::shared_ptr<ImageBuffer> pixmap)
{
    sqlite3_stmt* stmtInsert = nullptr;
    sqlite3_prepare_v2(dbHandle, "INSERT INTO pixmap (name, height, width, pixels) VALUES (?, ?, ?, ?)", -1, &stmtInsert, nullptr);
    sqlite3_bind_text(stmtInsert, 1, name.c_str(), (int)name.size(), SQLITE_STATIC);
    int h = pixmap->Height();
    sqlite3_bind_int(stmtInsert, 2, h);
    int w = pixmap->Width();
    sqlite3_bind_int(stmtInsert, 3, w);
    sqlite3_bind_blob(stmtInsert, 4, pixmap->GetBufferPtr(), h * w * sizeof(uint32_t), SQLITE_STATIC);
    if (sqlite3_step(stmtInsert) != SQLITE_DONE) {
        // LOGE();
    }
    sqlite3_finalize(stmtInsert);

}

int BackingStore::ExecCallback(int n, char **data, char **names)
{
    return 0;
}

static const char *createCmd =
    "CREATE TABLE pixmap (name TEXT, height INT, width INT, pixels BLOB);"
    "CREATE INDEX idx_pixmap_name ON pixmap(name);"
    "CREATE TABLE doc (name TEXT, expires INT, bindata BLOB);"
    "CREATE INDEX idx_doc_name ON doc(name);";

void BackingStore::CreateTables()
{
    char *errmsg = nullptr;
    int r = sqlite3_exec(dbHandle, createCmd, nullptr, nullptr, &errmsg);
    if (r || errmsg) {
        LOGF(fmt::format("Failed to create backing store database - {}", errmsg));
        sqlite3_free(errmsg);
        errmsg = nullptr;
    }
}

static int callback(void *p, int n, char **data, char **names)
{
    BackingStore *bs = reinterpret_cast<BackingStore *>(p);
    return bs->ExecCallback(n, data, names);
}



}
