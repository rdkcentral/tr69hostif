import os
import subprocess
import time

from behave import given, when, then


DBG_SERVICES_RFC = (
    "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Identity."
    "DbgServices.Enable"
)

DEVICE_TYPE_RFC = (
    "Device.DeviceInfo.X_RDKCENTRAL-COM_RFC.Identity."
    "DeviceType"
)

STATE_FILE = "/opt/enable_secure_dbg"


def run_command(command):
    return subprocess.run(
        command,
        shell=True,
        stdout=subprocess.PIPE,
        stderr=subprocess.PIPE,
        universal_newlines=True
    )


def set_rfc(parameter, value, data_type):
    command = (
        "tr181 -d -s -t {type} -v {value} {parameter}"
    ).format(
        type=data_type,
        value=value,
        parameter=parameter
    )

    result = run_command(command)

    assert result.returncode == 0, (
        "TR-181 set failed. command='{}' stdout='{}' stderr='{}'"
        .format(command, result.stdout, result.stderr)
    )

    assert "Set operation success" in result.stdout, (
        "TR-181 set operation did not report success: {}"
        .format(result.stdout)
    )


def wait_for_state(expected, timeout=5):
    end_time = time.time() + timeout

    while time.time() < end_time:
        if os.path.exists(STATE_FILE):
            with open(STATE_FILE, "r") as fp:
                actual = fp.read().strip()

            if actual == expected:
                return

        time.sleep(0.2)

    actual = "<missing>"

    if os.path.exists(STATE_FILE):
        with open(STATE_FILE, "r") as fp:
            actual = fp.read().strip()

    raise AssertionError(
        "{} expected '{}' but found '{}'"
        .format(STATE_FILE, expected, actual)
    )


@given('DeviceType RFC is set to "{value}"')
@when('DeviceType RFC is set to "{value}"')
def step_set_device_type(context, value):
    set_rfc(
        DEVICE_TYPE_RFC,
        value,
        "string"
    )


@given('DbgServices RFC is set to "{value}"')
@when('DbgServices RFC is set to "{value}"')
def step_set_dbg_services(context, value):
    set_rfc(
        DBG_SERVICES_RFC,
        value,
        "bool"
    )


@then('secure debug state file should contain "{expected}"')
def step_verify_secure_debug_state(context, expected):
    wait_for_state(expected)


@then("secure debug state file should be immutable")
def step_verify_file_immutable(context):
    result = run_command(
        "lsattr {}".format(STATE_FILE)
    )

    assert result.returncode == 0, (
        "lsattr failed: {}".format(result.stderr)
    )

    output = result.stdout.strip()
    assert output, "lsattr returned empty output"

    attributes = output.split()[0]

    assert "i" in attributes, (
        "{} is not immutable: {}"
        .format(STATE_FILE, output)
    )


@when("direct modification of secure debug state file is attempted")
def step_attempt_direct_modification(context):
    context.direct_write_result = run_command(
        "echo 0 > {}".format(STATE_FILE)
    )


@then("direct modification should fail")
def step_verify_direct_modification_failed(context):
    assert hasattr(context, "direct_write_result"), (
        "Direct modification was not attempted"
    )

    assert context.direct_write_result.returncode != 0, (
        "Direct modification unexpectedly succeeded"
    )
