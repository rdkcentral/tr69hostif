#!/usr/bin/env python3
"""
Generate Automatics Test Gap Analysis Report.

Parses the generated Markdown test case file and BDD feature files from
docs/features/, classifies each scenario as L2-testable (device-level) or
L1-only (internal/unit-level), maps Automatics test cases to the L2 subset,
and produces a README.md gap analysis report.

Usage:
    python3 generate_test_gap_report.py
    python3 generate_test_gap_report.py --md Filtered_Script_Details.md --features ../../../docs/features
"""

import argparse
import os
import re
import sys
from collections import defaultdict
from datetime import datetime


# ---------------------------------------------------------------------------
# L1-only indicators: scenarios matching these patterns are internal/unit-
# level and NOT practical for Automatics end-to-end device testing.
# ---------------------------------------------------------------------------
L1_ONLY_PATTERNS = [
    # ---- Data structures & memory management ----
    r"\bstruct(ure)?\b",
    r"\bfree\b.*\bstructure\b",
    r"\bcreate.*vector\b",
    r"\badd.*element.*vector\b",
    r"\bget.*element.*vector\b",
    r"\bremove.*element.*vector\b",
    r"\bvector.*auto.?resize\b",
    r"\bcreate.*hash.*map\b",
    r"\binsert.*hash.*map\b",
    r"\bretrieve.*hash.*map\b",
    r"\bremove.*hash.*map\b",
    r"\bdestroy.*hash.*map\b",

    # ---- Thread internals ----
    r"\bthread.*safe(ty)?\b",
    r"\block.*hierarchy\b",
    r"\bthread.*synchronization\b",

    # ---- Internal API init/uninit plumbing ----
    # Specific inits that are pure plumbing (not device-observable)
    r"\binitiali[sz]e\b.*\b(?:dca|t2\s*marker|http\s*connection|event\s*receiver|logging|whoami)\b",
    r"\buninitiali[sz]e\b",
    r"\bmodule\b.*\buninitiali\b",
    r"\bdestroy\b.*\b(?:hash|map|json|component|report)\b",
    r"\bclear\b.*\b(?:component\s*map|t2\s*marker|seek\s*map)\b",

    # ---- Internal register/unregister (API plumbing) ----
    r"\bregister\b.*\b(?:listener|consumer|dcm)\b",
    r"\bunregister\b",

    # ---- Internal get/set/check low-level API calls ----
    r"\bget\b.*\b(?:profile\s*hash|component\s*marker\s*list|marker\s*component\s*rbus)\b",
    r"\bget\b.*\bprofile\s*count\b",
    r"\bcheck\b.*\b(?:rbus\s*init|first\s*boot)\b",
    r"\bset\b.*\bt2\b.*\bevent\b.*\breceive\b.*\bstate\b",

    # ---- Internal store/save/load/populate plumbing ----
    r"\bstore\b.*\bmarker\b.*\bevent\b",
    r"\bsave\b.*\b(?:grep\s*config|seek\s*config|privacy\s*mode)\b",
    # NOTE: "Save MessagePack configuration" (writing config blob to disk) is L2 —
    # Automatics can verify config is persisted. Only the low-level grep/seek
    # config saves are pure L1 plumbing.
    r"\bremove\b.*\b(?:grep\s*config|profile\s*from\s*disk)\b",
    r"\bload\b.*\bsaved\b.*\bseek\b",
    r"\bpopulate\b.*\bcached\b.*\breport\b",
    r"\bcalculate\b.*\b(?:profile\s*)?memory\b",
    r"\bflag\b.*\bdca\b.*\breport\b",

    # ---- Internal encoding/prepare/free plumbing ----
    r"\bencode\b.*\bstatic\b.*\bjson\b",
    r"\bprepare\b.*\bjson\b.*\breport\b",
    r"\bprepare\b.*\bhttp\b.*\burl\b",
    r"\bfree\b.*\bprofile\b.*\bvalue\b",
    r"\btag\b.*\breport\b.*\bcached\b",

    # ---- Scheduler internals ----
    r"\bscheduler\b.*\b(?:initialization|uninitialization)\b",
    r"\bunregister\b.*\bprofile\b.*\bscheduler\b",
    r"\bscheduler\b.*\bhandles?\b.*\b(?:repeat\b|non.?repeat|delete\s*on\s*timeout)\b",
    r"\bsend\b.*\binterrupt\b.*\btimeout\b",
    r"\bget\b.*\blapsed\b.*\btime\b",
    r"\bretain\b.*\bseek\b.*\bmap\b.*\bflag\b",

    # ---- Marker internals (internal map management) ----
    r"\binitiali[sz]e\b.*\bt2\b.*\bmarker\b",
    r"\bupdate\b.*\bevent\b.*\bmap\b.*\bmarker\b",
    r"\badd\b.*\bt2\b.*\bevent\b.*\bmarker\b",
    r"\bcreate\b.*\bcomponent\b.*\bdata\b.*\belement\b",
    r"\bmaximum\b.*\bevent\b.*\bmarker\b.*\bname\b.*\blength\b",
    r"\bmarker\b.*\bregistration\b.*\bskip\b.*\bfrequency\b",

    # ---- Report profiles internal callbacks ----
    r"\bprofile\b.*\b(?:activation|reporting)\b.*\btimeout\b.*\bcallback\b",
    r"\binterrupt\b.*\breport\b.*\bprofiles?\b.*\bimmediate\b",
    r"\bgenerate\b.*\bdca\b.*\breport\b",
    # NOTE: "Process JSON/MessagePack report profiles blob" is L2 (webconfig config delivery
    # path — Automatics can push a blob and verify profiles are applied on device).
    # Only internal function-named scenarios are L1:
    r"\bReportProfiles_ProcessReportProfiles(?:Blob|MsgPackBlob)\b.*\bcalled\b",
    r"\breport\b.*\bprofiles?\b.*\bmodule\b.*\b(?:init|uninit)\b",

    # ---- DCA low-level internals ----
    r"\binitiali[sz]e\b.*\bdca\b.*\bproperties\b",
    r"\bget\b.*\bdca\b.*\bresults?\b.*\bvector\b",
    r"\bget\b.*\bgrep\b.*\bresults?\b.*\bseek\b",
    r"\bsave\b.*\bgrep\b.*\bconfig\b",
    r"\bremove\b.*\bgrep\b.*\bconfig\b",
    r"\bget\b.*\bprocess\b.*\b(?:cpu|memory|pid)\b",
    r"\bget\b.*\btotal\b.*\bcpu\b",
    r"\bmemory.?mapped\b.*\bfile\b",
    r"\bbounds\b.*\bcheck\b",
    r"\bGrepResult\b",

    # ---- Low-level utility functions ----
    r"\bstring\b.*\bduplication\b",
    r"\bsafe\b.*\bstring\b.*\bcopy\b",
    r"\blog\b.*\b(?:debug|info|warning|error)\b.*\bmessage\b",
    r"\binitiali[sz]e\b.*\blogging\b",
    r"\bconditional\b.*\blogging\b",
    r"\brdk\b.*\blogger\b",
    r"\bget\b.*\bcurrent\b.*\btimestamp\b",
    r"\bwhoa?mi\b.*\bsupport\b",

    # ---- Low-level parser internals ----
    r"\bget\b.*\bmap\b.*\bvalue\b.*\bmessagepack\b",
    r"\bget\b.*\barray\b.*\belement\b.*\bmessagepack\b",
    r"\bduplicate\b.*\bstring\b.*\bmessagepack\b",
    r"\bprint\b.*\bmessagepack\b.*\bdebug\b",
    r"\bcompare\b.*\bmessagepack\b.*\bstring\b",

    # ---- Internal callback/structure definitions ----
    r"\bcallback\b.*\btypedef\b",
    r"\bcurlResponseData\b",
    r"\bconfig\b.*\bstructure\b",
    r"\bscheduler\b.*\bprofile\b.*\bstructure\b",
    r"\bprocess\b.*\b(?:memory\b.*\bcpu|cpu\b.*\bmemory)\b.*\binfo\b.*\bstructure\b",
    r"\bT2Event\b.*\bstructure\b",
    r"\bT2HTTP\b.*\bdestination\b.*\bstructure\b",
    r"\bT2RBUS\b.*\bdestination\b.*\bstructure\b",
    r"\bHTTP\b.*\brequest\b.*\bparameter\b.*\bstructure\b",
    r"\bRBUS\b.*\bmethod\b.*\bparameter\b.*\bstructure\b",

    # ---- Connection pool internals ----
    r"\binitiali[sz]e\b.*\b(?:http\b.*)?connection\b.*\bpool\b",
    r"\bconnection\b.*\bpool\b.*\bcleanup\b",
    r"\bwrite\b.*\bresponse\b.*\bto\b.*\bfile\b",

    # ---- Event receiver internals ----
    r"\bevent\b.*\breceiver\b.*\b(?:initialization|uninitialization)\b",
    r"\bstart\b.*\bevent\b.*\bdispatch\b.*\bthread\b",
    r"\bstop\b.*\bevent\b.*\bdispatch\b.*\bthread\b",
    r"\bevent\b.*\bdispatch\b.*\bthread\b.*\bprocessing\b",
    r"\bfree\b.*\bT2Event\b",

    # ---- RBUS internals (pure plumbing, not device-facing) ----
    r"\brbus\b.*\blog\b.*\bhandler\b",
    r"\bpublish\b.*\breport\b.*\bupload\b.*\bstatus\b",

    # ---- Profile internal plumbing ----
    r"\bnotify\b.*\bprofile\b.*\btimeout\b",
    r"\bnotify\b.*\bscheduler\b.*\bstart\b",
    r"\bsend\b.*\blog\b.*\bupload\b.*\binterrupt\b.*\bscheduler\b",
    r"\bappend\b.*\btrigger\b.*\bcondition\b.*\bprofile\b",
    r"\breport\b.*\bgeneration\b.*\bcomplete\b.*\bnotification\b",

    # ---- Persistence internals (paths are L1, operations may be L2) ----
    r"\bseek\b.*\bmap\b.*\bpersistence\b.*\bpath\b",
    r"\bcached\b.*\bmessages?\b.*\bpath\b",

    # ---- Format/encoding type enums ----
    r"\bencoding\b.*\btypes?\b.*\bsupported\b",
    r"\bjson\b.*\breport\b.*\bformat\b.*\boptions\b",
    r"\btimestamp\b.*\bformat\b.*\boptions\b",
    r"\bHTTP\b.*\bmethod\b.*\btypes?\b.*\bsupported\b",

    # ---- XConf client plumbing ----
    r"\bxconf\b.*\bclient\b.*\binitiali[sz]ation\b",
    r"\bxconf\b.*\bclient\b.*\bdetermines?\b.*\bbuild\b.*\btype\b",

    # ---- t2_parser: only the internal msgpack utility helpers are L1 ----
    # High-level scenarios (Parse JSON/MsgPack config, parse profile fields,
    # handle invalid config) remain L2 — they test the config delivery path.
    r"\bmsgpack_get_map_value\b",
    r"\bmsgpack_get_array_element\b",
    r"\bmsgpack_strdup\b",
    r"\bmsgpack_print\b",
    r"\bmsgpack_strcmp\b",
    r"\bPrint\s+MessagePack\s+object\s+for\s+debugging\b",
    r"\bDuplicate\s+string\s+from\s+MessagePack\b",
    r"\bCompare\s+MessagePack\s+string\b",
    r"\bGet\s+(?:map\s+value|array\s+element)\s+from\s+MessagePack\b",
]
L1_COMPILED = [re.compile(p, re.IGNORECASE) for p in L1_ONLY_PATTERNS]

