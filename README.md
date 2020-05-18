
[![Build Status](https://circleci.com/gh/ryangraham/credz.svg?style=svg)](https://circleci.com/gh/ryangraham/credz)
[![GitHub license](https://img.shields.io/badge/license-MIT-blue.svg)](https://raw.githubusercontent.com/ryangraham/credz/master/LICENSE)

# credz

Turn your Okta identity into AWS credentials on the command line.

<img src="/images/usage.gif?raw=true"/>

## Table of Contents

- [Installation](#installation)
- [Usage](#usage)
- [Configuration](#configuration)
- [Goals](#goals)

## Installation

Install credz using [Homebrew](https://brew.sh/).

```
brew tap ryangraham/credz
brew install credz
credz -v
```
<img src="/images/install_1.gif?raw=true"/>

## Usage

_warning: this is alpha software and it will overwrite your ~/.aws/credentials file. I recommend taking a manual backup beforehand._

First run credz to populate a profile named test.
```
credz -p test
```
The first time you run credz you will be prompted for three things. This information will be cached for subsequent runs.
1. Okta Organization (_If your Okta URL is `https://scooterz.okta.com`, then your Okta organization will be `scooterz`_)
2. Okta Username
3. Okta Password

Next test out your new profile with AWS CLI.
```
aws sts get-caller-identity --profile test
```

## Configuration

credz will generate the default config file for you. (_~/.credz_)

```
[Okta]
organization = mycompany
username = ryang
enable_keychain = true
```

Use the _-c_ flag to specify an alternate config file.
```
credz -c other.cfg -p tao
```

## Goals

I started this project because I got frustrated trying to install the popular java CLI on Catalina. I pivoted to a few different python competitors and quickly ran into bugs I didn't want to fix. So I'd like to make this _easier_, but only time will tell. Please [file issues](https://github.com/ryangraham/credz/issues/new) as you run across them. :beer:
