
[![Build Status](https://circleci.com/gh/ryangraham/credz.svg?style=svg)](https://circleci.com/gh/ryangraham/credz)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/ryangraham/credz/master/LICENSE)

# credz

Turn your Okta identity into AWS credentials on the command line.

_warning: this is alpha software and it will overwrite your entire ~/.aws/credentials file. I recommend taking a manual backup beforehand._

- [Installation](#installation)
  - [Homebrew](#homebrew)
- [Usage](#usage)
- [Configuration](#configuration)
- [Goals](#goals)

## Installation

### Homebrew

This is the easiest and preferred method of installation.

```
brew tap ryangraham/credz
brew install credz
credz -v
```
<p align="center"><img src="/img/demo.gif?raw=true"/></p>

## Usage

On first run you will be prompted for your Okta organization, username, and password. credz will store the password in your keychain and everything else in a config file for future use.

Note: If your Okta URL is `https://scooterz.okta.com`, then your Okta organization will be `scooterz`

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

## Configuration

The default config file is `~/.credz`

```
[Okta]
organization = mycompany
username = ryang
enable_keychain = true
```

## Goals

I started this project because I got frustrated trying to install the popular java CLI on Catalina. I pivoted to a few different python competitors and quickly ran into bugs I didn't want to fix. So I'd like to make this _easier_, but only time will tell. Please [file issues](https://github.com/ryangraham/credz/issues/new) as you run across them. :beer:
