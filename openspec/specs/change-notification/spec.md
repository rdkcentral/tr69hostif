## Purpose
Define how tr69hostif detects profile value changes and propagates notifications to subscribed internal and external event consumers.

## Requirements

### Requirement: Periodic Update Polling
The system SHALL run a background update workflow that periodically checks registered profiles for value changes.

#### Scenario: No changes detected
- **WHEN** the update workflow polls registered profiles and no parameter value changes are detected
- **THEN** no value-change event is emitted and the workflow continues to the next polling interval

#### Scenario: Changes detected
- **WHEN** the update workflow polls registered profiles and one or more parameter value changes are detected
- **THEN** the workflow emits value-change notifications using the system notification pathways

### Requirement: IARM Value-Change Fanout
The system SHALL publish detected parameter value changes over IARM eventing for subscribed consumers.

#### Scenario: Managed parameter changes
- **WHEN** a managed parameter changes and is reported by the update workflow
- **THEN** an IARM value-change event is published with parameter identity, value payload, and type metadata

### Requirement: Parodus Notification Queueing
The system SHALL queue and publish value-change notifications to Parodus when the Parodus path is enabled.

#### Scenario: Parodus path enabled
- **WHEN** a value-change event is generated and Parodus integration is active
- **THEN** the notification is enqueued and transmitted through the Parodus notification adapter

#### Scenario: Parodus path disabled
- **WHEN** a value-change event is generated and Parodus integration is inactive
- **THEN** the system continues local event fanout without requiring Parodus publication