# Feature files that are entirely L1 (internal utility / parser internals)
# NOTE: t2_parser is NOT fully L1 — its config parsing scenarios (JSON/MsgPack profile
# delivery, field extraction, error handling) are L2-testable via webconfig/xconf path.
L1_ONLY_FEATURES = {"utils"}


def classify_scenario(feature_basename, scenario_name):
    """Return 'L1' if scenario is internal/unit-level, else 'L2'."""
    if feature_basename in L1_ONLY_FEATURES:
        return "L1"
    for pat in L1_COMPILED:
        if pat.search(scenario_name):
            return "L1"
    return "L2"


# ---------------------------------------------------------------------------
# Keyword mapping: test case name patterns -> feature file basenames
# ---------------------------------------------------------------------------
TC_FEATURE_MAP = {
    "MULTIPROFILE": [
        "profile_management",
        "report_profiles",
        "scheduler",
        "report_generation",
        "t2_parser",
    ],
    "SEEKMAP": ["dca_log_processing", "persistence"],
    "PREV_LOG": ["dca_log_processing", "persistence"],
    "REPORT_PROFILE": [
        "report_profiles",
        "report_generation",
        "t2_markers",
        "rbus_interface",
        "t2_parser",
    ],
    "LOG_UPLOAD": ["telemetry_daemon", "report_profiles"],
    "ON_DEMAND": ["telemetry_daemon", "report_profiles"],
    "BOOTIME": ["t2_markers", "dca_log_processing", "telemetry_daemon"],
    "BOOTTIME": ["t2_markers", "dca_log_processing", "telemetry_daemon"],
    "MARKER": ["t2_markers", "dca_log_processing"],
    "XCONF": ["xconf_client"],
    "RETRY": ["xconf_client"],
    "EVENT": ["event_receiver", "rbus_interface", "t2_markers"],
    "DATAMODEL": ["event_receiver", "rbus_interface", "t2_markers"],
    "GREP": ["dca_log_processing"],
    "ROOTNAME": ["report_generation", "protocol_http"],
    "URL_ENCODING": ["xconf_client", "protocol_http"],
    "ENCODING": ["xconf_client", "protocol_http"],
    "RA": [
        "report_profiles",
        "scheduler",
        "report_generation",
        "profile_management",
    ],
}

