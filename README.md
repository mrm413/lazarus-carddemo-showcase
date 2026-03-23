# Lazarus COBOL-to-C++17 Transpilation: AWS CardDemo

**44/44 programs compiled (100%)** | C++17 CICS Runtime Library | 189 Tests Passing | Web 3270 Terminal

## What Is This?

This repository showcases the output of the **Lazarus transpiler** applied to [AWS's CardDemo](https://github.com/aws-samples/aws-mainframe-modernization-carddemo) -- a full CICS/VSAM mainframe credit card processing system used by AWS as their reference application for mainframe modernization.

CardDemo is not a toy. It is a production-representative CICS/COBOL application with:

- **44 COBOL programs** spanning sign-on, account management, transaction processing, billing, credit card operations, reporting, and user administration
- **EXEC CICS** commands (SEND MAP, RECEIVE MAP, READ, WRITE, REWRITE, DELETE, STARTBR, READNEXT, READPREV, ENDBR, RETURN, XCTL)
- **EXEC SQL** embedded DB2 cursor operations
- **EXEC DLI** IMS/DL-I database calls
- **BMS screen maps** with full 3270 terminal I/O
- **VSAM file I/O** (KSDS, RRDS) with complex key structures
- **COPY/REPLACE** with 75 copybooks including CICS system copybooks (DFHAID, DFHBMSCA, DFHATTR)
- **EVALUATE/PERFORM/GO TO** control flow with deeply nested paragraph structures

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

**189 tests** (107 unit + 82 integration), **476 assertions**, all passing.

## Build

Requires CMake 3.16+ and a C++17 compiler (g++, MSVC, or clang++).

```bash
# Configure and build
cmake -B build
cmake --build build

# Run tests (189 tests)
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
- Function keys (F1-F12, Enter, Escape/Clear)
- BMS field attributes (protected, bright, dark, numeric, color)

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
| COUSR00C-03C | User management (list, add, update, delete) |
| COPAUA0C | Password authentication |
| COPAUS0C-2C | Password operations |
| CBPAUP0C | Batch password update |
| CODATE01 | Date conversion utility |
| COBSWAIT | CICS wait/delay handler |
| CBACT01C-04C | Batch account reports |
| CBCUS01C | Batch customer report |
| CBEXPORT/CBIMPORT | Data export/import |
| CBSTM03A/B | Statement generation (HTML/text) |
| CBTRN01C-03C | Batch transaction reports |
| DBUNLDGS | Database unload utility |
| PAUDBLOD/PAUDBUNL | DL/I database load/unload |

## Security Hardening

Every generated file includes security hardening:

- **Buffer overflow protection** -- `FixedString<N>` replaces raw `char[]` with bounds-enforced fixed-length strings
- **Integer overflow protection** -- `SafeInt<T>` wrapper with overflow detection
- **Decimal precision** -- `Decimal` class preserves COBOL COMP-3 packed decimal semantics
- **Bounds checking** -- `.at()` access for all array/string operations
- **Input validation** -- `safe::` namespace functions for all external data
- **Memory safety** -- RAII throughout, zero raw pointers, zero manual memory management
- **Type safety** -- COBOL-compatible type system (`FixedString`, `SafeInt`, `Decimal`)

## Dependencies

All dependencies are vendored (zero system requirements beyond a C++17 compiler):

- **SQLite** amalgamation (storage engine for VSAM + SQL)
- **cpp-httplib** single-header HTTP server
- **doctest** single-header test framework

## About Lazarus

Lazarus is a COBOL-to-C++17 transpiler developed by [Lazarus Systems](https://lazarus-systems.com). It converts legacy mainframe COBOL (including CICS, DB2, IMS/DL-I, and batch) into modern, hardened C++17.

## Related Showcases

- [CMS Medicare Ironclad Showcase](https://github.com/mrm413/cms-medicare-ironclad-showcase) -- 55 CMS Medicare pricing programs transpiled to Rust (100%)
- [CMS Medicare Lazarus Showcase](https://github.com/mrm413/cms-medicare-lazarus-showcase) -- 55 CMS Medicare pricing programs transpiled to C++17 (100%)
- [Lazarus COBOL Showcase](https://github.com/mrm413/lazarus-cobol-showcase) -- 1,607 GnuCOBOL test programs transpiled to C++17 (100%)

## License

Licensed under the [Apache License, Version 2.0](LICENSE).

The original CardDemo application is provided by AWS under the [MIT-0 License](https://github.com/aws-samples/aws-mainframe-modernization-carddemo/blob/main/LICENSE).

All modifications and additions -- including the C++17 transpiled programs, the CICS runtime library (`cobol-runtime/`), the React 3270 terminal UI (`carddemo-ui/`), the build system, program registry, and test suite -- are Copyright 2025 Michael R. Mull / Lazarus Systems. See [NOTICE](NOTICE) for details.
