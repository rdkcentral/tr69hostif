#!/usr/bin/env python3
"""
Extract ALL TR-181 data model parameters from XML data-model files.
Produces a CSV with columns: Parameter, bsUpdate, FactoryDefault, DataType, Access, SourceFile.
Deduplicates by (Parameter + SourceFile) keeping first occurrence.
"""

import xml.etree.ElementTree as ET
import csv
import os
import sys

XML_FILES = [
    r'src\hostif\parodusClient\waldb\data-model-generic.xml',
    r'src\hostif\parodusClient\waldb\data-model\data-model-generic.xml',
    r'src\hostif\parodusClient\waldb\data-model\data-model-stb.xml',
    r'src\hostif\parodusClient\waldb\data-model\data-model-tv.xml',
    r'src\hostif\parodusClient\waldb\snmp-data-model.xml',
]

OUTPUT_CSV = 'tr181_datamodel_parameters.csv'

KNOWN_TYPES = frozenset([
    'string', 'boolean', 'unsignedInt', 'int', 'unsignedLong',
    'long', 'dateTime', 'base64', 'hexBinary',
])


def get_data_type(syntax_elem):
    """Extract the data type from a <syntax> element."""
    for child in syntax_elem:
        if child.tag in KNOWN_TYPES:
            return child.tag
    return ''


def get_factory_default(syntax_elem):
    """Extract the factory default value if present."""
    default_elem = syntax_elem.find('default')
    if default_elem is not None and default_elem.get('type') == 'factory':
        return default_elem.get('value', '')
    return ''


def extract_all_parameters(xml_path):
    """Parse an XML file and extract ALL parameters with their attributes."""
    results = []
    tree = ET.parse(xml_path)
    root = tree.getroot()

    # Strip namespace from all tags for easier processing
    for elem in root.iter():
        if '}' in elem.tag:
            elem.tag = elem.tag.split('}', 1)[1]

    source = os.path.basename(xml_path)

    for model in root.findall('model'):
        for obj in model.findall('object'):
            obj_base = obj.get('base', '')

            for param in obj.findall('parameter'):
                param_base = param.get('base', '')
                full_name = obj_base + param_base

                param_access = param.get('access', '')
                bs_update = param.get('bsUpdate', '')

                syntax = param.find('syntax')
                data_type = get_data_type(syntax) if syntax is not None else ''
                factory_default = get_factory_default(syntax) if syntax is not None else ''

                results.append({
                    'Parameter': full_name,
                    'bsUpdate': bs_update,
                    'FactoryDefault': factory_default,
                    'DataType': data_type,
                    'Access': param_access,
                    'SourceFile': source,
                })

    return results


def deduplicate(all_results):
    """Remove duplicates by (Parameter, SourceFile), keeping first occurrence."""
    seen = set()
    deduped = []
    duplicates = 0
    for row in all_results:
        key = (row['Parameter'], row['SourceFile'])
        if key in seen:
            duplicates += 1
            continue
        seen.add(key)
        deduped.append(row)
    return deduped, duplicates


def main():
    base_dir = os.path.dirname(os.path.abspath(__file__))
    all_results = []

    for rel_path in XML_FILES:
        full_path = os.path.join(base_dir, rel_path)
        if not os.path.exists(full_path):
            print(f"WARNING: File not found: {full_path}", file=sys.stderr)
            continue
        print(f"Processing: {rel_path}")
        results = extract_all_parameters(full_path)
        factory_count = sum(1 for r in results if r['FactoryDefault'])
        bs_count = sum(1 for r in results if r['bsUpdate'])
        print(f"  {len(results)} params, {factory_count} with factory defaults, {bs_count} with bsUpdate")
        all_results.extend(results)

    # Sort by parameter name, then source file
    all_results.sort(key=lambda x: (x['Parameter'], x['SourceFile']))

    # Deduplicate
    all_results, dup_count = deduplicate(all_results)
    if dup_count:
        print(f"\nRemoved {dup_count} duplicate entries")

    # Write CSV
    fieldnames = ['Parameter', 'bsUpdate', 'FactoryDefault', 'DataType', 'Access', 'SourceFile']
    output_path = os.path.join(base_dir, OUTPUT_CSV)
    with open(output_path, 'w', newline='', encoding='utf-8-sig') as f:
        writer = csv.DictWriter(f, fieldnames=fieldnames)
        writer.writeheader()
        writer.writerows(all_results)

    print(f"\nTotal unique parameters: {len(all_results)}")
    print(f"Output written to: {output_path}")


if __name__ == '__main__':
    main()