# Semantic keyword groups found in test step descriptions -> feature basenames
STEP_KEYWORD_MAP = {
    "seekmap": ["dca_log_processing", "persistence"],
    "seek map": ["dca_log_processing", "persistence"],
    "previous_log": ["dca_log_processing", "persistence"],
    "previouslogs": ["dca_log_processing", "persistence"],
    "previous log": ["dca_log_processing", "persistence"],
    "cjson report": ["report_generation", "report_profiles"],
    "report sent successfully": ["protocol_http", "report_generation"],
    "report sent": ["protocol_http", "report_generation"],
    "http": ["protocol_http"],
    "curl": ["protocol_http"],
    "mtls": ["protocol_http"],
    "rbus": ["rbus_interface", "protocol_rbus"],
    "rbus_method": ["protocol_rbus", "rbus_interface"],
    "rbusmethod": ["protocol_rbus", "rbus_interface"],
    "signal 12": ["telemetry_daemon"],
    "signal 10": ["telemetry_daemon"],
    "signal 29": ["telemetry_daemon"],
    "exec_reload": ["telemetry_daemon", "xconf_client"],
    "sigterm": ["telemetry_daemon"],
    "xconf": ["xconf_client"],
    "configurl": ["xconf_client"],
    "config url": ["xconf_client"],
    "fetchremoteconfiguration": ["xconf_client"],
    "reportinginterval": ["scheduler", "report_profiles"],
    "reporting interval": ["scheduler", "report_profiles"],
    "activationtimeout": ["scheduler", "report_profiles", "profile_management"],
    "activation timeout": ["scheduler", "report_profiles", "profile_management"],
    "generatenow": ["report_profiles", "scheduler"],
    "generate now": ["report_profiles", "scheduler"],
    "reportonaupdate": ["scheduler", "report_profiles"],
    "reportonupdate": ["scheduler", "report_profiles"],
    "firstreportinginterval": ["scheduler", "report_profiles"],
    "first reporting interval": ["scheduler", "report_profiles"],
    "maxuploadlatency": ["scheduler", "report_profiles"],
    "triggercondi": ["profile_management", "rbus_interface"],
    "trigger condition": ["profile_management", "rbus_interface"],
    "multiprofile": ["profile_management", "report_profiles", "scheduler"],
    "msgpack": ["t2_parser", "report_profiles", "persistence"],
    "messagepack": ["t2_parser", "report_profiles", "persistence"],
    "webconfig": ["t2_parser", "report_profiles"],
    "parse profile": ["t2_parser"],
    "parse.*config": ["t2_parser", "xconf_client"],
    "event marker": ["t2_markers", "event_receiver"],
    "grep marker": ["dca_log_processing"],
    "datamodel marker": ["rbus_interface", "t2_markers"],
    "data model marker": ["rbus_interface", "t2_markers"],
    "count": ["dca_log_processing", "t2_markers"],
    "absolute": ["dca_log_processing"],
    "accumulate": ["t2_markers", "event_receiver"],
    "accumulation": ["t2_markers", "event_receiver"],
    "subscribe": ["rbus_interface"],
    "persistence": ["persistence"],
    "cached": ["persistence", "report_profiles"],
    "cached report": ["persistence", "report_profiles"],
    "cachedmessages": ["persistence", "report_profiles"],
    "nvram": ["persistence"],
    "telemetry2_0": ["telemetry_daemon"],
    "telemetry process": ["telemetry_daemon"],
    "daemon": ["telemetry_daemon"],
    "non-root": ["telemetry_daemon"],
    "non root": ["telemetry_daemon"],
    "log rotation": ["dca_log_processing"],
    "rotated log": ["dca_log_processing"],
    "reboot": ["telemetry_daemon"],
    "bootup": ["telemetry_daemon", "t2_markers"],
    "boot up": ["telemetry_daemon", "t2_markers"],
    "elastic": ["report_generation", "protocol_http"],
    "profile name": ["report_generation", "profile_management"],
    "hash": ["profile_management", "report_profiles"],
    "encoding": ["report_generation", "xconf_client"],
    "url": ["xconf_client", "protocol_http"],
    "process crash": ["telemetry_daemon"],
    "split": ["dca_log_processing", "t2_markers"],
}


