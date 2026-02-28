# Heidelberg DESFire Inspector (Starter)

This project is a practical starter for your **Uni Heidelberg card workflow** migration from MIFARE Classic assumptions to **MIFARE DESFire EV1 8K** card metadata.

Instead of trying to fake low-level DESFire decryption in browser JavaScript, this version focuses on what you can reliably do now:

- ingest Flipper Zero dump text,
- parse ISO14443A/4 markers (UID, ATQA, SAK, ATS),
- estimate whether the card is likely DESFire,
- surface interpretation notes useful for the next app stage.

## Why this shape

The original UDECard repository is built around UDE's card setup and MIFARE Classic-oriented assumptions. DESFire EV1 cards use a different application/file model and crypto flow, so a meaningful Heidelberg build needs DESFire-aware parsing first.

This project gives you that parsing baseline in a clean UI.

## Run locally

Open `index.html` directly in your browser, or serve it locally:

```bash
python3 -m http.server 8000
```

Then open `http://localhost:8000`.

## How to test with a Flipper Zero + Uni Heidelberg student card

> Only test cards you own or are explicitly authorized to inspect. Do not attempt to bypass access control or clone cards.

1. **Read the card on Flipper Zero**
   - On Flipper: `NFC` → `Read`.
   - Hold your student card still until the read completes.
   - Save the captured card (for example `uhd_card.nfc`).

2. **Confirm you captured DESFire-relevant metadata**
   - Open the saved card on Flipper (`NFC` → `Saved`) and verify you can see fields such as `UID`, `ATQA`, `SAK`, and ideally `ATS`.
   - DESFire EV1 cards commonly show markers like `ATQA: 03 44` and `SAK: 20` (not guaranteed for every deployment, but common).

3. **Move the dump to your computer**
   - Use qFlipper, mobile app, or SD card access to copy the `.nfc` file.
   - Open the file in a text editor.

4. **Analyze with this tool**
   - Paste the dump text into this app’s textarea.
   - Click **Analyze card**.
   - Check the result panel:
     - UID length and values,
     - ATQA / SAK,
     - ATS consistency (length byte check),
     - confidence + notes for DESFire likelihood.

5. **Sanity-check repeatability**
   - Read the same card multiple times on Flipper.
   - Paste multiple captures and compare fields (UID, ATQA, SAK should be stable; ATS should usually be stable).

## What this tool can and cannot do

### What it does now

- Parses Flipper-style dump text.
- Produces a quick DESFire-likelihood summary for triage.
- Helps verify that your card capture process is stable.

### What it does **not** do yet

- No DESFire application/file traversal (`SelectApplication`, `GetFileIDs`, `ReadData`).
- No DESFire authentication/crypto.
- No native on-device Flipper UI workflow.

## Next steps for a real Flipper app

1. Build/extend a native Flipper Zero app in C with official Furi APIs.
2. Add DESFire command support (`GetVersion`, `SelectApplication`, `GetFileIDs`, `GetFileSettings`, `ReadData`).
3. Capture Heidelberg-specific traces and document AIDs/file IDs that are legally readable.
4. Implement Heidelberg-specific decoders for those files.
5. Keep this inspector as a desktop validation tool for dump sanity checks.
