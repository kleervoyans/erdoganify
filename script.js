const dumpInput = document.getElementById("dumpInput");
const analyzeButton = document.getElementById("analyzeButton");
const loadSampleButton = document.getElementById("loadSampleButton");
const clearButton = document.getElementById("clearButton");
const statusMessage = document.getElementById("statusMessage");
const resultContent = document.getElementById("resultContent");

const SAMPLE_DUMP = `Filetype: Flipper NFC device\nVersion: 4\nDevice type: ISO14443-4A\nUID: 04 91 8E 62 C8 37 81\nATQA: 03 44\nSAK: 20\nATS: 75 77 81 02 80\nMifare version: EV1\nStorage: 8K`;

const HEX_BYTE = /\b[0-9a-fA-F]{2}\b/g;

function normalizeHex(hexString) {
  const bytes = hexString.match(HEX_BYTE) ?? [];
  return bytes.map((b) => b.toUpperCase());
}

function bytesToHex(bytes) {
  return bytes.join(" ");
}

function parseNamedFields(text) {
  const fields = {};
  for (const line of text.split(/\r?\n/)) {
    const match = line.match(/^\s*([\w\- ]+?)\s*:\s*(.+)\s*$/);
    if (!match) continue;
    const key = match[1].trim().toLowerCase();
    fields[key] = match[2].trim();
  }
  return fields;
}

function parseAts(atsBytes) {
  if (!atsBytes.length) return null;
  const tl = parseInt(atsBytes[0], 16);

  return {
    lengthByte: tl,
    declaredLengthValid: tl === atsBytes.length,
    ta1: atsBytes[2] ?? null,
    tb1: atsBytes[3] ?? null,
    tc1: atsBytes[4] ?? null,
    historicalBytes: atsBytes.slice(5),
  };
}

function inferCardProfile(info) {
  const profile = {
    isLikelyDesfire: false,
    confidence: "low",
    notes: [],
  };

  const atqa = info.atqaHex;
  const sak = info.sakHex;

  if (atqa === "03 44") {
    profile.notes.push("ATQA 03 44 matches many DESFire family cards.");
    profile.isLikelyDesfire = true;
  }

  if (sak === "20") {
    profile.notes.push("SAK 20 indicates ISO14443-4 support, expected for DESFire.");
    profile.isLikelyDesfire = true;
  }

  if (info.ats?.declaredLengthValid) {
    profile.notes.push("ATS length byte is internally consistent.");
  } else if (info.ats) {
    profile.notes.push("ATS length mismatch; dump may be truncated.");
  }

  if (profile.isLikelyDesfire && info.storageHint.includes("8")) {
    profile.notes.push("Storage hint contains 8K, consistent with EV1 8K cards.");
    profile.confidence = "high";
  } else if (profile.isLikelyDesfire) {
    profile.confidence = "medium";
  }

  if (!profile.isLikelyDesfire) {
    profile.notes.push("Card markers do not clearly identify DESFire.");
  }

  return profile;
}

function analyzeDump(inputText) {
  const fields = parseNamedFields(inputText);
  const allBytes = normalizeHex(inputText);

  const uidBytes = normalizeHex(fields.uid ?? "");
  const atqaBytes = normalizeHex(fields.atqa ?? "");
  const sakBytes = normalizeHex(fields.sak ?? "");
  const atsBytes = normalizeHex(fields.ats ?? "");

  const info = {
    uidHex: bytesToHex(uidBytes),
    uidLength: uidBytes.length,
    atqaHex: bytesToHex(atqaBytes),
    sakHex: bytesToHex(sakBytes),
    atsRaw: bytesToHex(atsBytes),
    ats: parseAts(atsBytes),
    totalBytes: allBytes.length,
    storageHint: fields.storage ?? fields["memory size"] ?? "",
    mifareVersion: fields["mifare version"] ?? "",
    deviceType: fields["device type"] ?? "",
  };

  const profile = inferCardProfile(info);

  return { info, profile };
}

function renderResult(result) {
  const { info, profile } = result;

  const atsSummary = info.ats
    ? [
        `Length byte: ${info.ats.lengthByte} (${info.ats.declaredLengthValid ? "valid" : "invalid"})`,
        info.ats.ta1 ? `TA1: ${info.ats.ta1}` : null,
        info.ats.tb1 ? `TB1: ${info.ats.tb1}` : null,
        info.ats.tc1 ? `TC1: ${info.ats.tc1}` : null,
      ]
        .filter(Boolean)
        .join(" • ")
    : "No ATS found";

  resultContent.innerHTML = `
    <div class="result-grid">
      <article class="kv"><span class="k">UID</span><div class="v">${info.uidHex || "n/a"}</div></article>
      <article class="kv"><span class="k">UID length</span><div class="v">${info.uidLength || "n/a"} bytes</div></article>
      <article class="kv"><span class="k">ATQA</span><div class="v">${info.atqaHex || "n/a"}</div></article>
      <article class="kv"><span class="k">SAK</span><div class="v">${info.sakHex || "n/a"}</div></article>
      <article class="kv"><span class="k">ATS</span><div class="v">${info.atsRaw || "n/a"}</div></article>
      <article class="kv"><span class="k">Device type</span><div class="v">${info.deviceType || "n/a"}</div></article>
      <article class="kv"><span class="k">MIFARE version hint</span><div class="v">${info.mifareVersion || "n/a"}</div></article>
      <article class="kv"><span class="k">Storage hint</span><div class="v">${info.storageHint || "n/a"}</div></article>
      <article class="kv"><span class="k">Raw byte count</span><div class="v">${info.totalBytes}</div></article>
      <article class="kv"><span class="k">DESFire confidence</span><div class="v">${profile.confidence.toUpperCase()}</div></article>
    </div>

    <article class="kv">
      <span class="k">ATS interpretation</span>
      <div class="v">${atsSummary}</div>
    </article>

    <article class="kv">
      <span class="k">Analysis notes</span>
      <ul>${profile.notes.map((note) => `<li>${note}</li>`).join("")}</ul>
    </article>
  `;
}

function runAnalysis() {
  const inputText = dumpInput.value.trim();
  if (!inputText) {
    statusMessage.textContent = "Input is empty. Paste a dump first.";
    resultContent.innerHTML = "";
    return;
  }

  const result = analyzeDump(inputText);
  renderResult(result);

  if (result.profile.isLikelyDesfire) {
    statusMessage.textContent = `Likely DESFire card (${result.profile.confidence} confidence).`;
  } else {
    statusMessage.textContent = "Could not confidently classify as DESFire.";
  }
}

analyzeButton.addEventListener("click", runAnalysis);
loadSampleButton.addEventListener("click", () => {
  dumpInput.value = SAMPLE_DUMP;
  runAnalysis();
});
clearButton.addEventListener("click", () => {
  dumpInput.value = "";
  resultContent.innerHTML = "";
  statusMessage.textContent = "Paste a dump and run analysis.";
});
