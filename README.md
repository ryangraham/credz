
[![Build Status](https://circleci.com/gh/ryangraham/credz.svg?style=svg)](https://circleci.com/gh/ryangraham/credz)

# credz

Turn your Okta identity into AWS credentials on the command line.

- [Installation](#installation)
  - [Homebrew](#homebrew)
  - [CMake](#cmake)
- [Usage](#usage)
- [Configuration](#configuration)
- [Goals](#goals)

## Installation

### Homebrew

WIP

### CMake

```
git clone git@github.com:ryangraham/credz.git
cd credz
make install
```
It should have installed to `/usr/local/bin/credz`, but lets test it out.
```
credz -v
```

## Usage

```
➜  credz git:(master) ✗ ./credz -p dev
Okta authentication complete.
Okta Push initiated. Waiting for response...
Okta Push confirmed.
AWS assume role with SAML complete.
Profile dev written to credentials file.
➜  credz git:(master) ✗
```
Then test it out with AWS CLI.
```
aws sts get-caller-identity --profile dev
```

On first run you may also be prompted for the name of your Okta org, username, & password. I recommend saving your organization and username in the config file. Credz will save the password in your keychain for you.

## Configuration

The default config file is `~/.credz`

```
[Okta]
organization = mycompany
username = ryang
enable_keychain = true

```
If your Okta URL is `https://scooterz.okta.com`, then your Okta organization in the config file should be `scooterz`

## Goals

WIP