# ---------------------------------------------------------------------------
# Parse markdown test cases
# ---------------------------------------------------------------------------
def parse_test_cases(md_path):
    """Parse Filtered_Script_Details.md and return list of test case dicts."""
    with open(md_path, "r", encoding="utf-8") as f:
        content = f.read()

    test_cases = []
    # Split by ### headings
    tc_blocks = re.split(r"^### ", content, flags=re.MULTILINE)

    for block in tc_blocks:
        block = block.strip()
        if not block:
            continue

        # First line is the heading: "N. TC_NAME"
        lines = block.split("\n", 1)
        heading = lines[0].strip()

        # Extract test case name — strip markdown escapes
        match = re.match(r"(\d+)\.\s+(.*)", heading)
        if not match:
            continue

        sno = int(match.group(1))
        tc_name = match.group(2).replace("\\_", "_").strip()

        # Parse table rows for descriptions and expected outputs
        steps = []
        if len(lines) > 1:
            table_content = lines[1]
            table_rows = re.findall(r"^\|(.+)\|$", table_content, re.MULTILINE)
            for row in table_rows:
                cells = [c.strip() for c in row.split("|")]
                # Skip header and separator rows
                if not cells or all(c.startswith("---") for c in cells if c):
                    continue
                if len(cells) >= 7:
                    # cells: S.No, MANUAL ID, STEP NUMBER, WEIGHTAGE, DESCRIPTION, ACTION DETAILS, EXPECTED OUTPUT, ...
                    desc = cells[4] if len(cells) > 4 else ""
                    expected = cells[6] if len(cells) > 6 else ""
                    if desc and desc not in ("DESCRIPTION", "---"):
                        steps.append(
                            {"description": desc, "expected_output": expected}
                        )

        test_cases.append(
            {"sno": sno, "name": tc_name, "steps": steps}
        )

    return test_cases


