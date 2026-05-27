## ADDED Requirements

### Requirement: On-demand RBUS event subscription for USP subscriptions
The plug-in SHALL register RBUS event subscriptions only for parameters that have active USP subscriptions, creating subscriptions when USP controllers subscribe and removing them when subscriptions expire or are deleted.

#### Scenario: USP ValueChange subscription triggers RBUS subscription
- **WHEN** a USP controller creates a Subscription with NotifType="ValueChange" targeting a parameter managed by the plug-in
- **THEN** the plug-in SHALL register an RBUS event subscription on that parameter name and store the mapping between the USP subscription ID and the RBUS subscription

#### Scenario: RBUS value change fires USP notification
- **WHEN** an RBUS value-change event fires for a subscribed parameter
- **THEN** the plug-in SHALL call `USP_SIGNAL_DataModelEvent()` with the parameter name and new value, which causes obuspa to send a USP Notify message to the subscribing controller

#### Scenario: USP subscription removal cleans up RBUS subscription
- **WHEN** a USP subscription is deleted or expires
- **THEN** the plug-in SHALL unsubscribe from the corresponding RBUS event to avoid unnecessary event traffic

### Requirement: Object creation and deletion notifications
The plug-in SHALL support USP ObjectCreation and ObjectDeletion notification types for table objects whose instances can change at runtime.

#### Scenario: Table object instance added
- **WHEN** a new instance is added to a table object managed by tr69hostif (detected via RBUS ObjectCreation event or instance refresh)
- **THEN** the plug-in SHALL call `USP_SIGNAL_ObjectAdded()` with the instance path

#### Scenario: Table object instance removed
- **WHEN** an instance is removed from a table object managed by tr69hostif
- **THEN** the plug-in SHALL call `USP_SIGNAL_ObjectDeleted()` with the instance path

### Requirement: Notification delivery does not block RBUS event thread
Notification processing SHALL be asynchronous — the RBUS event callback SHALL enqueue the notification and return immediately.

#### Scenario: High-frequency value changes
- **WHEN** multiple RBUS events fire within the same second for different subscribed parameters
- **THEN** all events SHALL be enqueued without blocking the RBUS event delivery thread, and each SHALL result in a separate USP Notify message delivered to the subscribing controller
