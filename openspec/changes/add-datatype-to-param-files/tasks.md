## 1. Preparation — type source research

- [x] 1.1 Parse `src/hostif/parodusClient/waldb/data-model/data-model-generic.xml` and extract `<parameter name="..." type="...">` pairs into a lookup table
- [x] 1.2 Parse `src/hostif/parodusClient/waldb/data-model/data-model-stb.xml` and merge into the same lookup table (STB-specific overrides)
- [x] 1.3 Review `src/hostif/profiles/DeviceInfo/Device_DeviceInfo.cpp` set-handlers for RFC opaque-store parameters that use `atoi`, `strcmp("true"...)`, or similar conversions and note their inferred types

## 2. Update `docs/api/bootstrap_rfc_parameters.txt`

- [x] 2.1 Add a disclaimer header note (implementation-derived types, not normative BBF schema)
- [x] 2.2 Verify and normalise the existing partial `Type` column so all routable Bootstrap parameters (Groups 1–5) have a consistent type value from the TR-069 base type vocabulary
- [x] 2.3 Add a `Type` column to non-routable boot-time default entries (Group 6) that currently lack one
- [x] 2.4 Verify column alignment is consistent across all rows in the file

## 3. Update `docs/api/rfc_namespace_parameters.txt`

- [x] 3.1 Add a disclaimer header note (implementation-derived types, not normative BBF schema)
- [x] 3.2 For each of the ~220 RFC parameter entries, look up the type from the XML lookup table (task 1.1–1.2); assign inferred type from task 1.3 where applicable; default to `string` for remainder
- [x] 3.3 Insert the `Type` column into each row in a consistent position and column width
- [x] 3.4 Verify no parameter row has a blank or missing `Type` field

## 4. Update `docs/api/non_rfc_namespace_parameters.txt`

- [x] 4.1 Add a disclaimer header note (implementation-derived types, not normative BBF schema)
- [x] 4.2 For each of the ~371 non-RFC parameter entries, look up the type from the XML lookup table (task 1.1–1.2); fall back to C++ source inspection for any gaps
- [x] 4.3 Insert the `Type` column into each row in a consistent position and column width matching the format used in task 3.3
- [x] 4.4 Verify no parameter row has a blank or missing `Type` field

## 5. Validation

- [x] 5.1 Spot-check at least 10 parameters per file against the XML data model or C++ source to confirm type accuracy
- [x] 5.2 Confirm `Type` values are restricted to: `string`, `boolean`, `unsignedInt`, `int`, `dateTime`, `base64`
- [x] 5.3 Confirm all three files carry the disclaimer header note
- [x] 5.4 Confirm column alignment is visually consistent in each file when viewed with a fixed-width font
