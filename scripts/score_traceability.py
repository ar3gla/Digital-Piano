#!/usr/bin/env python3
import io
import csv
import sys
from pathlib import Path

REQUIRED_COLS = [
    "Req ID","Type","Requirement","Spec reference",
    "Implementation (file:function)","Verification (TC IDs)","Evidence path(s)","Status"
]

EXPECTED_REQ_IDS = [
    "RQ-01","RQ-02","RQ-03","RQ-04","RQ-05","RQ-06","RQ-07",
    "RQ-08","RQ-09","RQ-10","RQ-11","RQ-12","RQ-13",
    "CN-01","CN-02","CN-03"
]

# Scoring (tweak to taste)
PTS_HAVE_ALL_ROWS = 25
PTS_REQUIRED_FIELDS = 50
PTS_EVIDENCE_EXISTS = 25

# ---- Grading mode toggles ----
GRADE_EVIDENCE = False   # Step 1: False. Later steps: True.
GRADE_IMPLEMENTATION = False  # Step 1: False. Later steps: True.
# -----------------------------------------------

def open_csv_with_fallback(path):
    """
    Open a CSV file using multiple possible encodings.
    Tries UTF-8 (with/without BOM) first, then Windows encodings.
    Returns a csv.DictReader.
    """
    encodings = ["utf-8-sig", "utf-8", "cp1252", "latin-1"]

    with open(path, "rb") as fh:
        raw = fh.read()

    for enc in encodings:
        try:
            text = raw.decode(enc)
            if enc not in ("utf-8", "utf-8-sig"):
                print(f"WARNING: CSV decoded using fallback encoding '{enc}'. Please save as UTF-8.")
            return csv.DictReader(io.StringIO(text))
        except UnicodeDecodeError:
            continue

    raise UnicodeDecodeError(
        "Unable to decode CSV using encodings: " + ", ".join(encodings),
        raw, 0, 1, "invalid start byte"
    )



def die(msg, code=2):
    print(f"ERROR: {msg}")
    sys.exit(code)

def main():

    if len(sys.argv) != 2:
        print("Usage: python3 scripts/check_traceability.py docs/traceability.csv")
        sys.exit(2)

    csv_path = Path(sys.argv[1])
    if not csv_path.exists():
        die(f"Missing file: {csv_path}")

    reader = open_csv_with_fallback(csv_path)
    
    if reader.fieldnames is None:
        die("CSV has no header row.")

    missing_cols = [c for c in REQUIRED_COLS if c not in reader.fieldnames]
    if missing_cols:
        die("Missing required columns: " + ", ".join(missing_cols))

    rows = list(reader)

    # Build index
    by_id = {r["Req ID"].strip(): r for r in rows if r.get("Req ID")}
    missing_ids = [rid for rid in EXPECTED_REQ_IDS if rid not in by_id]

    score = 0
    details = []

    present_count = len(EXPECTED_REQ_IDS) - len(missing_ids)
    total_count = len(EXPECTED_REQ_IDS)

    # 1) Required IDs
    if not missing_ids:
        score += PTS_HAVE_ALL_ROWS
        details.append(f"+{PTS_HAVE_ALL_ROWS} All required Req IDs present. 16/16")
    else:
        frac = present_count / total_count
        pts = round(PTS_HAVE_ALL_ROWS * frac)
        score += pts
        details.append(f"+{pts} Required Req IDs present: {present_count}/{total_count}")
        details.append(f"+0 Missing Req IDs: {', '.join(missing_ids)}")

    # 2) Required fields filled
    filled = 0
    total = 0
    
    #NEW: Only check expected IDs, in case extra rows present
    cols_to_check = REQUIRED_COLS[2:]  # skip Req ID, Type
    if not GRADE_EVIDENCE:
        cols_to_check = [c for c in cols_to_check if c != "Evidence path(s)"]
    if not GRADE_IMPLEMENTATION:
        cols_to_check = [c for c in cols_to_check if c != "Implementation (file:function)"]

    for rid in EXPECTED_REQ_IDS:
        r = by_id.get(rid, {})
        # If row missing, count as not filled
        for c in cols_to_check:  # skip Req ID, Type
            total += 1
            val = (r.get(c) or "").strip()
            if val:
                filled += 1
    if total > 0:
        frac = filled / total
        pts = round(PTS_REQUIRED_FIELDS * frac)
        score += pts
        details.append(f"+{pts} Required fields filled: {filled}/{total} ({frac:.0%}).")

    # 3) Evidence paths exist (only check non-empty evidence entries)
    repo_root = Path(".").resolve()
    ev_ok = 0 # Evidence files found
    ev_total = 0 # Evidence files checked

    for rid in EXPECTED_REQ_IDS:
        r = by_id.get(rid, {})
        ev = (r.get("Evidence path(s)") or "").strip()
        if not ev:
            continue
        # allow multiple paths separated by ; or ,
        parts = [p.strip() for sep in [";"] for p in ev.split(sep)]
        parts2 = []
        for p in parts:
            if "," in p:
                parts2.extend([x.strip() for x in p.split(",") if x.strip()])
            elif p:
                parts2.append(p)
        paths = [p for p in parts2 if p]
        for p in paths:
            ev_total += 1
            # allow URLs (skip existence check)
            if p.startswith("http://") or p.startswith("https://"):
                ev_ok += 1
                continue
            if (repo_root / p).exists():
                ev_ok += 1
    # NEW: Grading mode toggle
    raw_score = score
    if not GRADE_EVIDENCE:
        raw_max = PTS_HAVE_ALL_ROWS + PTS_REQUIRED_FIELDS
        score = round(score * 100 / (raw_max))
        pts = 0
        details.append("+0 Evidence path existence not graded in this step.")
    else:
        raw_max = PTS_HAVE_ALL_ROWS + PTS_REQUIRED_FIELDS + PTS_EVIDENCE_EXISTS
        if ev_total == 0:
            pts = 0
            details.append("+0 No evidence paths provided yet (skipping existence check).")
        else:
            frac = ev_ok / ev_total
            pts = round(PTS_EVIDENCE_EXISTS * frac)
            score += pts
            details.append(f"+{pts} Evidence files present: {ev_ok}/{ev_total} ({frac:.0%}).")

    # Clamp
    score = max(0, min(100, score))

    print("TRACEABILITY AUTOGRADE")
    print("----------------------")
    for d in details:
        print(d)
    print("----------------------")
    
    if not GRADE_EVIDENCE:
        print(f"RAW SCORE (for reference): {raw_score}/{raw_max}")
        print(f"SCALED SCORE: {score}/100")
    else:
        print(f"SCORE: {score}/100")
    
    # Exit code: fail (<30) so students get a red X if very incomplete
    sys.exit(0 if score >= 30 else 1)

if __name__ == "__main__":
    main()
