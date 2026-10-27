Feature: Secure debug state handling

  Scenario: Secure debug enabled for test device
    Given DeviceType RFC is set to "test"
    When DbgServices RFC is set to "true"
    Then secure debug state file should contain "1"
    And secure debug state file should be immutable

  Scenario: Secure debug disabled when DbgServices is false
    Given DeviceType RFC is set to "test"
    When DbgServices RFC is set to "false"
    Then secure debug state file should contain "0"
    And secure debug state file should be immutable

  Scenario: Secure debug disabled when DeviceType is not test
    Given DbgServices RFC is set to "true"
    When DeviceType RFC is set to "prod"
    Then secure debug state file should contain "0"
    And secure debug state file should be immutable

  Scenario: Secure debug disabled when both conditions are false
    Given DeviceType RFC is set to "prod"
    When DbgServices RFC is set to "false"
    Then secure debug state file should contain "0"
    And secure debug state file should be immutable

  Scenario: Secure debug state can be updated by RFC changes
    Given DeviceType RFC is set to "test"
    When DbgServices RFC is set to "true"
    Then secure debug state file should contain "1"
    And secure debug state file should be immutable
    When DbgServices RFC is set to "false"
    Then secure debug state file should contain "0"
    And secure debug state file should be immutable
    When DbgServices RFC is set to "true"
    Then secure debug state file should contain "1"
    And secure debug state file should be immutable

  Scenario: Direct modification of secure debug state file is blocked
    Given DeviceType RFC is set to "test"
    And DbgServices RFC is set to "true"
    Then secure debug state file should contain "1"
    And secure debug state file should be immutable
    When direct modification of secure debug state file is attempted
    Then direct modification should fail
    And secure debug state file should contain "1"
