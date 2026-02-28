# UniHD Card (Flipper Zero app)

This is a **native Flipper Zero app** for Uni Heidelberg student cards based on the UDECard concept, but adapted for **MIFARE DESFire** workflows.

## What it does

- Reads a DESFire card directly on-device (`Read card`).
- Loads previously saved `.nfc` dumps (`Load dump`).
- Shows student-useful technical data quickly:
  - UID
  - ATQA / SAK
  - ATS TL/T0
  - DESFire HW/SW version bytes
  - Number of DESFire applications
  - Free memory field (if present)

## Build

Use `ufbt` in this folder:

```bash
ufbt
```

Install with:

```bash
ufbt launch
```

## Notes

- This app is read-only.
- It depends on Flipper firmware NFC support for `NfcProtocolMfDesfire`.
- Heidelberg-specific semantic decoding (e.g., student number from app/file payload) requires known AIDs/file layouts and authorization.
