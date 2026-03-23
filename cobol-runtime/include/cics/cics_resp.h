#pragma once
/**
 * CicsResp — CICS response codes and DFHRESP constants
 */

#include <cstdint>

namespace lazarus {
namespace cics {

// DFHRESP constants (matching CICS specification)
constexpr int32_t DFHRESP_NORMAL        = 0;
constexpr int32_t DFHRESP_ERROR         = 1;
constexpr int32_t DFHRESP_EOF           = 2;
constexpr int32_t DFHRESP_NOTFND        = 13;
constexpr int32_t DFHRESP_DUPREC        = 14;
constexpr int32_t DFHRESP_DUPKEY        = 15;
constexpr int32_t DFHRESP_INVREQ        = 16;
constexpr int32_t DFHRESP_IOERR         = 17;
constexpr int32_t DFHRESP_DISABLED      = 18;
constexpr int32_t DFHRESP_NOTOPEN       = 19;
constexpr int32_t DFHRESP_ENDDATA       = 20;
constexpr int32_t DFHRESP_LENGERR       = 22;
constexpr int32_t DFHRESP_ITEMERR       = 26;
constexpr int32_t DFHRESP_PGMIDERR      = 27;
constexpr int32_t DFHRESP_QIDERR        = 44;
constexpr int32_t DFHRESP_NOTAUTH       = 70;
constexpr int32_t DFHRESP_CONTAINERERR  = 97;

enum class CicsResp : int32_t {
    Normal       = DFHRESP_NORMAL,
    Error        = DFHRESP_ERROR,
    Eof          = DFHRESP_EOF,
    NotFound     = DFHRESP_NOTFND,
    DuplicateRec = DFHRESP_DUPREC,
    DuplicateKey = DFHRESP_DUPKEY,
    InvalidReq   = DFHRESP_INVREQ,
    IoErr        = DFHRESP_IOERR,
    Disabled     = DFHRESP_DISABLED,
    NotOpen      = DFHRESP_NOTOPEN,
    EndData      = DFHRESP_ENDDATA,
    LenErr       = DFHRESP_LENGERR,
    ItemErr      = DFHRESP_ITEMERR,
    PgmIdErr     = DFHRESP_PGMIDERR,
    QIdErr       = DFHRESP_QIDERR,
    NotAuth      = DFHRESP_NOTAUTH,
    ContainerErr = DFHRESP_CONTAINERERR,
};

inline const char* cics_resp_str(CicsResp r) noexcept {
    switch (r) {
        case CicsResp::Normal:       return "NORMAL";
        case CicsResp::Error:        return "ERROR";
        case CicsResp::Eof:          return "EOF";
        case CicsResp::NotFound:     return "NOTFND";
        case CicsResp::DuplicateRec: return "DUPREC";
        case CicsResp::DuplicateKey: return "DUPKEY";
        case CicsResp::InvalidReq:   return "INVREQ";
        case CicsResp::IoErr:        return "IOERR";
        case CicsResp::Disabled:     return "DISABLED";
        case CicsResp::NotOpen:      return "NOTOPEN";
        case CicsResp::EndData:      return "ENDDATA";
        case CicsResp::LenErr:       return "LENGERR";
        case CicsResp::ItemErr:      return "ITEMERR";
        case CicsResp::PgmIdErr:     return "PGMIDERR";
        case CicsResp::QIdErr:       return "QIDERR";
        case CicsResp::NotAuth:      return "NOTAUTH";
        case CicsResp::ContainerErr: return "CONTAINERERR";
        default: return "UNKNOWN";
    }
}

} // namespace cics
} // namespace lazarus
