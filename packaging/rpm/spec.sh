#!/bin/sh
# USAGE:
#       spec.sh nootka-version sys-arch

cat <<EOF
Summary: Application to help with learning classical score notation.
Name: nootka
Version: $1
Release: 1
License: GPLv3
Group: Education
URL: http://nootka.sf.net
Packager: See Look <seelook@gmail.com>
BuildRoot: %_topdir/%{name}-%{version}
BuildArch: $2
EOF
# %description:

# %changelog:
