# User Scheduling Policy Tester

## Usage
This module is for running tests inside the VM (as the VM itself has no internal compiler). Run this code inside by executing:
```sh
userschedtest
```

## Buildroot Setup
1. Clone this repo inside the `package` directory of your buildroot instance.
```
git clone https://github.com/derschiw/userschedtest /path/to/buildroot/package/userschedtest
```
2. Add the package to the build process of buildroot
```sh
echo "BR2_PACKAGE_USERSCHEDTEST=y" >> path/to/buildroot/local.mk
```