# Lazarus COBOL-to-C++17 Transpilation: AWS CardDemo

**44/44 programs compiled (100%)** | 66,489 lines of hardened C++17 | Zero manual edits

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

Every program in `programs/` compiled with `g++ -std=c++17 -O2` with **zero errors**.

## Programs

| Program | Lines | Description |
|---------|------:|-------------|
| COSGN00C | 1,516 | CICS sign-on / authentication |
| COMEN01C | 1,365 | Main menu navigation |
| COADM01C | 1,299 | Administrative functions |
| COACCT01 | 1,274 | Account listing |
| COACTUPC | 3,846 | Account update (largest program) |
| COACTVWC | 1,616 | Account view |
| COBIL00C | 1,520 | Bill payment processing |
| COCRDLIC | 1,805 | Credit card listing |
| COCRDSLC | 1,247 | Credit card selection |
| COCRDUPC | 2,043 | Credit card update |
| COTRN00C | 1,612 | Transaction listing |
| COTRN01C | 1,278 | Transaction add |
| COTRN02C | 1,837 | Transaction update |
| COTRTLIC | 2,354 | Transaction type listing (CICS + SQL) |
| COTRTUPC | 2,042 | Transaction type update |
| CORPT00C | 1,767 | Report generation |
| COUSR00C | 1,490 | User listing |
| COUSR01C | 1,389 | User add |
| COUSR02C | 1,342 | User update |
| COUSR03C | 1,304 | User delete |
| COPAUA0C | 1,396 | Password authentication |
| COPAUS0C | 1,395 | Password search |
| COPAUS1C | 1,262 | Password update |
| COPAUS2C | 1,135 | Password confirmation |
| CBPAUP0C | 1,410 | Batch password update |
| CODATE01 | 1,274 | Date conversion utility |
| COBSWAIT | 1,033 | CICS wait/delay handler |
| COBTUPDT | 1,242 | Batch update driver |
| CSUTLDTC | 1,098 | Utility date conversion |
| CBACT01C | 1,161 | Batch account report |
| CBACT02C | 1,154 | Batch account extract |
| CBACT03C | 1,155 | Batch account summary |
| CBACT04C | 1,183 | Batch account cross-reference |
| CBCUS01C | 1,203 | Batch customer report |
| CBEXPORT | 1,305 | Data export utility |
| CBIMPORT | 1,232 | Data import utility |
| CBSTM03A | 1,520 | Statement generation (HTML) |
| CBSTM03B | 1,345 | Statement generation (text) |
| CBTRN01C | 1,198 | Batch transaction report |
| CBTRN02C | 1,499 | Batch transaction summary |
| CBTRN03C | 1,468 | Batch transaction detail |
| DBUNLDGS | 1,176 | Database unload utility |
| PAUDBLOD | 1,208 | DL/I database load |
| PAUDBUNL | 1,207 | DL/I database unload |

**Total: 66,489 lines of C++17 across 44 programs**

## Security Hardening

Every generated file includes a security manifest. Lazarus does not just transpile -- it hardens:

- **Buffer overflow protection** -- `FixedString<N>` replaces raw `char[]` with bounds-enforced fixed-length strings
- **Integer overflow protection** -- `SafeInt` wrapper with overflow detection
- **Decimal precision** -- `Decimal` class preserves COBOL COMP-3 packed decimal semantics
- **Bounds checking** -- `.at()` access instead of `[]` for all array/string operations
- **Input validation** -- `safe::` namespace functions for all external data
- **Exception handling** -- `try/catch` in main with COBOL-compatible return codes
- **Memory safety** -- RAII throughout, zero raw pointers, zero manual memory management
- **Type safety** -- COBOL-compatible type system (`FixedString`, `SafeInt`, `Decimal`)
- **CICS stubs** -- `lazarus::cics` namespace provides compile-compatible CICS operation stubs
- **Cross-size assignment** -- `FixedString<N> = FixedString<M>` works correctly for any N, M (COBOL MOVE semantics)

## Build

Every program compiles standalone with any C++17 compiler:

```bash
# Single program
g++ -std=c++17 -O2 -o COSGN00C COSGN00C_hardened.cpp

# All programs
for f in programs/*_hardened.cpp; do
    g++ -std=c++17 -O2 -o "${f%.cpp}" "$f"
done
```

No external dependencies. No build system. No runtime libraries. Each file is entirely self-contained.

## What Makes This Hard

CICS COBOL is the most challenging transpilation target in mainframe modernization:

1. **EXEC CICS** commands are not standard COBOL -- they are preprocessor directives that expand to runtime calls for terminal I/O, file operations, program control, and transaction management
2. **BMS maps** define 3270 terminal screen layouts that drive the entire UI flow
3. **Copybooks with REPLACE** create deeply nested data structures with `REDEFINES` that overlay memory regions
4. **EVALUATE TRUE / WHEN** control flow with fall-through semantics has no direct C++ equivalent
5. **Paragraph PERFORM THRU** creates non-local control flow that must be flattened to function calls
6. **COMP-3 packed decimal** arithmetic requires exact decimal semantics, not IEEE 754 floating point
7. **EBCDIC collating sequence** affects sort order, comparison, and string operations

Lazarus handles all of these. The 44 programs in this repository went from COBOL source to compiling C++17 with zero manual intervention.

## About Lazarus

Lazarus is a COBOL-to-C++17 transpiler developed by [Lazarus Systems](https://lazarus-systems.com). It converts legacy mainframe COBOL (including CICS, DB2, IMS/DL-I, and batch) into modern, hardened, self-contained C++17.

## Related Showcases

- [CMS Medicare Ironclad Showcase](https://github.com/mrm413/cms-medicare-ironclad-showcase) -- 55 CMS Medicare pricing programs transpiled to Rust (100%)
- [CMS Medicare Lazarus Showcase](https://github.com/mrm413/cms-medicare-lazarus-showcase) -- 55 CMS Medicare pricing programs transpiled to C++17 (100%)
- [Lazarus COBOL Showcase](https://github.com/mrm413/lazarus-cobol-showcase) -- 1,607 GnuCOBOL test programs transpiled to C++17 (100%)

## License

The original CardDemo application is provided by AWS under the [MIT-0 License](https://github.com/aws-samples/aws-mainframe-modernization-carddemo/blob/main/LICENSE).

The C++17 transpilation output in this repository is provided for demonstration purposes.
