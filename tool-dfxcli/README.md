# dfxcli tool

An executable command line utility which uses the API to perform operations. It
is not full-featured at present, but was constructed to test the basics.

Most of the commands try to return a JSON response so it can be subsequently
processed by [`jq`](https://stedolan.github.io/jq/).

## Supported Operations

| Operation                 | Description                                               |
| ------------------------- | --------------------------------------------------------- |
| dfxcli process            | Process a bunch of payloads (alias: create measurement)   |
| dfxcli config show        | Show the active configuration                             |
| dfxcli config list        | List the known configurations in dfxcloud.yaml            |
| dfxcli login              | Get a user auth-token                                     |
| dfxcli logout             | Release a user auth-token                                 |
| dfxcli register           | Get a device-token                                        |
| dfxcli unregister         | Release a device-token                                    |
| dfxcli get study          | Retrieve the studies and study config available on server |
| dfxcli get device         | Retrieve the devices                                      |
| dfxcli get user           | Retrieve the users                                        |
| dfxcli config sample      | Write a sample configuration file to stdout               |
| dfxcli create measurement | create a new measurement from payloads                    |

## Getting started

You need to create a `~/.dfxcloud.yaml` file containing common properties
used while making calls and is used to initialize the CloudConfig structure.

```bash
# Note this is a "simple" format, to get advanced with multiple contexts pass
# the --advanced option.
dfxcli config sample > ~/.dfxcloud.yaml
```

Edit `~/.dfxcloud.yaml` and provide the correct values for your environment.
With a proper auth-email, auth-password, auth-org you can perform a login
request to obtain a `auth-token` with:

```bash
dfxcli login
# Copy the auth-token value to your ~/.dfxcloud.yaml
```

Having an auth-token, you can place in the `~/.dfxcloud.yaml` and either remove
the auth-email, auth-password, auth-org or leave them there. The `dfxcli` first
checks the `CloudConfig` for an auth-token and if it is unable to locate it will
implicitly use the auth-email, auth-password, auth-org to obtain an auth-token
for the current session. When the session ends if `dfxcli` performed the login,
it will subsequently logout to release the token. If it was obtained from the
configuration file it will remain "cached" and available for future calls.

The `device-token` can be handled similarly but requires a license in addition
to the user authentication information and invoking:

```bash
dfxcli register
# Copy the device-token value to your ~/.dfxcloud.yaml
```

With a configuration file setup, using the CLI can be invoked with `--help` to
provide more information about available options that can be customized.

## Viewing configuration

```bash
# Display a list of available contexts, identifying which is the default
dfx config list

# Display the current configuration being used, append -f for full without OBFUSCATED
dfx config show

# Select a specific context 'na-rest' to use, context select can apply to all operations
dfx -c na-rest config show
```

## Performing a measurement

```bash
# Process a folder containing payloads, it will scan for ~/payloads/payload-*.bin and
# naturally sort them then start sending them to the server with a 1 second delay which
# can be changed with the -d option.
dfxcli process ~/payloads -d 500   # 500 milliseconds

# In Bash, you can pass all the payloads using a glob expression instead of a directory.
# The payload files will still be sorted naturally unless you pass -n to not sort.
dfxcli process ~/payloads/mypayload-*bin

# Capture the server response to results.json. If you ctrl-c, it will be intercepted
# and a proper JSON file up until that point will be provided.
dfxcli process ~/payloads/*.bin --output results.json

# Verbose will include the warnings and general operation logs which are useful for
# understanding what is being sent/received from the server
dfxcli process ~/payloads/*.bin --verbose --output results.json
```

## Other examples

```bash
dfxcli get study
dfxcli get device | jq -c '.[]'      # List all devices
dfxcli get device | jq -c '.[].ID'   # List just the IDs
dfxcli get device | jq -c '.[] | select(.StatusID == "ACTIVE")'  # active devices

dfxcli get user | jq -c '.[]'

# Active studies with Name and ID
dfxcli get study | jq -c '.[] | select(.StatusID == "ACTIVE") | {Name:.Name, ID:.ID}'

# If one study id given with -s option, the config will be saved
dfxcli get study -s studyConfig.bin 03da56f3-17c7-4769-ae2e-7854242bbdd8
```
