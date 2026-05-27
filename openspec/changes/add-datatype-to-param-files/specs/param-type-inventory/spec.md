## ADDED Requirements

### Requirement: RFC namespace parameter type annotations
The `docs/api/rfc_namespace_parameters.txt` file SHALL include a `Type` column for every parameter entry, using TR-069 base types (`string`, `boolean`, `unsignedInt`, `int`, `dateTime`, `base64`). Types SHALL be derived from `data-model-generic.xml` / `data-model-stb.xml` where the parameter is declared; for RFC opaque-store parameters not in the XML, type SHALL be inferred from the C++ set-handler logic and SHALL default to `string` when no type signal is present.

#### Scenario: Viewing an RFC namespace parameter entry
- **WHEN** a developer opens `docs/api/rfc_namespace_parameters.txt`
- **THEN** every parameter row SHALL contain a non-empty `Type` field that is one of the standard TR-069 base type names

#### Scenario: RFC opaque-store parameter with no XML declaration
- **WHEN** an RFC parameter has no `<parameter>` entry in `data-model-generic.xml` or `data-model-stb.xml`
- **THEN** the `Type` field SHALL be set to `string` unless the C++ handler contains an explicit type conversion (e.g. `atoi`, `strcmp("true"...)`)

### Requirement: Non-RFC namespace parameter type annotations
The `docs/api/non_rfc_namespace_parameters.txt` file SHALL include a `Type` column for every parameter entry. Types SHALL be derived from `data-model-generic.xml` or `data-model-stb.xml` where declared; otherwise from C++ getter/setter inspection.

#### Scenario: Viewing a standard TR-181 parameter entry
- **WHEN** a developer opens `docs/api/non_rfc_namespace_parameters.txt`
- **THEN** every parameter row SHALL contain a non-empty `Type` field aligned in a consistent column

#### Scenario: Parameter declared in waldb XML
- **WHEN** a parameter path matches a `<parameter name="...">` in `data-model-generic.xml` or `data-model-stb.xml`
- **THEN** the `Type` field SHALL match the `type` attribute value from that XML element

### Requirement: Bootstrap parameter type annotation consistency
The `docs/api/bootstrap_rfc_parameters.txt` file SHALL have a fully populated and consistently formatted `Type` column for every parameter entry, including both routable Bootstrap store parameters and non-routable boot-time defaults.

#### Scenario: Every bootstrap entry has a type
- **WHEN** a developer reads `docs/api/bootstrap_rfc_parameters.txt`
- **THEN** no row SHALL have a blank or missing `Type` field

#### Scenario: Type column format matches other files
- **WHEN** the bootstrap file is compared to the RFC and non-RFC files
- **THEN** the `Type` values SHALL use the same TR-069 base type vocabulary and column alignment style

### Requirement: Type derivation disclaimer header
Each of the three documentation files SHALL include a header note stating that data types are derived from the tr69hostif implementation (XML data model and C++ source) and may not reflect a normative CWMP/BBF Device Type schema.

#### Scenario: Reader checks type source
- **WHEN** a developer reads the file header
- **THEN** a disclaimer note SHALL be present explaining that types are implementation-derived and may differ from normative BBF TR-181 schemas

### Requirement: Standard TR-069 type vocabulary
The `Type` column in all three files SHALL use only the following values: `string`, `boolean`, `unsignedInt`, `int`, `dateTime`, `base64`. No custom or compound type strings are permitted in this change.

#### Scenario: Type value is a known TR-069 base type
- **WHEN** any `Type` cell is read from any of the three files
- **THEN** its value SHALL be one of: `string`, `boolean`, `unsignedInt`, `int`, `dateTime`, `base64`