# ---------------------------------------------------------------------------
# Parse feature files
# ---------------------------------------------------------------------------
def parse_feature_files(features_dir):
    """Parse all .feature files and return dict of feature_basename -> info.

    Each scenario is tagged with 'level' = 'L1' or 'L2'.
    """
    features = {}

    for fname in sorted(os.listdir(features_dir)):
        if not fname.endswith(".feature"):
            continue

        fpath = os.path.join(features_dir, fname)
        basename = fname.replace(".feature", "")

        with open(fpath, "r", encoding="utf-8") as f:
            content = f.read()

        # Extract Feature name
        feat_match = re.search(r"^Feature:\s*(.+)$", content, re.MULTILINE)
        feature_name = feat_match.group(1).strip() if feat_match else basename

        # Extract scenarios
        scenarios = []
        for m in re.finditer(
            r"^\s*Scenario:\s*(.+?)$", content, re.MULTILINE
        ):
            scenario_name = m.group(1).strip()

            # Grab the full scenario body until next Scenario or end
            start = m.end()
            next_match = re.search(
                r"^\s*Scenario:", content[start:], re.MULTILINE
            )
            if next_match:
                body = content[start : start + next_match.start()]
            else:
                body = content[start:]

            # Extract Given/When/Then lines
            gwt_lines = re.findall(
                r"^\s*(Given|When|Then|And)\s+(.+)$", body, re.MULTILINE
            )
            description = " ".join(line[1] for line in gwt_lines)

            level = classify_scenario(basename, scenario_name)

            scenarios.append(
                {
                    "name": scenario_name,
                    "description": description,
                    "covered_by": [],
                    "level": level,
                }
            )

        features[basename] = {
            "name": feature_name,
            "file": fname,
            "scenarios": scenarios,
        }

    return features


# ---------------------------------------------------------------------------
# Text similarity (simple token overlap)
# ---------------------------------------------------------------------------
def _tokenize(text):
    """Lowercase and split into word tokens."""
    return set(re.findall(r"[a-z0-9_]+", text.lower()))


def _similarity(text_a, text_b):
    """Jaccard-like similarity between two text strings."""
    tokens_a = _tokenize(text_a)
    tokens_b = _tokenize(text_b)
    if not tokens_a or not tokens_b:
        return 0.0
    intersection = tokens_a & tokens_b
    union = tokens_a | tokens_b
    return len(intersection) / len(union)


# ---------------------------------------------------------------------------
# Explicit scenario coverage rules for config-delivery test cases.
# Key: substring that must appear in TC name (upper). Value: list of
# (feature_basename, scenario_name_substring) tuples to mark as covered.
# ---------------------------------------------------------------------------
EXPLICIT_COVERAGE = {
    # Any TC that configures JSON + Msgpack profiles end-to-end covers the
    # parser scenarios for both formats and all field types.
    "REPORT_PROFILE": [
        ("t2_parser", "Parse JSON configuration"),
        ("t2_parser", "Parse MessagePack configuration"),
        ("t2_parser", "Parse profile with required fields"),
        ("t2_parser", "Parse profile with optional fields"),
        ("t2_parser", "Parse profile with event markers"),
        ("t2_parser", "Parse profile with grep markers"),
        ("t2_parser", "Parse profile with datamodel markers"),
        ("t2_parser", "Parse profile with HTTP destination"),
        ("t2_parser", "Handle invalid configuration"),
        ("t2_parser", "Handle missing required fields"),
        ("report_profiles", "Process JSON report profiles blob"),
        ("report_profiles", "Process MessagePack report profiles blob"),
    ],
    "MULTIPROFILE": [
        ("t2_parser", "Parse JSON configuration"),
        ("t2_parser", "Parse MessagePack configuration"),
        ("t2_parser", "Parse profile with required fields"),
        ("t2_parser", "Parse profile with optional fields"),
        ("t2_parser", "Parse profile with event markers"),
        ("t2_parser", "Parse profile with grep markers"),
        ("report_profiles", "Process JSON report profiles blob"),
        ("report_profiles", "Process MessagePack report profiles blob"),
        ("persistence", "Save MessagePack configuration"),
    ],
    "XCONF": [
        ("t2_parser", "Parse XConf-specific configuration"),
        ("t2_parser", "Parse JSON configuration"),
        ("t2_parser", "Parse profile with required fields"),
        ("t2_parser", "Parse profile with optional fields"),
    ],
}


# ---------------------------------------------------------------------------
# Map test cases to feature scenarios
# ---------------------------------------------------------------------------
def map_test_cases_to_features(test_cases, features):
    """Map each test case to relevant feature scenarios."""

    for tc in test_cases:
        tc_upper = tc["name"].upper()

        # Step 1: keyword mapping from test case name
        primary_features = set()
        for keyword, feat_list in TC_FEATURE_MAP.items():
            if keyword in tc_upper:
                primary_features.update(feat_list)

        # Step 2: keyword mapping from step descriptions
        all_step_text = " ".join(
            s["description"] + " " + s["expected_output"] for s in tc["steps"]
        ).lower()

        for keyword, feat_list in STEP_KEYWORD_MAP.items():
            if keyword in all_step_text:
                primary_features.update(feat_list)

        # Step 3: for each mapped feature, find best-matching scenarios
        for feat_basename in primary_features:
            if feat_basename not in features:
                continue

            feat = features[feat_basename]
            for scenario in feat["scenarios"]:
                # Check text similarity between test steps and scenario
                scenario_text = scenario["name"] + " " + scenario["description"]

                best_sim = 0.0
                for step in tc["steps"]:
                    step_text = step["description"] + " " + step["expected_output"]
                    sim = _similarity(step_text, scenario_text)
                    best_sim = max(best_sim, sim)

                # Also check scenario name keywords against step descriptions
                # Use min length 3 to catch short tokens like "json", "xml"
                scenario_name_lower = scenario["name"].lower()
                name_in_steps = any(
                    word in all_step_text
                    for word in _tokenize(scenario_name_lower)
                    if len(word) > 3
                )

                if best_sim >= 0.15 or name_in_steps:
                    if tc["name"] not in scenario["covered_by"]:
                        scenario["covered_by"].append(tc["name"])

        # Step 4: apply explicit coverage rules for config-delivery test cases
        for keyword, coverage_list in EXPLICIT_COVERAGE.items():
            if keyword not in tc_upper:
                continue
            for feat_basename, scenario_substr in coverage_list:
                if feat_basename not in features:
                    continue
                for scenario in features[feat_basename]["scenarios"]:
                    if scenario_substr.lower() in scenario["name"].lower():
                        if tc["name"] not in scenario["covered_by"]:
                            scenario["covered_by"].append(tc["name"])


