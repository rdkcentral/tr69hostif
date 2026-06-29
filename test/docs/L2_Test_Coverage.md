# L2 Functional Test Coverage

> Generated: June 29, 2026
> Scope reviewed: `test/functional-tests/tests/`, `test/functional-tests/features/`
> This file is the single source of truth for L2 coverage status.

---

## Coverage Summary

| Metric | Value |
|---|---:|
| Runnable L2 pytest files | 25 |
| Runnable L2 pytest test functions (`def test_`) | 313 |
| BDD feature files (`*.feature`) | 29 |
| Total feature scenarios (`Scenario` + `Scenario Outline`) | 355 |
| Estimated module surface (baseline) | ~761 |
| Estimated current implementation coverage | ~41.1% |
| Estimated remaining gap | ~448 |

Coverage formula:

- `313 / 761 = 41.1%`

---

## Delta From Previous Snapshot

Compared to the prior report state (`47` tests, `73` scenarios), the current suite has expanded to:

- `313` tests (`+266`)
- `355` scenarios (`+282`)
- `25` runnable test files (`+21`)
- `29` feature files (`+25`)

---

## Category-Wise Test Distribution

| Category | Test Functions |
|---|---:|
| Device/IP Core | 56 |
| MoCA | 53 |
| Custom/DeviceInfo | 43 |
| WebPA/Parodus | 30 |
| Ethernet | 24 |
| Thunder Plugins | 24 |
| Bootup/Lifecycle | 18 |
| RFC/Bootstrap Store | 16 |
| Time/Chrony | 15 |
| Handler Communications | 10 |
| HTTP Server | 8 |
| Processor/ProcessStatus | 8 |
| DHCPv4 | 4 |
| Negative/Edge Cases | 4 |

Category total check: `313`.

---

## Feature-to-Test Mapping

Alias-aware mapping used for legacy naming:

- `test_bootup_sequence.py` -> `tr69hostif_bootup_sequence.feature`
- `test_handlers_communications.py` -> `tr69hostif_handlers_communications.feature`

| Mapping Metric | Value |
|---|---:|
| Test files with matching feature file | 25 / 25 |
| Feature files with matching test file | 25 / 29 |
| Feature files currently documentation-only | 4 |

Documentation-only feature files:

1. `tr69hostif_ethernet.feature`
2. `tr69hostif_negative_tests.feature`
3. `tr69hostif_thunder_plugins.feature`
4. `tr69hostif_time_chrony.feature`

Note: equivalent runnable coverage exists in more granular feature/test pairs (for example `tr69hostif_ethernet_handlers.feature` and `tr69hostif_devicetime.feature`).

---

## Order and Runtime Reliability

| Metric | Value |
|---|---:|
| Total `@pytest.mark.run(order=...)` tags | 313 |
| Unique order values | 306 |
| Duplicate order values | 7 |

Duplicate order values found:

- `25` (x2)
- `26` (x2)
- `27` (x2)
- `28` (x2)
- `48` (x2)
- `49` (x2)
- `50` (x2)

Skip/disable observations:

- No static `@pytest.mark.skip` or `@pytest.mark.xfail` decorators were found.
- One runtime skip path exists in `tr69hostif_thunder_negative_edge_cases.py` if `127.0.0.1:9998` cannot be bound.

---

## Current Priorities

1. Resolve duplicate pytest order values to ensure deterministic execution order.
2. Keep umbrella documentation-only features aligned with granular runnable features.
3. Continue increasing coverage in the smallest-covered categories (`DHCPv4`, `Negative/Edge Cases`, `HTTP Server`, `Processor/ProcessStatus`).
4. Add CI checks for order collisions and missing feature-to-test mapping.

---

## Verification Method (June 29, 2026)

Counts were derived from workspace scans:

- Test function count: regex `^def test_` on `test/functional-tests/tests/*.py` excluding helper modules.
- Scenario count: regex `^\s*Scenario(?: Outline)?:` on `test/functional-tests/features/*.feature`.
- Order tags: regex `@pytest\.mark\.run\(order\s*=\s*(\d+)\)`.

---

## Related Paths

- `test/functional-tests/tests/`
- `test/functional-tests/features/`
- `test/functional-tests/features/README.md`
