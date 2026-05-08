import csv
from collections import Counter

with open('tr181_datamodel_parameters.csv', encoding='utf-8-sig') as f:
    rows = list(csv.DictReader(f))

# Check for remaining duplicates
keys = [(r['Parameter'], r['SourceFile']) for r in rows]
dups = [(k,c) for k,c in Counter(keys).items() if c > 1]
if dups:
    print(f'DUPLICATES FOUND: {len(dups)}')
    for d in dups[:10]:
        print(f'  {d}')
else:
    print('No duplicates found')

# Stats
print(f'Total rows: {len(rows)}')
sources = Counter(r['SourceFile'] for r in rows)
for s,c in sorted(sources.items()):
    print(f'  {s}: {c}')

# Sample first/last 5
print('\nFirst 5:')
for r in rows[:5]:
    print(f"  {r['Parameter']}")
print('\nLast 5:')
for r in rows[-5:]:
    print(f"  {r['Parameter']}")

# Verify no empty Parameter names
empty = [r for r in rows if not r['Parameter'].strip()]
print(f'\nEmpty parameter names: {len(empty)}')

# Verify all start with Device.
non_device = [r for r in rows if not r['Parameter'].startswith('Device.')]
print(f'Parameters not starting with Device.: {len(non_device)}')
for r in non_device[:5]:
    print(f"  '{r['Parameter']}'")

# Check factory default counts
factory = [r for r in rows if r['FactoryDefault']]
print(f'\nWith factory defaults: {len(factory)}')
bs = [r for r in rows if r['bsUpdate']]
print(f'With bsUpdate: {len(bs)}')