# ---------------------------------------------------------------------------
# Generate report
# ---------------------------------------------------------------------------
def generate_report(test_cases, features, output_path):
    """Generate the README.md gap analysis report.

    Only L2 scenarios are counted in coverage metrics.
    L1 scenarios are listed in a separate appendix.
    """

    lines = []
    now = datetime.now().strftime("%B %d, %Y")

    # Count totals — L2 only for primary metrics
    total_l2 = 0
    total_l2_covered = 0
    total_l2_gaps = 0
    total_l1 = 0
    feature_stats = []

    for basename in sorted(features.keys()):
        feat = features[basename]
        l2_scenarios = [s for s in feat["scenarios"] if s["level"] == "L2"]
        l1_scenarios = [s for s in feat["scenarios"] if s["level"] == "L1"]
        n_l2 = len(l2_scenarios)
        n_l1 = len(l1_scenarios)
        n_l2_covered = sum(1 for s in l2_scenarios if s["covered_by"])
        n_l2_gaps = n_l2 - n_l2_covered
        total_l2 += n_l2
        total_l2_covered += n_l2_covered
        total_l2_gaps += n_l2_gaps
        total_l1 += n_l1
        coverage_pct = (
            round(n_l2_covered / n_l2 * 100) if n_l2 > 0 else 100
        )

        if n_l2 == 0:
            status = "➖"  # no L2 scenarios
        elif coverage_pct >= 80:
            status = "✅"
        elif coverage_pct >= 40:
            status = "⚠️"
        else:
            status = "❌"

        feature_stats.append(
            {
                "basename": basename,
                "name": feat["name"],
                "file": feat["file"],
                "l2_scenarios": n_l2,
                "l1_scenarios": n_l1,
                "total_scenarios": n_l2 + n_l1,
                "covered": n_l2_covered,
                "gaps": n_l2_gaps,
                "coverage": coverage_pct,
                "status": status,
            }
        )

    overall_pct = (
        round(total_l2_covered / total_l2 * 100) if total_l2 > 0 else 0
    )

    # --- Executive Summary ---
    lines.append("# Telemetry 2.0 - RDK Automatics Test Gap Analysis\n")
    lines.append("## Executive Summary\n")
    lines.append(
        f"This report analyzes the **RDK Automatics test coverage** against the "
        f"BDD feature documentation in `docs/features/`. The {len(features)} feature files "
        f"contain **{total_l2 + total_l1} total scenarios**, of which **{total_l2}** are "
        f"**L2-testable** (device-level, end-to-end) and **{total_l1}** are "
        f"**L1-only** (internal APIs, data structures, thread safety — unit test candidates). "
        f"The analysis shows **~{overall_pct}% L2 coverage** from {len(test_cases)} Automatics test cases.\n"
    )
    lines.append(
        "> **Note:** The `docs/features/` directory contains ~{0} scenarios documenting "
        "source-code internals. Only the **{1} L2-testable scenarios** — those that "
        "exercise observable device behavior — are counted in coverage metrics. "
        "The remaining {2} L1-only scenarios are candidates for unit tests, not Automatics.".format(
            total_l2 + total_l1, total_l2, total_l1
        )
    )
    lines.append("")

    # Key Findings table
    lines.append("### Key Findings\n")
    lines.append("| Metric | Value |")
    lines.append("|--------|-------|")
    lines.append(f"| Feature Files Analyzed | {len(features)} |")
    lines.append(f"| Total Scenarios (docs/features) | {total_l2 + total_l1} |")
    lines.append(f"| L2 Scenarios (Automatics-testable) | **{total_l2}** |")
    lines.append(f"| L1 Scenarios (Unit test candidates) | {total_l1} |")
    lines.append(f"| Automatics Test Cases | **{len(test_cases)}** |")
    lines.append(f"| L2 Scenarios Covered | **{total_l2_covered}** |")
    lines.append(f"| L2 Scenarios Not Covered (Gaps) | **{total_l2_gaps}** |")
    lines.append(f"| **Overall L2 Coverage** | **~{overall_pct}%** |")
    lines.append("")

    lines.append("---\n")

    # --- L2 Coverage Summary Table ---
    lines.append("## L2 Coverage Summary by Feature\n")
    lines.append(
        "| Feature File | Feature Name | L2 Scenarios | Covered | Gaps | L1 (excluded) | Coverage | Status |"
    )
    lines.append(
        "|---|---|---|---|---|---|---|---|"
    )
    for fs in feature_stats:
        lines.append(
            f"| `{fs['file']}` | {fs['name']} | {fs['l2_scenarios']} "
            f"| {fs['covered']} | {fs['gaps']} | {fs['l1_scenarios']} "
            f"| **~{fs['coverage']}%** | {fs['status']} |"
        )
    lines.append(
        f"| **Total** | | **{total_l2}** | **{total_l2_covered}** "
        f"| **{total_l2_gaps}** | **{total_l1}** | **~{overall_pct}%** | |"
    )
    lines.append("")

    lines.append("---\n")

    # --- Per-Feature L2 Detail ---
    lines.append("## Detailed L2 Coverage Analysis\n")

    gap_id = 0
    all_gaps = []

    for fs in feature_stats:
        feat = features[fs["basename"]]
        l2_scenarios = [s for s in feat["scenarios"] if s["level"] == "L2"]

        if not l2_scenarios:
            continue  # skip features with only L1 scenarios

        lines.append(f"### {feat['name']} (`{feat['file']}`) {fs['status']}\n")

        lines.append("| Scenario | Status | Covered By |")
        lines.append("|----------|--------|------------|")

        for scenario in l2_scenarios:
            if scenario["covered_by"]:
                tc_list = ", ".join(f"`{tc}`" for tc in scenario["covered_by"])
                lines.append(f"| {scenario['name']} | ✅ Covered | {tc_list} |")
            else:
                lines.append(f"| {scenario['name']} | ❌ Gap | — |")
                gap_id += 1
                all_gaps.append(
                    {
                        "id": f"GAP-{gap_id:03d}",
                        "feature": feat["file"],
                        "feature_name": feat["name"],
                        "scenario": scenario["name"],
                    }
                )

        lines.append("")
        if fs["gaps"] > 0:
            lines.append(
                f"**Gaps:** {fs['gaps']} L2 scenario(s) without Automatics test coverage.\n"
            )
        else:
            lines.append("**Gaps:** None — all L2 scenarios have Automatics test coverage.\n")

        lines.append("---\n")

    # --- Gap Summary ---
    lines.append("## Summary of L2 Test Gaps\n")

    if all_gaps:
        # Group by feature
        gaps_by_feature = defaultdict(list)
        for g in all_gaps:
            gaps_by_feature[g["feature"]].append(g)

        lines.append(
            "| Gap ID | Feature | Scenario | Priority |"
        )
        lines.append("|--------|---------|----------|----------|")

        for feat_file, gaps in gaps_by_feature.items():
            for g in gaps:
                lines.append(
                    f"| {g['id']} | `{g['feature']}` | {g['scenario']} | MEDIUM |"
                )

        lines.append("")

        lines.append(f"**Total L2 Gaps: {len(all_gaps)} scenarios across "
                      f"{len(gaps_by_feature)} feature files.**\n")
    else:
        lines.append("No L2 gaps identified — all L2 feature scenarios have test coverage.\n")

    lines.append("---\n")

    # --- Remediation Plan ---
    lines.append("## Remediation Plan\n")
    lines.append("### Priority Areas\n")

    priority_features = sorted(
        [fs for fs in feature_stats if fs["gaps"] > 0],
        key=lambda x: x["gaps"],
        reverse=True,
    )

    if priority_features:
        lines.append("| Priority | Feature | L2 Gaps | Suggested Action |")
        lines.append("|----------|---------|---------|------------------|")
        for i, pf in enumerate(priority_features[:10], 1):
            lines.append(
                f"| {i} | `{pf['file']}` ({pf['name']}) | {pf['gaps']} gaps "
                f"| Add Automatics test cases covering uncovered L2 scenarios |"
            )
        lines.append("")
    else:
        lines.append("No remediation needed — full L2 coverage achieved.\n")

    lines.append("---\n")

    # --- Appendix A: Test Case → Feature Mapping ---
    lines.append("## Appendix A: Automatics Test Case → Feature Mapping\n")

    for tc in test_cases:
        tc_upper = tc["name"].upper()
        mapped_features = set()

        for keyword, feat_list in TC_FEATURE_MAP.items():
            if keyword in tc_upper:
                mapped_features.update(feat_list)

        all_step_text = " ".join(
            s["description"] + " " + s["expected_output"] for s in tc["steps"]
        ).lower()
        for keyword, feat_list in STEP_KEYWORD_MAP.items():
            if keyword in all_step_text:
                mapped_features.update(feat_list)

        feat_str = ", ".join(f"`{f}.feature`" for f in sorted(mapped_features))
        lines.append(f"### {tc['sno']}. `{tc['name']}` ({len(tc['steps'])} steps)\n")
        lines.append(f"**Mapped Features:** {feat_str}\n")

        # Show key step themes
        key_steps = []
        for step in tc["steps"]:
            desc = step["description"]
            if len(desc) > 20 and desc not in key_steps:
                key_steps.append(desc)

        if key_steps:
            lines.append("**Key Test Steps:**\n")
            for ks in key_steps[:8]:
                truncated = ks[:120] + "..." if len(ks) > 120 else ks
                lines.append(f"- {truncated}")
            if len(key_steps) > 8:
                lines.append(f"- ... and {len(key_steps) - 8} more steps")
            lines.append("")

    lines.append("---\n")

    # --- Appendix B: Uncovered L2 Scenarios ---
    lines.append("## Appendix B: All Uncovered L2 Scenarios\n")

    if all_gaps:
        current_feat = None
        for g in all_gaps:
            if g["feature"] != current_feat:
                current_feat = g["feature"]
                lines.append(f"### `{current_feat}` — {g['feature_name']}\n")
                lines.append("| # | Scenario |")
                lines.append("|---|----------|")

            lines.append(f"| {g['id']} | {g['scenario']} |")

        lines.append("")
    else:
        lines.append("All L2 scenarios are covered.\n")

    lines.append("---\n")

    # --- Appendix C: L1-Only Scenarios (Unit Test Candidates) ---
    lines.append("## Appendix C: L1-Only Scenarios (Unit Test Candidates)\n")
    lines.append(
        "> The following scenarios describe internal APIs, data structures, and "
        "utility functions. These are **not practical for Automatics end-to-end testing** "
        "and should be covered by L1 unit tests instead.\n"
    )

    l1_by_feature = defaultdict(list)
    for basename in sorted(features.keys()):
        feat = features[basename]
        for s in feat["scenarios"]:
            if s["level"] == "L1":
                l1_by_feature[feat["file"]].append(s["name"])

    if l1_by_feature:
        lines.append("| Feature File | L1 Scenarios | Examples |")
        lines.append("|---|---|---|")
        for feat_file, scenarios in l1_by_feature.items():
            examples = "; ".join(scenarios[:3])
            if len(scenarios) > 3:
                examples += f"; +{len(scenarios) - 3} more"
            lines.append(f"| `{feat_file}` | {len(scenarios)} | {examples} |")
        lines.append("")
        lines.append(f"**Total L1 scenarios: {total_l1}**\n")
    else:
        lines.append("No L1-only scenarios identified.\n")

    lines.append("---\n")
    lines.append(f"*Report generated on: {now}*\n")
    lines.append(
        "*Scope: RDK Automatics test gap analysis comparing "
        "`test/functional-tests/automatics/` against L2 subset of `docs/features/`*\n"
    )
    lines.append(
        f"*Key Finding: {total_l2} L2 scenarios with ~{overall_pct}% coverage; "
        f"{total_l2_gaps} scenarios need Automatics test coverage*\n"
    )

    # Write output
    report_text = "\n".join(lines)
    with open(output_path, "w", encoding="utf-8") as f:
        f.write(report_text)

    print(f"Gap analysis report generated: {output_path}")
    return output_path


