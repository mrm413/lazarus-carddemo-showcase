/**
 * Program Registry — Registers all CardDemo programs, VSAM files, BMS maps,
 * and TRANSID-to-program mappings.
 */

#include "program_registry.h"

namespace carddemo {

void register_all(lazarus::cics::CicsContext& ctx,
                  lazarus::transaction::TransactionLoop& loop) {
    using namespace lazarus::vsam;

    // Register VSAM files (KSDS = Key-Sequenced)
    ctx.register_vsam_file("ACCTDAT",  VsamOrganization::KSDS);
    ctx.register_vsam_file("CARDDAT",  VsamOrganization::KSDS);
    ctx.register_vsam_file("CUSTDAT",  VsamOrganization::KSDS);
    ctx.register_vsam_file("TRNXDAT",  VsamOrganization::KSDS);
    ctx.register_vsam_file("TRNXTYPE", VsamOrganization::KSDS);
    ctx.register_vsam_file("USRSEC",   VsamOrganization::KSDS);
    ctx.register_vsam_file("DISCGRP",  VsamOrganization::KSDS);

    // Register TRANSID → program mappings
    loop.registry().register_transid("SIGN", "COSGN00C");
    loop.registry().register_transid("MENU", "COMEN01C");
    loop.registry().register_transid("CADM", "COADM01C");
    loop.registry().register_transid("CACT", "COACCT01");
    loop.registry().register_transid("CAUP", "COACTUPC");
    loop.registry().register_transid("CAVW", "COACTVWC");
    loop.registry().register_transid("CBIL", "COBIL00C");
    loop.registry().register_transid("CCRD", "COCRDLIC");
    loop.registry().register_transid("CCSL", "COCRDSLC");
    loop.registry().register_transid("CCUP", "COCRDUPC");
    loop.registry().register_transid("CTRN", "COTRN00C");
    loop.registry().register_transid("CTR1", "COTRN01C");
    loop.registry().register_transid("CTR2", "COTRN02C");
    loop.registry().register_transid("CTRT", "COTRTLIC");
    loop.registry().register_transid("CTRU", "COTRTUPC");
    loop.registry().register_transid("CRPT", "CORPT00C");
    loop.registry().register_transid("CUSR", "COUSR00C");
    loop.registry().register_transid("CUS1", "COUSR01C");
    loop.registry().register_transid("CUS2", "COUSR02C");
    loop.registry().register_transid("CUS3", "COUSR03C");
    loop.registry().register_transid("CPAU", "COPAUA0C");
    loop.registry().register_transid("CPS0", "COPAUS0C");
    loop.registry().register_transid("CPS1", "COPAUS1C");
    loop.registry().register_transid("CPS2", "COPAUS2C");
    loop.registry().register_transid("CDAT", "CODATE01");
    loop.registry().register_transid("CWAI", "COBSWAIT");

    // Initialize CardDemo SQL schema
    ctx.sql_context().init_carddemo_schema();

    // Seed default admin user
    ctx.sql_context().insert(
        "INSERT OR IGNORE INTO USRSEC VALUES ('ADMIN', 'ADMIN', 'System', 'Admin', 'A')");
    ctx.sql_context().insert(
        "INSERT OR IGNORE INTO USRSEC VALUES ('USER1', 'PASS1', 'Test', 'User', 'U')");
}

} // namespace carddemo
