# Lazarus CardDemo: COBOL→C++17 with CICS Runtime + 3270 Web UI

**44 / 44 CICS COBOL programs transpiled and compiled (100%)** | **189 runtime tests passing (107 unit + 82 integration, 476 assertions)** | C++17 CICS Runtime Library | Web 3270 Terminal UI

---

## What Is This?

This repository showcases the output of the **Lazarus** COBOL→C++17 transpiler applied to [AWS's CardDemo](https://github.com/aws-samples/aws-mainframe-modernization-carddemo) — a full CICS/VSAM mainframe credit card processing system used by AWS as their reference application for mainframe modernization.

CardDemo is not a toy. It is a production-representative CICS/COBOL application with:

- **44 COBOL programs** spanning sign-on, account management, transaction processing, billing, credit card operations, reporting, and user administration
- **EXEC CICS** commands (SEND MAP, RECEIVE MAP, READ, WRITE, REWRITE, DELETE, STARTBR, READNEXT, READPREV, ENDBR, RETURN, XCTL)
- **EXEC SQL** embedded DB2 cursor operations
- **EXEC DLI** IMS/DL-I database calls
- **BMS screen maps** with full 3270 terminal I/O
- **VSAM file I/O** (KSDS, RRDS) with complex key structures
- **COPY/REPLACE** with 75 copybooks including CICS system copybooks (DFHAID, DFHBMSCA, DFHATTR)
- **EVALUATE/PERFORM/GO TO** control flow with deeply nested paragraph structures

Lazarus is a proprietary transpilation engine built by [Torsova LLC](https://lazarus-systems.com).

---

## Status (as of 2026-04-21)

| Claim | Number | What's measured |
|-------|-------:|-----------------|
| COBOL programs transpiled | **44 / 44** | Every CardDemo program produces a `_hardened.cpp` output |
| C++17 programs that compile | **44 / 44** | All compile clean under `-std=c++17 -Wall -Wextra -Wpedantic` |
| CICS runtime unit tests | **107 / 107** | doctest assertions across BMS, CICS, decimal, fixed-string, safe-int, SQL, VSAM |
| Integration tests | **82 / 82** | End-to-end transaction-loop tests including session, TRANSID routing, screen I/O |
| Total assertions | **476 / 476** | |

What this repo **does not** claim:
- It does **not** claim runtime parity against a live mainframe CICS region. CardDemo is AWS's reference application; there is no IBM-mainframe oracle to diff against.
- The **189 passing tests** are runtime tests of the CICS *runtime library* (this repo's own VSAM, BMS, SQL, decimal, etc.) — not parity tests against a reference output. They are full behavioral tests, but the oracle is the spec, not a reference binary.

---

## Runtime Library

The CICS runtime (`cobol-runtime/`) provides a complete execution environment:

| Component | Description |
|-----------|-------------|
| **VSAM Storage** | SQLite-backed KSDS/RRDS/ESDS file I/O with browse cursors |
| **BMS Screen Engine** | Field attributes, AID keys, map/mapset registry, screen I/O |
| **CICS Context** | 25+ CICS operations: file I/O, TSQ/TDQ, program control, containers |
| **SQL Context** | SQLite execution with host variable substitution, cursors, CardDemo schema |
| **Transaction Loop** | Session management, TRANSID routing, pseudo-conversational dispatch |
| **Web Server** | HTTP REST API (cpp-httplib) for 3270 terminal emulation |

---

## Build & Run

Requires CMake 3.16+ and a C++17 compiler (g++, MSVC, or clang++).

```bash
# Configure and build
cmake -B build
cmake --build build

# Run all 189 tests
./build/cobol-runtime/cobol-runtime-tests

# Run CardDemo server
./build/carddemo --web --port 8270
```

### React 3270 Terminal UI

```bash
cd carddemo-ui
npm install
npm run dev
# Open http://localhost:5173
```

The web UI connects to the CardDemo server at `localhost:8270` and provides a full 3270 terminal emulator with:
- 80x24 character grid with IBM Plex Mono font
- Field-level input with cursor navigation
- Tab/Shift+Tab between input fields
- Function keys (F1–F12, Enter, Escape/Clear)
- BMS field attributes (protected, bright, dark, numeric, color)

---

## How to Verify

```bash
# Compile every transpiled program
cd programs
for prog in *_hardened.cpp; do
    g++ -std=c++17 -Wall -Wextra -Wpedantic -O2 -c "$prog" -o /dev/null \
        && echo "OK: $prog" \
        || echo "FAIL: $prog"
done | tee compile_log.txt
grep -c "^OK:"  compile_log.txt   # should be 44

# Run runtime tests with verbose output
./build/cobol-runtime/cobol-runtime-tests --reporters=console --duration=true
# Expected: 189 test cases, 476 assertions, 0 failures
```

---

## Programs

| Program | Description |
|---------|-------------|
| COSGN00C | CICS sign-on / authentication |
| COMEN01C | Main menu navigation |
| COADM01C | Administrative functions |
| COACCT01 | Account listing |
| COACTUPC | Account update (largest program) |
| COACTVWC | Account view |
| COBIL00C | Bill payment processing |
| COCRDLIC | Credit card listing |
| COCRDSLC | Credit card selection |
| COCRDUPC | Credit card update |
| COTRN00C | Transaction listing |
| COTRN01C | Transaction add |
| COTRN02C | Transaction update |
| COTRTLIC | Transaction type listing (CICS + SQL) |
| COTRTUPC | Transaction type update |
| CORPT00C | Report generation |
| COUSR00C–03C | User management (list, add, update, delete) |
| COPAUA0C | Password authentication |
| COPAUS0C–2C | Password operations |
| CBPAUP0C | Batch password update |
| CODATE01 | Date conversion utility |
| COBSWAIT | CICS wait/delay handler |
| CBACT01C–04C | Batch account reports |
| CBCUS01C | Batch customer report |
| CBEXPORT / CBIMPORT | Data export/import |
| CBSTM03A / CBSTM03B | Statement generation (HTML/text) |
| CBTRN01C–03C | Batch transaction reports |
| DBUNLDGS | Database unload utility |
| PAUDBLOD / PAUDBUNL | DL/I database load/unload |

---

## Security Hardening

Every generated file includes security hardening:

- **Buffer overflow protection** — `FixedString<N>` replaces raw `char[]` with bounds-enforced fixed-length strings
- **Integer overflow protection** — `SafeInt<T>` wrapper with overflow detection
- **Decimal precision** — `Decimal` class preserves COBOL COMP-3 packed decimal semantics
- **Bounds checking** — `.at()` access for all array/string operations
- **Input validation** — `safe::` namespace functions for all external data
- **Memory safety** — RAII throughout, zero raw pointers, zero manual memory management
- **Type safety** — COBOL-compatible type system (`FixedString`, `SafeInt`, `Decimal`)

---

## Dependencies

All dependencies are vendored (zero system requirements beyond a C++17 compiler):

- **SQLite** amalgamation — storage engine for VSAM + SQL
- **cpp-httplib** single-header HTTP server
- **doctest** single-header test framework

---

## Sister Pipelines

Lazarus is one of three COBOL transpilers in the same suite:

- **[Lazarus federal-suite C++17](https://github.com/mrm413/lazarus-cobol-showcase)** — 1,607 GnuCOBOL test programs, **1,192 / 1,192 runtime parity (100%)**, full per-test JSON evidence
- **[CMS Medicare Lazarus](https://github.com/mrm413/cms-medicare-lazarus-showcase)** — 55 production CMS Medicare pricer programs, 100% compile (cross-validated via Ironclad/Rust byte-for-byte parity)
- **[CMS Medicare Ironclad](https://github.com/mrm413/cms-medicare-ironclad-showcase)** — same 55 CMS Medicare programs, Rust target with byte-for-byte parity verified across SNF/ESRD/Hospice/HH/IPF/IRF families

---

## About Lazarus

Lazarus is a COBOL-to-C++17 transpiler developed by [Lazarus Systems](https://lazarus-systems.com). It converts legacy mainframe COBOL (including CICS, DB2, IMS/DL-I, and batch) into modern, hardened C++17.

Lazarus is part of a suite of legacy modernization tools including transpilers for COBOL (C++17 and Rust), VB6, Stored Procedures, Crystal Reports, SAS, and Microsoft Access.

---

## License

Licensed under the [Apache License, Version 2.0](LICENSE).

The original CardDemo application is provided by AWS under the [MIT-0 License](https://github.com/aws-samples/aws-mainframe-modernization-carddemo/blob/main/LICENSE).

All modifications and additions — including the C++17 transpiled programs, the CICS runtime library (`cobol-runtime/`), the React 3270 terminal UI (`carddemo-ui/`), the build system, program registry, and test suite — are Copyright 2025–2026 Michael R. Mull / Lazarus Systems. See [NOTICE](NOTICE) for details.