# ---------------------------------------------------------------------------
# Main
# ---------------------------------------------------------------------------
def main():
    parser = argparse.ArgumentParser(
        description="Generate Automatics Test Gap Analysis Report"
    )
    parser.add_argument(
        "--md",
        default=None,
        help="Path to the generated test case markdown file",
    )
    parser.add_argument(
        "--features",
        default=None,
        help="Path to the docs/features directory",
    )
    parser.add_argument(
        "--output",
        default=None,
        help="Output path for README.md",
    )
    args = parser.parse_args()

    # Resolve paths relative to script location
    script_dir = os.path.dirname(os.path.abspath(__file__))
    repo_root = os.path.abspath(os.path.join(script_dir, "..", "..", ".."))

    md_path = args.md or os.path.join(script_dir, "Filtered_Script_Details.md")
    features_dir = args.features or os.path.join(repo_root, "docs", "features")
    output_path = args.output or os.path.join(script_dir, "README.md")

    if not os.path.isfile(md_path):
        print(f"ERROR: Markdown test file not found: {md_path}")
        sys.exit(1)

    if not os.path.isdir(features_dir):
        print(f"ERROR: Features directory not found: {features_dir}")
        sys.exit(1)

    print(f"Parsing test cases from: {md_path}")
    test_cases = parse_test_cases(md_path)
    print(f"  Found {len(test_cases)} test cases")

    print(f"Parsing feature files from: {features_dir}")
    features = parse_feature_files(features_dir)
    total_scenarios = sum(len(f["scenarios"]) for f in features.values())
    total_l2 = sum(
        1 for f in features.values() for s in f["scenarios"] if s["level"] == "L2"
    )
    total_l1 = total_scenarios - total_l2
    print(f"  Found {len(features)} features with {total_scenarios} scenarios")
    print(f"  Classified: {total_l2} L2-testable, {total_l1} L1-only")

    print("Mapping test cases to L2 feature scenarios...")
    map_test_cases_to_features(test_cases, features)

    covered = sum(
        1
        for f in features.values()
        for s in f["scenarios"]
        if s["covered_by"] and s["level"] == "L2"
    )
    print(f"  L2 Covered: {covered}/{total_l2} scenarios")

    generate_report(test_cases, features, output_path)
    print("Done.")


if __name__ == "__main__":
    main()
